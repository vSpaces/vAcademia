
#pragma once

#include "CommonHeader.h"
#include "Motion.h"
#include "Manager.h"
#include "LoadingMotionTask.h"
#include "PreloadMotionsTask.h"
#include <map>

typedef struct _SAsynchMotionDesc
{
	CMotion* motion;
	MP_STRING path;
	CLoadingMotionTask* task;

	_SAsynchMotionDesc(CMotion* const _motion, const std::string& _path, CLoadingMotionTask* const _task);
} SAsynchMotionDesc;

typedef struct _SCalCoreAnimation
{
	unsigned int refCount;
	CalCoreAnimation* animation;

	_SCalCoreAnimation()
	{
		refCount = 0;
		animation = NULL;
	}
} SCalCoreAnimation;

class CMotionManager : public CManager<CMotion>
{
friend class CGlobalSingletonStorage;

public:
	void AddCalCoreAnimation(const std::string& name, CalCoreAnimation* const animation);
	CalCoreAnimation* GetCalCoreAnimation(const std::string& name);
	bool ReleaseCalCoreAnimation(const std::string& name);

	void AddAsynchMotion(CMotion* const motion, const std::string& path);

	void PreloadMotions();
	void PreloadMotion(const std::string& fileName, const std::string& path);
	void PreloadMotions(const std::string path, const std::string ext);
	void AsyncPreloadMotions();
	
	bool IsMotionsPreloaded();

	void CorrectPath(std::string& path);

	void Update();

private:
	CMotionManager();
	CMotionManager(const CMotionManager&);
	CMotionManager& operator=(const CMotionManager&);
	~CMotionManager();

	CRITICAL_SECTION m_animationMapAccessCS;

	MP_STRING m_motionsBasePath;
	MP_MAP<MP_STRING, SCalCoreAnimation*> m_animationMap;
	MP_VECTOR<SAsynchMotionDesc *> m_asynchMotionDescs;

	CPreloadMotionsTask m_preloadMotionsTask;
};