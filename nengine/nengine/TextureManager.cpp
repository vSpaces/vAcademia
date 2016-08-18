
#include "stdafx.h"
#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"
#include "ResourceProcessor.h"
#include "HelperFunctions.h"
#include "TextureManager.h"
#include "DXTCompressor.h"
#include "LogWriter.h"
#include "UserData.h"
#include "XMLList.h"
#include "Stdio.h"
#include "Math.h"
#include "IniFile.h"
#include "NEngine.h"
#include <assert.h>

#include <memory>

#include "iasynchresourcemanager.h"
#include "iasynchresource.h"

_SSaveCompressionTextureInfo::_SSaveCompressionTextureInfo():
	MP_STRING_INIT(path)
{
	pboInfo = NULL;
	size = 0;
	framePassCount = 4;
}

CTextureManager::CTextureManager():
	m_isCheckingVisibleDynamicTextures(false),
	m_isTexturingEnable(true),
	m_currentMipmapBias(0.0f),
	m_loadInfoListener(NULL),
	m_cubeMapNum(0xFFFFFFFF),
	m_occlusionTexture(NULL),
	m_canBeCompressed(true),
	m_skipLoadingFrames(0),	
	m_errorTextureID(0),
	m_lastUpdateTime(0),
	m_whiteTextureID(0),
	m_greyTextureID(0),	
	m_lastHeight(0),
	m_lastWidth(0),
	m_new(0),
	m_startTexturesChanged(-1),
	m_commonTime(0),
	m_deltaTime(0),
	MP_VECTOR_INIT(m_dynamicTextures),
	MP_VECTOR_INIT(m_updatingTextures),
	MP_VECTOR_INIT(m_loadingTextures),
	MP_VECTOR_INIT(m_loadingTexturesTasks),
	MP_VECTOR_INIT(m_avatarTextureID),
	MP_VECTOR_INIT(m_readCompressedTexturesTasks),
	MP_VECTOR_INIT(m_texturesForSave)
{
	ReserveMemory(1000);

	RefreshTextureOptimization();
}

int CTextureManager::GetGreyTextureID()const
{
	return m_greyTextureID;
}

int CTextureManager::GetWhiteTextureID()const
{
	return m_whiteTextureID;
}

int CTextureManager::GetErrorTextureID()const
{
	return m_errorTextureID;
}

int CTextureManager::AddResampledTexture(int textureID, int width, int height)
{
	CTexture* texture = g->tm.GetObjectPointer(textureID);
	void* data = texture->GetRawData();
	int textureWidth = texture->GetTextureWidth();
	int textureHeight = texture->GetTextureHeight();
	int realTextureWidth = texture->GetTextureRealWidth();
	int realTextureHeight = texture->GetTextureRealHeight();
	bool isTransparent = texture->IsTransparent();
	m_textureLoader.LoadFromMemory(IMAGE_FORMAT_BMP, data, realTextureWidth, realTextureHeight, isTransparent);
	m_textureLoader.Crop(textureWidth, textureHeight);
	m_textureLoader.Resize(width, height);

	int _textureID = g->tm.AddObject("resampled_texture_" + IntToStr(textureID));
	CTexture* _texture = g->tm.GetObjectPointer(_textureID);
	_texture->SetPath(texture->GetPath());
	_texture->SetTextureWidth(width);
	_texture->SetTextureHeight(height);
	_texture->KeepInSystemMemory(texture->IsKeepInSystemMemory());
	RegisterTexture(_texture);
	InitTexture(&m_textureLoader, _texture, false, false);

	MP_DELETE_ARR(data);

	return _textureID;
}

void CTextureManager::ResizeTexture(int textureID, int width, int height)
{
	CTexture* texture = g->tm.GetObjectPointer(textureID);
	void* data = texture->GetRawData();

	if (data != NULL)
	{
		int textureWidth = texture->GetTextureWidth();
		int textureHeight = texture->GetTextureHeight();
		int realTextureWidth = texture->GetTextureRealWidth();
		int realTextureHeight = texture->GetTextureRealHeight();

		texture->SetTextureSizeBeforeResize(textureWidth, textureHeight, realTextureWidth, realTextureHeight );

		bool isTransparent = texture->IsTransparent();
		m_textureLoader.LoadFromMemory(IMAGE_FORMAT_BMP, data, realTextureWidth, realTextureHeight, isTransparent);		
		m_textureLoader.Crop(textureWidth, textureHeight);
		m_textureLoader.Resize(width, height);

		texture->SetTextureWidth(width);
		texture->SetTextureHeight(height);
		texture->GetLoadingInfo()->SetMipMapStatus(false);
		texture->GetLoadingInfo()->SetFilteringMode(FILTERING_MODE_LINEAR_NO_MIPMAP);
		InitTexture(&m_textureLoader, texture, false, false);

		texture->GetDynamicTexture()->SetFreezState(true);

		MP_DELETE_ARR(data);
	}
}

void CTextureManager::ResetResizeTexture(int textureID, int /*width*/, int /*height*/)
{
	CTexture* texture = g->tm.GetObjectPointer(textureID);
	if (texture->GetDynamicTexture())
	{
		texture->GetDynamicTexture()->SetFreezState(false);	
	}
}

void CTextureManager::UpdateDynamicTextureSize(IDynamicTexture* dynamicTexture)
{
	if (!g->gi.IsNPOTSupport())
	{
		int newWidth = 2;
		while (newWidth < dynamicTexture->GetTextureWidth())
		{
			newWidth *= 2;
		}
		if (newWidth != dynamicTexture->GetTextureWidth())
		{
			newWidth = newWidth / 2;
		}

		int newHeight = 2;
		while (newHeight < dynamicTexture->GetTextureHeight())
		{
			newHeight *= 2;
		}
		if (newHeight != dynamicTexture->GetTextureHeight())
		{
			newHeight = newHeight / 2;
		}

		dynamicTexture->SetTextureSize(newWidth, newHeight);
	}
	else
	{
		if ((dynamicTexture->GetTextureWidth() > 512 || dynamicTexture->GetTextureHeight() > 512) && g->rs.GetInt(TEXTURE_SIZE_DIV) >= 2)
			dynamicTexture->SetTextureSize(dynamicTexture->GetTextureWidth() / 2, dynamicTexture->GetTextureHeight() / 2);
		else
			dynamicTexture->SetTextureSize(dynamicTexture->GetTextureWidth(), dynamicTexture->GetTextureHeight());
	}
}

int CTextureManager::AddDynamicTexture(IDynamicTexture* dynamicTexture)
{
	int textureID = g->tm.AddObject("dynamic_texture_" + IntToStr(GetCount()));
	CTexture* texture = g->tm.GetObjectPointer(textureID);
	
	UpdateDynamicTextureSize(dynamicTexture);

	texture->SetTextureWidth(dynamicTexture->GetTextureWidth());
	texture->SetTextureHeight(dynamicTexture->GetTextureHeight());
	texture->SetTransparentStatus(dynamicTexture->GetColorChannelCount() == 4);
	texture->SetDynamicTexture(dynamicTexture);
	texture->SetBindedStatus(true);
	texture->SetReallyUsedStatus(true);
	texture->SetLoadedStatus(TEXTURE_LOADED);
	RegisterTexture(texture);

	m_lastWidth = texture->GetTextureWidth();
	m_lastHeight = texture->GetTextureHeight();
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
	texture->SetCurrentFiltration(GL_LINEAR, GL_LINEAR, 1, 0);	
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);	
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	GLfloat color[4];
	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	color[3] = 1.0f;
	GLFUNC(glTexParameterfv)(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);
	GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, texture->IsTransparent() ? GL_RGBA : GL_RGB, m_lastWidth, m_lastHeight, 0, texture->IsTransparent() ? GL_BGRA_EXT : GL_BGR_EXT, GL_UNSIGNED_BYTE, dynamicTexture->GetFrameData());
	RefreshTextureOptimization();

	char* filterName = (char*)dynamicTexture->GetUserData(1);
	if (filterName)
	{
		/*unsigned int fsID = g->fsp.GetObjectNumber(filterName);
		if (fsID != 0xFFFFFFFF)
		{
			CFilterSequence* filterSequence = g->fsp.GetObjectPointer(fsID);
			filterSequence->SetResultImage(0, textureID);
			filterSequence->SetSourceImage(0, textureID);
			filterSequence->PrepareForExecution();
		}

		dynamicTexture->SetUserData(0, (void *)fsID);*/
	}
	else
	{
		//dynamicTexture->SetUserData(0, (void *)0xFFFFFFFF);
	}

	m_dynamicTextures.push_back(texture);

	return textureID;	
}

