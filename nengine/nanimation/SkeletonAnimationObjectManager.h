
#pragma once

#include "CommonHeader.h"
#include "Manager.h"
#include "LoadingSAOTask.h"
#include "SkeletonAnimationObject.h"

class CSkeletonAnimationObjectManager : public CManager<CSkeletonAnimationObject>
{
public:
	CSkeletonAnimationObjectManager();
	~CSkeletonAnimationObjectManager();

	int AddObject(C3DObject* obj, const std::string& path);
	int AddObject(C3DObject* obj, const std::wstring& path);
	int ManageObject(CSkeletonAnimationObject* sao, C3DObject* obj, const std::string& path);
	int ManageObject(CSkeletonAnimationObject* sao, C3DObject* obj, const std::wstring& path);
	CSkeletonAnimationObject* AddObjectPtr(C3DObject* obj, const std::string& path);

	int GetObjectNumber(C3DObject* obj, const std::string& path);
	int GetObjectNumber(C3DObject* obj, const std::wstring& path);

	void AddAsynchLoadingTask(CLoadingSAOTask* task);

	void Update();

private:
	MP_VECTOR<CLoadingSAOTask *> m_asynchLoadingTasks;
};