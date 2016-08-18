
#pragma once

#include "CommonHeader.h"

#include "ITask.h"
#include "SkeletonAnimationObject.h"

class CLoadingMotionTask : public ITask
{
public:
	CLoadingMotionTask();
	~CLoadingMotionTask();

	void SetParams(CMotion* const motion, const std::string& path);

	void Start();

	void PrintInfo();

	void Destroy() { assert(true); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	CMotion* m_motion;
	MP_STRING m_path;
};