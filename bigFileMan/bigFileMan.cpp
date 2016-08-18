// ServiceMan.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "BigFileMan.h"
#include "BigFileManager.h"

#define MAX_LOADSTRING 100

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(2726186);
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

namespace service
{

	struct BigFileManagerUsing
	{
		CBigFileManager *bigFileManager;
		int count;
	};

	std::map<CWComString, BigFileManagerUsing> bigFileManagerUsingMap;

	BIGFILEMAN_API unsigned int CreateBigFileManagerBase( CBigFileManager **ppBigFileManagerBase, const wchar_t* aModuleFile, int right, bool bRead, bool bOnlyInfoRead)
	{
		std::map<CWComString, BigFileManagerUsing>::iterator iter = bigFileManagerUsingMap.find( aModuleFile);
		if(iter != bigFileManagerUsingMap.end())
		{
			iter->second.count++;
			*ppBigFileManagerBase = iter->second.bigFileManager;
			return iter->second.count;
		}
		else
		{
			CBigFileManager *pBigFileManager = MP_NEW( CBigFileManager);
			BigFileManagerUsing bigFileManagerUsing;
			bigFileManagerUsing.bigFileManager = pBigFileManager;
			bigFileManagerUsing.count = 1;
			bigFileManagerUsingMap.insert( std::map<CWComString, BigFileManagerUsing>::value_type( aModuleFile, bigFileManagerUsing));
			if(bRead)
			{
				bool bRes = pBigFileManager->ReadBigFileAndCreateTreeInfo( aModuleFile, right, bOnlyInfoRead);
				if(!bRes)
				{
					std::map<CWComString, BigFileManagerUsing>::iterator iter = bigFileManagerUsingMap.find( aModuleFile);
					pBigFileManager->Destroy();
					pBigFileManager = NULL;
					bigFileManagerUsingMap.erase( iter);
				}
			}
			*ppBigFileManagerBase = pBigFileManager;

		}
		return 0;
	}

	BIGFILEMAN_API unsigned int CreateBigFileManagerReader( IBigFileManagerReader **ppBigFileManagerReader, const wchar_t* aModuleFile)
	{
		CBigFileManager *pBigFileManagerBase = NULL;
		CreateBigFileManagerBase( &pBigFileManagerBase, aModuleFile, 0, true, true);
		*ppBigFileManagerReader = pBigFileManagerBase;	
		return 0;
	}

	BIGFILEMAN_API unsigned int CreateBigFileManagerWriter( IBigFileManagerWriter **ppBigFileManagerWriter, const wchar_t* aModuleFile)
	{
		CBigFileManager *pBigFileManagerBase = NULL;
		CreateBigFileManagerBase( &pBigFileManagerBase, aModuleFile, 1, false, true);
		*ppBigFileManagerWriter = pBigFileManagerBase;	
		return 0;
	}

	BIGFILEMAN_API unsigned int CreateBigFileManager( IBigFileManager **ppBigFileManager, const wchar_t* aModuleFile)
	{
		CBigFileManager *pBigFileManagerBase = NULL;
		CreateBigFileManagerBase( &pBigFileManagerBase, aModuleFile, 0, true, false);
		*ppBigFileManager = pBigFileManagerBase;	
		return 0;
	}

	BIGFILEMAN_API void DestroyBigFileManager(  IBigFileManagerBase *pBigFileManager, const wchar_t* aModuleFile)
	{
		if (pBigFileManager == NULL)
			return;
		std::map<CWComString, BigFileManagerUsing>::iterator iter = bigFileManagerUsingMap.find( aModuleFile);
		if(iter != bigFileManagerUsingMap.end())
		{
			iter->second.count--;
			if( iter->second.count < 1)
			{
				pBigFileManager->Destroy();
				pBigFileManager = NULL;
				bigFileManagerUsingMap.erase( iter);
			}
		}
	}

	/*BIGFILEMAN_API void DestroyBigFileManagerWriter(  IBigFileManagerWriter *pBigFileManagerWriter, LPCTSTR aModuleFile)
	{
		if (pBigFileManagerWriter == NULL)
			return;
		std::map<CComString, BigFileManagerUsing>::iterator iter = bigFileManagerUsingMap.find( aModuleFile);
		if(iter != bigFileManagerUsingMap.end())
		{
			iter->second.count--;
			if( iter->second.count < 1)
			{
				pBigFileManagerWriter->Destroy();
				pBigFileManagerWriter = NULL;
				bigFileManagerUsingMap.erase( iter);
			}
		}
	}*/
}
