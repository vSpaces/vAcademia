
#include "StdAfx.h"
#include "Material.h"
#include <assert.h>
#include <malloc.h>
#include "TinyXML.h"
#include "HDRPostEffect.h"
#include "HelperFunctions.h"
#include "PlatformDependent.h"
#include "GlobalSingletonStorage.h"
#include "FileFunctions.h"

#define TEXTURE_NOT_FOUND				-1
#define TEXTURE_MAIN_CURRENT_TEXTURE	-2
#define TEXTURE_REFRACTION_TEXTURE		-3
#define TEXTURE_REFLECTION_TEXTURE		-4

#define MM_NONE							0
#define	MM_NEAREST						1
#define MM_AVERAGE						2

#define	MF_NEAREST_STR					"nearest"
#define MF_LINEAR_STR					"linear"

#define MF_NEAREST						0
#define MF_LINEAR						1

#define MANAGE_TEXTURES_FOR_MATERIAL	1

_SParam::_SParam():
	MP_STRING_INIT(name)
{
	param[0] = 0;
	location = -1;
	paramType = 0;
}

_SParam::~_SParam()
{
}

_STextureAnimation::_STextureAnimation():
	MP_VECTOR_INIT(textureIDs)
{
	delta = 0;
}

_SAsynchTexture::_SAsynchTexture():
	MP_STRING_INIT(name)
{
	level = 0;
	subID = 0;
	isPacked = true;
	type = TEXTURE_SLOT_EMPTY;
}

_SAsynchTexture::_SAsynchTexture(_SAsynchTexture* a):
	MP_STRING_INIT(name)
{
	name = a->name;
	level = a->level;
	subID = a->subID;
	isPacked = a->isPacked;
	type = a->type;
}

void _STextureSlot::CopyFrom(_STextureSlot* slot)
{
	slotID = slot->slotID;
	type = slot->type;

	if (slot->content)
	{
		switch (slot->type)
		{
			case TEXTURE_SLOT_TEXTURE_3D:
				slot->content = content;
				break;

			case TEXTURE_SLOT_TEXTURE_CUBEMAP:
				slot->content = content;
				break;

			case TEXTURE_SLOT_TEXTURE_2D:
				{
					_STexture* newTexture = MP_NEW(_STexture);
					_STexture* texture = (_STexture *)slot->content;
					newTexture->isAlphaBlend = texture->isAlphaBlend;
					newTexture->textureID = texture->textureID;
					content = newTexture;
				}
				break;

			case TEXTURE_SLOT_ANIMATION:
				{
					_STextureAnimation* newTexture = MP_NEW(_STextureAnimation);
					_STextureAnimation* texture = (_STextureAnimation *)slot->content;
					newTexture->delta = texture->delta;
					newTexture->textureIDs = texture->textureIDs;
					content = newTexture;
				}
				break;
		}
	}
}

void _SParam::CopyFrom(_SParam* _param)
{
	name = _param->name;
	paramType = _param->paramType;
	location = _param->location;
	memcpy(param, _param->param, sizeof(param));
}

CMaterial::CMaterial():
	m_isShaderParamsOptimized(false),
	m_isTextureMatrixChanged(false),
	m_isQualityChanged(false),
	m_isManageTextures(false),
	m_alphaBlendTextureID(-1),
	m_normalMapLevelID(-1),
	m_isQualityUsed(false),
	m_isFirstBind(true),	
	m_isUnique(false),	
	m_shaderID(-1),
	m_shader(NULL),
	m_sourceID(0),
	m_id(-1),	
	m_data(NULL),
	MP_VECTOR_INIT(m_usedTextureSlots),	
	MP_MAP_INIT(m_shaderParamsMap),
	MP_MAP_INIT(m_updateOnceShaderParamsMap),
	MP_VECTOR_INIT(m_shaderParams),
	MP_STRING_INIT(m_qualityParam),
	MP_VECTOR_INIT(m_notFoundTextures),
	MP_STRING_INIT(m_sourcePath)
{
	m_shaderParamsMap.clear();

	Init();

	short k;
	for (k = 0; k < STATE_COUNT; k++)
	{
		m_states[k] = NULL;
	}

	m_textureScale.Set(1.0f, 1.0f);

	for (k = 0; k < 8; k++)
	{
		m_textureSlots[k].slotID = k;
	}

	m_usedTextureSlots.reserve(4);
	m_shaderParams.reserve(12);
	m_notFoundTextures.reserve(2);
}

void CMaterial::SetUniqueStatus(bool isUnique)
{
	m_isUnique = isUnique;
}

bool CMaterial::IsUnique()const
{
	return m_isUnique;
}

void CMaterial::Clear()
{
	m_shaderParamsMap.clear();
	m_shaderParams.clear();

	short k;
	for (k = 0; k < STATE_COUNT; k++)
	if (k != STATE_TEXTURE_MATRIX)
	{
		m_states[k] = NULL;
	}

//	m_textureScale.Set(1.0f, 1.0f);

	for (k = 0; k < 8; k++)
	{
		m_textureSlots[k].slotID = k;
	}

	m_shaderID = -1;
	m_shader = NULL;

	m_isFirstBind = true;

	m_alphaBlendTextureID = TEXTURE_NOT_FOUND;

	m_isTextureMatrixChanged = false;
	m_isShaderParamsOptimized = false;
}

void CMaterial::Init()
{
	m_minFilter = /*GL_LINEAR_MIPMAP_LINEAR*/0;
	m_renderMode = MODE_RENDER_SOLID;
	m_minFilterParam = MF_LINEAR;
	m_mipmapParam = MM_AVERAGE;
	m_normalMapLevelID = -1;
	m_magFilter = /*GL_LINEAR*/0; 		
	m_mipmapBias = 0;
	m_anisotropy = 1;
}

void CMaterial::SetFiltration(const int minFilter, const int magFilter, const int anisotropy, const float mipmapBias)
{
	m_minFilter = minFilter;
	m_magFilter = magFilter;
	m_anisotropy = anisotropy;
	m_mipmapBias = mipmapBias;
}

void CMaterial::SetSource(int sourceID, std::string fileName)
{
	m_sourceID = sourceID;
	m_sourcePath = fileName;
}

void CMaterial::GetSource(int& sourceID, std::string& fileName)
{
	sourceID = m_sourceID;
	fileName = m_sourcePath;
}

void CMaterial::Clone(CMaterial* material)
{
	m_textureScale = material->m_textureScale;
	m_shader = material->m_shader;

	for (int i = 0; i < 8; i++)
	{
		m_textureSlots[i].CopyFrom(&material->m_textureSlots[i]);
	}	
	
	for (int k = 0; k < STATE_COUNT; k++)
	if (material->m_states[k])
	{
		m_states[k] = MP_NEW(SParam);
		m_states[k]->name = material->m_states[k]->name;
		m_states[k]->paramType = material->m_states[k]->paramType;
		m_states[k]->location = material->m_states[k]->location;
		memcpy(m_states[k]->param, material->m_states[k]->param, sizeof(m_states[k]->param));		
	}
	else
	{
		m_states[k] = NULL;
	}

	MP_MAP<MP_STRING, SParam*>::iterator it = material->m_shaderParamsMap.begin();
	MP_MAP<MP_STRING, SParam*>::iterator itEnd = material->m_shaderParamsMap.end();

	for ( ; it != itEnd; it++)
	{
		SParam* param = MP_NEW(SParam);
		param->CopyFrom((*it).second);
		m_shaderParamsMap.insert(MP_MAP<MP_STRING, SParam*>::value_type((*it).first, param));

		std::vector<SParam*>::iterator iter = material->m_shaderParams.begin();
		std::vector<SParam*>::iterator iterEnd = material->m_shaderParams.end();

		for ( ; iter != iterEnd; iter++)
		if (*iter == (*it).second)
		{
			m_shaderParams.push_back(param);
		}
	}

	m_id = GetID();
	m_shaderID = material->m_shaderID;
	m_renderMode = material->m_renderMode;
	m_normalMapLevelID = material->m_normalMapLevelID;
	m_alphaBlendTextureID = material->m_alphaBlendTextureID;
	m_isFirstBind = material->m_isFirstBind;
	m_isTextureMatrixChanged = material->m_isTextureMatrixChanged;
	m_isShaderParamsOptimized = false;
	m_magFilter = material->m_magFilter;
	m_minFilter = material->m_minFilter;
	m_anisotropy = material->m_anisotropy;
	m_sourcePath = material->m_sourcePath;
	m_sourceID = material->m_sourceID;

	std::vector<SAsynchTexture *>::iterator ait = material->m_notFoundTextures.begin();
	std::vector<SAsynchTexture *>::iterator aitEnd = material->m_notFoundTextures.end();

	for ( ; ait != aitEnd; ait++)
	{
		MP_NEW_P(newAsynchTexture, SAsynchTexture, *ait);		
		m_notFoundTextures.push_back(newAsynchTexture);

		// надо еще каждой текстуре добавлять себя в качестве листенера.
	}	
}

