
#pragma once

#include "SkeletonAnimationObject.h"
#include "CommonHeader.h"
#include "ITask.h"

class CLoadingSAOTask : public ITask
{
public:
	CLoadingSAOTask();
	~CLoadingSAOTask();

	void SetParams(CSkeletonAnimationObject* sao, const std::string& path, const bool isBlend, CSkeletonAnimationObject* const parent);

	void PrintInfo();
	void Start();

	STransitionalInfo* GetTransitionalInfo();
	std::string GetPath();
	CSkeletonAnimationObject* GetParent();
	CSkeletonAnimationObject* GetSAO();
	bool IsLoaded()const;

	void Destroy() { assert(true); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	bool m_isBlend;
	bool m_isLoaded;

	MP_STRING m_path;
	STransitionalInfo* m_info;
	
	CSkeletonAnimationObject* m_sao;
	CSkeletonAnimationObject* m_parent;
};