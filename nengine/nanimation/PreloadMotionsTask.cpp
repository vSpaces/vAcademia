#include "stdafx.h"
#include ".\PreloadMotionsTask.h"
#include "GlobalSingletonStorage.h"

CPreloadMotionsTask::CPreloadMotionsTask(void)
{
}

void CPreloadMotionsTask::PrintInfo()
{
	g->lw.WriteLn("Preloading Motions Task ");
}

void CPreloadMotionsTask::SetParams()
{
}

void CPreloadMotionsTask::Start()
{
	g->motm.PreloadMotions();
}

void CPreloadMotionsTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CPreloadMotionsTask::~CPreloadMotionsTask(void)
{
}