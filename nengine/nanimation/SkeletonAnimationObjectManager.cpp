
#include "StdAfx.h"
#include "SkeletonAnimationObjectManager.h"

CSkeletonAnimationObjectManager::CSkeletonAnimationObjectManager():
	MP_VECTOR_INIT(m_asynchLoadingTasks)
{
}

int CSkeletonAnimationObjectManager::GetObjectNumber(C3DObject* obj, const std::string& path)
{
	USES_CONVERSION;
	std::wstring wpath = A2W( path.c_str());
	return GetObjectNumber( obj, wpath);
}

int CSkeletonAnimationObjectManager::GetObjectNumber(C3DObject* obj, const std::wstring& path)
{
	return CManager<CSkeletonAnimationObject>::GetObjectNumber(((obj) ? obj->GetName() : L"") + path);
}

int CSkeletonAnimationObjectManager::AddObject(C3DObject* obj, const std::string& path)
{
	USES_CONVERSION;
	std::wstring wpath = A2W( path.c_str());
	return AddObject( obj, wpath);
}

int CSkeletonAnimationObjectManager::AddObject(C3DObject* obj, const std::wstring& path)
{
	return CManager<CSkeletonAnimationObject>::AddObject(((obj) ? obj->GetName() : L"") + path);
}

int CSkeletonAnimationObjectManager::ManageObject(CSkeletonAnimationObject* sao, C3DObject* obj, const std::string& path)
{
	USES_CONVERSION;
	std::wstring wpath = A2W( path.c_str());
	return ManageObject( sao, obj, wpath);
}

int CSkeletonAnimationObjectManager::ManageObject(CSkeletonAnimationObject* sao, C3DObject* obj, const std::wstring& path)
{
	sao->SetName(((obj) ? obj->GetName() : L"") + path);
	return CManager<CSkeletonAnimationObject>::ManageObject(sao);
}

CSkeletonAnimationObject* CSkeletonAnimationObjectManager::AddObjectPtr(C3DObject* obj, const std::string& path) 
{
	int saoID = AddObject(obj, path);
	return GetObjectPointer(saoID);
}

void CSkeletonAnimationObjectManager::Update()
{
	if (g->stat.GetSAOQueueTime() > g->rs.GetInt(MAX_SAO_LOADING_FRAME_TIME))
	{
		return;
	}

	g->pm.CustomBlockStart(CUSTOM_BLOCK_SAO_LOADING);

	__int64 t1 = g->tp.GetTickCount();

	std::vector<CLoadingSAOTask *>::iterator it = m_asynchLoadingTasks.begin();
	std::vector<CLoadingSAOTask *>::iterator itEnd = m_asynchLoadingTasks.end();

	for ( ; it != itEnd; it++)
	if ((*it)->IsPerformed())
	{
		bool isNotDead = !(*it)->GetSAO()->IsDeleted();
		STransitionalInfo* info = (*it)->GetTransitionalInfo();
		if (isNotDead)
		{
			bool isLoaded = (*it)->IsLoaded();
			if (isLoaded)
			{
				(*it)->GetSAO()->LoadEngineObjects((*it)->GetParent(), info);
			}
			(*it)->GetSAO()->SetLoadedStatus(isLoaded);
			(*it)->GetSAO()->OnChanged(0);
		}
		else
		{
			(*it)->GetSAO()->FreeResources();
			info->Clear();						
		}
		
		MP_DELETE((*it));//delete (*it);
		m_asynchLoadingTasks.erase(it);
		break;
	}

	__int64 t2 = g->tp.GetTickCount();
	g->stat.AddSAOQueueTime((int)(t2 - t1));

	g->pm.CustomBlockEnd(CUSTOM_BLOCK_SAO_LOADING);
}

void CSkeletonAnimationObjectManager::AddAsynchLoadingTask(CLoadingSAOTask* task)
{
	m_asynchLoadingTasks.push_back(task);
}

CSkeletonAnimationObjectManager::~CSkeletonAnimationObjectManager()
{
}