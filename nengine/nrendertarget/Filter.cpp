
#include "StdAfx.h"
#include "Filter.h"
#include "TinyXML.h"
#include "GlobalSingletonStorage.h"

CFilter::CFilter():
	m_textureStepLocation(-1),
	m_lastHeight(0xFFFFFFFF),
	m_lastWidth(0xFFFFFFFF),
	m_usedSlotCount(0),
	m_shader(NULL),
	MP_STRING_INIT(m_fragmentShaderSource),
	MP_STRING_INIT(m_vertexShaderSource),
	MP_MAP_INIT(m_floatMap),
	MP_MAP_INIT(m_imageMap),
	MP_VECTOR_INIT(m_floats),
	MP_VECTOR_INIT(m_floatNames)
{
}

bool CFilter::Load(std::string fileName)
{	
	USES_CONVERSION;
	TiXmlDocument doc( fileName.c_str());
	bool isLoaded = g->rl->LoadXml( (void*)&doc, fileName);

	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] Loading filter ", fileName, " failed...");
		return false;
	}

	TiXmlNode* filter = doc.FirstChild("filter");

	const char* shader = filter->ToElement()->Attribute("shader");
	int shaderID = -1;
	if (shader)
	{
		shaderID = g->sm.GetObjectNumber(shader);
	}
	else
	{
		TiXmlNode* func = filter->FirstChild("function");
		if (!func)
		{
			g->lw.WriteLn("[ERROR] Loading filter ", fileName, " without any functions");
			return false;
		}

		m_fragmentShaderSource = "";
		m_vertexShaderSource = "";

		while (func != NULL)
		{
			if (!func->ToElement()->Attribute("name"))
			{
				g->lw.WriteLn("[ERROR] Loading filter ", fileName, " because of unnamed function");
				return false;
			}

			std::string funcName = func->ToElement()->Attribute("name");
			if (funcName == "GetColor")
			{
				if (!MakeFragmentShader(func))
				{
					return false;
				}
			}
			else if (funcName == "GetOffset")
			{
				if (!MakeVertexShader(func))
				{
					return false;
				}
			}
			else
			{
				g->lw.WriteLn("[ERROR] Loading filter ", fileName, " because of non-standard function");
				return false;
			}

			func = filter->IterateChildren("function", func);
		}

		if (m_vertexShaderSource.empty())
		{
			m_vertexShaderSource = "void main(void)\n{\ngl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\ngl_TexCoord[0] = gl_MultiTexCoord0;\n}\n";
		}

		shaderID = g->sm.LoadShaderFromMemory(GetName(), m_vertexShaderSource, m_fragmentShaderSource);
	}

	m_shader = NULL;
	if (shaderID != -1)
	{
		m_shader = g->sm.GetObjectPointer(shaderID);
		for (unsigned char slotID = 0; slotID < 8; slotID++)
		{
			int loc = m_shader->SetUniformTexture(("texture" + IntToStr(slotID)).c_str(), slotID);
			if (loc != -1)
			{
				m_usedSlotCount = slotID + 1;
			}
		}
		m_textureStepLocation = m_shader->SetUniformVector2D("textureStep", CVector2D(0, 0));
		m_shader->Unbind();
	}

	MP_VECTOR<MP_STRING>::iterator itf = m_floatNames.begin();
	MP_VECTOR<MP_STRING>::iterator itfEnd = m_floatNames.end();

	for ( ; itf != itfEnd; itf++)
	{
		int location = SetFloatArgument(*itf, 1.0f);
		m_floats.push_back(location);
	}

	return true;
}

void CFilter::Bind(unsigned int width, unsigned int height)
{
	m_shader->Bind();

	for (unsigned char slotID = m_usedSlotCount - 1; slotID != 255; slotID--)
	{
		g->stp.SetActiveTexture(slotID);
		GLFUNC(glEnable)(GL_TEXTURE_2D);
		g->tm.BindTexture(m_textureSlots[slotID]);
	}

	if (m_textureStepLocation != -1)
	{
		if ((width != m_lastWidth) || (height != m_lastHeight))
		{
			m_shader->SetUniformVector2D(m_textureStepLocation, CVector2D(1.0f / (float)width, 1.0f / (float)height));
			m_lastWidth = width;
			m_lastHeight = height;
		}
	}
}

void CFilter::Unbind()
{
	for (unsigned char slotID = m_usedSlotCount - 1; slotID > 0; slotID--)
	{
		g->stp.SetActiveTexture(slotID);
		GLFUNC(glDisable)(GL_TEXTURE_2D);
	}

	g->stp.SetActiveTexture(0);

	m_shader->Unbind();
}

void CFilter::SetSourceImage(int slotID, int textureID)
{
	m_textureSlots[slotID] = textureID;
}

