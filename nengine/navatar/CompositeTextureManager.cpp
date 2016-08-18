#include "StdAfx.h"
#include <Assert.h>
#include <glew.h>
#include <TinyXML.h>
#include <sys/stat.h>
#include "FrameBuffer.h"
#include "Commands.h"
#include "HelperFunctions.h"
#include "PlatformDependent.h"
#include "CompositeTextureManager.h"
#include "AddNCacheDataTask.h"
#include "DXTCompressor.h"

_SCompositeTexture::_SCompositeTexture()
{
	for (int m = 0; m < 4; m++)
	{
		MP_STRING_DC_INIT(fileNames[m]);
	}

	srcTextureID = -1;
	resultTextureID = -1;
	levelCount = 0;
	isAddEnabled = false;
	isUpdated = false;
	head = 1;

	for (int i = 0; i < MAX_LEVELS; i++)
	{
		colors[i] = 0xFFFFFFFF;
		alphaRefs[i] = 0;
		sprites[i] = NULL;
		texturePosX[i] = 0;
		texturePosY[i] = 0;
		texturePosYNeedInvert[i] = false;
	}
}

_SCompositeTexture::_SCompositeTexture(const _SCompositeTexture& tmp)
{
	for (int m = 0; m < 4; m++)
	{
		MP_STRING_DC_INIT(fileNames[m]);
	}

	srcTextureID = tmp.srcTextureID;
	resultTextureID = tmp.resultTextureID;
	levelCount = tmp.levelCount;
	isAddEnabled = tmp.isAddEnabled;
	isUpdated = tmp.isUpdated;
	head = tmp.head;

	for (int i = 0; i < MAX_LEVELS; i++)
	{
		colors[i] = tmp.colors[i];
		sprites[i] = tmp.sprites[i];
		alphaRefs[i] = tmp.alphaRefs[i];
		alphaFuncs[i] = tmp.alphaFuncs[i];
		texturePosX[i] = tmp.texturePosX[i];
		texturePosY[i] = tmp.texturePosY[i];
		texturePosYNeedInvert[i] = tmp.texturePosYNeedInvert[i];
	}

	for (int k = 0; k < 4; k++)
	{
		fileNames[k] = tmp.fileNames[k];
	}

	for (int x = 0; x < 4; x++)
	for (int y = 0; y < 3; y++)
	{
		skinColors[x][y] = tmp.skinColors[x][y];
	}
}

CCompositeTextureManager::CCompositeTextureManager():
	m_headID(1),
	m_obj3d(NULL),
	m_lastHeadID(0),
	m_cacheTask(NULL),
	m_isMyAvatar(false),
	m_isCompressed(false),
	m_isUpdatePermitted(true),
	m_isEditorVisible(false),
	MP_VECTOR_INIT(m_compressingImageTasks),
	MP_WSTRING_INIT(m_name),
	MP_STRING_INIT(m_backgroundTextureName),
	MP_VECTOR_INIT(m_textures),
	MP_STRING_INIT(m_lastStr),
	MP_MAP_INIT(m_texturesByID)
{	
	for (int m = 0; m < MAX_HEADS; m++)
	{
		MP_STRING_DC_INIT(m_str[m]);
	}

	m_renderImpls[0] = MP_NEW(CGPUCompositeTextureManager);
	m_renderImpls[0]->SetMainManager(this);
	m_renderImpls[1] = MP_NEW(CCPUCompositeTextureManager);
	m_renderImpls[1]->SetMainManager(this);
	m_currentRenderImplID = 0;

	for (int i = 0; i < MAX_LEVELS; i++)
	for (int k = 0; k < 2; k++)
	{
		m_timeForLevel[i][k] = 0;
	}

	m_currentRenderImplID = g->cc.IsTextureRenderingSupported() ? 0 : 1;

	static bool isModeLogged = false;
	if (!isModeLogged)
	{
		g->lw.WriteLn("Render mode choosed: ", m_currentRenderImplID);
		isModeLogged = true;
	}
}

void CCompositeTextureManager::SetSpecialDiffuseKoef(int textureID, float koef)
{
	if ((textureID < 0) || ((unsigned int)textureID >= m_textures.size()))
	{
		return;
	}

	int startColorNum  = (int)(koef - fmodf(koef, 1.0f));
	float koef2 = fmodf(koef, 1.0f);
	float koef1 = 1.0f - koef2;	

	int color[3];
	for (int i = 0; i < 3; i++)
	{
		color[i] = (int)(m_textures[textureID].skinColors[startColorNum][i] * koef1 + m_textures[textureID].skinColors[(startColorNum + 1)%4][i] * koef2);
	}
	DWORD colorWord = 0xFF000000 + (color[2] << 16) + (color[1] << 8) + color[0];

	SetDiffuse(textureID, 0, colorWord);
}