void CTextureManager::RemoveDynamicTexture(IDynamicTexture* texture)
{
	std::vector<CTexture*>::iterator it = m_dynamicTextures.begin();
	std::vector<CTexture*>::iterator itEnd = m_dynamicTextures.end();

	for ( ; it != itEnd; it++)
	if ((*it)->GetDynamicTexture() == texture)
	{
		RemoveDynamicTexture(*it);	
		break;
	}
}
	
void CTextureManager::RemoveDynamicTexture(CTexture* texture)
{
	std::vector<CTexture*>::iterator it = m_dynamicTextures.begin();
	std::vector<CTexture*>::iterator itEnd = m_dynamicTextures.end();

	for ( ; it != itEnd; it++)
	if (*it == texture)
	{
		m_dynamicTextures.erase(it);
		break;
	}

	bool isFound = false;

	it = m_updatingTextures.begin();
	itEnd = m_updatingTextures.end();	
	for ( ; it != itEnd; it++)
	if (*it == texture)
	{
		isFound = true;
	}

	if ((!isFound) && (texture->GetDynamicTexture()))
	{
		texture->GetDynamicTexture()->Destroy();
		texture->SetDynamicTexture(NULL);
	}

	DeleteObject(GetObjectNumber(texture->GetName()));
}

void CTextureManager::Update()
{
	stat_texturesSize = 0;
	
	int tasksCheckTimeStart = (int)g->tp.GetTickCount();

	g->pm.CustomBlockStart(CUSTOM_BLOCK_TEXTURES_LOADING);

	if (m_loadingTexturesTasks.size() > 0)	
	{
		std::vector<CLoadingTexturesTask *>::iterator it = m_loadingTexturesTasks.begin();
		for (; it != m_loadingTexturesTasks.end(); ++it)
		{
			CLoadingTexturesTask * task = *it;
			if (task->IsPerformed())
			{
				if (task->IsLoaded())
				{
					SLoadingTexture* loadingTexture = task->GetTexture();

					if ((loadingTexture) && (loadingTexture->isCompressedExists))
					{
						if (!LoadCompressed(loadingTexture->texture, loadingTexture->cacheFileName))
						{
							g->lw.WriteLn("LoadCompressed failed for ", loadingTexture->texture->GetName());
						}

						task->OnLoaded();

						MP_DELETE(loadingTexture);
					}
					else if (loadingTexture)
					{
						loadingTexture->texture->SetTextureWidth(loadingTexture->textureWidth);
						loadingTexture->texture->SetTextureHeight(loadingTexture->textureHeight);

						if (loadingTexture->realTextureWidth != 0)
						{
							loadingTexture->texture->SetTextureRealWidth(loadingTexture->realTextureWidth);
						}
						if (loadingTexture->realTextureHeight != 0)
						{
							loadingTexture->texture->SetTextureRealHeight(loadingTexture->realTextureHeight);
						}
						
						InitTexture(loadingTexture->textureLoader, loadingTexture->texture, loadingTexture->isMipmap, loadingTexture->isMaskNeeded, &loadingTexture->cacheFileName);

						loadingTexture->texture->OnAfterChanges();

						task->OnLoaded();

						MP_DELETE(loadingTexture);
					}
				}
				else
				{
					SLoadingTexture* loadingTexture = task->GetTexture();
					if (loadingTexture)
					{
						loadingTexture->texture->SetLoadedStatus(TEXTURE_LOAD_ERROR);
						loadingTexture->texture->OnAfterChanges(0);						
						g->lw.WriteLn("[LOADERROR] error loading (decoding via FreeImage) ", loadingTexture->path);
						MP_DELETE(loadingTexture);
					}
				}
				
				m_loadingTexturesTasks.erase(it);
				MP_DELETE(task);
				if (m_loadingTexturesTasks.empty())
					break;
				it = m_loadingTexturesTasks.begin();
				//--it;

				if (g->tp.GetTickCount() - tasksCheckTimeStart > g->rs.GetInt(MAX_TEXTURE_LOADING_TIME))
				{
					break;
				}
			}
		}
	}

	g->pm.CustomBlockEnd(CUSTOM_BLOCK_TEXTURES_LOADING);

	g->pm.CustomBlockStart(CUSTOM_BLOCK_TEXTURES_LOADING2);

	if (!m_loadingTextures.empty())
	{
		int loadingTimeStart = (int)g->tp.GetTickCount();
		bool isLoadCompressed = true;

		if (m_skipLoadingFrames > 0)
		{
			m_skipLoadingFrames--;
			isLoadCompressed = false;
		}

		std::vector<SLoadingTexture *> nonPerformedTextures;

		std::vector<SLoadingTexture *>::iterator it = m_loadingTextures.begin();
		std::vector<SLoadingTexture *>::iterator end = m_loadingTextures.end();
		for ( ; it != end; it++)
		{
			SLoadingTexture* loadingTexture = *it;

			if (loadingTexture->isCompressedExists)
			{
				if (isLoadCompressed)
				{
					if (!LoadCompressed(loadingTexture->texture, loadingTexture->cacheFileName))
					{
						loadingTexture->isCompressedExists = false;
					}
					else
					{
						if (g->tp.GetTickCount() - loadingTimeStart > g->rs.GetInt(MAX_TEXTURE_LOADING_TIME))
						if (g->rs.GetBool(SKIP_FRAMES_TEXTURE_LOADING))
						{
							isLoadCompressed = false;

							m_skipLoadingFrames = (int)((g->tp.GetTickCount() - loadingTimeStart - g->rs.GetInt(MAX_TEXTURE_LOADING_TIME)) / g->rs.GetInt(MAX_TEXTURE_LOADING_TIME));
							if (m_skipLoadingFrames <= 1)
							{
								m_skipLoadingFrames = 1;
							}

							if (m_skipLoadingFrames > 10)
							{
								m_skipLoadingFrames = 10;
							}
						}
					}
				}
				else
				{
					nonPerformedTextures.push_back(loadingTexture);
				}
			}

			if (!loadingTexture->isCompressedExists)
			{
				CLoadingTexturesTask* task = MP_NEW(CLoadingTexturesTask);
				task->Rewind();
				task->SetTexture(loadingTexture, g->rs.GetInt(TEXTURE_SIZE_DIV));

				m_loadingTexturesTasks.push_back(task);

				g->taskm.AddTask(task, MUST_BE_MULTITHREADED, PRIORITY_LOW);
			}
		}

		m_loadingTextures.clear();

		it = nonPerformedTextures.begin();
		end = nonPerformedTextures.end();

		for ( ; it != end; it++)
		{
			m_loadingTextures.push_back(*it);
		}
	}

	g->pm.CustomBlockEnd(CUSTOM_BLOCK_TEXTURES_LOADING2);

	g->pm.CustomBlockStart(CUSTOM_BLOCK_TEXTURES_READING_CACHE);

	{
		std::vector<CReadingCacheResourceTask *>::iterator it = m_readCompressedTexturesTasks.begin();

		for ( ; it != m_readCompressedTexturesTasks.end(); )
		{
			if ((*it)->IsPerformed())
			{
				CTexture* texture = (CTexture *)(*it)->GetUserData();

				if (((*it)->GetData()) && ((*it)->GetDataSize() > 0))
				{
					LoadCompressed(texture, (unsigned char*)(*it)->GetData(), (*it)->GetDataSize());
				}
				else
				{
					texture->SetLoadedStatus(TEXTURE_LOAD_ERROR);
					texture->OnAfterChanges(0);
					g->lw.WriteLn("[LOADERROR] error loading from ncache ", texture->GetName());
				}

				MP_DELETE(*it);
				it = m_readCompressedTexturesTasks.erase(it);
				
				if (m_readCompressedTexturesTasks.size() == 0)
				{
					break;
				}			
			}
			else
			{
				it++;
			}
		}
	}

	g->pm.CustomBlockEnd(CUSTOM_BLOCK_TEXTURES_READING_CACHE);

	if (g->ne.time - m_lastUpdateTime < g->rs.GetInt(VIDEO_UPDATE_DELTA_TIME))
	{
		return;
	}

	if (!m_updateDynamicTexturesTask.IsPerformed())
	{
		return;
	}

	g->pm.CustomBlockStart(CUSTOM_BLOCK_UPDATE_DYNAMIC_TEXTURES);

	std::vector<CTexture *>::iterator it = m_dynamicTextures.begin();

	while (it != m_dynamicTextures.end())
	{
		if ((*it)->IsDeleted())
		{
			it = m_dynamicTextures.erase(it);
		}
		else
		{
			it++;
		}
	}
	
	it = m_updatingTextures.begin();
	std::vector<CTexture *>::iterator itEnd = m_updatingTextures.end();
	
	for ( ; it != itEnd; it++)
	if (!(*it)->IsDeleted())
	{
		void* ptr = (*it)->GetDynamicTexture()->GetFrameData();

		if (ptr)
		{
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, (*it)->GetNumber());
			if (!(*it)->GetDynamicTexture()->IsFreezed())
			if (((*it)->GetDynamicTexture()->GetTextureWidth() != (*it)->GetTextureWidth()) || ((*it)->GetDynamicTexture()->GetTextureHeight() != (*it)->GetTextureHeight()))
			{
				UpdateDynamicTextureSize((*it)->GetDynamicTexture());
				m_lastWidth = (*it)->GetDynamicTexture()->GetTextureWidth();
				m_lastHeight = (*it)->GetDynamicTexture()->GetTextureHeight();
				(*it)->SetTextureWidth(m_lastWidth);
				(*it)->SetTextureHeight(m_lastHeight);

				(*it)->SetCurrentFiltration(GL_LINEAR, GL_LINEAR, 1, 0);				
				GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, (*it)->IsTransparent() ? GL_RGBA : GL_RGB, m_lastWidth, m_lastHeight, 0, (*it)->IsTransparent() ? GL_BGRA_EXT : GL_BGR_EXT, GL_UNSIGNED_BYTE, (*it)->GetDynamicTexture()->GetFrameData());
			}
			else	
			{
				m_lastWidth = (*it)->GetTextureWidth();
				m_lastHeight = (*it)->GetTextureHeight();

				(*it)->SetCurrentFiltration(GL_LINEAR, GL_LINEAR, 1, 0);
				GLFUNC(glTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, m_lastWidth, m_lastHeight, (*it)->IsTransparent() ? GL_BGRA_EXT : GL_BGR_EXT, GL_UNSIGNED_BYTE, ptr);
			}

			/*unsigned int fsID = (unsigned int)((*it)->GetDynamicTexture()->GetUserData(0));
			CFilterSequence* fs = g->fsp.GetObjectPointer(fsID);
			fs->Execute();*/

			RefreshTextureOptimization();
		}
	}
	else
	{
		if ((*it)->GetDynamicTexture())
		{
			(*it)->GetDynamicTexture()->Destroy();
			(*it)->SetDynamicTexture(NULL);
			(*it)->MarkDeleted();
		}
	}

	m_updatingTextures.clear();

	it = m_dynamicTextures.begin();
	itEnd = m_dynamicTextures.end();

	stat_dynamicTextureCount = m_dynamicTextures.size();

	for ( ; it != itEnd; it++)
	{
	if (((*it)->GetDynamicTexture()->IsNeedUpdate()) && ((*it)->IsBinded()) && ((*it)->IsReallyUsed()))
	{
		(*it)->GetDynamicTexture()->Update();
		m_updateDynamicTexturesTask.AddDynamicTexture((*it)->GetDynamicTexture());
		m_updatingTextures.push_back(*it);
	}
	}

	if (m_updatingTextures.size() > 0)
	{
		m_updateDynamicTexturesTask.Rewind();
		g->taskm.AddTask(&m_updateDynamicTexturesTask, MAY_BE_MULTITHREADED, PRIORITY_LOW);
	}

	g->pm.CustomBlockEnd(CUSTOM_BLOCK_UPDATE_DYNAMIC_TEXTURES);
}