int CMaterial::GetNotFoundTexturesCount()const
{	
	return m_notFoundTextures.size();
}

bool CMaterial::IsZeroLevelUsed()const
{
	return (m_textureSlots[0].type != TEXTURE_SLOT_EMPTY);
}

bool CMaterial::IsNotFoundTextureNormalMap(unsigned int id)
{
	if (m_notFoundTextures.size() > id)
	{
		return (m_notFoundTextures[id]->level == m_normalMapLevelID);
	}
	else
	{
		return false;
	}
}

bool CMaterial::GetNotFoundTexturePackedStatus(unsigned int id)
{
	if (m_notFoundTextures.size() > id)
	{
		return m_notFoundTextures[id]->isPacked;
	}
	else
	{
		return true;
	}
}

std::string& CMaterial::GetNotFoundTextureName(unsigned int id)
{
	if (m_notFoundTextures.size() > id)
	{
		return m_notFoundTextures[id]->name;
	}
	else
	{
#pragma warning(push)
#pragma warning(disable : 4172)
#pragma warning(disable : 4239)
		return std::string("");
#pragma warning(pop)
	}
}

int CMaterial::GetNotFoundTextureType(unsigned int id)
{
	if (m_notFoundTextures.size() > id)
	{
		return m_notFoundTextures[id]->type;
	}
	else
	{
		return -1;
	}
}

void CMaterial::UpdateNotFoundTexture(unsigned int id, unsigned int textureID)
{
	SAsynchTexture* asynchTexture = m_notFoundTextures[id];

	m_textureSlots[asynchTexture->level].type = asynchTexture->type;

	switch (asynchTexture->type)
	{
	case TEXTURE_SLOT_TEXTURE_2D:
		((STexture *)m_textureSlots[asynchTexture->level].content)->textureID = textureID;
#ifdef MANAGE_TEXTURES_FOR_MATERIAL
		if (m_isManageTextures)
		{
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			texture->AddRef();
		}
#endif
		break;

	case TEXTURE_SLOT_ANIMATION:
		((STextureAnimation *)m_textureSlots[asynchTexture->level].content)->textureIDs[asynchTexture->subID] = textureID;
#ifdef MANAGE_TEXTURES_FOR_MATERIAL
		if (m_isManageTextures)
		{
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			texture->AddRef();
		}
#endif
		break;

	case TEXTURE_SLOT_TEXTURE_CUBEMAP:
		int objectID = g->cmm.GetObjectNumber(asynchTexture->name);
		if (TEXTURE_NOT_FOUND == objectID)
		{
			objectID = g->cmm.AddObject(asynchTexture->name);
		}
		m_textureSlots[asynchTexture->level].content = (void*)objectID;
		break;
	}

	UpdateUsedTextureSlots();

	if (0 == asynchTexture->level)
	{
		UpdateRenderingMode();

		OnAfterChanges(3);
	}
}

void CMaterial::SetTextureLevel(const int level, const short type, void* const content)
{
#ifdef MANAGE_TEXTURES_FOR_MATERIAL
	if ((content != NULL) && (!m_isQualityChanged) && (m_isManageTextures))
	switch (type)
	{
	case TEXTURE_SLOT_TEXTURE_2D:
		{
			STexture* texture = (STexture *)content;
			CTexture* _texture = g->tm.GetObjectPointer(texture->textureID);
			_texture->AddRef();			
		}
		break;

	case TEXTURE_SLOT_ANIMATION:
		STextureAnimation* animation = (STextureAnimation *)content;
		std::vector<int>::iterator it = animation->textureIDs.begin();
		std::vector<int>::iterator itEnd = animation->textureIDs.end();

		for ( ; it != itEnd; it++)
		{
			CTexture* texture = g->tm.GetObjectPointer(*it);
			texture->AddRef();
		}
		break;
	}
#endif

	if (m_textureSlots[level].type != TEXTURE_SLOT_EMPTY)
	{
		switch (m_textureSlots[level].type)
		{
		case TEXTURE_SLOT_TEXTURE_2D:
			{
			STexture* texture = (STexture *)m_textureSlots[level].content;
#ifdef MANAGE_TEXTURES_FOR_MATERIAL
			if ((!m_isQualityChanged) && (m_isManageTextures))
			{
				CTexture* _texture = g->tm.GetObjectPointer(texture->textureID);
				_texture->ReleaseRef();			
			}
#endif
			MP_DELETE(texture);
			}
			break;

		case TEXTURE_SLOT_ANIMATION:
#ifdef MANAGE_TEXTURES_FOR_MATERIAL
			STextureAnimation* animation = (STextureAnimation *)m_textureSlots[level].content;
			if ((!m_isQualityChanged) && (m_isManageTextures))
			{				
				std::vector<int>::iterator it = animation->textureIDs.begin();
				std::vector<int>::iterator itEnd = animation->textureIDs.end();

				for ( ; it != itEnd; it++)
				{
					CTexture* texture = g->tm.GetObjectPointer(*it);
					texture->ReleaseRef();
				}
			}
#endif
			MP_DELETE(animation);
			break;
		}
	}

	m_textureSlots[level].type = type;
	m_textureSlots[level].content = content;

	UpdateUsedTextureSlots();

	if (0 == level)
	{
		UpdateRenderingMode();

		OnAfterChanges(3);
	}
}

void CMaterial::SetID(int id)
{
	m_id = id;
}

int CMaterial::GetID()
{
	return m_id;
}

void CMaterial::ReInitialize()
{
	m_isFirstBind = true;
	m_isShaderParamsOptimized = false;
}