bool CCompositeTextureManager::IsNeeded()
{
	if (!m_obj3d)
	{
		return true;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if (sao)
	if ((sao->GetCompatabilityID() < 0) || (sao->GetCompatabilityID() >= 20))
	{
		return false;
	}

	return true;
}

bool CCompositeTextureManager::LoadCompositeTextureStrings(std::string fileName, CCommandCache* commandCache)
{
	TiXmlDocument doc;
		
	ifile* file = g->rl->GetResMemFile(fileName.c_str());
	if (file)
	{
		unsigned char* data = MP_NEW_ARR(unsigned char, file->get_file_size() + 1);
		data[file->get_file_size()] = 0;
		file->read(data, file->get_file_size());
		doc.Parse((const char*)data);
		g->rl->DeleteResFile(file);

		MP_DELETE_ARR(data);
	}
	else
	{
		return false;
	}
	
	TiXmlNode* avatar = doc.FirstChild("avatar");
	if (!avatar)
	{
		return false;
	}

	std::string basePath = "";
	if (avatar->ToElement()->Attribute("basePath"))
	{
		basePath = avatar->ToElement()->Attribute("basePath");
	}
	
	TiXmlNode* composites = avatar->FirstChild("composites");
	if (!composites)
	{
		return false;
	}

	TiXmlNode* compositeTexture = composites->FirstChild("composite");

	commandCache->ClearAll();
	
	while (compositeTexture != NULL)
	{
		std::string createString = "";

		if ((!compositeTexture->ToElement()->Attribute("src")) || (!compositeTexture->ToElement()->Attribute("name")))
		{
			return false;
		}

		std::string src = compositeTexture->ToElement()->Attribute("src");
		std::string name = compositeTexture->ToElement()->Attribute("name");
		std::string head = "1";
		
		if (compositeTexture->ToElement()->Attribute("head"))
		{
			head = compositeTexture->ToElement()->Attribute("head");
		}

		createString += name;
		createString += "*";
		createString += src;
		createString += "*";

		TiXmlNode* composite = compositeTexture->FirstChild("composite");	

		while (composite != NULL)
		{
			if ((!composite->ToElement()->Attribute("src")) || (!composite->ToElement()->Attribute("level")))
			{
				return false;
			}

			std::string _src = basePath + composite->ToElement()->Attribute("src");
			std::string _level = composite->ToElement()->Attribute("level");
			std::string _diffuse = "";
			std::string _alpharef = "";
			std::string _posX = "";
			std::string _posY = "";

			if (composite->ToElement()->Attribute("diffuse"))
			{
				_diffuse = composite->ToElement()->Attribute("diffuse");
			}

			if (composite->ToElement()->Attribute("alpharef"))
			{
				_alpharef = composite->ToElement()->Attribute("alpharef");
			}

			if (composite->ToElement()->Attribute("posx"))
			{
				_posX = composite->ToElement()->Attribute("posx");
			}

			if (composite->ToElement()->Attribute("posy"))
			{
				_posY = composite->ToElement()->Attribute("posy");
			}

			createString += _src;
			createString += "*";
			createString += _level;
			createString += "*";
			createString += _diffuse;
			createString += "*";
			createString += _alpharef;
			createString += "*";
			createString += _posX;
			createString += "*";
			createString += _posY;
			createString += "*";

			composite = compositeTexture->IterateChildren("composite", composite);
		}

		std::string colorString = "";

		TiXmlNode* color = compositeTexture->FirstChild("color");	
		while (color != NULL)
		{
			unsigned char r = 255, g = 255, b = 255;

			if (color->ToElement()->Attribute("r"))
			{
				r = (unsigned char)rtl_atoi(color->ToElement()->Attribute("r"));
			}

			if (color->ToElement()->Attribute("g"))
			{
				g = (unsigned char)rtl_atoi(color->ToElement()->Attribute("g"));
			}

			if (color->ToElement()->Attribute("b"))
			{
				b = (unsigned char)rtl_atoi(color->ToElement()->Attribute("b"));
			}

			colorString += IntToStr(r);
			colorString += "&";
			colorString += IntToStr(g);
			colorString += "&";
			colorString += IntToStr(b);
			colorString += "&";

			color = compositeTexture->IterateChildren("color", color);		
		}

		if (colorString == "")
		for (int k = 0; k < 9; k++)
		{
			colorString += "255*";
		}
		
		createString = colorString + createString;

		compositeTexture = composites->IterateChildren("composite", compositeTexture);				

		commandCache->CacheCreateCommand(head, createString);
	}

	return true;
}

bool CCompositeTextureManager::IsUpdated()
{
	if (m_textures.size() > 0)
	{
		return m_textures[0].isUpdated;
	}
	else
	{
		return true;
	}
}
bool CCompositeTextureManager::IsLoaded()
{
	for (unsigned int textureID = 0; textureID < m_textures.size(); textureID++)
	for (int levelID = 0; levelID < 2; levelID++)
	{
		if (m_textures[textureID].sprites[levelID])
		if (!m_textures[textureID].sprites[levelID]->IsLoaded())
		{
			return false;
		}
	}

	return true;
}

void CCompositeTextureManager::SetHeadID(C3DObject* obj3d, int headID)
{
	if (headID != m_headID)
	{
		if (m_textures.size() > 0)
		for (int levelID = 0; levelID < MAX_LEVELS; levelID++)
		{
			if (m_textures[0].sprites[levelID])
			{
				//delete m_textures[0].sprites[levelID];
				m_textures[0].sprites[levelID] = NULL;
			}
		}
	}

	bool isNeedSaveOptions = (m_textures.size() != 0);
	if (isNeedSaveOptions)
	{
		SaveOptions(0);
	}

	UpdateLinksToCompositeTextures(obj3d, false);
	m_headID = headID;
	AddCompositeTexture(headID, m_str[headID]);

	if (isNeedSaveOptions)
	{
		RestoreOptions(0);
	}

	UpdateTexture(0);
}

int  CCompositeTextureManager::GetHeadID()const
{
	return m_headID;
}

void CCompositeTextureManager::SetMyAvatar(bool isMyAvatar)
{
	m_isMyAvatar = isMyAvatar;
}

bool CCompositeTextureManager::IsMyAvatar()
{
	return m_isMyAvatar;
}

void CCompositeTextureManager::SetEditorVisible(bool isEditorVisible)
{
	m_isEditorVisible = isEditorVisible;
}

void CCompositeTextureManager::SetUpdatePermission(bool isPermitted)
{
	m_isUpdatePermitted = isPermitted;
}

void CCompositeTextureManager::UpdateLinksToCompositeTextures(C3DObject* obj3d, bool isLink)
{
	if (!IsNeeded())
	{
		return;
	}

	assert(obj3d);
	if (!obj3d)
	{
		return;
	}

	if (m_textures.size() == 0)
	{
		return;
	}
	
	if (!obj3d->GetAnimation())
	{
		return;
	}
	assert(obj3d->GetAnimation());

	CModel* model = obj3d->GetAnimation()->GetModel();
	assert(model);
	if (!model)
	{
		return;
	}

	int textureCount = model->GetTextureCount();
	for (int i = 0; i < textureCount - 1; i++)
	{
		int textureID = model->GetTextureNumber(i);

		if (-1 == textureID)
		{
			continue;
		}

		if (!m_textures.empty())
		{
			std::vector<SCompositeTexture>::iterator it = m_textures.begin();
			std::vector<SCompositeTexture>::iterator itEnd = (m_textures.size() > 0) ? (m_textures.begin() + 1) : m_textures.end();			

			for ( ; it != itEnd; it++)
			{
				if (isLink)
				{
					if (i == 0)					
					{						
						model->SetTextureNumber(i, ((SCompositeTexture)(*it)).resultTextureID);						
					}
				}
				else
				{
					if (i == 0)					
					{
						model->SetTextureNumber(i, ((SCompositeTexture)(*it)).srcTextureID);
					}
				}
			}
		}
	}
}

void CCompositeTextureManager::UpdateLinksToCompositeTextures(C3DObject* pObj, std::vector<SAvatarObjPart>& avatarParts)
{
	if (!IsNeeded())
	{
		return;
	}

	assert(pObj);

	m_name = avatarParts[0].obj3d->GetName();
	m_obj3d = avatarParts[0].obj3d;

	UpdateLinksToCompositeTextures(pObj);
}


void CCompositeTextureManager::EnableAddMode(unsigned int textureID)
{
	if (textureID < m_textures.size())
	{
		m_textures[textureID].isAddEnabled = true;
	}
}

CSprite* CCompositeTextureManager::LoadSprite(std::string fileName, int level)
{
	// определение даты модификации файла		
	__int64 time = 0;
	USES_CONVERSION;
	g->rl->GetAsynchResMan()->GetModifyTime( A2W(fileName.c_str()), time, true);

	/*CComString fileName2 = fileName.c_str();
	fileName2.Replace(":", "\\");
	std::string path = GetApplicationRootDirectory();
	path += "\\";
	path += fileName2;

	struct _stat buf;
	_stat(path.c_str(), &buf);
	__int64 time = buf.st_mtime;*/

	m_timeForLevel[level][0] = *(unsigned int*)&time;
	m_timeForLevel[level][1] = *((unsigned int*)&time + 1);
	//

	CSprite* sprite = MP_NEW(CSprite);
	// we must use compressed textures only for skin
	if ((level >= 0) && (level <= 3))
	{
		//sprite->SetCanBeCompressed(true);
	}
	if (m_currentRenderImplID == 1)
	{
		sprite->KeepInSystemMemory(true);
	}

	if (sprite->LoadFromTexture(fileName))
	{
		if (m_currentRenderImplID == 1)
		{
			CTexture* texture = g->tm.GetObjectPointer(sprite->GetTextureID(0));
			texture->PremultiplyAlpha();
		}
		return sprite;
	}
	else
	{
		MP_DELETE(sprite);
		return NULL;
	}

	return NULL;
}

int CCompositeTextureManager::GetNextStringPart(int initialPos, std::string str, std::string& result)
{
	result = "";

	while (((unsigned int)initialPos < str.length()) && (str[initialPos] != '*'))
	{
		result += str[initialPos];
		initialPos++;
	}

	initialPos++;

	return initialPos;
}

int CCompositeTextureManager::GetNextColorPart(int initialPos, std::string str, std::string& result)
{
	result = "";

	while (((unsigned int)initialPos < str.length()) && (str[initialPos] != '&'))
	{
		result += str[initialPos];
		initialPos++;
	}

	initialPos++;

	return initialPos;
}

DWORD CCompositeTextureManager::GetDiffuseFromString(std::string str)
{
	if (str.length() != 9)
	{
		return 0xFFFFFFFF;
	}
	else
	{
		if (str[0] != '#')
		{
			return 0xFFFFFFFF;
		}

		return HexStringToDec(str.c_str() + 1);
	}
}

int CCompositeTextureManager::AddCompositeTexture(int headID, std::string str)
{
	m_str[headID] = MAKE_MP_STRING(str);
	if (headID != m_headID)
	{
		return -1;
	}

	std::string src, name, clrPart;
	int maxLevel = ERROR_VALUE;
	int levelCount = 0;

	int pos = 0;
	unsigned char rawColors[9];

	for (int l = 0; l < 9; l++)
	{
		pos = GetNextColorPart(pos, str, clrPart);
		rawColors[l] = (unsigned char)rtl_atoi(clrPart.c_str());
	}

	pos = GetNextStringPart(pos, str, name);
	pos = GetNextStringPart(pos, str, src);

	int currentCompositeIndex = GetCompositeTextureID(name);

#ifdef AVED_LOGGING
	g->lw.WriteLn("CCompositeTextureManager::AddCompositeTexture str == ", str.GetBuffer());
	g->lw.WriteLn("CCompositeTextureManager::AddCompositeTexture GetCompositeTextureID == ", currentCompositeIndex);
#endif

	if ((currentCompositeIndex != ERROR_VALUE) && (str == m_lastStr) && (m_lastHeadID == m_headID))
		return currentCompositeIndex;

	if (m_textures.size() > 0)
	{
		DeleteAllCompositeTextures();
		m_headID = headID;
	}

	m_lastHeadID = m_headID;

	m_lastStr = str;

	std::string texturesSrc[MAX_LEVELS];
	std::string texturesDiffuse[MAX_LEVELS];
	std::string texturesAlpharef[MAX_LEVELS];
	std::string texturesPosX[MAX_LEVELS];
	std::string texturesPosY[MAX_LEVELS];
	
	if ((src.empty()) || (name.empty()))
	{
		return ERROR_VALUE;
	}

	bool end = false;
	while (!end)
	{
		std::string tmpSrc;
		std::string tmpLevel;
		std::string tmpDiffuse;
		std::string tmpAlpharef;
		std::string tmpPosX;
		std::string tmpPosY;

		pos = GetNextStringPart(pos, str, tmpSrc);
		pos = GetNextStringPart(pos, str, tmpLevel);
		pos = GetNextStringPart(pos, str, tmpDiffuse);
		pos = GetNextStringPart(pos, str, tmpAlpharef);
		pos = GetNextStringPart(pos, str, tmpPosX);
		pos = GetNextStringPart(pos, str, tmpPosY);

		if ((tmpSrc.empty()) || (tmpLevel.empty()))
		{
			end = true;
			continue;
		}
		else
		{
			levelCount++;
		}

		int level = rtl_atoi(tmpLevel.c_str());

		texturesSrc[level] = tmpSrc;
		texturesDiffuse[level] = tmpDiffuse;
		texturesAlpharef[level] = tmpAlpharef;
		texturesPosX[level] = tmpPosX;
		texturesPosY[level] = tmpPosY;

		if (maxLevel < level)
		{
			maxLevel = level;
		}
	}

	if (maxLevel < 1)
	{
#ifdef AVED_LOGGING
		g->lw.WriteLn("[ERROR]CCompositeTextureManager::AddCompositeTexture maxLevel < 1,  maxLevel == ",maxLevel);
#endif
		return ERROR_VALUE;
	}

	int textureID = AddCompositeTexture(name, src, texturesSrc[0], texturesSrc[1], m_headID);

	if (-1 == textureID)
	{
#ifdef AVED_LOGGING
		g->lw.WriteLn("[ERROR]CCompositeTextureManager::AddCompositeTexture textureID == -1  texture321sSrc[0] == ",texturesSrc[0]," texturesSrc[1] == ",texturesSrc[1]);
#endif
		return -1;
	}	

	for (int i = 2; i <= maxLevel; i++)	
	{
		AddLevelToCompositeTexture(textureID, texturesSrc[i]);
	}

	m_textures[textureID].sprites[0] = NULL;
	m_textures[textureID].sprites[1] = NULL;

	if (!texturesDiffuse[0].empty())
	{
		SetDiffuse(textureID, 0, GetDiffuseFromString(texturesDiffuse[0]));
	}

	bool isUpdateNeeded = false;

	for (int i = 1; i <= maxLevel; i++)
	{
		if (!texturesDiffuse[i].empty())
		{
			SetDiffuse(textureID, i, GetDiffuseFromString(texturesDiffuse[i]));
			isUpdateNeeded = true;
		}
		
		if (!texturesAlpharef[i].empty())
		{
			SetAlphaRef(textureID, i, (BYTE)rtl_atoi(texturesAlpharef[i].c_str()));
			isUpdateNeeded = true;
		}
		
		if (!texturesPosX[i].empty())
		{
			SetTexturePosX(textureID, i, rtl_atoi(texturesPosX[i].c_str()));
			isUpdateNeeded = true;
		}

		if (!texturesPosY[i].empty())
		{
			bool isNeedInvert = false;
			if (texturesPosY[i][0] == '$')
			{
				isNeedInvert = true;				
			}
			SetTexturePosY(textureID, i, rtl_atoi(texturesPosY[i].c_str() + (int)isNeedInvert), isNeedInvert);
			isUpdateNeeded = true; 
		}
	}

	EnableAddMode(textureID);

	if (isUpdateNeeded)
	{
		UpdateTexture(textureID);
	}

	SetSkinColors(textureID, (unsigned char *)&rawColors);

	m_texturesByID.insert(TextureList::value_type(MAKE_MP_STRING(name), textureID));

#ifdef AVED_LOGGING
	g->lw.WriteLn("[ERROR]CCompositeTextureManager::AddCompositeTexture str ",name.GetBuffer()," textureID = ",textureID);
#endif

	return textureID;
}

int CCompositeTextureManager::GetCompositeTextureID(std::string name)
{
#ifdef AVED_LOGGING
	g->lw.WriteLn("GetCompositeTextureID ", name.GetBuffer(0));
#endif

	TextureList::iterator iter;

	iter = m_texturesByID.find(MAKE_MP_STRING(name));

	if (iter != m_texturesByID.end())
	{
#ifdef AVED_LOGGING
		g->lw.WriteLn("return ", (*iter).second);
#endif
		return (*iter).second;
	}
	else
	{
#ifdef AVED_LOGGING
		g->lw.WriteLn("return -1");
#endif
		return ERROR_VALUE;
	}
}

int CCompositeTextureManager::GetNamedCompositeIndex(std::string& resultTextureName)
{
	int srcTextureID = g->tm.GetObjectNumber(resultTextureName);

	for (unsigned int i = 0; i < m_textures.size(); i++)
	if (m_textures[i].srcTextureID == srcTextureID)
	{
		return i;
	}

	return -1;
}

bool CCompositeTextureManager::ExistsNamedComposite(std::string& resultTextureName)
{
	return (GetNamedCompositeIndex(resultTextureName) != -1);
}

int CCompositeTextureManager::AddCompositeTexture(std::string name, std::string resultTextureName, std::string backgroundTextureName, std::string secondTextureName, int headID)
{
	SCompositeTexture tmp;
	tmp.srcTextureID = g->tm.GetObjectNumber(resultTextureName);

	if (-1 == tmp.srcTextureID)
	{
		return -1;
	}

	CTexture* texture = g->tm.GetObjectPointer(tmp.srcTextureID);

	bool isTransparent = texture->IsTransparent();
	int format = isTransparent ? GL_RGBA : GL_RGB;
	int internalFormat = isTransparent ? GL_RGBA : GL_RGB8;

	int textureID = CFrameBuffer::CreateColorTexture(GL_TEXTURE_2D, texture->GetTextureWidth(), texture->GetTextureHeight(), 
		format, internalFormat, true);

	tmp.resultTextureID = g->tm.ManageGLTexture("composite", textureID, texture->GetTextureWidth(), texture->GetTextureHeight());
	CTexture* _texture = g->tm.GetObjectPointer(tmp.resultTextureID);
	_texture->UpdateCurrentFiltration(GL_LINEAR, GL_LINEAR);
	
	m_backgroundTextureName = backgroundTextureName;
 	tmp.sprites[0] = /*LoadSprite(backgroundTextureName, 0)*/NULL;
	tmp.fileNames[0] = backgroundTextureName;
	tmp.sprites[1] = /*LoadSprite(secondTextureName, 1)*/NULL;
	tmp.fileNames[1] = secondTextureName;
	tmp.levelCount = 2;
	tmp.head = headID;

	for (int k = 0; k < MAX_LEVELS; k++)
	{
		tmp.alphaFuncs[k] = GL_GREATER;
	}

	if (GetCompositeTextureID(name) == -1)
	{
		m_textures.push_back(tmp);
	}
	else
	{
		m_textures[0] = tmp;
	}

	return (m_textures.size() - 1);
}

void CCompositeTextureManager::AddLevelToCompositeTexture(unsigned int textureID, std::string fileName)
{
	if (textureID >= m_textures.size())	
	{
		return;
	}

	if (m_textures[textureID].levelCount >= MAX_LEVELS)
	{
		return;
	}

	if (m_textures[textureID].sprites[m_textures[textureID].levelCount])
	{
		MP_DELETE(m_textures[textureID].sprites[m_textures[textureID].levelCount]);
		m_textures[textureID].sprites[m_textures[textureID].levelCount] = NULL;
	}

	if (m_textures[textureID].levelCount <= 3)
	{
        m_textures[textureID].sprites[m_textures[textureID].levelCount] = NULL;
		m_textures[textureID].fileNames[m_textures[textureID].levelCount] = fileName;
	}
	else
	{
		m_textures[textureID].sprites[m_textures[textureID].levelCount] = LoadSprite(fileName, m_textures[textureID].levelCount);
	}
	m_textures[textureID].levelCount++;
}

void CCompositeTextureManager::ReplaceLevelInCompositeTexture(unsigned int textureID, int level, std::string fileName)
{
	if (textureID >= m_textures.size())	
	{
		return;
	}

	if (level >= MAX_LEVELS)
	{
		return;
	}

	if (m_textures[textureID].sprites[level])
	{
		MP_DELETE(m_textures[textureID].sprites[level]);
		m_textures[textureID].sprites[level] = NULL;
	}

	m_textures[textureID].sprites[level] = LoadSprite(fileName, level);
}

void CCompositeTextureManager::CopyTextureFromLevel(unsigned int textureID, int levelTo, int levelFrom)
{
	if (textureID >= m_textures.size())	
	{
		return;
	}

	if (m_textures[textureID].levelCount <= levelTo)	
	{
		return;
	}

	if (m_textures[textureID].levelCount <= levelFrom)	
	{
		return;
	}

	m_textures[textureID].sprites[levelTo] = m_textures[textureID].sprites[levelFrom];
}

void CCompositeTextureManager::SetSkinColors(unsigned int textureID, unsigned char* colors)
{
	if (textureID >= m_textures.size())	
	{
		return;
	}

	m_textures[textureID].skinColors[0][0] = 255;
	m_textures[textureID].skinColors[0][1] = 255;
	m_textures[textureID].skinColors[0][2] = 255;
	m_textures[textureID].skinColors[1][0] = colors[0];
	m_textures[textureID].skinColors[1][1] = colors[1];
	m_textures[textureID].skinColors[1][2] = colors[2];
	m_textures[textureID].skinColors[2][0] = colors[3];
	m_textures[textureID].skinColors[2][1] = colors[4];
	m_textures[textureID].skinColors[2][2] = colors[5];
	m_textures[textureID].skinColors[3][0] = colors[6];
	m_textures[textureID].skinColors[3][1] = colors[7];
	m_textures[textureID].skinColors[3][2] = colors[8];
}

void CCompositeTextureManager::SetDiffuse(unsigned int textureID, int nPass, DWORD color)
{
	if (textureID >= m_textures.size())	
	{
		return;
	}

	if (nPass >= m_textures[textureID].levelCount)	
	{
		return;
	}

	m_textures[textureID].colors[nPass] = color;
}

void CCompositeTextureManager::SetTexturePosX(unsigned int uitextureID, int iLevel, int uipos)
{
	if (m_textures.size() <= uitextureID)
	{
		return;
	}

	m_textures[uitextureID].texturePosX[iLevel] = uipos;
}

void CCompositeTextureManager::SetTexturePosY(unsigned int uitextureID, int iLevel, int uipos, bool isNeedInvert)
{
	if (m_textures.size() <= uitextureID)
	{
		return;
	}

	m_textures[uitextureID].texturePosY[iLevel] = uipos;
	m_textures[uitextureID].texturePosYNeedInvert[iLevel] = isNeedInvert;
}

DWORD CCompositeTextureManager::GetDiffuse(unsigned int textureID, int nLevel)
{
	if (m_textures.size() <= textureID)
	{

		return 0;
	}

	return m_textures[textureID].colors[nLevel];
}

void CCompositeTextureManager::SetAlphaRef(unsigned int textureID, int nPass, BYTE alphaRef)
{
	m_textures[textureID].alphaRefs[nPass] = alphaRef;
}

void CCompositeTextureManager::SetAlphaFunc(unsigned int textureID, int nPass, BYTE alphaFunc)
{
	if (m_textures.size() <= textureID)
	{
		return;
	}

	m_textures[textureID].alphaFuncs[nPass] = alphaFunc;
}

std::string CCompositeTextureManager::GetCacheFileName(int textureID)
{
	if (m_textures.size() <= (unsigned int)textureID)
	{
		return "";
	}

	std::string cacheFileName = "av";
	cacheFileName += IntToStr(m_headID);
	cacheFileName += "_";
	cacheFileName += m_backgroundTextureName;
	cacheFileName += "_";
	unsigned int time = 0;
	for (int i = 0; i < m_textures[textureID].levelCount; i++)
	{
		time += m_timeForLevel[i][0];
		time += m_timeForLevel[i][1];
	}
	cacheFileName += IntToStr(time);
	cacheFileName += "_";
	for (int i = 0; i < m_textures[textureID].levelCount; i++)
	{
		int tmp = m_textures[textureID].colors[i];
		cacheFileName += "_";
		cacheFileName += IntToStr(tmp);

		if (i > 3)
		{
			cacheFileName += "_";
			cacheFileName += IntToStr(m_textures[textureID].alphaRefs[i]);
		}
	}

	StringReplace(cacheFileName, "\\", "_");
	StringReplace(cacheFileName, ":", "_");

	return cacheFileName;
}

CReadingCacheFileTask* CCompositeTextureManager::GetReadingCacheTask()
{
	return m_cacheTask;
}

void CCompositeTextureManager::SetReadingCacheTask(CReadingCacheFileTask* task)
{	
	m_cacheTask = task;
}

void CCompositeTextureManager::ClearCompressedTexture(CTexture* texture)
{
	if ((m_isCompressed) && (m_currentRenderImplID != 1))
	{		
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());		
		GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, texture->IsTransparent() ? GL_RGBA : GL_RGB, 
			texture->GetTextureWidth(), texture->GetTextureHeight(), 0, 
			texture->IsTransparent() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, NULL);
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);

		g->tm.RefreshTextureOptimization();
	}
}