void CTextureManager::DisableTexturing()
{
	m_isTexturingEnable = false;
}

void CTextureManager::EnableTexturing()
{
	m_isTexturingEnable = true;
}

void CTextureManager::SetCubeMapTexture(int target, int format, bool /*isMipmap*/, void* ptr)
{
	// будем использовать всегда один, безопасный способ
	GLFUNC(gluBuild2DMipmaps)(target, GL_COMPRESSED_RGB_ARB, m_lastWidth, m_lastHeight, format, GL_UNSIGNED_BYTE, ptr);		
}

int CTextureManager::ManageGLTexture(std::string dest, unsigned int oglTextureID, int width, int height)
{
	std::string name = dest;
	name += "_texture_";
	name += IntToStr(oglTextureID);
	int id = AddObject(name);
	CTexture* texture = GetObjectPointer(id);
	texture->SetNumber(oglTextureID);
	texture->SetTextureWidth(width);
	texture->SetTextureHeight(height);
	texture->SetLoadedStatus(TEXTURE_LOADED);

	return id;
}

void CTextureManager::Initialize()
{
	m_new = -1;
}

void CTextureManager::RegisterTexture(CTexture* texture)
{
	m_new++;

	unsigned int textureID;
	GLFUNC(glGenTextures)(1, &textureID);
	texture->SetNumber(textureID);
}

int CTextureManager::LoadTextureFromMemory(void* /*buf*/, int /*type*/, std::string /*name*/)
{
	assert(false);

	return 0;
}

void CTextureManager::SetLoadInfoListener(ILoadInfoListener* loadInfoListener)
{
	assert(loadInfoListener);
	m_loadInfoListener = loadInfoListener;
}