void CMaterial::LoadFromXML(void* const _doc, const std::wstring& fileName, bool isManageTextures)
{
	m_isManageTextures = isManageTextures;
	TiXmlDocument* doc = (TiXmlDocument*)_doc;
	TiXmlNode* groups = doc->FirstChild();

	if (!groups)
	{
		return;
	}

	if ((groups->NoChildren()) || (groups == NULL))
	{
		return;
	}

	TiXmlNode* material = doc->FirstChild("material");

	while (material != NULL)
	{
		TiXmlElement* elem = material->ToElement();
		if  (elem->Attribute("target"))
		{
			m_isQualityUsed = true;

			std::string target = elem->Attribute("target");
			target += ";";

			int delimPos = target.find(";");

			while(delimPos != -1)
			{
				std::string  mode = target.substr(0, delimPos);
				if  (mode == g->rs.GetQualityMode() )
				{
					SetQualityParam(mode);
					groups = material;
					material = NULL;
					break;
				}
				target = target.substr(delimPos + 1, target.size() - delimPos - 1);
				delimPos = target.find(";");
			}

			if (material == NULL) break;

		}
		material = doc->IterateChildren("material", material);	
	}                          

	TiXmlNode* state = groups->FirstChild("state");
		
	while (state != NULL)
	{
		TiXmlElement* elem = state->ToElement();

		if ((!elem->Attribute("name")) && (!elem->Attribute("value")))
		{
			g->lw.WriteLn("[ERROR] State in material incomplete");
			return;
		}

		AddState(elem->Attribute("name"), elem->Attribute("value"));

		state = groups->IterateChildren("state", state);
	}

	if ((!g->rs.IsNotOnlyLowQualitySupported()) && (GetBillboardOrient() == 1))
	{
		return;
	}

	if (g->rs.IsNotOnlyLowQualitySupported())
	{
		TiXmlNode* group = groups->FirstChild("shader");

		while (group != NULL)
		{
			TiXmlElement* elem = group->ToElement();

			if (elem)
				if (elem->GetText())
				{
					std::string shaderName = elem->GetText();

					int shaderID = g->sm.GetObjectNumber(shaderName);
					if (shaderID == -1)
					{
						g->lw.WriteLn("[ERROR] Shader for material not found");
					}
					else
					{
						SetShaderID(shaderID);
					}
				}

				group = groups->IterateChildren("shader", group);
		}

		group = groups->FirstChild("param");

		while (group != NULL)
		{
			TiXmlElement* elem = group->ToElement();

			if ((!elem->Attribute("type")) || (!elem->Attribute("name")) || (!elem->Attribute("value")))
			{
				g->lw.WriteLn("[ERROR] Wrong param definition in material");
			}

			std::string type = elem->Attribute("type");
			std::string name = elem->Attribute("name");
			std::string value = elem->Attribute("value");

			int paramTypeID = g->sm.GetParamTypeID(type);
			int valueID = g->sm.GetParamTypeID(type);

			if (valueID != -1)
			{
				switch(paramTypeID)
				{
				case -1:
					g->lw.WriteLn("[ERROR] Wrong param definition in material");
					break;

				case SHADER_PARAM_FLOAT:
					SetShaderFloatParameter(name, StringToFloat(value.c_str()));				
					break;

				case SHADER_PARAM_INT:
					SetShaderIntParameter(name, rtl_atoi(value.c_str()));
					break;

				case SHADER_PARAM_VECTOR:
					{
						CVector3D tmp = StrToVector3D(value);
						SetShaderVectorParameter(name, CVector4D(tmp.x, tmp.y, tmp.z, 1.0f));
					}
					break;

				case SHADER_PARAM_TEXTURE:
					{
						int _value = rtl_atoi(value.c_str());
						SetShaderTextureParameter(name, _value);
						if (name == "normalMap")
						{
							m_normalMapLevelID = _value;
						}
					}
					break;
				}
			}
			else
			{
				SetPredefinedParam(name, g->sm.GetPredefinedParamID(value));
			}

			group = groups->IterateChildren("param", group);
		}
	}
	

	TiXmlNode* texture = groups->FirstChild("texture");
		
	while (texture != NULL)
	{
		TiXmlElement* elem = texture->ToElement();

		if ((!elem->Attribute("level")) || (!elem->Attribute("src")))
		{
			g->lw.WriteLn("[ERROR] Wrong texture definition in material");
			return;
		}

		std::string src = elem->Attribute("src");
		src = StringToUpper(src);
		int levelID = rtl_atoi(elem->Attribute("level"));
		bool isAlphaBlend = false;
		bool isPacked = true;

		if (elem->Attribute("alphablend"))
		{
			std::string ab = elem->Attribute("alphablend");
			if (ab == "true")
			{
				isAlphaBlend = true;
			}
		}

		if (!g->rs.IsNotOnlyLowQualitySupported() && levelID!=0 && !isAlphaBlend)
		{
			// не загружаем вспомогательные текстуры
			texture = groups->IterateChildren("texture", texture);
			continue;
		}

		if (elem->Attribute("packed"))
		{
			std::string packed = elem->Attribute("packed");
			if (packed == "false")
			{
				isPacked = false;
			}
		}

		int textureID = TEXTURE_MAIN_CURRENT_TEXTURE;
		
		if (src != "%CURRENT_MAIN_TEXTURE")
		{
			if (src == "%REFRACTION_TEXTURE")
			{				
				textureID = TEXTURE_REFRACTION_TEXTURE; 
			}
			else
			{
				if (src == "%REFLECTION_TEXTURE")
				{				
					textureID = TEXTURE_REFLECTION_TEXTURE; 
				}
				else
				{
					textureID = g->tm.GetObjectNumber(src);
				}
			}
		}
        
		if (textureID != TEXTURE_NOT_FOUND)
		{
			if (TEXTURE_SLOT_TEXTURE_2D == m_textureSlots[levelID].type)
			{
				STexture* _texture = (STexture*)m_textureSlots[levelID].content;
				if (_texture->isAlphaBlend == isAlphaBlend)
				{
					textureID = _texture->textureID;
				}
			}

			STexture* texture = MP_NEW(STexture);
			texture->isAlphaBlend = isAlphaBlend;
			texture->textureID = textureID;
			SetTextureLevel(levelID, TEXTURE_SLOT_TEXTURE_2D, texture);
		}
		else
		{
			SAsynchTexture* asynchTexture = MP_NEW(SAsynchTexture);
			asynchTexture->name = src;
			asynchTexture->level = (unsigned char)levelID;
			asynchTexture->type = TEXTURE_SLOT_TEXTURE_2D;
			asynchTexture->isPacked = isPacked;
			m_notFoundTextures.push_back(asynchTexture);

			STexture* texture = MP_NEW(STexture);
			texture->isAlphaBlend = isAlphaBlend;
			texture->textureID = TEXTURE_NOT_FOUND;
			SetTextureLevel(levelID, TEXTURE_SLOT_TEXTURE_2D, texture);

			//g->lw.WriteLn("[WARNING] Texture ", src, " not found in material ", fileName, " . Possible texture can be downloaded from server");
		}

		texture = groups->IterateChildren("texture", texture);
	}

	TiXmlNode* cubemap = groups->FirstChild("cubemap");
		
	while (cubemap != NULL)
	{
		TiXmlElement* elem = cubemap->ToElement();

		if (!g->rs.IsNotOnlyLowQualitySupported())
		{
			// не загружаем вспомогательные текстуры
			cubemap = groups->IterateChildren("cubemap", cubemap);
			continue;
		}

		if ((!elem->Attribute("level")) || (!elem->Attribute("src")))
		{
			g->lw.WriteLn("[ERROR] Wrong cubemap definition in material");
		}

		std::string src = elem->Attribute("src");
		src = StringToUpper(src);
		int levelID = rtl_atoi(elem->Attribute("level"));

		int textureID = g->cmm.GetObjectNumber(src);

		if (textureID != TEXTURE_NOT_FOUND)
		{
			assert(levelID >= 0);
			assert((unsigned int)levelID < 8);
			SetTextureLevel(levelID, TEXTURE_SLOT_TEXTURE_CUBEMAP, (void*)textureID);
		}
		else
		{
			SAsynchTexture* asynchTexture = MP_NEW(SAsynchTexture);
			asynchTexture->name = src;
			asynchTexture->level = (unsigned char)levelID;
			asynchTexture->type = TEXTURE_SLOT_TEXTURE_CUBEMAP;
			m_notFoundTextures.push_back(asynchTexture);

			SetTextureLevel(levelID, TEXTURE_SLOT_TEXTURE_CUBEMAP, (void*)TEXTURE_NOT_FOUND);

			//g->lw.WriteLn("[WARNING] Cubemap ", src, " not found in material ", fileName, " . Possible cubemap can be downloaded from server");
		}

		cubemap = groups->IterateChildren("cubemap", cubemap);
	}

	TiXmlNode* animation = groups->FirstChild("animation");
		
	while (animation != NULL)
	{
		TiXmlElement* elem = animation->ToElement();

		if (!g->rs.IsNotOnlyLowQualitySupported())
		{
			// не загружаем вспомогательные текстуры
			animation = groups->IterateChildren("animation", animation);
			continue;
		}

		if ((!elem->Attribute("level")) || (!elem->Attribute("delta")))
		{
			g->lw.WriteLn("[ERROR] Wrong animation definition in material");
			return;
		}

		unsigned char level = (unsigned char)rtl_atoi(elem->Attribute("level"));

		if ((level >= 0) && (level < 8))
		{
			STextureAnimation* newAnimation = MP_NEW(STextureAnimation);
			newAnimation->delta = rtl_atoi(elem->Attribute("delta"));

			TiXmlNode* texture = animation->FirstChild("texture");

			while (texture != NULL)
			{
				TiXmlElement* elem = texture->ToElement();

				if (!elem->Attribute("src"))
				{
					g->lw.WriteLn("[ERROR] Invalud definition of texture in animation in material ", fileName);
					MP_DELETE(newAnimation);
					return;
				}

				std::string src = elem->Attribute("src");
				src = StringToUpper(src);
				int textureID = g->tm.GetObjectNumber(src);

				if (textureID != TEXTURE_NOT_FOUND)
				{
					newAnimation->textureIDs.push_back(textureID);
				}
				else
				{
					newAnimation->textureIDs.push_back(TEXTURE_NOT_FOUND);

					SAsynchTexture* asynchTexture = MP_NEW(SAsynchTexture);
					asynchTexture->name = src;
					asynchTexture->subID = (unsigned char)(newAnimation->textureIDs.size() - 1);
					asynchTexture->level = level;
					asynchTexture->type = TEXTURE_SLOT_ANIMATION;

					m_notFoundTextures.push_back(asynchTexture);
					//g->lw.WriteLn("[WARNING] Texture ", src, " not found in material ", fileName, " . Possible texture can be downloaded from server");					
				}

				texture = animation->IterateChildren("texture", texture);
			}

			SetTextureLevel(level, TEXTURE_SLOT_ANIMATION, newAnimation);
		}

		animation = groups->IterateChildren("animation", animation);
	}

	TiXmlNode* texture3d = groups->FirstChild("texture3d");
		
	while (texture3d != NULL)
	{
		TiXmlElement* elem = texture3d->ToElement();

		if (!g->rs.IsNotOnlyLowQualitySupported())
		{
			// не загружаем вспомогательные текстуры
			texture3d = groups->IterateChildren("texture3d", texture3d);
			continue;
		}

		if ((!elem->Attribute("level")) || (!elem->Attribute("src")))
		{
			g->lw.WriteLn("[ERROR] Wrong 3d texture definition in material");
			return;
		}

		std::string src = elem->Attribute("src");
		int levelID = rtl_atoi(elem->Attribute("level"));

		int textureID = g->t3dm.GetObjectNumber(src);

 		if (textureID != TEXTURE_NOT_FOUND)
		{
			SetTextureLevel(levelID, TEXTURE_SLOT_TEXTURE_3D, (void*)textureID);
		}
		else
		{
			g->lw.WriteLn("[ERROR] 3D Texture ", src, " not found in material ", fileName);
		}

		texture3d = groups->IterateChildren("texture3d", texture3d);
	}

	UpdateRenderingMode();
	OnAfterChanges(3);		
}

