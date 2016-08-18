#include "stdafx.h"
#include ".\groupcontroller.h"
#include "GlobalSingletonStorage.h"
#include "RMContext.h"

_GroupInfo::_GroupInfo():
	MP_STRING_INIT(groupName)
{

}

CGroupController::CGroupController():
	MP_MAP_INIT(m_groupList)
{
}

void CGroupController::OnGroup(const std::string& objectName, const std::string& groupName, const int groupType)
{
	C3DObject* obj = GetObjectByName(objectName);
	if (obj!= NULL)
	{
		if (groupName == "")
		{
				obj->SetPivotObject(NULL);
		}
		else if (groupType == vespace::VE_GROUP_PIVOT_TYPE)
		{
			C3DObject* group = GetObjectByName(groupName);

			if (group != NULL)
			{
				obj->SetPivotObject(group);
			}
		}
	}
	
}

void CGroupController::OnChangeGroup(const std::string& groupName, const CVector3D& groupCoord)
{
	C3DObject* obj = GetObjectByName(groupName);
	if (obj == NULL)
	{
		g->o3dm.AddObject(groupName);
		obj = GetObjectByName(groupName);
		obj->SetCoords(groupCoord);
		obj->SetVisible(false);
	}
}

C3DObject* CGroupController::GetObjectByName(const std::string& name)
{
	int objectId = g->o3dm.GetObjectNumber(name);

	C3DObject* obj = NULL;
	if (objectId != ERR_OBJECT_NOT_FOUND)
	{
		obj = g->o3dm.GetObjectPointer(objectId);
	}

	return obj;
}

void CGroupController::AddGroupToList(const unsigned int id, GroupInfo* const groupInfo)
{
	m_groupList.insert(GroupList::value_type(id, groupInfo));
}

GroupInfo* CGroupController::FindGroupInList(const unsigned int id)
{
	GroupList::const_iterator it = m_groupList.find(id);

	if (it != m_groupList.end()) //итератор на элемент за последним, обращаться по нему нельзя
	{
		return (*it).second;
	}
	else
	{
		return NULL;
	}
}

CGroupController::~CGroupController()
{
	std::map<int, GroupInfo* > ::iterator it = m_groupList.begin();
	for ( ; it != m_groupList.end(); it++)
	{
		MP_DELETE_UNSAFE((*it).second);
	}
}