void CTextureManager::LoadAll()
{
	g->rp.OpenResFile(g->ne.GetTexturesResourceFileID());

	g->lw.WriteLn("Loading textures...");

	std::vector<std::string> tmpNames;
	std::vector<int> tmpSizes;
	int i = 0, kfiles = 0;

	kfiles = g->rp.GetShort();
	for (i = 0; i < kfiles; i++)
	{
		// texture type
		int type = (int)g->rp.GetShort();

		char ic = 1;

		std::string tmpName = "";

		while (ic != 0)
		{
			ic = g->rp.GetChar();

			if (ic != 0)
			{
				char tmp[2];
				tmp[0] = ic;
				tmp[1] = 0;
				tmpName += tmp;
			}
		}

		tmpNames.push_back(tmpName);

		int num = AddObject(tmpNames[i]);
		CTexture* texture = GetObjectPointer(num);
	
		texture->SetTextureType(type);

		// translate?
		g->rp.GetShort();
		// translate delta
		g->rp.GetLong();
		// rotate?
		g->rp.GetShort();
		// rotate delta
		g->rp.GetLong();
		// ksecondtextures
		int count = (int)g->rp.GetShort();
		if (count != 0)
		{
			// deltatime
			g->rp.GetShort();
			// multitype
			g->rp.GetShort();

			for (int k = 0; k < count; k++)
			{
				ic = 2;
				
				std::string secondTextureFileName = "";

				while (ic != 0)
				{
					ic = g->rp.GetChar();

					secondTextureFileName += ic;
				}	
			}
		}
		//mipmap?
		texture->GetLoadingInfo()->SetMipMapStatus(BoolFromUShort(g->rp.GetShort()));
		//heightmap?
		g->rp.GetShort();
		//alpha?
		g->rp.GetShort();
		//edges?
		g->rp.GetShort();
		//alpha level
		g->rp.GetShort();
		//transparent?
		texture->SetTransparentStatus(BoolFromUShort(g->rp.GetShort()));
		//transparent r-color
		int colorR = g->rp.GetShort();
		//transparent g-color
		int colorG = g->rp.GetShort();
		//transparent b-color
		int colorB = g->rp.GetShort();

		texture->SetColor((unsigned char)colorR, (unsigned char)colorG, (unsigned char)colorB);

		tmpSizes.push_back(g->rp.GetLong());
	}

	bool cubeMapLoading = FALSE;
	int cubeMapTextures = 6;
	
	for (i = 0; i < kfiles; i++)
	{
		int ml = GetObjectNumber(tmpNames[i]);

		CTexture* texture = GetObjectPointer(ml);

		if (g->cmm.IsObjectExists(tmpNames[i]))
		{
			cubeMapLoading = TRUE;
			cubeMapTextures = 6;

			
			{
				GLFUNC(glGenTextures)(1, &m_cubeMapNum);

				g->cmm.AddCubeMap(tmpNames[i], m_cubeMapNum);
	
				GLFUNC(glBindTexture)(GL_TEXTURE_CUBE_MAP_ARB, m_cubeMapNum);
				GLFUNC(glPixelStorei)(GL_UNPACK_ALIGNMENT, 1);

				GLFUNC(glTexParameterf)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				GLFUNC(glTexParameterf)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				GLFUNC(glTexParameteri)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			}
		}

		if (cubeMapLoading == TRUE)
		{
			cubeMapTextures--;
		}

		g->lw.Write(tmpNames[i]);

		bool isMipmap = true;
		if (!texture->GetLoadingInfo()->IsMipMap())
		{
			isMipmap = false;
		}

//		EnterCriticalSection(&m_textureLoaderCS);
		if (m_textureLoader.LoadTexture(tmpNames[i], g->rp.GetCurrentPointer(), tmpSizes[i]))
		{
			texture->SetTextureWidth(m_textureLoader.GetWidth());
			texture->SetTextureHeight(m_textureLoader.GetHeight());
			texture->SetTextureRealWidth(m_textureLoader.GetWidth());
			texture->SetTextureRealHeight(m_textureLoader.GetHeight());
			g->lw.WriteLn(" - ok.");
		}
		else
		{
			g->lw.WriteLn(" - failed.");
		}
		
		m_lastWidth = m_textureLoader.GetWidth();
		m_lastHeight = m_textureLoader.GetHeight();
		g->rp.MovePosition(tmpSizes[i]);

		if (((texture->IsTransparent()) && (m_textureLoader.GetChannelCount() != 4)) || (texture->GetKeyColorType() != KEY_COLOR_NOT_USED))
		{
			unsigned char r, _g, b;
			texture->GetColor(r, _g, b);
			m_textureLoader.ApplyKeyColor(r, _g, b);
		}

		if (cubeMapLoading == FALSE)
		{
			texture->GetLoadingInfo()->SetFilteringMode(FILTERING_MODE_LINEAR);
			int minFilter, magFilter;
			m_textureLoader.LoadToOpenGL(texture->GetNumber(), texture->GetLoadingInfo(), minFilter, magFilter);
			texture->UpdateCurrentFiltration(minFilter, magFilter);
		}
		else 
		{
			SetCubeMapTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + (5 - cubeMapTextures), m_textureLoader.IsDirectChannelOrder() ? GL_RGB : GL_BGR_EXT, true, (void *)m_textureLoader.GetData(0));
		}

		//texture->SetTextureWidth(m_lastWidth);
		//texture->SetTextureHeight(m_lastHeight);
		texture->SetYInvertStatus(false);
		texture->SetLoadedStatus(TEXTURE_LOADED);

		if ((cubeMapTextures == 0) && (cubeMapLoading == TRUE))
		{
			cubeMapLoading = FALSE;

			GLFUNC(glEnable)(GL_TEXTURE_CUBE_MAP_ARB);

			GLFUNC(glTexParameteri)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			GLFUNC(glTexParameteri)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			GLFUNC(glDisable)(GL_TEXTURE_CUBE_MAP_ARB);
		}

		if (m_loadInfoListener)
		{
			m_loadInfoListener->On2DTexturesAndCubemapsLoaded(i + 1, kfiles);
		}
//		LeaveCriticalSection(&m_textureLoaderCS);
	}
	
	LoadAllExternal();
}

int CTextureManager::LoadAsynch(std::string &name, std::string &path, std::vector<void*>* owners, bool onlyAsynch, CNTextureLoadingInfo* loadInfo, unsigned char maxDecreaseKoef)
{
	std::wstring wname = CharToWide( name.c_str());
	std::wstring wpath = CharToWide( path.c_str());
	return LoadAsynch( wname, wpath, owners, onlyAsynch, loadInfo, maxDecreaseKoef);
}

