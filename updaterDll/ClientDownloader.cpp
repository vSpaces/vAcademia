#include "stdafx.h"
#include "ClientDownloader.h"
#include "FileSys.h"
//#include "gd/gd_XMLArchive.h"
#include <math.h>

#include <shellapi.h>
#include <shlobj.h>
#include <windows.h>
#include "updaterdll.h"

#define tmpBuffSize 65535


DWORD WINAPI LoadingFileThread(LPVOID lpParameter)
{
	ATLASSERT( lpParameter);
	((CClientDowloader*)lpParameter)->loadingFile();
	return 0;
}

CClientDowloader::CClientDowloader( oms::omsContext *apOMSContext, CUpdaterDll *apDll)
{
	pDll = apDll;
	pOMSContext = apOMSContext;
	DWORD dwThreadID;
	//eventDownloadBegin = CreateEvent( NULL, TRUE, FALSE, NULL);
	hLoadingFilesThread = CreateThread( NULL, THREAD_STACK_SIZE, (LPTHREAD_START_ROUTINE)::LoadingFileThread, this, 0, &dwThreadID);
	//hBigFileModule = NULL;
	//pBigFileManUI = NULL;
	//pBigFileManCommon = NULL;
}

CClientDowloader::~CClientDowloader()
{
	ATLASSERT( pOMSContext);
	if( !pOMSContext)
		return;
	ReleaseThread();
	//delCache();
}

void CClientDowloader::ReleaseThread()
{
	if(hLoadingFilesThread!=NULL)
	{
		DWORD exitCode;
		BOOL res = GetExitCodeThread(hLoadingFilesThread, &exitCode);

		if (!res)
		{
			exitCode = GetLastError();
		}

		if(exitCode == STILL_ACTIVE)
		{
			TerminateThread(hLoadingFilesThread, exitCode);
			CloseHandle(hLoadingFilesThread);
		}
		hLoadingFilesThread = NULL;
	}
}

void CClientDowloader::setDll(  CUpdaterDll *apDll)
{
	pDll = apDll;
}

bool CClientDowloader::ErrorMessageBox( const char *apInfo, int aCode)
{
	CComString sTitle = m_Lang == "rus" ? "Внимание" : "Attention";
	unsigned int status = pOMSContext->mpComMan->GetConnectionStatus();
	CComString sInfo = apInfo;	
	if( aCode != 0)
	{
		char ff[5];
		_itoa(aCode, ff, 10);
		sInfo += ff;
	}
	sInfo += m_Lang == "rus" ? "\nПовторить?" : "\nRepeat?";
	if ( ::MessageBox( NULL, sInfo, sTitle, MB_YESNO | MB_ICONWARNING) == IDYES)
		return true;
	return false;
}