bool CCompositeTextureManager::CheckForCompressedData(CTexture* texture, std::string& /*cacheFileName*/)
{
	if (!g->gi.IsSaveCompressionSupported())
	{
		ClearCompressedTexture(texture);
		return false;
	}

	if (m_cacheTask)
	{
		void* data = NULL;
		int size = 0;
		
		if (m_cacheTask->IsPerformed())
		{
			data = m_cacheTask->GetData();
			size = m_cacheTask->GetDataSize();
		}
		else if (g->taskm.RemoveTask(m_cacheTask))
		{
			MP_DELETE(m_cacheTask);
			m_cacheTask = NULL;		
		}
		else
		{
			m_cacheTask->DestroyResultOnEnd();
			m_cacheTask = NULL;
		}
		
		if ((data) && (size > 4))
		{
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
			GLFUNC(glPixelStorei)(GL_UNPACK_ALIGNMENT, 4);
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);

			int format = *(unsigned int *)data;
			void* _data = (void *)((unsigned int *)data + 1);

			if (size - 4 != (int)(texture->GetTextureWidth() * texture->GetTextureHeight() / 2))
			{
				ClearCompressedTexture(texture);
				MP_DELETE_ARR(data);
				MP_DELETE(m_cacheTask);
				m_cacheTask = NULL;	
				return false;
			}

			GLFUNC(glCompressedTexImage2D)(GL_TEXTURE_2D, 0, format, texture->GetTextureWidth(), texture->GetTextureHeight(),
				0, size - 4, _data);			
			int glErr = GLFUNCR(glGetError());
			if (glErr != 0)
			{
				GLFUNC(glCompressedTexImage2D)(GL_TEXTURE_2D, 0, format, texture->GetTextureWidth(), texture->GetTextureHeight(),
					0, size - 4, _data);	
				glErr = GLFUNCR(glGetError());
			}
			
			g->tm.RefreshTextureOptimization();

			MP_DELETE_ARR(data);

			m_isCompressed = true;
			
			if (glErr != 0)
			{				
				ClearCompressedTexture(texture);
				MP_DELETE(m_cacheTask);
				m_cacheTask = NULL;	
				return false;
			}

			g->lw.WriteLn("Load compressed(1) for ", GetAvatarName(), " - ok");
			
			MP_DELETE(m_cacheTask);
			m_cacheTask = NULL;	
			return true;
		}
	}

	ClearCompressedTexture(texture);	

	return false;
}