int CTextureManager::LoadAsynch( std::wstring &name, std::wstring &path, std::vector<void*>* owners, bool onlyAsynch, CNTextureLoadingInfo* loadInfo, unsigned char maxDecreaseKoef)
{
	int textureID = GetObjectNumber(name);
	if (textureID != -1)
	{
		return -1;
	}

	path = CorrectPath(path);

	CTexture* texture = MP_NEW(CTexture);
	if (maxDecreaseKoef != 0)
	{
		texture->SetMaxDecreaseKoef(maxDecreaseKoef);
	}
	texture->SetName(name);	
	if (loadInfo)
	{
		texture->GetLoadingInfo()->CopyFrom(loadInfo);

		unsigned char r, g, b;
		unsigned char keyType = loadInfo->GetColorKey(r, g, b);
		if (keyType > 0)
		{			
			texture->SetKeyColor(keyType);
			texture->SetColor(r, g, b);
		}
	}
	texture->SetPath(path);
	if (texture->GetUserData(USER_DATA_SRC))
	{
		std::wstring* tmp = (std::wstring *)texture->GetUserData(USER_DATA_SRC);
		MP_DELETE(tmp);	
	}
	MP_NEW_P(_path, std::wstring, path);
	texture->SetUserData(USER_DATA_SRC, _path);
	RegisterTexture(texture);

	const int emptyTextureLinearSize = 4;
	int textureSize = emptyTextureLinearSize * emptyTextureLinearSize * 3;
	unsigned char* tmp = (unsigned char*)_alloca(textureSize);
	for (int k = 0; k < textureSize; k += 3)
	{
		tmp[k + 0] = 127;
		tmp[k + 1] = 127;
		tmp[k + 2] = 127;
	}

	m_lastWidth = emptyTextureLinearSize;
	m_lastHeight = emptyTextureLinearSize;	
	m_textureLoader.LoadFromMemory(IMAGE_FORMAT_BMP, tmp, emptyTextureLinearSize, emptyTextureLinearSize, false);	
	int minFilter, magFilter;
	m_textureLoader.LoadToOpenGL(texture->GetNumber(), texture->GetLoadingInfo(), minFilter, magFilter);
	texture->UpdateCurrentFiltration(minFilter, magFilter);	
	g->tm.RefreshTextureOptimization();
	if (!loadInfo)
	{
		texture->GetLoadingInfo()->SetMipMapStatus(true);
	}
	else if (loadInfo->IsKeepInSystemMemory())
	{
		texture->KeepInSystemMemory(true);
	}
	texture->SetTransparentStatus(false);
	texture->SetColor(255, 255, 255);

	ifile* file = NULL;

	if (!onlyAsynch)
	{
		file = g->rl->GetResMemFile(path.c_str());
	}	
	
	if (!file)
	{
		__int64 modifyTime;
		if (texture->GetLoadingInfo()->CanTextureBeCompressed())
		if (g->gi.IsSaveCompressionSupported())
		if (g->rs.GetInt(TEXTURE_SIZE_DIV) == 1)
		if (g->rl->GetAsynchResMan()->GetModifyTime( path.c_str(), modifyTime))
		{	
			std::string sPath = WideToChar( (wchar_t*)path.c_str());
			std::string cacheFileName = GetCacheName( sPath, modifyTime, true);

			if (g->dc.CheckIfDataExists(cacheFileName))
			{
				SLoadingTexture* loadingTexture = MP_NEW(SLoadingTexture);
				loadingTexture->texture = texture;
				loadingTexture->textureLoader =	NULL;
				loadingTexture->size = 0;
				loadingTexture->data = NULL;
				loadingTexture->isMipmap = true;
				loadingTexture->isMaskNeeded = false;
				loadingTexture->path = path;
				loadingTexture->cacheFileName = cacheFileName;
				loadingTexture->isCompressedExists = true;

				m_loadingTextures.push_back(loadingTexture);

				textureID = g->tm.ManageObject(texture);

				return textureID;
			}
		}
	}	

	if (onlyAsynch)
	{
		file = g->rl->GetResMemFile(path.c_str());
	}

	if (!file)
	{
		USES_CONVERSION;
		IAsynchResource* asynch = g->rl->GetAsynchResMan()->GetAsynchResource( W2A(path.c_str()), true, RES_TYPE_TEXTURE, texture, g->rl, owners, texture);

		if (asynch)
		{
			// resource not found on local disk
			g->rl->GetAsynchResMan()->AddLoadHandler(asynch, texture);						
		}
		else
		{
			//g->tm.DeleteObject(textureID);
			MP_DELETE(texture);
			return -1;			
		}
	}
	else
	{		
		LoadFromIFile(texture, file, loadInfo ? loadInfo->IsMipMap() : true);
		g->rl->DeleteResFile(file);
	}

	textureID = g->tm.ManageObject(texture);

	return textureID;
}

int CTextureManager::AddColorTexture(std::string textureName)
{

	int textureID = GetObjectNumber(textureName);
	if (-1 == textureID)
	{
		unsigned int color = 0;
		sscanf_s(textureName.substr(1, textureName.size() - 1).c_str(), "%u", &color);		 
		return AddColorTexture(textureName, color);
	}
	else
	{
		return textureID;
	}
}

void CTextureManager::ReplaceColorTexture(int textureID, unsigned int color)
{
	CTexture* texture = g->tm.GetObjectPointer(textureID);

	const int colorTextureLinearSize = 4;
	m_lastWidth = colorTextureLinearSize;
	m_lastHeight = colorTextureLinearSize;

	unsigned int tmp[colorTextureLinearSize * colorTextureLinearSize];
	int offset = 0;
	while (offset < colorTextureLinearSize * colorTextureLinearSize)
	{
		tmp[offset++] = color;
	}

	texture->GetLoadingInfo()->SetMipMapStatus(true);
	m_textureLoader.LoadFromMemory(IMAGE_FORMAT_BMP, tmp, colorTextureLinearSize, colorTextureLinearSize, true);	
	int minFilter, magFilter;
	m_textureLoader.LoadToOpenGL(texture->GetNumber(), texture->GetLoadingInfo(), minFilter, magFilter);	
	texture->UpdateCurrentFiltration(minFilter, magFilter);
		
	texture->SetTransparentStatus(!(color & 0xFF000000));
	texture->SetColor(255, 255, 255);
	texture->SetLoadedStatus(TEXTURE_LOADED);

	texture->OnAfterChanges();
	texture->UpdateCurrentFiltration(minFilter, magFilter);
}

int CTextureManager::AddColorTexture(std::string textureName, unsigned int color)
{
	int textureID = g->tm.AddObject(textureName);
	CTexture* texture = g->tm.GetObjectPointer(textureID);
	RegisterTexture(texture);

	ReplaceColorTexture(textureID, color);

	return textureID;
}

