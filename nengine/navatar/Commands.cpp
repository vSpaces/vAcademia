
#include "StdAfx.h"
#include "Commands.h"

#define CHECK_TIME			1000

_SCompositeCommand::_SCompositeCommand():
	MP_STRING_INIT(command),
	MP_STRING_INIT(commandPath),
	MP_STRING_INIT(commandParam),
	MP_STRING_INIT(commandValue)
{
	commandID = 0;
}

_SCompositeCommand::_SCompositeCommand(const _SCompositeCommand& other):
	MP_STRING_INIT(command),
	MP_STRING_INIT(commandPath),
	MP_STRING_INIT(commandParam),
	MP_STRING_INIT(commandValue)
{
	commandID = other.commandID;
	command = other.command;
	commandParam = other.commandParam;
	commandValue = other.commandValue;
	commandPath = other.commandPath;
}

CCommandCache::CCommandCache():
	m_lastUpdateTime(0),
	m_isEditorVisible(false),
	MP_VECTOR_INIT(m_compositeTextureCommands)
{
}

void CCommandCache::ClearAll()
{
	m_compositeTextureCommands.clear();
}

void CCommandCache::SetAvatar(ICommandPerformer* avatar)
{
	m_avatar = avatar;
}

void CCommandCache::SetCompositeTextureManager(CCompositeTextureManager* manager)
{
	m_compositeTextureManager = manager;
}

void CCommandCache::RefreshTime()
{
	m_lastUpdateTime = g->ne.time - CHECK_TIME;
}

void CCommandCache::ExecuteQueueImpl()
{
	if ((g->ne.time - m_lastUpdateTime > CHECK_TIME) || (m_compositeTextureManager->IsMyAvatar()))
	{
		bool isLastOperationSuccessful = true;
		bool m_isChanged = false;

		while ((m_compositeTextureCommands.size() > 0) && (isLastOperationSuccessful))
		{
			isLastOperationSuccessful = false;

			unsigned int index = 0;

			while ( (index < m_compositeTextureCommands.size()) &&
				(COMMAND_ID_CREATE_COMPOSITE_TEXTURE == m_compositeTextureCommands[index].commandID) &&
				(m_compositeTextureManager->GetHeadID() != rtl_atoi(m_compositeTextureCommands[index].commandParam.c_str())))
			{
				index++;
			}

			if (index == m_compositeTextureCommands.size())
			{
				return;
			}

			if (COMMAND_ID_CREATE_COMPOSITE_TEXTURE == m_compositeTextureCommands[index].commandID)
			{
				std::string str = m_compositeTextureCommands[index].command;
				int head = rtl_atoi(m_compositeTextureCommands[index].commandParam.c_str());
				int pos0 = str.find("*");
				int pos = str.find("*", pos0 + 1);
				int pos2 = str.find("*", pos + 1);

				std::string mainTextureName = "";
				for (int i = pos + 1; i < pos2; i++)
				{
					mainTextureName += str[i];
				}

				std::string mainTextureName2 = "";
				for (int i = pos0 + 1; i < pos; i++)
				{
					mainTextureName2 += str[i];
				}				
			
				int textureID = g->tm.GetObjectNumber(mainTextureName.c_str());
				int textureID3 = g->tm.GetObjectNumber(mainTextureName2.c_str());
				if ((textureID != -1) && (textureID3 != -1) && (g->stp.GetCurrentHeight() >= 480))
				{
					CTexture* texture = g->tm.GetObjectPointer(textureID);
					CTexture* texture3 = g->tm.GetObjectPointer(textureID3);
					if (texture->GetTextureWidth() > 0)
					if (texture3->GetTextureWidth() > 0)
					{	
						m_compositeTextureManager->AddCompositeTexture(head, str);
						m_compositeTextureCommands.erase(m_compositeTextureCommands.begin() + index);
						isLastOperationSuccessful = true;
						m_isChanged = true;
						m_avatar->ApplyDefaultSettings();
					}
				}
			}
			else if (m_compositeTextureManager->IsLoaded())
			{
				unsigned int oldSize = m_compositeTextureCommands.size();

				m_avatar->HandleSetParameter(m_compositeTextureCommands[index].command, m_compositeTextureCommands[index].commandPath, m_compositeTextureCommands[index].commandParam, m_compositeTextureCommands[index].commandValue);

				m_compositeTextureCommands.erase(m_compositeTextureCommands.begin() + index);

				if (oldSize == m_compositeTextureCommands.size())
				{
					m_compositeTextureCommands.erase(m_compositeTextureCommands.end() - 1);
				}

				m_avatar->ApplyDefaultSettings();

				isLastOperationSuccessful = true;
				m_isChanged = true;
			}			
		}

		m_lastUpdateTime = g->ne.time;		
	}	
}