void CCompositeTextureManager::UpdateTexture(unsigned int textureID)
{
	/*if ((!m_isUpdatePermitted) && (!m_isMyAvatar))
	{
		return; 
	}*/

	if ((textureID == -1) || (textureID >= m_textures.size()))
	{
		return;
	}

	//g->lw.WriteLn("UpdateTexture for ", GetAvatarName());

	m_textures[0].isUpdated = true;

	int _textureID = m_textures[textureID].resultTextureID;
	CTexture* texture = g->tm.GetObjectPointer(_textureID);

	for (int k = 0; k < 1; k++)
	if (!m_textures[textureID].sprites[k])
	{		
		m_textures[textureID].sprites[k] = LoadSprite(m_textures[textureID].fileNames[k], k);
		m_textures[textureID].sprites[k]->SetSize(texture->GetTextureWidth(), texture->GetTextureHeight());		
	}

	std::string cacheFileName = GetCacheFileName(textureID);	
	
	if (CheckForCompressedData(texture, cacheFileName))
	{		
		g->lw.WriteLn("CheckForCompressedData success for ", GetAvatarName());
		return;
	}

	m_renderImpls[m_currentRenderImplID]->SetObject3D(m_obj3d);
	m_renderImpls[m_currentRenderImplID]->UpdateTexture(textureID);

#ifdef AVED_LOGGING
	g->lw.WriteLn("[ERROR]CCompositeTextureManager::UpdateTexture end");
#endif
}