bool CTextureManager::InitCompressedTexture(CTexture* texture, void* data, int size)
{
	if (m_textureLoader.LoadFromMemory(texture->GetLoadingInfo()->IsMipMap() ? IMAGE_FORMAT_NEC : IMAGE_FORMAT_NEC_NO_MIPMAPS, data, size, 0, false))
	{		
		if (g->rs.GetBool(USE_VBO))
		{
			GLFUNC(glBindBufferARB)(GL_ELEMENT_ARRAY_BUFFER, 0);
			g->stp.SetVertexBuffer(-1, 0);
			g->stp.SetIndexBuffer(-1);
			g->stp.SetNormalBuffer(-1);
			g->stp.SetTextureCoordsBuffer(0, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
		}
		g->stp.PrepareForRender();

		std::wstring name = texture->GetPath();
		if ((name.find(L"geometry") != -1) && (name.find(L"prev") == -1))
		{
			texture->GetLoadingInfo()->SetMarkNonPOT(true);
		}		

		int minFilter, magFilter;
		m_textureLoader.LoadToOpenGL(texture->GetNumber(), texture->GetLoadingInfo(), minFilter, magFilter);
		texture->UpdateCurrentFiltration(minFilter, magFilter);
		
		if (g->rs.GetBool(USE_VBO))
		{
			GLFUNC(glBindBufferARB)(GL_ELEMENT_ARRAY_BUFFER, 0);
			g->stp.SetVertexBuffer(-1, 0);
			g->stp.SetIndexBuffer(-1);
			g->stp.SetNormalBuffer(-1);
			g->stp.SetTextureCoordsBuffer(0, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
		}

		if (g->rs.GetBool(SKIP_FRAMES_TEXTURE_LOADING))
		{
			texture->SetPassFrameCount(5);
		}

		GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);
		g->tm.RefreshTextureOptimization();
	}
	else
	{
		return false;
	}
		
	texture->SetTextureWidth(m_textureLoader.GetWidth());
	texture->SetTextureHeight(m_textureLoader.GetHeight());
	texture->SetLoadedStatus(TEXTURE_LOADED);
	texture->SetYInvertStatus(false);

	texture->GetLoadingInfo()->SetMipMapStatus(m_textureLoader.GetFrameCount() != 1);
	texture->SetTransparentStatus(m_textureLoader.GetChannelCount() == 4);
	texture->SetColor(255, 255, 255);

	return true;
}

void CTextureManager::InitTexture(CTextureLoader* textureLoader, CTexture* texture, bool isMipmap, bool isMaskNeeded, std::string* /*cacheFileName*/)
{
	m_lastWidth = textureLoader->GetWidth();
	m_lastHeight = textureLoader->GetHeight();
	m_canBeCompressed = texture->GetLoadingInfo()->CanTextureBeCompressed();

	if (texture->GetKeyColorType() == KEY_COLOR_FROM_OPTIONS)
	{
		unsigned char r, g, b;
		texture->GetColor(r, g, b);
		textureLoader->ApplyKeyColor(r, g, b);
	}
	else if (texture->GetKeyColorType() == KEY_COLOR_FROM_IMAGE)
	{
		textureLoader->ApplyKeyColorFromImage();
	}
	
	bool isTransparent = (textureLoader->GetChannelCount() == 4);

	if (textureLoader->GetFrameCount() == 6)
	{
		GLuint textureID;
		GLFUNC(glGenTextures)(1, &textureID);
		int minFilter, magFilter;
		textureLoader->LoadToOpenGL(textureID, texture->GetLoadingInfo(), minFilter, magFilter);
		texture->UpdateCurrentFiltration(minFilter, magFilter);
		g->cmm.AddCubeMap(texture->GetName(), textureID);
	}
	else if (!texture->IsKeepInSystemMemory())
	{
		std::wstring name = texture->GetPath();
		if ((name.find(L"geometry") != -1) && (name.find(L"prev") == -1))
		{
			texture->GetLoadingInfo()->SetMarkNonPOT(true);
		}
		int minFilter, magFilter;
		textureLoader->LoadToOpenGL(texture->GetNumber(), texture->GetLoadingInfo(), minFilter, magFilter);
		texture->UpdateCurrentFiltration(minFilter, magFilter);
				
		if (isMaskNeeded)
		{
			unsigned char *mask = NULL;
			unsigned int maskSize = 0;
			textureLoader->GetMask(&mask, maskSize);
			texture->SetMask(mask, maskSize);
		}
	}
	else
	{
		int dataSize = textureLoader->GetDataSize(0);
		void* temp = MP_NEW_ARR(unsigned char, dataSize);
		memcpy(temp, textureLoader->GetData(0), dataSize);
		texture->SetRawData(temp, dataSize, textureLoader->IsDirectChannelOrder());
	}

	texture->SetLoadedStatus(TEXTURE_LOADED);
	texture->SetYInvertStatus(false);
	texture->GetLoadingInfo()->SetMipMapStatus(isMipmap);
	texture->SetTransparentStatus(isTransparent);
	texture->SetColor(255, 255, 255);

	if (texture->IsNeedPreMultiply())
	{
		texture->PremultiplyAlpha();
	}

	g->tm.RefreshTextureOptimization();
}

int counter = 0;

bool CTextureManager::LoadFromIFile(CTexture* texture, ifile* file, bool isMipmap, bool isMaskNeeded)
{
	bool isCompressedExists = false;
	std::string cacheFileName = GetCacheName(file, true);

	if (texture->GetLoadingInfo()->CanTextureBeCompressed())
	if (g->gi.IsSaveCompressionSupported())
	if (g->rs.GetInt(TEXTURE_SIZE_DIV) == 1)
	if ((g->dc.CheckIfDataExists(cacheFileName)) && (isMipmap))
	{
		isCompressedExists = true;	
	}

	std::wstring _path = file->get_file_path();
	if ((!isMipmap) && (!texture->GetLoadingInfo()->CanTextureBeCompressed()))
	{
		unsigned long bufSize = 0;
		const void* buf = file->get_file_data( &bufSize);
		assert(buf);
		assert(bufSize > 0);

		if (!m_textureLoader.LoadTexture(_path, buf, bufSize))
		{
			texture->SetLoadedStatus(TEXTURE_LOAD_ERROR);
			texture->OnAfterChanges(0);
			g->lw.WriteLn("[LOADWARNING] error loading ", file->get_file_path());
			file->free_file_data();
			return false;
		}
		else
		{ 
			if (g->rs.GetInt(TEXTURE_SIZE_DIV) >= 2)
			if (texture->GetLoadingInfo()->CanBeResized())
			{
				int sizeDiv = g->rs.GetInt(TEXTURE_SIZE_DIV);
				if (sizeDiv > texture->GetMaxDecreaseKoef())
				{
					sizeDiv = texture->GetMaxDecreaseKoef();
				}
				m_textureLoader.Resize(m_textureLoader.GetWidth() / sizeDiv, m_textureLoader.GetHeight() / sizeDiv);				
			}

			texture->SetTextureWidth(m_textureLoader.GetWidth());
			texture->SetTextureHeight(m_textureLoader.GetHeight());
			
			if (((!g->gi.IsNPOTSupport()) || ((texture->GetLoadingInfo()->CanBeCompressed(4, 1)) && (g->gi.GetVendorID() == VENDOR_ATI))) && (!isMipmap))
			{
				m_textureLoader.EnlargeTexture();
			}

			texture->SetTextureRealWidth(m_textureLoader.GetWidth());
			texture->SetTextureRealHeight(m_textureLoader.GetHeight());
		}

		InitTexture(&m_textureLoader, texture, isMipmap, isMaskNeeded);

		texture->OnAfterChanges();

		file->free_file_data();

		return true;
	}

	SLoadingTexture* loadingTexture = MP_NEW(SLoadingTexture);
	loadingTexture->texture = texture;
	loadingTexture->textureLoader = &m_bgTextureLoader;
	if (!isCompressedExists)
	{
		loadingTexture->size = file->get_file_size();
		loadingTexture->data = MP_NEW_ARR(char, loadingTexture->size);
		file->read((unsigned char *)loadingTexture->data, loadingTexture->size);
	}
	else
	{
		loadingTexture->size = 0;
		loadingTexture->data = NULL;		
	}
	loadingTexture->isMipmap = isMipmap;
	loadingTexture->isMaskNeeded = isMaskNeeded;
	loadingTexture->path = file->get_file_path();
	loadingTexture->cacheFileName = cacheFileName;
	loadingTexture->isCompressedExists = isCompressedExists;

	m_loadingTextures.push_back(loadingTexture);

	return true;
}

bool CTextureManager::LoadCompressed(CTexture* texture, std::string& cacheFileName)
{
	if (!g->dc.CheckIfDataExists(cacheFileName))
	{
		return false;
	}

	CReadingCacheResourceTask* task = MP_NEW(CReadingCacheResourceTask);
	task->Rewind();
	task->SetUserData(texture);
	task->SetFileName(cacheFileName);

	m_readCompressedTexturesTasks.push_back(task);
	g->taskm.AddTask(task, MUST_BE_MULTITHREADED, PRIORITY_HIGH, true);

	return true;
}

bool CTextureManager::LoadCompressed(CTexture* texture, unsigned char* data, unsigned int size)
{
	bool res = InitCompressedTexture(texture, data, size);
	MP_DELETE_ARR(data);

	if (res)
	{
		texture->OnAfterChanges();

		return true;
	}
	else
	{
		return false;
	}
}

int CTextureManager::LoadTextureFromFile(std::string &name, std::string &path)
{
	std::wstring wpath = CharToWide(path.c_str());
	return LoadTextureFromFile( name, wpath);
}

int CTextureManager::LoadTextureFromFile(std::string &name, std::wstring &path)
{
	int textureID = g->tm.AddObject(name);
	CTexture* texture = g->tm.GetObjectPointer(textureID);
	texture->SetPath(path);
	MP_NEW_P(_path, std::wstring, path);
	texture->SetUserData(USER_DATA_SRC, _path);
	/*if (texture->GetUserData(USER_DATA_SRC))
	{
		std::string* tmp = (std::string *)texture->GetUserData(USER_DATA_SRC);
		MP_DELETE(tmp);	
	}*/

	if (!m_textureLoader.LoadTexture(path))
	{
		texture->SetLoadedStatus(TEXTURE_LOAD_ERROR);
		texture->OnAfterChanges(0);
		g->lw.WriteLn("[LOADWARNING] error loading ", path);
		return -1;
	}
	else
	{
		texture->SetTextureWidth(m_textureLoader.GetWidth());
		texture->SetTextureHeight(m_textureLoader.GetHeight());
		texture->SetTextureRealWidth(m_textureLoader.GetWidth());
		texture->SetTextureRealHeight(m_textureLoader.GetHeight());
	}

	RegisterTexture(texture);

	InitTexture(&m_textureLoader, texture);

	return textureID;
}

bool CTextureManager::SaveTexture(int textureID, std::wstring& fileName, SCropRect* const rect)
{
	return SaveTexture(GetObjectPointer(textureID), fileName, rect);
}

bool CTextureManager::SaveTexture(CTexture *texture, std::wstring& fileName, SCropRect* const rect)
{
	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();
	int num = texture->GetNumber();
	bool isTextureTransparent = texture->IsTransparent();
	m_textureSaver.SetForceGrayscale(g->gi.IsGetTexImageBroken());

	if (rect)
	{
		if (rect->x + rect->width > width)
		{
			rect->width = width - rect->x - 1;
		}
	
		if (rect->y + rect->height > height)
		{
			rect->height = height - rect->y - 1;
		}
	}

	return m_textureSaver.SaveTexture(num, width, height, isTextureTransparent, fileName, rect);
}

bool CTextureManager::SaveTexture(int openGLTextureID, unsigned int width, unsigned int height, bool isTransparent, std::wstring fileName)
{
	return m_textureSaver.SaveTexture(openGLTextureID, width, height, isTransparent, fileName, NULL);
}

bool CTextureManager::CompressImageData(int pictureWidth, int pictureHeight, int channelsCount,  void* pictureData
									  , void** pictureDataOut, unsigned long* dataSize, int quality)
{
	return m_textureSaver.SaveTexture(IMAGE_FORMAT_JPEG, pictureWidth, pictureHeight, channelsCount
											, pictureData, pictureDataOut, dataSize, quality);
}

bool CTextureManager::StretchCompressedImageData(int srcX, int srcY, int srcWidth, int srcHeight, int destWidth
												 , int destHeight, void* pictureData
												 , void** pictureDataOut, unsigned long* dataSize, int quality)
{
	return m_textureSaver.StretchTexture(srcX, srcY, srcWidth, srcHeight, destWidth, destHeight
		, pictureData, pictureDataOut, dataSize, quality);
}

void CTextureManager::LoadAllExternal()
{
	m_errorTextureID = g->tm.GetCount();
	CXMLList texturesList(GetApplicationRootDirectory() + L"textures.xml", true);

	std::string textureName;
	bool isSuccess = texturesList.GetNextValue(textureName);

	bool cubeMapLoading = FALSE;
	int cubeMapTextures = 6;

	USES_CONVERSION;
	while (isSuccess)
	{
		std::wstring fileName = GetApplicationRootDirectory();
		fileName += L"\\external\\textures\\";
		fileName += A2W(textureName.c_str());

		if (fileName[fileName.size() - 1] != L'i')
		{
			fileName = fileName.substr(0, fileName.size() - 1);
		}

		CIniFile iniFile(fileName);

		std::string fn = iniFile.GetString("MAIN", "fileName");

		if (fn.empty())
		{
			g->lw.WriteLn("[ERROR] fileName property in external texture description ", fileName,  " is empty!");
			isSuccess = texturesList.GetNextValue(textureName);
			continue;
		}

		if (g->cmm.IsObjectExists(fn))
		{
			cubeMapLoading = TRUE;
			cubeMapTextures = 6;

			
			{
				GLFUNC(glGenTextures)(1, &m_cubeMapNum);

				g->cmm.AddCubeMap(fn, m_cubeMapNum);

				GLFUNC(glBindTexture)(GL_TEXTURE_CUBE_MAP_ARB, m_cubeMapNum);
				GLFUNC(glPixelStorei)(GL_UNPACK_ALIGNMENT, 1);

				GLFUNC(glTexParameterf)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				GLFUNC(glTexParameterf)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				GLFUNC(glTexParameteri)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			}
		}

		if (cubeMapLoading == TRUE)
		{
			cubeMapTextures--;
		}

		bool isMipmap = iniFile.GetBool("PROPERTIES", "isMipmap");
		bool isTransparent = iniFile.GetBool("PROPERTIES", "isTransparent");
		bool isNotCompressed = iniFile.GetBool("PROPERTIES", "isNotCompressed");

		unsigned char r = (unsigned char)iniFile.GetInt("DATA", "transparentR");
		unsigned char gg = (unsigned char)iniFile.GetInt("DATA", "transparentG");
		unsigned char b = (unsigned char)iniFile.GetInt("DATA", "transparentB");

		fileName = GetApplicationRootDirectory();
		fileName += L"\\external\\textures\\";
		fileName += A2W( fn.c_str());


		int ml = GetObjectNumber(fn);

		// такая текстура уже есть?
		// тогда переназначим её
		if (ml == -1)
		{
			ml = AddObject(fn);
		}
		
		CTexture* texture = GetObjectPointer(ml);
		RegisterTexture(texture);

		if (!m_textureLoader.LoadTexture(fileName))
		{
			g->lw.WriteLn("[ERROR] error loading ", fn);
			isSuccess = texturesList.GetNextValue(textureName);
			continue;
		}
		else
		{
			texture->SetTextureWidth(m_textureLoader.GetWidth());
			texture->SetTextureHeight(m_textureLoader.GetHeight());
			texture->SetTextureRealWidth(m_textureLoader.GetWidth());
			texture->SetTextureRealHeight(m_textureLoader.GetHeight());
		}

		m_lastWidth = m_textureLoader.GetWidth();
		m_lastHeight = m_textureLoader.GetHeight();

		bool oldRGBCompressed = false;
		bool oldRGBACompressed = false;
		if (isNotCompressed)
		{
			oldRGBCompressed = g->rs.GetBool(NORMAL_TEXTURES_PACKING);
			oldRGBACompressed = g->rs.GetBool(TRANSPARENT_TEXTURES_PACKING);
			g->rs.SetBool(NORMAL_TEXTURES_PACKING, false);
			g->rs.SetBool(TRANSPARENT_TEXTURES_PACKING, false);
		}
		else
		{
			texture->GetLoadingInfo()->SetCanBeCompressed(true);
		}

		if (!cubeMapLoading)
		{
			texture->GetLoadingInfo()->SetFilteringMode(FILTERING_MODE_LINEAR);
			int minFilter, magFilter;
			m_textureLoader.LoadToOpenGL(texture->GetNumber(), texture->GetLoadingInfo(), minFilter, magFilter);
			texture->UpdateCurrentFiltration(minFilter, magFilter);
		}
		else
		{
			SetCubeMapTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + (5 - cubeMapTextures), m_textureLoader.IsDirectChannelOrder() ? GL_RGB : GL_BGR_EXT, true, (void *)m_textureLoader.GetData(0));
		}

		if (isNotCompressed)
		{
			g->rs.SetBool(NORMAL_TEXTURES_PACKING, oldRGBCompressed);
			g->rs.SetBool(TRANSPARENT_TEXTURES_PACKING, oldRGBACompressed);
		}

		texture->SetYInvertStatus(false);
		texture->SetLoadedStatus(TEXTURE_LOADED);

		if ((cubeMapTextures == 0) && (cubeMapLoading == TRUE))
		{
			cubeMapLoading = FALSE;

			GLFUNC(glEnable)(GL_TEXTURE_CUBE_MAP_ARB);

			GLFUNC(glTexParameteri)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			GLFUNC(glTexParameteri)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			GLFUNC(glDisable)(GL_TEXTURE_CUBE_MAP_ARB);

			GLFUNC(glPixelStorei)(GL_UNPACK_ALIGNMENT, 4);
		}

		texture->SetYInvertStatus(false);

		texture->GetLoadingInfo()->SetMipMapStatus(isMipmap);
		texture->SetTransparentStatus(isTransparent);

		texture->SetColor(r, gg, b);

		g->lw.WriteLn("Loaded ", fileName, " - ok");
 
		isSuccess = texturesList.GetNextValue(textureName);
	}

	m_greyTextureID = AddColorTexture("#" + ULongToStr(0xFF777777));
	m_whiteTextureID = AddColorTexture("#" + ULongToStr(0xFFFFFFFF));

	std::string  textureFile;

	std::vector<std::string> optionalFields;
	optionalFields.push_back("deltaTime");
	CXMLList textureList(GetApplicationRootDirectory() + L"avatar_show.xml", optionalFields, true);

	std::string deltaTime = "";
	textureList.GetCommonOptionalFieldValue("deltaTime", deltaTime);

	m_deltaTime = rtl_atoi(deltaTime.c_str());

	while (textureList.GetNextValue(textureFile))
	{
		m_avatarTextureID.push_back(g->tm.GetObjectNumber(textureFile));
	}

	m_commonTime = m_avatarTextureID.size()*m_deltaTime;
}	