void CCommandCache::SetEditorVisibleStatus(const bool isEditorVisible)
{
	m_isEditorVisible = isEditorVisible;
}

void CCommandCache::ExecuteQueue()
{
	if (!m_avatar)
	{
		return;
	}

	if (!m_compositeTextureManager->IsNeeded())
	{
		return;
	}

	if ((g->stat.GetCompositeTexturesQueueTime() > g->rs.GetInt(MAX_COMPOSITE_TEXTURES_FRAME_TIME))
		&& (!m_isEditorVisible) && (!m_compositeTextureManager->IsMyAvatar()))
	{
		return;
	}
	
	__int64 t1 = g->tp.GetTickCount();
	m_compositeTextureManager->SetUpdatePermission(false);
	unsigned int oldQueueSize = m_compositeTextureCommands.size();
	ExecuteQueueImpl();
	m_compositeTextureManager->SetUpdatePermission(true);
	CReadingCacheFileTask* cacheTask = m_compositeTextureManager->GetReadingCacheTask();
	bool isNeedToCreateNewTask = (m_compositeTextureCommands.size() != oldQueueSize);
	if ((isNeedToCreateNewTask) || (cacheTask))
	{		
		if ((cacheTask) && (cacheTask->GetFileName() != m_compositeTextureManager->GetCacheFileName(0)))
		{
			if ((cacheTask->IsPerformed()) || (g->taskm.RemoveTask(cacheTask)))
			{
				if (cacheTask->GetData())
				{
					MP_DELETE_ARR_UNSAFE(cacheTask->GetData());
				}

				MP_DELETE(cacheTask);
			}
			else
			{
				cacheTask->DestroyResultOnEnd();
			}

			cacheTask = NULL;
			m_compositeTextureManager->SetReadingCacheTask(NULL);
		}

		if (isNeedToCreateNewTask)
		if (!cacheTask)
		{
			std::string cacheFileName = m_compositeTextureManager->GetCacheFileName(0);
			if (g->dc.CheckIfDataExists(cacheFileName))
			{
				cacheTask = g->dc.GetDataAsynch(cacheFileName);
				m_compositeTextureManager->SetReadingCacheTask(cacheTask);
			}
			else
			{
				m_compositeTextureManager->UpdateTexture(0);					
			}
		}

		if ((cacheTask) && (cacheTask->IsPerformed()))
		{
			m_compositeTextureManager->UpdateTexture(0);		

			cacheTask = m_compositeTextureManager->GetReadingCacheTask();
			if (cacheTask)
			{
				if (cacheTask->GetData())
				{
					MP_DELETE_ARR_UNSAFE(cacheTask->GetData());
				}

				MP_DELETE(cacheTask);
				cacheTask = NULL;
				m_compositeTextureManager->SetReadingCacheTask(NULL);
			}

		}
	}
	m_compositeTextureManager->SetUpdatePermission(true);	
	__int64 t2 = g->tp.GetTickCount();
	g->stat.AddCompositeTexturesQueueTime((int)(t2 - t1));
}
	
bool CCommandCache::IsCommandMustBeCached(std::string& commandID)
{
	return ((("interpolate_texture" == commandID) || ("set_alpharef" == commandID) || 
				("set_diffuse_color" == commandID)) && (m_compositeTextureCommands.size() > 0));
}

void CCommandCache::CacheApplyCommand(std::string& commandID, std::string& commandPath, std::string& commandParam, std::string& commandValue)
{
	SCompositeCommand cmd;
	cmd.command = commandID;
	cmd.commandID = COMMAND_ID_APPLY_COMPOSITE_COMMAND;
	cmd.commandPath = commandPath;
	cmd.commandParam = commandParam;
	cmd.commandValue = commandValue;
	m_compositeTextureCommands.push_back(cmd);
}

void CCommandCache::CacheCreateCommand(std::string& headID, std::string& commandID)
{
	SCompositeCommand cmd;
	cmd.command = commandID;
	cmd.commandParam = headID;
	cmd.commandID = COMMAND_ID_CREATE_COMPOSITE_TEXTURE;
	m_compositeTextureCommands.push_back(cmd);
}

std::string CCommandCache::GetValueLastCachedCommand(std::string commandID)
{
	std::string res = "";

	std::vector<SCompositeCommand>::iterator it = m_compositeTextureCommands.begin();
	std::vector<SCompositeCommand>::iterator itEnd = m_compositeTextureCommands.end();

	for ( ; it != itEnd; it++)
	if ((*it).command == commandID)
	{
		res = (*it).commandValue;
	}

	return res;
}

CCommandCache::~CCommandCache()
{
}