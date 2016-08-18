#include "stdafx.h"
#include ".\removefilesfromncachetask.h"
#include "GlobalSingletonStorage.h"

CRemoveFilesFromNcacheTask::CRemoveFilesFromNcacheTask(void)
{
}

void CRemoveFilesFromNcacheTask::PrintInfo()
{
	g->lw.WriteLn("Remove Files From Ncache Task ");
}

void CRemoveFilesFromNcacheTask::Start()
{
	std::string filePath = "";
/*	while ((filePath = g->dc.DeleteNextFile()) != "")
	{
		Sleep(1);
	}*/
}

void CRemoveFilesFromNcacheTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CRemoveFilesFromNcacheTask::~CRemoveFilesFromNcacheTask(void)
{
}