int CTextureManager::GetAvatarTextureID()
{
	if (m_deltaTime > 0)
	{
		int id;

		if (m_startTexturesChanged == -1)
			m_startTexturesChanged = g->ne.time;

		id = ((g->ne.time - m_startTexturesChanged)%m_commonTime)/m_deltaTime;

		return m_avatarTextureID[id];
	}

	return m_greyTextureID;
}

void CTextureManager::SetCurrentMipMapBias(float mipmapBias)
{
	if (m_currentMipmapBias != mipmapBias)
	{
		m_currentMipmapBias = mipmapBias;
		GLFUNC(glTexEnvf)(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, -mipmapBias);
	}
}

void CTextureManager::BindTexture(int textureID)
{
	if (g->rs.GetBool(DISABLE_TEXTURES))
	{
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, GetObjectPointer(m_errorTextureID)->GetNumber());
		return;
	}

	int textureLevel = g->stp.GetActiveTexture();
	if (textureID == m_lastTextureID[textureLevel]) 
	{
		if (0 == textureLevel)
		{
			CTexture* texture = GetObjectPointer(textureID);
			if (m_currentMipmapBias != texture->GetMipmapBias())
			{			
				m_currentMipmapBias = texture->GetMipmapBias();
				GLFUNC(glTexEnvf)(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, -texture->GetMipmapBias());
			}
		}

		return;
	}

	CTexture* texture = GetObjectPointer(textureID);	

	texture->SetBindedStatus(true);

	if (texture->GetPassFrameCount() > 0)
	{	
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, GetObjectPointer(m_greyTextureID)->GetNumber());
		m_lastTextureID[textureLevel] = textureID;
		return;
	}

	if (texture->IsDeleted())
	{	
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, GetObjectPointer(m_whiteTextureID)->GetNumber());
		m_lastTextureID[textureLevel] = textureID;
		return;
	}

	if (0 == textureLevel)
	{
		if (m_currentMipmapBias != texture->GetMipmapBias())
		{
			GLFUNC(glTexEnvf)(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, -texture->GetMipmapBias());
			m_currentMipmapBias = texture->GetMipmapBias();
		}

		if (m_occlusionTexture)
		{
			m_occlusionTexture->EndOcclusionQuery();
			m_occlusionTexture = NULL;
		}

		if ((m_isCheckingVisibleDynamicTextures) || (!texture->GetDynamicTexture()))
		{
			if (((texture->IsNeededToCheckVisibility()) && (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)) && (texture->IsBinded()))
			{
				texture->StartOcclusionQuery();
				m_occlusionTexture = texture;
			}
		}
	}

	GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
	m_lastTextureID[textureLevel] = textureID;

	/*GLint a, b;
	//glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &a);
	//glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &b);
	//GLfloat c;
	//glGetTexEnvfv(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, &c);
	if (texture->GetMinFilter() != a)
	{
		assert(false);
	}
	if (texture->GetMagFilter() != b)
	{
		assert(false);
	}
	if (c != -texture->GetMipmapBias())
	{
		assert(false);
	}*/
}

