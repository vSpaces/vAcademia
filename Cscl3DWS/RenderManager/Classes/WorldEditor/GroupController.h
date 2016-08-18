#pragma once

#include "../CommonRenderManagerHeader.h"

#include "DataStorage.h"
#include "Vector3D.h"
#include "../../nengine/nengine/3DObject.h"
#include <map>

typedef struct _GroupInfo
{
	MP_STRING groupName;		
	int	groupType;

	_GroupInfo();
} GroupInfo;

typedef MP_MAP<int, GroupInfo* > GroupList;

class CGroupController
{
public:
	CGroupController();
	~CGroupController();

	C3DObject* GetObjectByName(const std::string& name);
	void OnGroup(const std::string& objectName, const std::string& groupName, const int groupType);
	void OnChangeGroup(const std::string& groupName, const CVector3D& coord);

	void AddGroupToList(const unsigned int id, GroupInfo*  const groupInfo);
	GroupInfo* FindGroupInList(const unsigned int id);

private:
	GroupList m_groupList;
};