void CCompositeTextureManager::CheckTextureForMyAvatar(CTexture* texture, std::string& /*cacheFileName*/)
{
	unsigned char* textureData = (unsigned char *)texture->GetRawData();

	if (!textureData)
	{
		return;
	}

	bool isBlackImage = false;

	if ((textureData[0] == 0 ) && (textureData[1] == 0 ) && (textureData[2] == 0))
	{
		isBlackImage = true;
	}

	if (isBlackImage)
	{
		MP_DELETE_ARR(textureData);

		int textureID = m_textures[0].sprites[0]->GetTextureID(0);
		CTexture* _texture = g->tm.GetObjectPointer(textureID);
		unsigned char* textureData = (unsigned char *)_texture->GetRawData();		

		GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
		GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, texture->IsTransparent() ? GL_COMPRESSED_RGBA_ARB : GL_COMPRESSED_RGB_ARB, 
			texture->GetTextureWidth(), texture->GetTextureHeight(), 0, texture->IsTransparent() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, textureData);
		return;
	}
}

void CCompositeTextureManager::CompressTexture(CTexture* texture, std::string& cacheFileName)
{
	//поддерживается  сжатие текстур карточкой или нет
	if (!g->gi.IsSaveCompressionSupported())
	{
		return;
	}

	unsigned char* textureData = (unsigned char *)texture->GetRawData();

	if (!textureData)
	{
		return;
	}

	CompressTexture(textureData, texture, cacheFileName, false);

	//delete[] textureData;
}