bool CClientDowloader::loadingFile()
{
	pDll->SetStep(8);

	CComString sRes = "Setup loading file is begin";
	CComString sCode;
	pDll->WriteLog( sRes, sCode);

	/// качаем сразу с http
	pDll->RequestDownloadSetupFromHTTP();
	return false;

	/// 

	sp_ComMan = pOMSContext->mpComMan;

	pDll->setStatus( 0);
	sp_ResourceManager = pOMSContext->mpIResM;

	// сетап будем ложить во временную папку, откуда запускаем апдейтер

	WCHAR tempSetupPath[MAX_PATH * 2];
	wcscpy(tempSetupPath, lpcIniFullPathName);

	tempSetupPath[wcslen(lpcIniFullPathName) - wcslen(L"player.ini")] = L'\0';
	sp_ResourceManager->setCacheBase( tempSetupPath);

	pDll->setCurrentLoaded(0);
	pOMSContext->mpResM->ClearInfoList();

	unsigned int dwAllSize = 0;
	int countConnect = 0;
	int dwCurentSize = 0;

	int code = 0;
	wchar_t sFileName[ MAX_PATH2];
	int count = 0;
	int status = 12;
	USES_CONVERSION;

	__int64 freeBytesAvailable = 0;
	bool freeDiskSpaceIsOk = false;
	while (1)
	{		
		if( !freeDiskSpaceIsOk)
		{
			GetDiskFreeSpaceExW(glpcRootPath, (PULARGE_INTEGER)&freeBytesAvailable, NULL, NULL);
		}

		if( !freeDiskSpaceIsOk && freeBytesAvailable < 110 * 1024 * 1024 /*100 мегабайт*/)
		{
			if(ErrorMessageBox(m_Lang == "rus" ? "Недостаточно места на диске.\nНеобходимо не менее 110 Мегабайт свободного пространства.\nПожалуйста, освободите место на диске и нажмите Повторить." : "Not Enough Disk Space.\nYou should have at least 110 Megabytes of free disk space.\nPlease free disk space and press Retry button.", code))
			{
				continue;
			}
			else
			{
				pDll->ExitWithError( 21, "");
				return false;
			}			
		}

		freeDiskSpaceIsOk = true;

		if (countConnect > 0)
		{	
			Sleep(300);
		}
		if (countConnect == 3)
		{
			pDll->RequestDownloadSetupFromHTTP();
			return false;			
		}
		if( code != 0)
			++countConnect;

		wchar_t *p = &sFileName[0];

		code = pOMSContext->mpResM->PackClientSetup(p, MAX_PATH2, dwAllSize);
		if( code > 0)
			break;
		if(code == 0)
		{
			Sleep(300);
			count++;
		}
		if(count>9 && status < 19)
		{
			count = 0;
			status++;
			pDll->setStatus( status);
		}
	}

	sRes = "Setup info is got success";	
	pDll->WriteLog( sRes, sCode);

	pDll->setStatus( 20);
	pDll->NextStep(); // 9
	pDll->SetLoadingSetupStatus(LOADING_SETUP_PROCESS);

	CWComString setupFilePath = L""; 
	if( code == 1 && dwAllSize > 0)
	{
		pDll->setAllSize( dwAllSize/1024);

		wchar_t *foo = (wchar_t*)sp_ResourceManager->getCacheBase();
		fullPathFile = foo;
		fullPathFile = fullPathFile + L"\\" + sFileName;
		fullPathFile.Replace(L"/",L"\\");
		fullPathFile.Replace(L"\\\\",L"\\");

		int code2 = 0;
		while (1)
		{
			if( freeBytesAvailable < 2*dwAllSize && !GetDiskFreeSpaceExW(glpcRootPath, (PULARGE_INTEGER)&freeBytesAvailable, NULL, NULL))
			{
				int ff = GetLastError();
				ff+=0;
			}
			if( freeBytesAvailable < 2*dwAllSize)
			{
				CComString sInfo = m_Lang == "rus" ? "Недостаточно места на диске. Освободите " : "Not Enough Disk Space. Release ";
				char ff[10];
				int countKb = ceil((((double)(2*dwAllSize - freeBytesAvailable) + 1))/1024);
				_itoa( countKb, ff, 10);
				sInfo += ff;
				sInfo += m_Lang == "rus" ? "Кб" : "Kb";
				if(ErrorMessageBox( sInfo.GetBuffer(), 0))
					continue;
				else
				{
					pDll->ExitWithError( 21, "");
					return false;
				}			
			}

			if (countConnect > 0)
			{
				sRes = "error read ant try countConnect++";	
				pDll->WriteLog( sRes, sCode);
				Sleep(300);
			}
			if (countConnect == 3)
			{
				pDll->RequestDownloadSetupFromHTTP();
				return false;
			}
			++countConnect;

			if (!SUCCEEDED(sp_ResourceManager->openResource( sFileName, RES_REMOTE | RES_CACHE_READ, &sp_Resource))) 
			{
				code2 = -11;
				continue;
			}

			DWORD dwStatus;
			sp_Resource->getStatus( &dwStatus);
			if ((dwStatus & RES_EXISTS) == 0)//ресурс не найден
			{
				sp_Resource->destroy();
				sp_Resource = NULL;
				code2 = -12;
				continue;
			}
			DWORD dwArchiveSize = 0;
			if( FAILED(sp_Resource->getSize(&dwArchiveSize)) || dwArchiveSize == 0)//ошибка загрузки
			{
				sp_Resource->destroy();
				sp_Resource = NULL;
				code2 = -13;
				continue;
			}

			pDll->setOneFileStatus(0);

			DWORD aDownloadSize = 0;
			DWORD aReadSize = 0;
			BYTE tmpBuf[tmpBuffSize];

			if(dwArchiveSize == -1 || dwArchiveSize == 0)
			{
				dwArchiveSize = -1;
			}

			int countErrorRead = 0;

			while ( aDownloadSize < dwArchiveSize )
			{
				DWORD	requiredSize = tmpBuffSize;
				if( dwArchiveSize!=-1 && requiredSize + aDownloadSize > dwArchiveSize)
					requiredSize = dwArchiveSize - aDownloadSize;
	
				DWORD error = 0;
				sp_Resource->read(tmpBuf,requiredSize,&aReadSize, &error);

				if (  error>0 && requiredSize != aReadSize)
				{

					ULONG iStatus = 0;
					iStatus = sp_ComMan->GetConnectionStatus();
					if(iStatus & RES_SERVER_STATUS_BIT)
					{
						code2 = error;
						//sprintf((char*)&text, "Ошибка чтения файла с сервера: %s", comStrUrl.GetBuffer());
						//pDll->setInfoText( &text[0]);
					}
					else
					{
						code2 = error;
						//sprintf((char*)&text, "Соединение оборвано", comStrUrl.GetBuffer());
						//pDll->setInfoText( &text[0]);
					}
					if(code2 == 4 || code2 == 8)
						countErrorRead++;

					if( countErrorRead > 2)
					{
						sRes = "error read countErrorRead > 2";	
						pDll->WriteLog( sRes, sCode);
						sp_Resource->destroy();
						sp_Resource = NULL;
						break;	
					}
					else
						Sleep(100);
				}
				else
					countErrorRead = 0;
				if( sp_Resource != NULL && dwArchiveSize == -1)
				{
					sp_Resource->getSize( &dwArchiveSize);
				}

				aDownloadSize += aReadSize;
				dwCurentSize += aReadSize;

				pDll->setCurrentLoaded( dwCurentSize/1024);
				
				pDll->setOneFileStatus((100*aDownloadSize)/dwArchiveSize);
				pDll->setStatus( 20 + (60*dwCurentSize)/dwAllSize);				
			}
			if (aDownloadSize < dwArchiveSize)
			{
				sRes = "error read aDownloadSize < dwArchiveSize";	
				pDll->WriteLog( sRes, sCode);
				dwCurentSize -=aDownloadSize;
				continue;
			}

			pDll->setCurrentLoaded( dwArchiveSize/1024);
			pDll->setOneFileStatus(100);
			pDll->setStatus(80);
			Sleep(10);

			setupFilePath = fullPathFile;
			std::wstring path = fullPathFile.GetBuffer();
			for (int i = path.length() - 1; i > 0; --i)
			{
				if (path[i] == L'\\')
				{
					path[i+1] = L'\0';
					break;
				}
			}
		
			sp_Resource->destroy();
			sp_Resource = NULL;
			break;
		}
		fcloseall();
	}
	//else
		//pDll->NextStep(); // 9

	pDll->setStatus(100);
	pDll->SetLoadingSetupStatus(LOADING_SETUP_FINISH);

	sRes = "loading setup file is finish";
	pDll->WriteLog( sRes, sCode);

	pDll->OpenSetup(setupFilePath.GetBuffer());

	
	return true;
}
     