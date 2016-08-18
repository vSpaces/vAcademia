
#pragma once

#include "CommonHeader.h"
#include "ITask.h"
#include "SkeletonAnimationObject.h"

class CUpdateSkeletonAnimationTask : public ITask
{
public:
	CUpdateSkeletonAnimationTask();
	~CUpdateSkeletonAnimationTask();

	void AddAnimation(CSkeletonAnimationObject* const sao);
	void RemoveAnimation(CSkeletonAnimationObject* const sao);

	void SetParams(const int deltaTime, const float distSq);

	void Start();

	void PrintInfo();

	void Destroy() { assert(false); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	MP_VECTOR<CSkeletonAnimationObject*> m_saoList;
	int m_deltaTime;
	float m_distSq;

	CRITICAL_SECTION m_cs;
};