// updaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "updaterFiles.h"
#include "FileSys.h"
#include "gd/gd_XMLArchive.h"
#include <math.h>

#include <shellapi.h>

#include "updaterdll.h"

#define tmpBuffSize 65535

//CComModule _Module;

DWORD WINAPI LoadingFilesThread(LPVOID lpParameter)
{
	ATLASSERT( lpParameter);
	((CupdaterFiles*)lpParameter)->loadingFiles();
	return 0;
}

CupdaterFiles::CupdaterFiles( oms::omsContext *apOMSContext, CUpdaterDll *apDll)
{
	//iStep = 0;
	pDll = apDll;
	pOMSContext = apOMSContext;
	DWORD dwThreadID;
	eventDownloadBegin = CreateEvent( NULL, TRUE, FALSE, NULL);
	hLoadingFilesThread = CreateThread( NULL, THREAD_STACK_SIZE, (LPTHREAD_START_ROUTINE)::LoadingFilesThread, this, 0, &dwThreadID);
	//MutexBatExecuting = CreateMutex(NULL, FALSE, "updater vacademia");
	hBigFileModule = NULL;
	pBigFileManUI = NULL;
	pBigFileManCommon = NULL;
	pBigFileManGeometry = NULL;
	bUIChange = false;
	bCommonChange = false;
	bGeometryChange = false;
}

CupdaterFiles::~CupdaterFiles()
{
	Clear();
	ATLASSERT( pOMSContext);
	if( !pOMSContext)
		return;
	ReleaseThread();
	//delCache();
}

void CupdaterFiles::ReleaseThread()
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
		CloseHandle(eventDownloadBegin);
		eventDownloadBegin = NULL;
	}
}

void CupdaterFiles::setEvent()
{
	SetEvent(eventDownloadBegin);
}

/*void CupdaterFiles::wait()
{
	if (MutexBatExecuting != NULL)
	{
		WaitForSingleObject(MutexBatExecuting, INFINITE);
	}
}*/

void CupdaterFiles::releaseMutex()
{
	/*if (MutexBatExecuting != NULL)
	{
		ReleaseMutex(MutexBatExecuting);
		CloseHandle(MutexBatExecuting);
	}
	MutexBatExecuting = NULL;*/
}

bool FileExist(const TCHAR* alpcFileName){
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(alpcFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);
	return true;
}

bool CupdaterFiles::CreateEmptyFile(CWComString fullFilePath)
{
	CWComString path = L"";
	int i;
	for ( i = 0; i < fullFilePath.GetLength(); ++i)
	{
		if (fullFilePath[i] == L'\\')
		{
			CreateDirectoryW(path.GetBuffer(), NULL);
		}
		path += fullFilePath[i];
	}

	HANDLE hFile; 

	hFile = CreateFileW( fullFilePath.GetBuffer(),           // open MYFILE.TXT 
		GENERIC_READ,              // open for reading 
		FILE_SHARE_READ,           // share for reading 
		NULL,                      // no security 
		CREATE_ALWAYS,             // existing file only 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,     // normal file 
		NULL);                     // no attr. template 

	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		return false;
	}
	CloseHandle(hFile);	

	return true;
}

std::vector<CComString> v_fileSkip;

bool CupdaterFiles::ErrorMessageBox( const char *apInfo, int aCode)
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