std::string CCompositeTextureManager::GetAvatarName()
{
	USES_CONVERSION;
	return ((m_obj3d) ? W2A(m_obj3d->GetName()) : "unknown avatar");
}

void CCompositeTextureManager::CompressTexture(unsigned char* textureData, CTexture* texture, std::string& cacheFileName, bool isTextureRGBA)
{	
	//поддерживается  сжатие текстур карточкой или нет
	if (!g->gi.IsSaveCompressionSupported())
	{
		return;
	}

	//g->lw.WriteLn("CompressTexture for ", GetAvatarName());
 	
	bool isBlackImage = false;

	if ((textureData[0] == 0 ) && (textureData[1] == 0 ) && (textureData[2] == 0))
	{
		isBlackImage = true;
	}

	unsigned char* data = NULL;
	unsigned int size = 0;

	if (!isBlackImage)
	{		
		if ((m_isEditorVisible) || (m_isMyAvatar))
		{
			if (CompressImage(textureData, (void **)&data, (unsigned short)texture->GetTextureWidth(), (unsigned short)texture->GetTextureHeight(),
				isTextureRGBA ? -3 : 3, true, false, size))
			{
				GLFUNC(glPixelStorei)(GL_UNPACK_ALIGNMENT, 4);			

				GLFUNC(glCompressedTexImage2D)(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, texture->GetTextureWidth(), texture->GetTextureHeight(), 0, size - 16, data + 16);									
				int glErr = 0;
				glErr = GLFUNCR(glGetError());
				if (glErr != 0)
				{
					GLFUNC(glCompressedTexImage2D)(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, texture->GetTextureWidth(), texture->GetTextureHeight(), 0, size - 16, data + 16);									
					glErr = GLFUNCR(glGetError());
					if (glErr != 0)
					{
						size = 0;
						isBlackImage = true;
						g->lw.WriteLn("Load compressed texture to OpenGL error for ", GetAvatarName());
					}
				}

				if (!isBlackImage)
				{
					g->lw.WriteLn("Load compressed(2) for ", GetAvatarName(), " - ok");
				}
			}
			MP_DELETE_ARR(textureData);			
			
		}
		else
		{
			DeleteCompressingTasks();

			CCompressingImageTask* task = MP_NEW(CCompressingImageTask);
			task->SetParams(textureData, texture, cacheFileName, isTextureRGBA);
			m_compressingImageTasks.push_back(task);

			g->taskm.AddTask(task, MAY_BE_MULTITHREADED, PRIORITY_LOW);

			return;
		}		
	}

	if (isBlackImage)
	{
		int textureID = m_textures[0].sprites[0]->GetTextureID(0);
		CTexture* _texture = g->tm.GetObjectPointer(textureID);
		unsigned char* textureData = (unsigned char *)_texture->GetRawData();

		g->lw.WriteLn("Black texture was rendered for ", GetAvatarName());
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
		GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, texture->IsTransparent() ? GL_COMPRESSED_RGBA_ARB : GL_COMPRESSED_RGB_ARB, 
			texture->GetTextureWidth(), texture->GetTextureHeight(), 0, texture->IsTransparent() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, textureData);

		MP_DELETE_ARR(textureData);
		MP_DELETE_ARR(data);

		return;
	}

	if (size != 0)
	{
		*(unsigned int*)(data + 3) = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

		m_isCompressed = true;

		g->dc.AddDataAsynch(data + 12, size - 12, cacheFileName);		
	}

	MP_DELETE_ARR(data);
}