void CMaterial::LoadFromXML(const std::wstring& fileName, bool isManageTextures)
{
	USES_CONVERSION;
	TiXmlDocument doc( W2A(fileName.c_str()));
	bool isLoaded = g->rl->LoadXml( (void*)&doc, fileName);

	if (!isLoaded)
	{
		g->lw.WriteLn("[LOADWARNING] Loading material failed... ", fileName);
		return;
	}
	
	FILE* fl = FileOpen(fileName, L"rb");
	fseek(fl, 0, 2);
	int size = ftell(fl);
	fseek(fl, 0, 0);
	unsigned char* data = MP_NEW_ARR(unsigned char, size + 1);
	data[size] = 0;
	fread(data, size, 1, fl);
	fclose(fl);
	if (m_isQualityUsed)
	{
		m_data = data;
	}
	else
	{
		MP_DELETE_ARR(data);
	}

	LoadFromXML(&doc, fileName, isManageTextures);
}

void CMaterial::LoadFromXML(ifile* const file, bool isManageTextures)
{
	TiXmlDocument doc;
	if (file)
	{
		m_data = MP_NEW_ARR(unsigned char, file->get_file_size() + 1);
		m_data[file->get_file_size()] = 0;
		file->read(m_data, file->get_file_size());
		doc.Parse((const char *)m_data);	
	}

	LoadFromXML(&doc, file->get_file_path(), isManageTextures);

	if (!m_isQualityUsed)
	{
		MP_DELETE_ARR(m_data);
		m_data = NULL;
	}
}

void CMaterial::LoadFromXML(const std::string& fileName, unsigned char* const data, bool isManageTextures)
{
	TiXmlDocument doc;
	doc.Parse((const char*)data);

	int size = strlen((const char*)data);
	m_data = MP_NEW_ARR(unsigned char, size + 1);
	memcpy(m_data, data, size + 1);

	USES_CONVERSION;
	LoadFromXML(&doc, A2W(fileName.c_str()), isManageTextures);

	if (!m_isQualityUsed)
	{
		MP_DELETE_ARR(m_data);
		m_data = NULL;
	}
}

unsigned int* CMaterial::GetState(int stateID)
{
	if (m_states[stateID])
	{
		return &m_states[stateID]->param[0];
	}
	else
	{
		return NULL;
	}
}

void CMaterial::AddState(std::string name, std::string value)
{
	name = StringToLower(name);
	value = StringToLower(value);

	int stateID = g->matm.GetStateID(name);
	if (-1 == stateID)
	{
		return;
	}

	SParam* param = NULL;

	if (m_states[stateID] == NULL)
	{
		param = MP_NEW(SParam);
		m_states[stateID] = param;
	}
	else
	{
		param = m_states[stateID];
	}

	param->name = name;

	if (STATE_ALPHATEST == stateID)
	{
		param->param[0] = ("true" == value) ? 1 : 0;

		UpdateRenderingMode();
		OnAfterChanges(0);
	} 
	else if (STATE_ALPHAVALUE == stateID)
	{				
		*(float*)(&param->param[0]) = (StringToFloat(value.c_str()) / 255.0f);
	}
	else if (STATE_ALPHAFUNC == stateID)
	{
		unsigned int func = GL_NEVER;

		if ("greater" == value)
		{
			func = GL_GREATER;
		}
		else if ("equal" == value)
		{
			func = GL_EQUAL;
		}
		else if ("less" == value)
		{
			func = GL_LESS;
		}
		else if ("lessorequal" == value)
		{
			func = GL_LEQUAL;
		}
		else if ("notequal" == value)
		{
			func = GL_NOTEQUAL;
		}
		else if ("greaterorequal" == value)
		{
			func = GL_GEQUAL;
		}
		else if ("always" == value)
		{
			func = GL_ALWAYS;
		}
		
		param->param[0] = func;
	}
	else if (STATE_ANISOTROPY == stateID)
	{
		param->param[0] = rtl_atoi(value.c_str());
		m_isFirstBind = true;

		DetectFiltering();
	}
	else if (STATE_MIN_FILTER == stateID)
	{
		if (MF_LINEAR_STR == value)
		{
			m_minFilterParam = MF_LINEAR;
		}
		else if (MF_NEAREST_STR == value)
		{
			m_minFilterParam = MF_NEAREST;
		}

		m_minFilter = GetMinFilter(m_minFilterParam, m_mipmapParam);
	}
	else if (STATE_MIPMAP == stateID)
	{
		m_mipmapParam = (unsigned char)rtl_atoi(value.c_str());

		m_minFilter = GetMinFilter(m_minFilterParam, m_mipmapParam);
	}
	else if (STATE_MAG_FILTER == stateID)
	{
		unsigned int filter = GL_LINEAR_MIPMAP_LINEAR;

		if (MF_LINEAR_STR == value)
		{
			filter = GL_LINEAR;
		}
		else if (MF_NEAREST_STR == value)
		{
			filter = GL_NEAREST;
		}

		m_magFilter = filter;
	}
	else if (STATE_ANISOTROPY_POWER == stateID)
	{
		m_anisotropy = rtl_atoi(value.c_str());
	}
	else if (STATE_MIPMAP_BIAS == stateID)
	{
		m_mipmapBias = StringToFloat(value.c_str());
	}
	else if (STATE_LINE_SIZE == stateID)
	{
		param->param[0] = rtl_atoi(value.c_str());
	}
	else if (STATE_COLOR ==	stateID)
	{
		int pos1 = value.find_first_of(";");
		int pos2 = value.find_last_of(";");

		assert(pos1 != -1);
		assert(pos2 != -1);
		assert(pos1 != pos2);

		param->param[0] = (unsigned char)rtl_atoi(value.substr(0, pos1).c_str());
		param->param[1] = (unsigned char)rtl_atoi(value.substr(pos1 + 1, pos2 - 1).c_str());
		param->param[2] = (unsigned char)rtl_atoi(value.substr(pos2 + 1, value.size() - pos2).c_str());
	}
	else if (STATE_OPACITY == stateID)
	{
		param->param[0] = rtl_atoi(value.c_str());
	}
	else if (STATE_BLEND ==	stateID)
	{
		param->param[0] = ((value == "true") ? 2 : 1);

		UpdateRenderingMode();
		OnAfterChanges(3);
	}
	else if (STATE_Z_WRITE == stateID)
	{
		param->param[0] = ((value == "true") ? 2 : 1);
	}
	else if (STATE_TWO_SIDED_TRANSPARENCY == stateID)
	{
		param->param[0] = ((value == "true") ? 2 : 1);

		UpdateRenderingMode();
		OnAfterChanges(3);
	}
	else if (STATE_TWO_SIDED == stateID)
	{
		param->param[0] = ((value == "true") ? 2 : 1);

		UpdateRenderingMode();
		OnAfterChanges(3);
	}
	else if (STATE_BILLBOARD_ORIENT == stateID)
	{
		param->param[0] = 0;
		if (value == "true")
		{
			param->param[0] = 1;
		}
		else if (value == "even_on_cpu")
		{
			param->param[0] = 2;
		}

		UpdateRenderingMode();
		OnAfterChanges(3);
	}
	else if (STATE_NEED_TANGENTS == stateID)
	{
		param->param[0] = ((value == "true") ? 2 : 1);

		UpdateRenderingMode();
		OnAfterChanges(3);
	}
	else if (STATE_BLEND_METHOD == stateID)
	{
		int func = 0;

		if ("alpha" == value)
		{
			func = 0;
		}
		else if ("add" == value)
		{
			func = 1;
		}
		else if ("mul" == value)
		{
			func = 2;
		}
					
		param->param[0] = func;
	}

	UpdateRenderingMode();
}