bool CupdaterFiles::loadingFiles()
{
	WaitForSingleObject( eventDownloadBegin, INFINITE);
	CComString sRes = "loadingFiles is begin";
	CComString sCode;
	pDll->WriteLog( sRes, sCode);
	sp_ComMan = pOMSContext->mpComMan;

	pDll->setStatus( 12);
	sp_ResourceManager = pOMSContext->mpIResM;
	
	CWComString ClientPath = m_ClientPath;//glpcRootPath;
	ClientPath.MakeLower();	
	sp_ResourceManager->setCacheBase( ClientPath.GetBuffer());
	///sp_ResourceManager->setCacheBase(CComBSTR(glpcRootPath));

	pDll->setCurrentLoaded(0);
	pOMSContext->mpResM->ClearInfoList();

	unsigned int dwAllSize = 0;
	int countConnect = 0;
	int dwCurentSize = 0;
	int tryUnPack = 0;
	int code = 0;
	wchar_t sFileName[ MAX_PATH2];
	int count = 0;
	int status = 12;
	USES_CONVERSION;
	wstring wsVersion = A2W( ucCurentVersion.GetBuffer());
	__int64 freeBytesAvailable = 0;
	bool freeDiskSpaceIsOk = false;
	while (1)
	{
		/*_diskfree_t dsk;
		_getdiskfree();*/
		//__int64 totalNumberOfBytes = 0;
		//__int64 totalNumberOfFreeBytes = 0;
		if( !freeDiskSpaceIsOk)
		{
			GetDiskFreeSpaceExW(glpcRootPath, (PULARGE_INTEGER)&freeBytesAvailable, NULL, NULL);
		}

		if( !freeDiskSpaceIsOk && freeBytesAvailable < 110 * 1024 * 1024 /*100 мегабайт*/)
		{
			if(ErrorMessageBox(m_Lang == "rus" ? "Недостаточно места на диске.\nНеобходимо не менее 110 Мегабайт свободного пространства.\nПожалуйста, освободите место на диске и нажмите Повторить." : "Not enough disk space.\nIt is required at least 110 MB of free disk space.\nPlease free disk space and press Retry button.", code))
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

		wchar_t *p = &sFileName[0];
	
		code = pOMSContext->mpResM->PackUpdateClientFilesAndGetInfo( wsVersion.c_str(), p, MAX_PATH2, dwAllSize);
		if( code > 0)
			break;
		if( code < 0)
		{
			pDll->RequestDownloadSetupFromHTTP();
			return false;
		}
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

	countConnect = 0;
	sRes = "loadingFiles:: info is got success";
	pDll->WriteLog( sRes, sCode);

	//CComString sFileInfo = sFileName;
	//pDll->setInfoText( sFileInfo.GetBuffer());

	//glpcRootPath - путь к файлам
	pDll->setStatus( 20);
	pDll->NextStep(); // 4
	if( code == 1 && dwAllSize > 0)
	{
		pDll->setAllSize( dwAllSize/1024);
		
		/*CWComString foo;
		foo.GetBufferSetLength(3124);
		sp_ResourceManager->getCacheBase( foo);*/
		wchar_t *foo = (wchar_t*)sp_ResourceManager->getCacheBase();
		fullPathFile = foo;
		fullPathFile = fullPathFile + L"\\" + sFileName;
		fullPathFile.Replace(L"/",L"\\");
		fullPathFile.Replace(L"\\\\",L"\\");
		
		bool bUnpacking = false;
		int code2 = 0;
		while (1)
		{
			if( freeBytesAvailable < 10*dwAllSize && !GetDiskFreeSpaceExW(glpcRootPath, (PULARGE_INTEGER)&freeBytesAvailable, NULL, NULL))
			{
				int ff = GetLastError();
				ff+=0;
			}
			if( freeBytesAvailable < 10*dwAllSize)
			{
				CComString sInfo = m_Lang == "rus" ? "Недостаточно места на диске. Освободите " : "Not enough disk space. Free ";
				char ff[10];
				int countKb = ceil((((double)(10*dwAllSize - freeBytesAvailable) + 1))/1024);
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
				//pDll->setStatus(-1);
				if( countConnect == 3)
					Sleep( 1000);
				else
					Sleep(300);
			}
			if (countConnect == 5)
			{
				char ff[5];
				_itoa( code2, ff, 10);
				sRes = "Update error 2: countConnect = 5, code = ";
				sRes += ff;
				pDll->WriteLog( sRes, sCode);

				/*if(ErrorMessageBox(m_Lang == "rus" ? "Ошибка обновления." : "Update error", 0))
				{
					countConnect = 0;
					continue;
				}
				else
				{
					CComString s =  sFileName;
					pDll->ExitWithError( 8, s.GetBuffer());
					return false;
				}*/
				pDll->RequestDownloadSetupFromHTTP();
			}
			sCode = "";
			++countConnect;
			
			if (!SUCCEEDED(sp_ResourceManager->openResource( sFileName, RES_REMOTE | RES_CACHE_READ, &sp_Resource))) 
			{
				sRes = "openResource - ";
				sRes += sFileName;
				sRes += " is failed";
				sCode = "-11";
				pDll->WriteLog( sRes, sCode);
				code2 = -11;
				continue;
			}

			DWORD dwStatus;
			sp_Resource->getStatus( &dwStatus);
			if ((dwStatus & RES_EXISTS) == 0)//ресурс не найден
			{
				sRes = "openResource and getStatus - "; 
				sRes += sFileName;
				sRes += " is failed";
				pDll->WriteLog( sRes, dwStatus);
				sp_Resource->destroy();
				sp_Resource = NULL;
				code2 = -12;
				continue;
			}
			DWORD dwArchiveSize = 0;
			if( FAILED(sp_Resource->getSize(&dwArchiveSize)) || dwArchiveSize == 0)//ошибка загрузки
			{
				sRes = "getSize ";
				sRes += sFileName;
				sRes += " is failed or dwArchiveSize == 0";				
				pDll->WriteLog( sRes, sCode);
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
				//if ( requiredSize + aDownloadSize == dwArchiveSize )
				//{
				//	int i = 1;
				//}
				DWORD error = 0;
				sp_Resource->read(tmpBuf,requiredSize,&aReadSize, &error);

				if (  error>0 && requiredSize != aReadSize)
				{
					
					ULONG iStatus = 0;
					iStatus = sp_ComMan->GetConnectionStatus();
					//iStatus = sp_ResourceManager->GetConnectionStatus();
					char ff[5];
					_itoa(  error, ff, 10);
					sCode = ff;
					code2 = error;
					sRes = "file - ";
					sRes += sFileName;
					if(iStatus & RES_SERVER_STATUS_BIT)
					{						
						sRes += " is not reading from server";				
						pDll->WriteLog( sRes, sCode);						
					}
					else
					{						
						sRes += " is not reading. Connection is lost";				
						pDll->WriteLog( sRes, sCode);						
					}
					if(code2 == 4 || code2 == 8)
						countErrorRead++;

					if( countErrorRead > 4)
					{
						sRes = " countErrorRead > 2 and sp_Resource is destroing";
						sCode = "";
						pDll->WriteLog( sRes, sCode);
						sp_Resource->destroy();
						sp_Resource = NULL;
						break;	
					}
					else
					{
						if( countErrorRead == 3)
							Sleep(1000);
						Sleep(100);
					}
				}
				else
					countErrorRead = 0;
				if( sp_Resource != NULL && dwArchiveSize == -1)
				{
					sp_Resource->getSize( &dwArchiveSize);
				}

				aDownloadSize += aReadSize;
				dwCurentSize += aReadSize;

				//sprintf((char*)&text, "Загружено %d из %d Кбайт", dwCurentSize / 1024, dwAllSize / 1024);
				//sprintf((char*)&text, "Загружено %d из %d Кбайт", dwCurentSize / 1024, dwAllSize / 1024);
				pDll->setCurrentLoaded( dwCurentSize/1024);
				//pDll->SendDlgItemMessage( IDC_LABEL4, WM_SETTEXT, 0, (LPARAM)&text );

				//pDll->SendDlgItemMessage(IDC_PROGRESS2, PBM_SETPOS, (WPARAM)(100*aDownloadSize/dwArchiveSize), 0);
				pDll->setOneFileStatus((100*aDownloadSize/dwArchiveSize));
				pDll->setStatus((20 + (40*dwCurentSize)/dwAllSize));
			}
			if (aDownloadSize < dwArchiveSize)
			{
				dwCurentSize -=aDownloadSize;
				continue;
			}

			sRes = "file - ";
			sRes += sFileName;
			sRes += " is successfully downloaded";				
			pDll->WriteLog( sRes, sCode);

			pDll->setCurrentLoaded( dwArchiveSize/1024);
			pDll->setOneFileStatus(100);
			pDll->setStatus(60);
			Sleep(10);

			CComString zipFilePath = fullPathFile;
			std::wstring path = fullPathFile.GetBuffer();
			for (int i = path.length() - 1; i > 0; --i)
			{
				if (path[i] == L'\\')
				{
					path[i+1] = L'\0';
					break;
				}
			}
			if(!bUnpacking)
			{
				pDll->NextStep(); // 5
				bUnpacking = true;
			}
			DWORD dwUnpackTotalSize = 0;
			if (PathFileExistsW(path.c_str()))
			{
				sp_Resource->unpack( path.c_str(), FALSE, freeBytesAvailable);
				BYTE percent = 0;				
				while (percent != 100)
				{					
					sp_Resource->getUnpackProgress(&percent, &dwUnpackTotalSize);
					if( percent <= 100)
					{
						pDll->setCurrentLoaded(percent);
						if(percent>5)
							pDll->setStatus(59 + percent/5);
					}
					//pDll->setOneFileStatus(percent);
					Sleep(50);
				}
			}
			sp_Resource->getStatus( &dwStatus);
			if ((dwStatus & RES_UNPACK_ERROR) == RES_UNPACK_ERROR)//ресурс не распакован
			{
				sp_Resource->destroy();
				sp_Resource = NULL;
				sRes = "file - ";
				sRes += sFileName;
				sRes += " RES_UNPACK_ERROR";
				char ff[5];
				_itoa( dwStatus, ff, 10);
				sCode = ff;
				pDll->WriteLog( sRes, sCode);
				if(  tryUnPack < 1 && dwUnpackTotalSize < freeBytesAvailable)
				{
					//sprintf((char*)&text, "Повтор загрузки файла: %s\n",comStrUrl.GetBuffer());
					//pDll->setInfoText( &text[0]);
					countConnect = 0;
					tryUnPack++;					
				}
				else
				{
					bool bCheck = true;
					if( dwUnpackTotalSize >= freeBytesAvailable)
					{
						CComString sInfo = m_Lang == "rus" ? "Недостаточно места на диске. Освободите " : "Not enough disk space. Free ";
						char ff[10];
						int countKb = ceil((((double)(10*dwAllSize - freeBytesAvailable) + 1))/1024);
						_itoa( countKb, ff, 10);
						sInfo += ff;
						sInfo += m_Lang == "rus" ? "Кб" : "Kb";
						bCheck = ErrorMessageBox(m_Lang, 0);
					}
					else
						bCheck = ErrorMessageBox(m_Lang == "rus" ? "Ошибка распаковки файла\nПовторить или остановить обновление?\nВозможна некорректная работа Виртуальной Академии при пропуске файла." : "Error of unpacking of file.\nYou can skip it or to stop updating.\nvAcademia may work incorrectly if you skip the file.", 0);					
					if ( !bCheck)
					{
						pDll->ExitWithError( 21, "");
						return false;
					}
				}
				continue;
			}
			if( sp_Resource != NULL)
			{
				sp_Resource->destroy();
				sp_Resource = NULL;
			}

			break;
		}
		fcloseall();
	}
	else
	{
		pDll->NextStep(); // 5
	}
	/*CWindow wnd = pDll->GetDlgItem(IDC_PROGRESS2);
	wnd.ShowWindow( SW_HIDE);//скрываем статус бар загрузки файла*/
	
	pDll->setStatus(70);
	//pDll->setInfoText( "Загрузка завершена");
	//Sleep(1000);

	sRes = "loadingFiles is finish";
	pDll->WriteLog( sRes, sCode);

	CWComString sFullPathDir = L"";
	if( code == 1)
	{
		sFullPathDir = fullPathFile.Left( fullPathFile.ReverseFind('\\')+1);
		CComString sDel = "loadingFiles::del file: ";
		sDel += W2A(fullPathFile.GetBuffer());
		sCode.Empty();
		pDll->WriteLog( sDel, sCode);
		::DeleteFileW( fullPathFile.GetBuffer());
	}
	UpdateClient_CopyFile( sFullPathDir, dwAllSize);
	return true;
}