void CCompositeTextureManager::LoadAsynchCompressedTexture()
{
	if (m_compressingImageTasks.size() > 0)	
	{
		std::vector<CCompressingImageTask*>::iterator it = m_compressingImageTasks.begin();
		for (; it != m_compressingImageTasks.end(); it++)
		{
			CCompressingImageTask* task = *it;
			if (task->IsPerformed())				
			{
				assert(task->IsCompressed());
				if (task->IsCompressed())
				{					
					CTexture* texture = (*it)->GetTexture();

					unsigned int size = (*it)->GetSize();
					unsigned char* data = (*it)->GetData();

					if (!texture->IsDeleted())
					{
						GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());

						std::string cacheFileName = (*it)->GetCacheFileName();

						GLFUNC(glPixelStorei)(GL_UNPACK_ALIGNMENT, 4);															

						GLFUNC(glCompressedTexImage2D)(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, texture->GetTextureWidth(), texture->GetTextureHeight(), 0, size - 16, data + 16);					
						int glErr = GLFUNCR(glGetError());
						if (glErr != 0)
						{
							GLFUNC(glCompressedTexImage2D)(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, texture->GetTextureWidth(), texture->GetTextureHeight(), 0, size - 16, data + 16);					
							glErr = GLFUNCR(glGetError());
						}
						g->tm.RefreshTextureOptimization();

						if ((size != 0) && (glErr == 0))
						{
							*(unsigned int*)(data + 3) = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

							g->lw.WriteLn("Load compressed(3) for ", GetAvatarName(), " - ok");
				
							m_isCompressed = true;

							g->dc.AddDataAsynch(data + 12, size - 12, cacheFileName);		
						}									
					}
					
					MP_DELETE_ARR(data);
					MP_DELETE(*it);
					m_compressingImageTasks.erase(it);
					break;
				}
			}
		}
	}
}