unsigned int CMaterial::GetMinFilter(const unsigned char minFilterParam, const unsigned char mipmapParam)
{
	unsigned int filter = GL_LINEAR_MIPMAP_LINEAR;

	if (mipmapParam == MM_NONE)
	{
		if (minFilterParam == MF_LINEAR) 
		{
			filter = GL_LINEAR;
		}
		else if (minFilterParam == MF_NEAREST)
		{
			filter = GL_NEAREST;
		}
	}
	else if (mipmapParam == MM_NEAREST)
	{
		if (minFilterParam == MF_LINEAR) 
		{
			filter = GL_NEAREST_MIPMAP_NEAREST;
		}
		else if (minFilterParam == MF_NEAREST)
		{
			filter = GL_LINEAR_MIPMAP_NEAREST;
		}
	}
	else if (mipmapParam == MM_AVERAGE)
	{
		if (minFilterParam == MF_LINEAR) 
		{
			filter = GL_NEAREST_MIPMAP_LINEAR;
		}
		else if (minFilterParam == MF_NEAREST)
		{
			filter = GL_LINEAR_MIPMAP_LINEAR;
		}
	}

	return filter;
} 

int CMaterial::GetTextureID(const unsigned int textureLevel)
{
	if (textureLevel < 8)
	{
		if (TEXTURE_SLOT_TEXTURE_2D == m_textureSlots[textureLevel].type)
		{
			return ((STexture*)(m_textureSlots[textureLevel].content))->textureID;
		}
		else
		{
			return TEXTURE_NOT_FOUND;
		}
	}
	else
	{
		return TEXTURE_NOT_FOUND;
	}
}

void CMaterial::SetShaderID(const int shaderID)
{
	assert(shaderID != -1);

	m_shaderID = shaderID;
	m_shader = g->sm.GetObjectPointer(m_shaderID);
}

int CMaterial::GetShaderID()
{
	return m_shaderID;
}

float CMaterial::GetShaderParamFloat(std::string name)
{
	MP_MAP<MP_STRING, SParam*>::iterator it = m_shaderParamsMap.find(MAKE_MP_STRING(name));

	if (it != m_shaderParamsMap.end())
	{
		if ((*it).second->paramType == SHADER_PARAM_FLOAT)
		{
			return *(float*)(&(*it).second->param[0]);
		}
		else
		{
			return 0.0f;
		}
	}
	else
	{
		return 0.0f;
	}
}

void CMaterial::SetShaderParamValue(std::string& name, std::string& type, std::string& value)
{
	MP_MAP<MP_STRING, SParam*>::iterator it = m_shaderParamsMap.find(MAKE_MP_STRING(name));

	if (it != m_shaderParamsMap.end())
	{
		std::vector<SParam*>::iterator iter = m_shaderParams.begin();
		std::vector<SParam*>::iterator iterEnd = m_shaderParams.end();
		
		for ( ; iter != iterEnd; iter++)
		if ((*iter) == (*it).second)
		{
			MP_DELETE(*iter);
			m_shaderParams.erase(iter);
			break;
		}

		m_shaderParamsMap.erase(it);
	}
	
	int paramTypeID = g->sm.GetParamTypeID(type);
	switch (paramTypeID)
	{
	case SHADER_PARAM_FLOAT:
		SetShaderFloatParameter(name, StringToFloat(value.c_str()));
		break;

	case SHADER_PARAM_TEXTURE:
		SetShaderTextureParameter(name, rtl_atoi(value.c_str()));
		break;

	case SHADER_PARAM_INT:
		SetShaderIntParameter(name, rtl_atoi(value.c_str()));
		break;
	}
}

void CMaterial::SetPredefinedParam(std::string name, int valueID)
{
	SParam* _param = MP_NEW(SParam);
	_param->paramType = SHADER_PARAM_PREDEFINED;	
	_param->param[0] = valueID;
	_param->name = name;

	m_shaderParamsMap.insert(MP_MAP<MP_STRING, SParam*>::value_type(MAKE_MP_STRING(name), _param));
	m_shaderParams.push_back(_param);
	m_isShaderParamsOptimized = false;
}

void CMaterial::SetShaderFloatParameter(std::string name, float param)
{
	SParam* _param = MP_NEW(SParam);
	_param->paramType = SHADER_PARAM_FLOAT;
	*(float*)(&_param->param[0]) = param;
	_param->name = name;

	m_shaderParamsMap.insert(MP_MAP<MP_STRING, SParam*>::value_type(MAKE_MP_STRING(name), _param));
	m_shaderParams.push_back(_param);
	m_isShaderParamsOptimized = false;
}

void CMaterial::SetShaderIntParameter(std::string name, int param)
{
	SParam* _param = MP_NEW(SParam);
	_param->paramType = SHADER_PARAM_INT;
	_param->param[0] = param;
	_param->name = name;

	m_shaderParamsMap.insert(MP_MAP<MP_STRING, SParam*>::value_type(MAKE_MP_STRING(name), _param));
	m_shaderParams.push_back(_param);
	m_isShaderParamsOptimized = false;
}

void CMaterial::SetShaderVectorParameter(std::string name, CVector4D vec)
{
	SParam* _param = MP_NEW(SParam);
	_param->paramType = SHADER_PARAM_VECTOR;	
	*((CVector4D*)&_param->param) = vec;
	_param->name = name;

	m_shaderParamsMap.insert(MP_MAP<MP_STRING, SParam*>::value_type(MAKE_MP_STRING(name), _param));
	m_shaderParams.push_back(_param);
	m_isShaderParamsOptimized = false;
}

void CMaterial::SetShaderTextureParameter(std::string name, int textureLevel)
{
	SParam* _param = MP_NEW(SParam);
	_param->paramType = SHADER_PARAM_TEXTURE;	
	_param->param[0] = textureLevel;
	_param->name = name;

	m_shaderParamsMap.insert(MP_MAP<MP_STRING, SParam*>::value_type(MAKE_MP_STRING(name), _param));
	m_shaderParams.push_back(_param);
	m_isShaderParamsOptimized = false;
}

void CMaterial::SetQualityParam(const std::string& name)
{
	m_qualityParam = name;
}

std::string CMaterial::GetQualityParam() const
{
	return m_qualityParam ;
}

bool CMaterial::IsAlphaBlendExists()
{
	for (unsigned int j = 0; j < 8; j++)
	if (TEXTURE_SLOT_TEXTURE_2D == m_textureSlots[j].type)
	{
		if (((STexture*)(m_textureSlots[j].content))->isAlphaBlend)
		{
			return true;
		}
	}

	return false;
}

void CMaterial::OnFirstShaderBind()
{
	if (!m_shader)
	{
		return;
	}

	int count = m_shader->GetDefaultsCount();

	for (int i = 0; i < count; i++)
	{
		std::string name;
		float value;

		m_shader->GetDefaultVar(i, name, value);

		MP_MAP<MP_STRING, SParam*>::iterator it = m_shaderParamsMap.find(MAKE_MP_STRING(name));
		if (it == m_shaderParamsMap.end())
		{
			SetShaderFloatParameter(name, value);
		}
	}

	std::vector<SParam*>::iterator iter = m_shaderParams.begin();

	for ( ; iter != m_shaderParams.end(); iter++)
	{
		switch ((*iter)->paramType)
		{
		case SHADER_PARAM_FLOAT:
			(*iter)->location = m_shader->SetUniformFloat((*iter)->name.c_str(), *(float *)(&(*iter)->param[0]));
			break;

		case SHADER_PARAM_VECTOR:
			(*iter)->location = m_shader->SetUniformVector((*iter)->name.c_str(), *(CVector4D *)(&(*iter)->param[0]));
			break;

		case SHADER_PARAM_TEXTURE:
			(*iter)->location = m_shader->SetUniformTexture((*iter)->name.c_str(), (*iter)->param[0]);
			break;

		case SHADER_PARAM_PREDEFINED:
			(*iter)->location = m_shader->SetUniformPredefined((*iter)->name, (*iter)->param[0]);
			break;
		}
	}

	iter = m_shaderParams.begin();

	for ( ; iter != m_shaderParams.end(); )
	if ((-1 == (*iter)->location) || (m_shader->GetVariableUpdateType((*iter)->name) == SHADER_VAR_UPDATE_TYPE_ONCE))
	{
        MP_MAP<MP_STRING, SParam*>::iterator it = m_shaderParamsMap.begin();
		for ( ; it != m_shaderParamsMap.end(); it++)
		if ((*it).second == *iter)
		{
			m_updateOnceShaderParamsMap.insert(MP_MAP<MP_STRING, SParam*>::value_type((*it).first, (*it).second));
			m_shaderParamsMap.erase(it);
			break;
		}
		
		m_shaderParams.erase(iter);
		iter = m_shaderParams.begin();
	}
	else
	{
		iter++;
	}
}

