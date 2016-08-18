
#pragma once

#include "CommonAvatarHeader.h"

#include <vector>
#include "ICommandPerformer.h"
#include "GlobalSingletonStorage.h"
#include "CompositeTextureManager.h"

#define COMMAND_ID_CREATE_COMPOSITE_TEXTURE	0
#define COMMAND_ID_APPLY_COMPOSITE_COMMAND	1

typedef struct _SCompositeCommand
{
	int commandID;
	MP_STRING command;
	MP_STRING commandPath;
	MP_STRING commandParam;
	MP_STRING commandValue;

	_SCompositeCommand();
	_SCompositeCommand(const _SCompositeCommand& other);
} SCompositeCommand;

class CCompositeTextureManager;

class CCommandCache
{
public:
	CCommandCache();
	~CCommandCache();

	bool IsCommandMustBeCached(std::string& commandID);

	void CacheApplyCommand(std::string& commandID, std::string& commandPath, std::string& commandParam, std::string& commandValue);
	void CacheCreateCommand(std::string& headID, std::string& commandID);

	void SetCompositeTextureManager(CCompositeTextureManager* manager);
	void SetAvatar(ICommandPerformer* avatar);

	void RefreshTime();
	void ExecuteQueue();

	void ClearAll();

	std::string GetValueLastCachedCommand(std::string commandID);

	void SetEditorVisibleStatus(const bool isEditorVisible);

private:
	void ExecuteQueueImpl();

	MP_VECTOR<SCompositeCommand> m_compositeTextureCommands;

	ICommandPerformer* m_avatar;	
	CCompositeTextureManager* m_compositeTextureManager;

	bool m_isEditorVisible;
	__int64 m_lastUpdateTime;
};