void CCompositeTextureManager::DeleteAllCompositeTextures()
{
	if ((m_obj3d) && (!m_obj3d->IsDeleted()) && (m_obj3d->GetAnimation()))
	{
		UpdateLinksToCompositeTextures(m_obj3d, false);
	}

	DeleteCompositeTextures();

	m_textures.clear();
	m_texturesByID.clear();

	m_headID = 1;
	m_lastHeadID = 0;
	m_lastStr = "";

#ifdef AVED_LOGGING
	g->lw.WriteLn("[LOG]CCompositeTextureManager::DeleteAllCompositeTextures");
#endif
}

void CCompositeTextureManager::SaveOptions(int textureID)
{
	if ((textureID < 0) || ((unsigned int)textureID >= m_textures.size()))
	{
		return;
	}

	m_savedTexture = m_textures[textureID];
}

void CCompositeTextureManager::RestoreOptions(int textureID)
{
	if ((textureID < 0) || ((unsigned int)textureID >= m_textures.size()))
	{
		return;
	}

	for (int i = 0; i < m_savedTexture.levelCount; i++)
	{
		m_textures[textureID].alphaFuncs[i] = m_savedTexture.alphaFuncs[i];
		m_textures[textureID].alphaRefs[i] = m_savedTexture.alphaRefs[i];
		m_textures[textureID].colors[i] = m_savedTexture.colors[i];
	}
}

void CCompositeTextureManager::DeleteCompositeTexture(SCompositeTexture* texture)
{
	g->tm.DeleteObject(texture->resultTextureID);

	for (int levelID = 0; levelID < MAX_LEVELS; levelID++)
	if (texture->sprites[levelID])
	{
		MP_DELETE(texture->sprites[levelID]);
	}
}

void CCompositeTextureManager::DeleteCompositeTextures()
{
	std::vector<SCompositeTexture>::iterator it = m_textures.begin();
	std::vector<SCompositeTexture>::iterator itEnd = m_textures.end();

	for ( ; it != itEnd; it++)
	{
		DeleteCompositeTexture(&(*it));
	}
}

void CCompositeTextureManager::DeleteCompressingTasks()
{
	std::vector<CCompressingImageTask*>::iterator it = m_compressingImageTasks.begin();
	for (; it != m_compressingImageTasks.end(); it++)
	{
		CCompressingImageTask* task = *it;
		if ((task->IsPerformed()) || (g->taskm.RemoveTask(task)))
		{
			if (task->GetData())
			{
				MP_DELETE_ARR_UNSAFE(task->GetData());
			}

			MP_DELETE(task);
		}
		else
		{
			task->DestroyResultOnEnd();
		}
	}

	m_compressingImageTasks.clear();
}

CCompositeTextureManager::~CCompositeTextureManager()
{
	DeleteCompressingTasks();

	if (m_cacheTask)
	if ((m_cacheTask->IsPerformed()) || (g->taskm.RemoveTask(m_cacheTask)))
	{
		MP_DELETE(m_cacheTask);
	}
	else
	{
		m_cacheTask->DestroyResultOnEnd();
	}
}