void CMaterial::DetectFiltering()
{
	int value = 1;

	if (m_states[STATE_ANISOTROPY])
	{
		value = m_states[STATE_ANISOTROPY]->param[0];
		int maxSupported = g->gi.GetMaxAnisotropy();

		if (value > maxSupported)
		{
			value = maxSupported;
		}
	
		if (value > g->rs.GetInt(MAX_ANISOTROPY))
		{
			value = g->rs.GetInt(MAX_ANISOTROPY);

			if (value < 1)
			{
				value = 1;
			}
		}		
	}

	if (value > 0)
	{
		m_minFilter = GL_LINEAR_MIPMAP_LINEAR;
		m_magFilter = GL_LINEAR;
		m_anisotropy = value;
	}
	else if (value == 0)
	{
		m_minFilter = GL_LINEAR_MIPMAP_LINEAR;
		m_magFilter = GL_LINEAR;
		m_anisotropy = 1;							
	}
	else if (value == -3)
	{
		m_minFilter = GL_LINEAR_MIPMAP_NEAREST;
		m_magFilter = GL_LINEAR;
		m_anisotropy = 1;								
	}
	else if (value == -2)
	{
		m_minFilter = GL_LINEAR;
		m_magFilter = GL_LINEAR;
		m_anisotropy = 1;				
	}
	else if (value == -1)
	{
		m_minFilter = GL_NEAREST;
		m_magFilter = GL_NEAREST;
		m_anisotropy = 1;				
	}	
	else
	{
		m_minFilter = GL_LINEAR_MIPMAP_LINEAR;
		m_magFilter = GL_LINEAR;
		m_anisotropy = 1;
	}
	m_mipmapBias = 0.0f;
}

void CMaterial::DumpTexturesDEBUG()
{
	/*for (unsigned char i = 0; i < 8; i++)
	{
		g->stp.SetActiveTexture(i);
		GLboolean b = GLFUNCR(glIsEnabled(GL_TEXTURE_2D));
		if (b)
		{
			GLint a;
			GLFUNC(glGetIntegerv)(GL_TEXTURE_BINDING_2D, &a);
			int textureID = g->tm.GetTextureIDByOGLID_DEBUG(a);
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			//std::wstring name = texture->GetName();
		}
	}

	g->stp.SetActiveTexture(0);	*/
}