bool CFilter::MakeFragmentShader(void* _func)
{
	TiXmlNode* func = (TiXmlNode*)_func;

	TiXmlNode* args = func->FirstChild("arguments");

	std::vector<std::string> textures;
	
	TiXmlNode* image = args->FirstChild("image");
	while (image != NULL)
	{
		TiXmlElement* imageElement = image->ToElement();
		if (!imageElement->GetText())
		{
			g->lw.WriteLn("[ERROR] Argument in function with empty name!");
			return false;
		}

		std::string texture = imageElement->GetText();

		if (!CheckVarName(texture))
		{
			g->lw.WriteLn("[ERROR] Invalid argument name");
			return false;
		}

		textures.push_back(texture);
		m_imageMap.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(texture), textures.size() - 1));
		
		image = args->IterateChildren("image", image);
	}

	TiXmlNode* floatVar = args->FirstChild("float");
	while (floatVar != NULL)
	{
		TiXmlElement* floatElement = floatVar->ToElement();
		if (!floatElement->GetText())
		{
			g->lw.WriteLn("[ERROR] Argument in function with empty name!");
			return false;
		}

		std::string floatVarName = floatElement->GetText();

		if (!CheckVarName(floatVarName))
		{
			g->lw.WriteLn("[ERROR] Invalid argument name");
			return false;
		}

		m_floatNames.push_back(MAKE_MP_STRING(floatVarName));
		m_floatMap.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(floatVarName), m_floatNames.size() - 1));
		
		floatVar = args->IterateChildren("float", floatVar);
	}

	TiXmlNode* body = func->FirstChild("body");
	if (!body)
	{
		g->lw.WriteLn("[ERROR] Function body is not exists");
		return false;
	}

	if (!body->ToElement()->GetText())
	{
		g->lw.WriteLn("[ERROR] Empty function body");
		return false;
	}

	m_fragmentShaderSource = body->ToElement()->GetText();

	m_fragmentShaderSource = "void main (void)\n{\n" + m_fragmentShaderSource;
	m_fragmentShaderSource += "}\n";

	{
		MP_VECTOR<MP_STRING>::iterator it = m_floatNames.begin();
		MP_VECTOR<MP_STRING>::iterator itEnd = m_floatNames.end();
	
		for ( ; it != itEnd; it++)
		{
			std::string floatDeclaration = "uniform float ";
			floatDeclaration += *it;
			floatDeclaration += ";";
			m_fragmentShaderSource = floatDeclaration + m_fragmentShaderSource;
		}
	}

	int i = 0;
	std::vector<std::string>::iterator it = textures.begin();
	std::vector<std::string>::iterator itEnd = textures.end();

	for ( ; it != itEnd; it++, i++)
	{
		std::string textureDeclaration = "uniform sampler2D texture";
		textureDeclaration += IntToStr(i);
		textureDeclaration += ";\n";
		m_fragmentShaderSource = textureDeclaration + m_fragmentShaderSource;

		std::string textureFunc = *it;
		textureFunc += ".getCurrentPixel(";
		std::string newTextureFunc = "texture2D(texture";
		newTextureFunc += IntToStr(i);
		newTextureFunc += ", gl_TexCoord[0].xy";
		StringReplace(m_fragmentShaderSource, textureFunc, newTextureFunc);

		textureFunc = *it;
		textureFunc += ".getOfsPixel(";

		int pos = m_fragmentShaderSource.find(textureFunc);
		while (pos != -1)
		{
			int pos2 = pos + textureFunc.size();
			while (((unsigned int)pos2 < m_fragmentShaderSource.size()) && (m_fragmentShaderSource[pos2] != ')'))
			{
				pos2++;
			}

			if (m_fragmentShaderSource[pos2] != ')')
			{
				g->lw.WriteLn("[ERROR] Error in getOfsPixel arguments");
				return false;
			}

			int argPos = pos + textureFunc.size();
			std::string args = m_fragmentShaderSource.substr(argPos, pos2 - argPos);
			int delimPos = args.find(",");
			if (delimPos == -1)
			{
				g->lw.WriteLn("[ERROR] Error in getOfsPixel arguments");
				return false;
			}

			std::string arg1 = args.substr(0, delimPos);
			std::string arg2 = args.substr(delimPos + 1, args.size() - delimPos - 1);

			StringReplace(arg1, " ", "");
			StringReplace(arg2, " ", "");

			/*float x = atof(arg1.c_str());
			float y = atof(arg2.c_str());

			if ((x == 0.0f) && (arg1 != "0") && (arg1 != "0.0"))
			{
				g->lw.WriteLn("[ERROR] Error in 1st getOfsPixel argument");
				return false;
			}

			if ((y == 0.0f) && (arg2 != "0") && (arg2 != "0.0"))
			{
				g->lw.WriteLn("[ERROR] Error in 1st getOfsPixel argument");
				return false;
			}*/

			std::string newTextureFunc = "texture2D(texture";
			newTextureFunc += IntToStr(i);
			newTextureFunc += ", gl_TexCoord[0].xy + vec2(";

			/*if (x == 0.0f)
			{
				newTextureFunc += "0.0, ";
			}
			else*/
			{
				newTextureFunc += /*FloatToStr(x)*/arg1;
				newTextureFunc += " * textureStep.x, ";
			}

			/*if (y == 0.0f)
			{
				newTextureFunc += "0.0))";
			}
			else*/
			{
				newTextureFunc += /*FloatToStr(y)*/arg2;
				newTextureFunc += " * textureStep.y))";
			}

			std::string oldTextureFunc = m_fragmentShaderSource.substr(pos, pos2 - pos + 1);

			StringReplace(m_fragmentShaderSource, oldTextureFunc, newTextureFunc);

			pos = m_fragmentShaderSource.find(textureFunc);
		}

		textureFunc = *it;
		textureFunc += ".getPixel(";

		pos = m_fragmentShaderSource.find(textureFunc);
		while (pos != -1)
		{
			int pos2 = pos + textureFunc.size();
			int cnt = 1;
			while (((unsigned int)pos2 < m_fragmentShaderSource.size()) && ((m_fragmentShaderSource[pos2] != ')') || (cnt != 1)))
			{
				if (m_fragmentShaderSource[pos2] != '(')
				{
					cnt++;
				}
				if (m_fragmentShaderSource[pos2] != ')')
				{
					cnt--;
				}
				pos2++;
			}

			if (m_fragmentShaderSource[pos2] != ')')
			{
				g->lw.WriteLn("[ERROR] Error in getPixel arguments");
				return false;
			}

			int argPos = pos + textureFunc.size();
			std::string args = m_fragmentShaderSource.substr(argPos, pos2 - argPos);
			int delimPos = args.find(",");
			if (delimPos == -1)
			{
				g->lw.WriteLn("[ERROR] Error in getPixel arguments");
				return false;
			}

			std::string arg1 = args.substr(0, delimPos);
			std::string arg2 = args.substr(delimPos + 1, args.size() - delimPos - 1);

			StringReplace(arg1, "%x", "(gl_TexCoord[0].x / textureStep.x)");
			StringReplace(arg1, "%y", "(gl_TexCoord[0].y / textureStep.y)");
			StringReplace(arg1, "%width", "(1.0 / textureStep.x)");
			StringReplace(arg1, "%height", "(1.0 / textureStep.y)");

			StringReplace(arg2, "%x", "(gl_TexCoord[0].x / textureStep.x)");
			StringReplace(arg2, "%y", "(gl_TexCoord[0].y / textureStep.y)");
			StringReplace(arg2, "%width", "(1.0 / textureStep.x)");
			StringReplace(arg2, "%height", "(1.0 / textureStep.y)");

			std::string newTextureFunc = "texture2D(texture";
			newTextureFunc += IntToStr(i);
			newTextureFunc += ", vec2((";
			newTextureFunc += arg1;
			newTextureFunc += ") * textureStep.x, (";
			newTextureFunc += arg2;
			newTextureFunc += ") * textureStep.y))";

			std::string oldTextureFunc = m_fragmentShaderSource.substr(pos, pos2 - pos + 1);

			StringReplace(m_fragmentShaderSource, oldTextureFunc, newTextureFunc);

			pos = m_fragmentShaderSource.find(textureFunc);
		}
	}

	m_fragmentShaderSource = "uniform vec2 textureStep;\n" + m_fragmentShaderSource;

	StringReplace(m_fragmentShaderSource, " less ", " < ");
	StringReplace(m_fragmentShaderSource, " more ", " > ");
	StringReplace(m_fragmentShaderSource, " less_or_equal ", " <= ");
	StringReplace(m_fragmentShaderSource, " more_or_equal ", " >= ");
	StringReplace(m_fragmentShaderSource, "%x", "(gl_TexCoord[0].x / textureStep.x)");
	StringReplace(m_fragmentShaderSource, "%y", "(gl_TexCoord[0].y / textureStep.y)");
	StringReplace(m_fragmentShaderSource, "%width", "(1.0 / textureStep.x)");
	StringReplace(m_fragmentShaderSource, "%height", "(1.0 / textureStep.y)");
	StringReplace(m_fragmentShaderSource, "return ", "gl_FragColor = ");
	StringReplace(m_fragmentShaderSource, " and ", " && ");

	g->lw.WriteLn(m_fragmentShaderSource);

	return true;
}

bool CFilter::MakeVertexShader(void* /*_func*/)
{
	return true;
}

int CFilter::SetFloatArgument(std::string argumentName, float value)
{
	return m_shader->SetUniformFloat(argumentName.c_str(), value);
}

void CFilter::SetFloatArgument(int argumentID, float value)
{
	m_shader->Bind();
	m_shader->SetUniformFloat(m_floats[argumentID], value);
	m_shader->Unbind();
}

unsigned int CFilter::GetFloatID(std::string& name)
{
	MP_MAP<MP_STRING, int>::iterator it = m_floatMap.find(MAKE_MP_STRING(name));
	if (it != m_floatMap.end())
	{
		return (*it).second;
	}
	else
	{
		return 0xFFFFFFFF;
	}
}

unsigned int CFilter::GetImageID(std::string& name)
{
	MP_MAP<MP_STRING, int>::iterator it = m_imageMap.find(MAKE_MP_STRING(name));
	if (it != m_imageMap.end())
	{
		return (*it).second;
	}
	else
	{
		return 0xFFFFFFFF;
	}
}

CFilter::~CFilter()
{
}