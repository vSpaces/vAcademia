#include "stdafx.h"
#include ".\preloadblendstask.h"
#include "GlobalSingletonStorage.h"

CPreloadBlendsTask::CPreloadBlendsTask(void)
{
}

void CPreloadBlendsTask::PrintInfo()
{
	g->lw.WriteLn("Preloading Blends Task ");
}

void CPreloadBlendsTask::SetParams()
{
}

void CPreloadBlendsTask::Start()
{
	g->bm.PreloadBlends();
}

void CPreloadBlendsTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CPreloadBlendsTask::~CPreloadBlendsTask(void)
{
}