void CTextureManager::RestoreDefaultSettings()
{
	g->stp.SetState(ST_BLEND, false);
}

void CTextureManager::BeforeNewFrame()
{
	RefreshTextureOptimization();

	static int callCount = 0;
	std::vector<CTexture*>::iterator it = m_dynamicTextures.begin();
	std::vector<CTexture*>::iterator itEnd = m_dynamicTextures.end();

	if (callCount%10 == 0)
	{
		for ( ; it != itEnd; it++)
		{
			(*it)->SetBindedStatus(false);
		}
	}

	m_isCheckingVisibleDynamicTextures = (callCount%4 == 1);

	if (callCount%10 == 9)
	{
		for ( ; it != itEnd; it++)
		if ((*it)->IsBinded())
		{
			(*it)->SetReallyUsedStatus((*it)->GetOcclusionQueryResult());
		}
		else
		{
			(*it)->SetReallyUsedStatus(false);
		}
	}

	if (g->rs.GetBool(SKIP_FRAMES_TEXTURE_LOADING))
	{
		it = GetLiveObjectsBegin();
		itEnd = GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		if ((*it)->GetPassFrameCount() > 0)
		{
			(*it)->SetPassFrameCount((*it)->GetPassFrameCount() - 1);
		}
	}

	callCount++;
}

void CTextureManager::AfterNewFrame()
{
	RefreshTextureOptimization();
}

void CTextureManager::RefreshTextureOptimization()
{
	for (int k = 0; k < 8; k++)
	{
		m_lastTextureID[k] = -1;
	}
}

void CTextureManager::DisableMultitexturing2TL()
{
	g->stp.SetActiveTexture(1);
	GLFUNC(glDisable)(GL_TEXTURE_2D);
	
	g->stp.SetActiveTexture(0);
	GLFUNC(glEnable)(GL_TEXTURE_2D);
}

void CTextureManager::DisableMultitexturing3TL()
{
	g->stp.SetActiveTexture(2);
	GLFUNC(glDisable)(GL_TEXTURE_2D);

	g->stp.SetActiveTexture(1);
	GLFUNC(glDisable)(GL_TEXTURE_2D);

	g->stp.SetActiveTexture(0);
	GLFUNC(glEnable)(GL_TEXTURE_2D);
}

void CTextureManager::SetTextureForLevel(int level, int mode, int textureID)
{
	g->stp.SetActiveTexture((unsigned char)level);

	GLFUNC(glEnable)(GL_TEXTURE_2D);
	GLFUNC(glTexEnvi)(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);

	BindTexture(textureID);
}

void CTextureManager::SetTextureForLevel(int level, int mode, std::string fileName)
{
	int fl = GetObjectNumber(fileName);

	SetTextureForLevel(level, mode, fl);
}

int CTextureManager::GetObjectNumber(std::string name)
{
	return CManager<CTexture>::GetObjectNumber(name);
}

int CTextureManager::GetObjectNumber(std::wstring name)
{
	return CManager<CTexture>::GetObjectNumber(name);
}

int CTextureManager::GetCurrentTextureID()const
{
	return m_lastTextureID[0];
}

int CTextureManager::GetTextureIDByOGLID_DEBUG(int oglTextureID)
{
	std::vector<CTexture *>::iterator it = GetLiveObjectsBegin();
	std::vector<CTexture *>::iterator itEnd = GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	if ((*it)->GetNumber() == oglTextureID)
	{
		return (*it)->GetID();
	}

	return -1;
}

CTextureManager::~CTextureManager()
{
	std::vector<SLoadingTexture *>::iterator it1 = m_loadingTextures.begin();
	for ( ; it1 != m_loadingTextures.end(); it1++)
	{
		MP_DELETE(*it1);
	}

	std::vector<CLoadingTexturesTask *>::iterator it = m_loadingTexturesTasks.begin();
	for ( ; it != m_loadingTexturesTasks.end(); it++)
	{
		SLoadingTexture* texture = (*it)->GetTexture();
		MP_DELETE(texture);
		MP_DELETE(*it);
	}
}