void CMaterial::BindMaterial()
{
	if (g->rs.GetBool(DISABLE_TEXTURES))
	{
		return;
	}

	stat_materialChangeCount++;

	std::vector<STextureSlot *>::iterator it = m_usedTextureSlots.begin();
	std::vector<STextureSlot *>::iterator itEnd = m_usedTextureSlots.end();

	for ( ; it != itEnd; it++)
	{
		switch ((*it)->type)
		{
		case TEXTURE_SLOT_TEXTURE_2D:
			{
				bool isAlphaBlend = ((STexture*)(*it)->content)->isAlphaBlend;
				int textureID = ((STexture*)(*it)->content)->textureID;

				switch (textureID)
				{
				case TEXTURE_MAIN_CURRENT_TEXTURE:
					if (TEXTURE_SLOT_TEXTURE_2D == m_textureSlots[0].type)
					{
						textureID = ((STexture*)(m_textureSlots[0].content))->textureID;
					}
					if (TEXTURE_MAIN_CURRENT_TEXTURE == textureID)
					{
						textureID = g->tm.GetCurrentTextureID();
					}
					break;

				case TEXTURE_REFLECTION_TEXTURE:
					if (g->scm.GetActiveScene())
					{
						textureID = g->scm.GetActiveScene()->GetReflectionTextureID();
					}
					break;

				case TEXTURE_REFRACTION_TEXTURE:
					CHDRPostEffect* postEffect = (CHDRPostEffect *)g->scm.GetActiveScene()->GetPostEffect();
					if (postEffect)
					{
						textureID = postEffect->GetRefractionTextureID();						
					}
					break;
				}

				if (((*it)->slotID == 0) || (g->rs.GetBool(SHADERS_USING)) || (isAlphaBlend))
				{
					g->stp.SetActiveTexture((unsigned char)(*it)->slotID);
					GLFUNC(glEnable)(GL_TEXTURE_2D);
					g->tm.BindTexture(textureID);

					/*if ((*it)->slotID != 0)
					{
						CTexture* texture = g->tm.GetObjectPointer(textureID);
						texture->SetCurrentFiltration(m_minFilter, m_magFilter, m_anisotropy);						
					}*/

					if ((isAlphaBlend) && (m_alphaBlendTextureID != textureID) && (!m_shader))
					{
						m_alphaBlendTextureID = textureID;

						GLFUNC(glTexEnvi)(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

						GLFUNC(glTexEnvf)(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
						GLFUNC(glTexEnvf)(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
						GLFUNC(glTexEnvf)(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
						GLFUNC(glTexEnvf)(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
						GLFUNC(glTexEnvf)(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_TEXTURE);
						GLFUNC(glTexEnvf)(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);
						GLFUNC(glTexEnvf)(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);

						GLFUNC(glTexEnvf)(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);
						GLFUNC(glTexEnvf)(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
						GLFUNC(glTexEnvf)(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);				
					}
				}
			}
			break;

		case TEXTURE_SLOT_TEXTURE_3D:
			if (g->rs.GetBool(SHADERS_USING))
			{
				g->stp.SetActiveTexture((unsigned char)(*it)->slotID);
				GLFUNC(glDisable)(GL_TEXTURE_2D);
				GLFUNC(glEnable)(GL_TEXTURE_3D);
				g->t3dm.BindTexture3D((int)(*it)->content);
			}
			break;

		case TEXTURE_SLOT_TEXTURE_CUBEMAP:
			if (g->rs.GetBool(SHADERS_USING))
			{
				g->stp.SetActiveTexture((unsigned char)(*it)->slotID);
				GLFUNC(glDisable)(GL_TEXTURE_2D);
				GLFUNC(glEnable)(GL_TEXTURE_CUBE_MAP);	
				GLFUNC(glBindTexture)(GL_TEXTURE_CUBE_MAP, g->cmm.GetObjectPointer((int)(*it)->content)->GetCubeMapID());
			}
			break;

		case TEXTURE_SLOT_ANIMATION:
			if (((*it)->slotID == 0) || (g->rs.GetBool(SHADERS_USING)))
			{
				g->stp.SetActiveTexture((unsigned char)(*it)->slotID);
				GLFUNC(glEnable)(GL_TEXTURE_2D);

				STextureAnimation* animation = (STextureAnimation*)(*it)->content;

				int allTime = animation->delta * animation->textureIDs.size();
				/*int animTime = (int)g->ne.time%allTime;
				animTime = animTime; 
				int frameNum = (int)(animTime / animation->delta);	
				assert((unsigned int)frameNum < animation->textureIDs.size());*/
				float frameNumf = fmodf(g_shaderTime, (float)allTime / 1000.0f);
				unsigned int frameNum = (int)(frameNumf * 1000.0f / (float)animation->delta);

				if (frameNum >= animation->textureIDs.size())
				{
					frameNum = animation->textureIDs.size() - 1;
				}

				if (0 == allTime)
				{
					continue;
				}

				g->tm.BindTexture(animation->textureIDs[frameNum]);
			}
			break;
		}
	}

	SetShaderParams();
		
	SetMaterialStates();

	/*if (g->mm.GetCurrentRenderingModel())
	{
		std::string name = g->mm.GetCurrentRenderingModel()->GetName();
		if (name.find("shirt") != -1)
		{
			DumpTexturesDEBUG();

			if (m_shader)
			{
				m_shader->DumpUniformsDEBUG();
			}
		}
	}*/
}

void CMaterial::SetShaderParams()
{
	bool isNeedToSetShaderParams = false;
	if (g->rs.GetBool(SHADERS_USING))
	if (m_shaderID != -1)
	{		
		isNeedToSetShaderParams = g->sm.BindShader(m_shader, m_id);
	}
	else
	{
		g->sm.UnbindShader();
	}

	g->stp.SetActiveTexture(0);

	if (!g->ne.isIconicAndIntel)	
	if ((isNeedToSetShaderParams) || ((!m_isShaderParamsOptimized) && (g->rs.GetBool(SHADERS_USING))))
	{
		if (!m_isShaderParamsOptimized)
		{
			OnFirstShaderBind();

			m_isShaderParamsOptimized = true;
		}
		else
		{
			SetShaderParamsImpl();
		}
	}
}

void CMaterial::SetShaderParamsImpl()
{
	std::vector<SParam*>::iterator iter = m_shaderParams.begin();
	std::vector<SParam*>::iterator iterEnd = m_shaderParams.end();

	for ( ; iter != iterEnd; iter++)
	{
		switch ((*iter)->paramType)
		{
		case SHADER_PARAM_FLOAT:
			m_shader->SetUniformFloat((*iter)->location, *(float *)(&(*iter)->param[0]));
			break;

		case SHADER_PARAM_VECTOR:
			m_shader->SetUniformVector((*iter)->location, *(CVector4D *)(&(*iter)->param[0]));
			break;

		case SHADER_PARAM_TEXTURE:
			m_shader->SetUniformTexture((*iter)->location, (*iter)->param[0]);
			break;

		case SHADER_PARAM_PREDEFINED:
			m_shader->SetUniformPredefined((*iter)->location, (*iter)->param[0]);
			break;
		}
	}
}

void CMaterial::SetMaterialStates()
{
	if (m_states[STATE_LINE_SIZE])
	{
		if (m_states[STATE_LINE_SIZE]->param[0] > 0)
		{
			int lineWidth = m_states[STATE_LINE_SIZE]->param[0];
			GLFUNC(glLineWidth)((GLfloat)lineWidth);
			GLFUNC(glPolygonMode)(GL_FRONT_AND_BACK, GL_LINE);
			GLFUNC(glDisable)(GL_TEXTURE_2D);
		}
	}

	if (m_states[STATE_ALPHATEST])
	{
		if (m_states[STATE_ALPHATEST]->param[0] > 0)
		{
			if ((m_states[STATE_ALPHAFUNC]) && (m_states[STATE_ALPHAVALUE]))
			{
				unsigned int alphafunc = (m_states[STATE_ALPHAFUNC]->param[0]);
				float alphavalue = *(float*)(&m_states[STATE_ALPHAVALUE]->param[0]);

				g->stp.SetState(ST_ALPHATEST, true);
				g->stp.SetAlphaFunc(alphafunc, alphavalue);	
			}
		}
	}

	if (m_states[STATE_COLOR])
	{
		unsigned int* color = (unsigned int*)(&m_states[STATE_COLOR]->param[0]);
		if (m_states[STATE_OPACITY])
		{
			g->stp.SetColor((unsigned char)color[0], (unsigned char)color[1], (unsigned char)color[2], (unsigned char)m_states[STATE_OPACITY]->param[0]);			
		}
		else
		{			
			g->stp.SetColor((unsigned char)color[0], (unsigned char)color[1], (unsigned char)color[2]);
		}		
	}

	if (m_states[STATE_BLEND])
	{
		if (g->mr.GetMode() != MODE_RENDER_TRANSPARENT)
		{
			bool isBlendEnabled = (m_states[STATE_BLEND]->param[0] == 2);
			g->stp.SetState(ST_BLEND, isBlendEnabled);
		}

		if (m_states[STATE_BLEND_METHOD])
		{
			unsigned int blendMethod = m_states[STATE_BLEND_METHOD]->param[0];
			switch (blendMethod)
			{
			case 0:
				g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;

			case 1:
				g->stp.SetBlendFunc(GL_ONE, GL_ONE);
				break;

			case 2:
				g->stp.SetBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
				break;
			}
		}
		else
		{
			g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}

	if (m_states[STATE_Z_WRITE])
	{
		bool isZWriteEnabled = ((int)m_states[STATE_Z_WRITE]->param[0] == 2);
		g->stp.SetState(ST_ZWRITE, isZWriteEnabled);
	}

	int textureID = TEXTURE_NOT_FOUND;
	if (TEXTURE_SLOT_TEXTURE_2D == m_textureSlots[0].type)
	{
		textureID = ((STexture*)(m_textureSlots[0].content))->textureID;
	}
	if (TEXTURE_NOT_FOUND == textureID)
	{
		textureID = g->tm.GetCurrentTextureID();
	}
	CTexture* texture = g->tm.GetObjectPointer(textureID);

	if (texture->GetPassFrameCount() > 0)
	{
		
	}
	else
	{
		if (texture->GetDynamicTexture())
		{
			texture->SetCurrentFiltration(GL_LINEAR, GL_LINEAR, 1, 0);
		}
		else if ((m_minFilter > 0) && (m_magFilter > 0))
		{			
			texture->SetCurrentFiltration(m_minFilter, m_magFilter, m_anisotropy, m_mipmapBias);
		}
	}

	if (((m_states[STATE_TEXTURE_MATRIX]) && (m_states[STATE_TEXTURE_MATRIX]->param[0] == 2)) ||
		(m_isTextureMatrixChanged))
	{
		GLFUNC(glMatrixMode)(GL_TEXTURE);
		GLFUNC(glLoadIdentity)();
		if ((m_textureScale.x != 1.0f) && (m_textureScale.x > 0))
		{			
			GLFUNC(glTranslatef)(0.5f, 0.0f, 0.0f);
		}

		if ((m_textureScale.y != 1.0f) && (m_textureScale.y > 0))
		{			
			GLFUNC(glTranslatef)(0.0f, 0.5f, 0.0f);
		}

		GLFUNC(glScalef)(fabsf(m_textureScale.x), fabsf(m_textureScale.y), 1.0f);
		if ((m_textureScale.x != 1.0f) && (m_textureScale.x > 0))
		{
			GLFUNC(glTranslatef)(-0.5f, 0.0f, 0.0f);
		}

		if ((m_textureScale.y != 1.0f) && (m_textureScale.y > 0))
		{			
			GLFUNC(glTranslatef)(0.0f, -0.5f, 0.0f);
		}

		if ((m_isTextureMatrixChanged) && (m_textureScale.x > 0.0f))
		if ((m_textureScale.x != 1.0f) || (fabsf(m_textureScale.y) != 1.0f))
		{
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			GLfloat color[4];
			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;
			GLFUNC(glTexParameterfv)(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);
			m_isTextureMatrixChanged = false;

			CTexture* texture = g->tm.GetObjectPointer(g->tm.GetCurrentTextureID());
			if (texture->GetPassFrameCount() > 0)
			{
				m_isTextureMatrixChanged = true;
		}
		}
		
		GLFUNC(glMatrixMode)(GL_MODELVIEW);
	}
}

void CMaterial::UnbindMaterial()
{
	if (g->rs.GetBool(DISABLE_TEXTURES))
	{
		return;
	}

	if (m_states[STATE_TEXTURE_MATRIX])
	if (m_states[STATE_TEXTURE_MATRIX]->param[0] == 2)
	{
		GLFUNC(glMatrixMode)(GL_TEXTURE);
		GLFUNC(glLoadIdentity)();
		GLFUNC(glMatrixMode)(GL_MODELVIEW);
	}

	std::vector<STextureSlot *>::iterator it = m_usedTextureSlots.begin();
	std::vector<STextureSlot *>::iterator itEnd = m_usedTextureSlots.end();

	for ( ; it != itEnd; it++)
	{
		switch ((*it)->type)
		{
		case TEXTURE_SLOT_TEXTURE_2D:
			if ((*it)->slotID != 0)
			{
				bool isAlphaBlend = ((STexture*)m_textureSlots[(*it)->slotID].content)->isAlphaBlend;

				if ((g->rs.GetBool(SHADERS_USING)) || (isAlphaBlend))
				{
					g->stp.SetActiveTexture((unsigned char)(*it)->slotID);
					GLFUNC(glDisable)(GL_TEXTURE_2D);
				}
			}
			break;

		case TEXTURE_SLOT_TEXTURE_3D:
			if (g->rs.GetBool(SHADERS_USING))
			{
				g->stp.SetActiveTexture((unsigned char)(*it)->slotID);
				GLFUNC(glDisable)(GL_TEXTURE_3D);
			}
			break;
 
		case TEXTURE_SLOT_TEXTURE_CUBEMAP:
			if (g->rs.GetBool(SHADERS_USING))
			{
				g->stp.SetActiveTexture((unsigned char)(*it)->slotID);
				GLFUNC(glDisable)(GL_TEXTURE_CUBE_MAP);
			}
			break;

		case TEXTURE_SLOT_ANIMATION:
			if ((*it)->slotID != 0)
			{
				g->stp.SetActiveTexture((unsigned char)(*it)->slotID);
				GLFUNC(glDisable)(GL_TEXTURE_2D);
			}
			break;
		}
	}

	g->stp.SetActiveTexture(0);

	if (m_states[STATE_COLOR])
	{
		g->stp.SetColor(255, 255, 255);
	}

	if (m_states[STATE_ALPHATEST])
	{
		g->stp.SetState(ST_ALPHATEST, false);
	}

	if (m_states[STATE_Z_WRITE])
	{
		g->stp.SetState(ST_ZWRITE, true);
	}

	if (m_states[STATE_BLEND])
	{
		if (m_states[STATE_BLEND_METHOD])
		{
			if (g->mr.GetMode() != MODE_RENDER_TRANSPARENT)
			{
				g->stp.SetState(ST_BLEND, false);
			}

			g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}	
	}

	if (m_states[STATE_LINE_SIZE])
	{
		if (m_states[STATE_LINE_SIZE]->param[0] > 0)
		{
			GLFUNC(glEnable)(GL_TEXTURE_2D);
			GLFUNC(glPolygonMode)(GL_FRONT_AND_BACK, GL_FILL);
}
	}
}

void CMaterial::UpdateUsedTextureSlots()	
{
	m_usedTextureSlots.clear();
	for (int k = 0; k < 8; k++)
	if (m_textureSlots[k].type != TEXTURE_SLOT_EMPTY)
	{
		m_usedTextureSlots.push_back(&m_textureSlots[k]);
	}
}

void CMaterial::UpdateRenderingMode()
{
	if (m_states[STATE_TREAT_AS_ALPHATEST])
	{
		m_renderMode = MODE_RENDER_ALPHATEST;
		return;
	}

	if (m_states[STATE_ALPHATEST])
	{
		bool isAlphaTestEnabled = (m_states[STATE_ALPHATEST]->param[0] > 0);
		if (isAlphaTestEnabled)
		{
			m_renderMode = MODE_RENDER_ALPHATEST;
			return;
		}
	}

	if (m_states[STATE_BLEND])
	{
		bool isBlendEnabled = (m_states[STATE_BLEND]->param[0] == 2);
		if (!isBlendEnabled)
		{
			m_renderMode = MODE_RENDER_SOLID;
			return;
		}
		else
		{
			m_renderMode = MODE_RENDER_TRANSPARENT;
			return;
		}
	}

	bool isTexturesFound = false;

	if (TEXTURE_SLOT_TEXTURE_2D == m_textureSlots[0].type)
	{
		int textureID = ((STexture*)(m_textureSlots[0].content))->textureID;
		bool isAlphaBlend = ((STexture*)(m_textureSlots[0].content))->isAlphaBlend;

		if ((TEXTURE_NOT_FOUND != textureID) && (!isAlphaBlend))
		{
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			if (texture->IsTransparent())
			{
				m_renderMode = MODE_RENDER_TRANSPARENT;
				return;
			}
	
			isTexturesFound = true;	
		}
	}

	m_renderMode = (isTexturesFound) ? MODE_RENDER_SOLID : MODE_RENDER_UNKNOWN;
}

int CMaterial::GetCorrespondingRenderingMode()const
{
	return m_renderMode;
}

bool CMaterial::IsTwoSidedTransparencyNeeded()const
{
	if (m_states[STATE_TWO_SIDED_TRANSPARENCY])
	{
		return (m_states[STATE_TWO_SIDED_TRANSPARENCY]->param[0] == 2);
	}
	else
	{
		return false;
	}
}

bool CMaterial::IsTwoSidedNeeded()const
{
	if (m_states[STATE_TWO_SIDED])
	{
		return (m_states[STATE_TWO_SIDED]->param[0] == 2);
	}
	else
	{
		return false;
	}
}

bool CMaterial::IsTangentsNeeded()const
{
	if (m_states[STATE_NEED_TANGENTS])
	{
		return (m_states[STATE_NEED_TANGENTS]->param[0] == 2);
	}
	else
	{	
		return false;
	}
}

int CMaterial::GetBillboardOrient()const
{
	if (m_states[STATE_BILLBOARD_ORIENT])
	{
		return m_states[STATE_BILLBOARD_ORIENT]->param[0];
	}
	else
	{
		return 0;
	}
}

void CMaterial::EnableTextureMatrix(const CVector2D& textureScale)
{
	m_states[STATE_TEXTURE_MATRIX] = MP_NEW(SParam);
	m_states[STATE_TEXTURE_MATRIX]->param[0] = 2;
	m_textureScale = textureScale;
	m_isTextureMatrixChanged = true;
}

void CMaterial::DisableTextureMatrix()
{
	if (m_states[STATE_TEXTURE_MATRIX])
	{
		MP_DELETE(m_states[STATE_TEXTURE_MATRIX]);
		m_states[STATE_TEXTURE_MATRIX] = NULL;		
	}

	m_textureScale.Set(1.0f, 1.0f);
	m_isTextureMatrixChanged = true;
}

CVector2D CMaterial::GetTextureMatrixKoefs()
{
	return m_textureScale;
}

void CMaterial::ClearShaderLocations()
{
	std::vector<SParam*>::iterator it = m_shaderParams.begin();
	std::vector<SParam*>::iterator itEnd = m_shaderParams.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->location = -1;
	}

	MP_MAP<MP_STRING, SParam*>::iterator iter = m_updateOnceShaderParamsMap.begin();
	MP_MAP<MP_STRING, SParam*>::iterator iterEnd = m_updateOnceShaderParamsMap.end();

	for ( ; iter != iterEnd; iter++)
	{
		(*iter).second->location = -1;
		m_shaderParams.push_back((*iter).second);
		m_shaderParamsMap.insert(MP_MAP<MP_STRING, SParam*>::value_type((*iter).first, (*iter).second));
	}

	m_updateOnceShaderParamsMap.clear();

	m_isShaderParamsOptimized = false;
}

void CMaterial::OnMaterialQualityChanged() 
{
	if (m_data)
	{
		m_isQualityChanged = true;
		Init();
		Clear();
		LoadFromXML(m_sourcePath, m_data, false);
		m_isQualityChanged = false;
	}
}

void CMaterial::FreeResources()
{
	ClearListeners();
	DisableTextureMatrix();

	std::vector<SParam*>::iterator iter = m_shaderParams.begin();

	for ( ; iter != m_shaderParams.end(); iter++)
	{
		if (*iter)
		{
			MP_DELETE(*iter);
		}
	}

	m_shaderParamsMap.clear();
	m_shaderParams.clear();

	int i;
	for (i = 0; i < 8; i++)
	{
		SetTextureLevel(i, TEXTURE_SLOT_EMPTY, NULL);
	}

	for (i = 0; i < STATE_COUNT; i++)
	if (m_states[i])
	{
		MP_DELETE(m_states[i]);
		m_states[i] = NULL;
	}

	std::vector<SAsynchTexture *>::iterator it = m_notFoundTextures.begin();
	std::vector<SAsynchTexture *>::iterator itEnd = m_notFoundTextures.end();

	for ( ; it != itEnd; it++)
	{
		MP_DELETE(*it);
	}

	{
		MP_MAP<MP_STRING, SParam*>::iterator it = m_updateOnceShaderParamsMap.begin();
		MP_MAP<MP_STRING, SParam*>::iterator itEnd = m_updateOnceShaderParamsMap.end();

		for ( ; it != itEnd; it++)
		{
			MP_DELETE((*it).second);
		}
	}

	m_notFoundTextures.clear();

	if (m_data)
	{
		MP_DELETE_ARR(m_data);
		m_data = NULL;
	}

	m_usedTextureSlots.clear();
}

CMaterial::~CMaterial()
{
	FreeResources();
}