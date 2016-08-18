
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "LoadingMotionTask.h"

CLoadingMotionTask::CLoadingMotionTask():
	m_motion(NULL),
	MP_STRING_INIT(m_path)
{
	OnDone();
}

void CLoadingMotionTask::PrintInfo()
{
	g->lw.WriteLn("LoadingMotionTask ", m_path);
}

void CLoadingMotionTask::SetParams(CMotion* const motion, const std::string& path)
{
	m_motion = motion;
	m_path = path;
}

void CLoadingMotionTask::Start()
{
	if ((!m_motion) || (m_path.size() == 0))
	{
		return;
	}

	if (!m_motion->LoadFromFile(m_path))
	{
		return;
	}

	g->motm.AddCalCoreAnimation(m_path, m_motion->GetCoreAnimation());
	m_motion->SetLoadedStatus(true);
}

void CLoadingMotionTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CLoadingMotionTask::~CLoadingMotionTask()
{
}