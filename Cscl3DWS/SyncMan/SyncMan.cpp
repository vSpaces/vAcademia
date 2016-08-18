// SyncMan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SyncMan.h"
#include "SyncManager.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

	syncSeanceInfo::syncSeanceInfo():
		MP_WSTRING_INIT(mpwcName),
		MP_WSTRING_INIT(mpwcMembers),
		MP_WSTRING_INIT(mpwcAuthor),
		MP_WSTRING_INIT(mpwcURL)
	{
		muSeanceID = 0;
		mlRecordID = 0;
		mlDate = 0;
		mlMode = 999;
	}

	syncSeanceInfo::syncSeanceInfo(unsigned int pSeanceID, unsigned int pRecordID, const wchar_t* pwcName, const wchar_t* pwcMembers, const wchar_t* mpwcAuthor, unsigned long  alDate, unsigned int alMode):
		MP_WSTRING_INIT(mpwcName),
		MP_WSTRING_INIT(mpwcMembers),
		MP_WSTRING_INIT(mpwcAuthor),
		MP_WSTRING_INIT(mpwcURL)
	{
		muSeanceID  = pSeanceID;
		if( pwcName)
			mpwcName = pwcName;
		if( pwcMembers)
			mpwcMembers = pwcMembers;
		if( mpwcAuthor)
			mpwcAuthor = mpwcAuthor;
		mlDate = alDate;
		mlMode = alMode;
		mlRecordID = pRecordID;
	}

namespace sync
{

	SYNCMAN_API omsresult CreateSyncManager( oms::omsContext* aContext)
	{
		MP_NEW_P( pSyncMan, CSyncManager, aContext);
		aContext->mpSyncMan = pSyncMan;
		return OMS_OK;
	}

	SYNCMAN_API void DestroySyncManager( oms::omsContext* aContext)
	{
		if (aContext == NULL || aContext->mpSyncMan == NULL)
			return;
		aContext->mpSyncMan->Release();
		aContext->mpSyncMan = NULL;
	}

}
