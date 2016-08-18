// updaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "updaterFiles.h"
//#include "crmMemResourceFile.h"
#include "ComString.h"
#include "FileSys.h"
#include "gd/gd_XMLArchive.h"

#include <shellapi.h>

#include ".\updaterdlg.h"

#define tmpBuffSize 65535

//CComModule _Module;

DWORD WINAPI LoadingFilesThread(LPVOID lpParameter)
{
	ATLASSERT( lpParameter);
	((CupdaterFiles*)lpParameter)->LoadingFiles();
	return 0;
}

CupdaterFiles::CupdaterFiles( oms::omsContext *apOMSContext, CUpdaterDlg *apDlg)
{
	iStep = 0;
	pDlg = apDlg;
	pOMSContext = apOMSContext;
	DWORD dwThreadID;
	eventDownloadBegin = CreateEvent( NULL, TRUE, FALSE, NULL);
	hLoadingFilesThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)::LoadingFilesThread, this, 0, &dwThreadID);
	MutexBatExecuting = CreateMutex(NULL, FALSE, "updater vacademia");
	hBigFileModule = NULL;
	pBigFileManUI = NULL;
	pBigFileManCommon = NULL;
}

CupdaterFiles::~CupdaterFiles()
{
	ATLASSERT( pOMSContext);
	if( !pOMSContext)
		return;
	ReleaseThread();
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

void CupdaterFiles::wait()
{
	if (MutexBatExecuting != NULL)
	{
		WaitForSingleObject(MutexBatExecuting, INFINITE);
	}
}

void CupdaterFiles::releaseMutex()
{
	if (MutexBatExecuting != NULL)
	{
		ReleaseMutex(MutexBatExecuting);
		CloseHandle(MutexBatExecuting);
	}
	MutexBatExecuting = NULL;
}

bool FileExist(const TCHAR* alpcFileName){
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(alpcFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);
	return true;
}

bool CupdaterFiles::CreateEmptyFile(CComString fullFilePath)
{
	CComString path = "";
	int i;
	for ( i = 0; i < fullFilePath.GetLength(); ++i)
	{
		if (fullFilePath[i] == '\\')
		{
			CreateDirectory(path.GetBuffer(), NULL);
		}
		path += fullFilePath[i];
	}

	HANDLE hFile; 

	hFile = CreateFile((LPCTSTR)fullFilePath.GetBuffer(),           // open MYFILE.TXT 
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

bool CupdaterFiles::LoadingFiles()
{
	WaitForSingleObject( eventDownloadBegin, INFINITE);
	sp_ComMan = pOMSContext->mpComMan;
	unsigned long aDataSize = 0;
	BYTE *aData = NULL;
	char text[512];//строка состояния
	pDlg->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(12), 0);
	sprintf((char*)&text, "Получаем запрос для обновления ...");
	pDlg->SendDlgItemMessage( IDC_LABEL5, WM_SETTEXT, 0, (LPARAM)&text );	
	sp_ResourceManager = pOMSContext->mpIResM;
	
	sp_ResourceManager->setCacheBase(CComBSTR(glpcRootPath));

	pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );

	CComString sParam;

	sParam.Format( "CurentVersion=%s", ucCurentVersion.GetBuffer());
	sp_ComMan->QueryData(CComBSTR( "updater/get_xml.php"), CComBSTR( sParam ), &aDataSize, &aData );
	if (aData == NULL)
	{	
		pDlg->ExitWithError(  "Ошибка соединения с сервером");
		return false;
	}

	DWORD dwAllSize = 0;//обнуляем объём закачиваемых файлов
	//парсинг XML
	crmMemResourceFile* file;
	file = new crmMemResourceFile();
	file->Attach( aData, aDataSize);
	gd_xml_stream::CXMLArchive<crmMemResourceFile>	archive( file );

	archive.SkipHeader();

	BSTR pVal;
	//sp_ComMan->getCacheBase( &pVal);
	sp_ResourceManager->getCacheBase( &pVal);

	pOMSContext->mpResM->ClearInfoList();
	pOMSContext->mpResM->SetModuleID(L"vu2008-02-20", false);
	pOMSContext->mpResM->SetModuleID(L"client", false);

	while ( archive.ReadNext() )
	{
		CComString sName = (const TCHAR*)archive.GetName(); sName.MakeLower();
		if ( sName == "file" )
		{
			CComString sAttrValue = archive.GetAttribute("value");//
			CComBSTR url = archive.GetAttribute("value");//

			CComString fullPathFile = pVal;
			fullPathFile = fullPathFile + sAttrValue;
			fullPathFile.Replace("/","\\");
			fullPathFile.Delete(fullPathFile.GetLength() - 4, 4);

			if(FileExist(fullPathFile.GetBuffer()))
			{
				if (!SUCCEEDED(sp_ResourceManager->openResource((BSTR)(url), RES_REMOTE | RES_CACHE_READ, &sp_Resource)))
				{ 
				}
				DWORD dwArchiveSize = 0;
				if( FAILED(sp_Resource->getSize(&dwArchiveSize)) )//ошибка загрузки
				{
				}
				else
				{
					dwAllSize += dwArchiveSize;
					continue;
				}
			}

			CComString sFileInfo = sAttrValue;
			sFileInfo.Replace(".zip", "");

			sprintf((char*)&text, "Запрос информации о файле: %s\n",sFileInfo.GetBuffer());
			pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );	

			unsigned int countConnect = 0;
			while (1)			
			{
				if (countConnect > 0)
				{
					sprintf((char*)&text, "Попытка повторной загрузки информации о файле: %s\n",sFileInfo.GetBuffer());
					pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
					Sleep(300);
				}
				if (countConnect == 3)
				{
					if (pDlg->MessageBox("Ошибка загрузки информации о файле\nПовторить?", "Внимание", MB_OKCANCEL | MB_ICONINFORMATION) != IDCANCEL)
					{
						sprintf((char*)&text, "Повтор загрузки информации о файле: %s\n",sFileInfo.GetBuffer());
						pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
						countConnect = 0;
						pOMSContext->mpResM->ClearInfoList();
						pOMSContext->mpResM->SetModuleID(L"vu2008-02-20", false);
						pOMSContext->mpResM->SetModuleID(L"client", false);
						continue;
					}
					else
					{
						if(sp_Resource != NULL)
							sp_Resource->destroy();
						sp_Resource = NULL;
						sFileInfo = "Ошибка загрузки информации о файле: " + sFileInfo;
						pDlg->ExitWithError( sFileInfo);
						return false;
					}
				}
				++countConnect;
				if (!SUCCEEDED(sp_ResourceManager->openResource((BSTR)(url), RES_REMOTE | RES_CACHE_READ, &sp_Resource)))
				{ 
					sprintf((char*)&text, "Ошибка открытия файла: %s\n",sFileInfo.GetBuffer());
					pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
					continue;
				}
				DWORD dwStatus;
				sp_Resource->getStatus( &dwStatus);
				if ((dwStatus & RES_EXISTS) == 0)//ресурс не найден
				{
					sprintf((char*)&text, "Ресурс не найден: %s\n",sFileInfo.GetBuffer());
					pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
					sp_Resource->destroy();
					sp_Resource = NULL;
					continue;
				}
				DWORD dwArchiveSize = 0;
				if( FAILED(sp_Resource->getSize(&dwArchiveSize)) )//ошибка загрузки
				{
					sprintf((char*)&text, "Ошибка получения размера файла: %s\n",sFileInfo.GetBuffer());
					pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
					sp_Resource->destroy();
					sp_Resource = NULL;
					continue;
				}
				if (dwArchiveSize == 0)
				{
					CComString filePath = ClientPath + sAttrValue;
					filePath.Replace("/","\\");
					if (!CreateEmptyFile(filePath))
					{ // не критично (только для пустых файлов)
						sprintf((char*)&text, "Ошибка создания файла: %s возможно недостаточно прав\n",sFileInfo.GetBuffer());
						pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
					}
				}
				dwAllSize += dwArchiveSize;
				sp_Resource->destroy();
				sp_Resource = NULL;
                break;
			}

			//sp_Resource->download( notifier);
		}
	}
	//glpcRootPath - путь к файлам
	pDlg->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(20), 0);
	//CComString ClientPath = glpcRootPath;

	crmMemResourceFile* file2;
	file2 = new crmMemResourceFile();
	file2->Attach( aData, aDataSize);
	gd_xml_stream::CXMLArchive<crmMemResourceFile>	archive2( file2 );

	archive2.SkipHeader();
	DWORD dwCurentSize = 0;
	while ( archive2.ReadNext() )
	{
		CComString sName = (const TCHAR*)archive2.GetName(); sName.MakeLower();
		if ( sName == "file" )
		{
			CComString sAttrValue = archive2.GetAttribute("value");//
			CComBSTR url = archive2.GetAttribute("value");//
			CComString sNewFileName = sAttrValue;
			sprintf((char*)&text, "Загрузка\n%s",sAttrValue.GetBuffer());
			pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );

			unsigned int countConnect = 0;
			unsigned int tryUnPack = 0;
			while (1)
			{
				if (countConnect > 0)
				{
					CComString comStrUrl = url;
					sprintf((char*)&text, "Попытка повторной загрузки файла: %s\n",comStrUrl.GetBuffer());
					pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
					Sleep(300);
				}
				if (countConnect == 3)
				{
					if (pDlg->MessageBox("Ошибка загрузки файла\nПовторить?", "Внимание", MB_OKCANCEL | MB_ICONINFORMATION) != IDCANCEL)
					{
						CComString comStrUrl = url;
						sprintf((char*)&text, "Повтор загрузки файла: %s\n",comStrUrl.GetBuffer());
						pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
						countConnect = 0;
						continue;
					}
					else
					{
						CComString comStrUrl = "Ошибка загрузки файла: ";
						comStrUrl += url;
						pDlg->ExitWithError( comStrUrl);
						return false;
					}
				}
				++countConnect;
				
				if (!SUCCEEDED(sp_ResourceManager->openResource((BSTR)(url), RES_REMOTE | RES_CACHE_READ, &sp_Resource))) 
				{
					CComString comStrUrl = url;
					sprintf((char*)&text, "Ошибка загрузки файла: %s\n",comStrUrl.GetBuffer());
					pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
					continue;
				}

				DWORD dwStatus;
				sp_Resource->getStatus( &dwStatus);
				if ((dwStatus & RES_EXISTS) == 0)//ресурс не найден
				{
					CComString comStrUrl = url;
					sprintf((char*)&text, "Ресурс не найден: %s\n", comStrUrl.GetBuffer());
					pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
					sp_Resource->destroy();
					sp_Resource = NULL;
					continue;
				}
				DWORD dwArchiveSize = 0;
				if( FAILED(sp_Resource->getSize(&dwArchiveSize)) || dwArchiveSize == 0)//ошибка загрузки
				{
					CComString comStrUrl = url;
					sprintf((char*)&text, "Ошибка загрузки файла: %s\n", comStrUrl.GetBuffer());
					pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
					sp_Resource->destroy();
					sp_Resource = NULL;
					continue;
				}


				//показываем прогрессбар загрузки файла
				CWindow wnd = pDlg->GetDlgItem(IDC_PROGRESS2);
				wnd.ShowWindow( SW_SHOW);
				
				//устанавливаем размеры прогрессбара
				pDlg->SendDlgItemMessage(IDC_PROGRESS2, 0, MAKELPARAM(0, 100));
				pDlg->SendDlgItemMessage(IDC_PROGRESS2, PBM_SETPOS, (WPARAM)(0), 0);
				
				//показываем строку состояния
				wnd = pDlg->GetDlgItem(IDC_LABEL3);
				wnd.ShowWindow( SW_SHOW);
				
				wnd = pDlg->GetDlgItem(IDC_LABEL4);
				wnd.ShowWindow( SW_SHOW);

				sprintf((char*)&text, "%s",sNewFileName.GetBuffer());
				pDlg->SendDlgItemMessage( IDC_LABEL3, WM_SETTEXT, 0, (LPARAM)&text );

				DWORD aDownloadSize = 0;
				DWORD aReadSize = 0;
				BYTE tmpBuf[tmpBuffSize];

				/*BSTR pVal;
				//sp_ComMan->getCacheBase( &pVal);
				sp_ResourceManager->getCacheBase( &pVal);*/

				CComString fullPathFile = pVal;
				fullPathFile = fullPathFile + sAttrValue;
				fullPathFile.Replace("/","\\");

				if(dwArchiveSize == -1 || dwArchiveSize == 0)
				{
					dwArchiveSize = -1;
				}

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
						CComString comStrUrl = url;

						ULONG iStatus = 0;
						iStatus = sp_ComMan->GetConnectionStatus();
						if(iStatus & RES_SERVER_STATUS_BIT)
						{
							sprintf((char*)&text, "Ошибка чтения файла с сервера: %s", comStrUrl.GetBuffer());
							pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
						}
						else
						{
							sprintf((char*)&text, "Соединение оборвано", comStrUrl.GetBuffer());
							pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
						}

						//sp_Resource->close();
						sp_Resource->destroy();
						sp_Resource = NULL;
						//fclose(pFile);
						break;	
					}
					if( dwArchiveSize == -1)
					{
						sp_Resource->getSize( &dwArchiveSize);
					}

					aDownloadSize += aReadSize;
					dwCurentSize += aReadSize;

					sprintf((char*)&text, "Загружено %d из %d Кбайт", dwCurentSize / 1024, dwAllSize / 1024);
					pDlg->SendDlgItemMessage( IDC_LABEL4, WM_SETTEXT, 0, (LPARAM)&text );

					pDlg->SendDlgItemMessage(IDC_PROGRESS2, PBM_SETPOS, (WPARAM)(100*aDownloadSize/dwArchiveSize), 0);
				}
				if (aDownloadSize < dwArchiveSize)
				{
					dwCurentSize -=aDownloadSize;
					continue;
				}

				CComString zipFilePath = fullPathFile;
				std::string path = fullPathFile.GetBuffer();
				for (int i = path.length() - 1; i > 0; --i)
				{
					if (path[i] == '\\')
					{
						path[i+1] = '\0';
						break;
					}
				}
				if (PathFileExists(path.c_str()))
				{
					CComBSTR tmp(path.c_str());
					sp_Resource->unpack((BSTR)(tmp), FALSE);
					BYTE percent = 0;
					while (percent != 100)
					{
						sp_Resource->getUnpackProgress(&percent);
						Sleep(10);
					}
				}
				sp_Resource->getStatus( &dwStatus);
				if ((dwStatus & RES_UNPACK_ERROR) == RES_UNPACK_ERROR)//ресурс не распакован
				{
					if(  tryUnPack < 1)
					{
						CComString comStrUrl = url;
						sprintf((char*)&text, "Повтор загрузки файла: %s\n",comStrUrl.GetBuffer());
						pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
					countConnect = 0;
						tryUnPack++;
					continue;
				}
					else
					{
						if (pDlg->MessageBox("Ошибка распаковки файла\nПропустить файл или остановить обновление?\n1. Возможна некорректная работа Виртуальной Академии при пропуске файла.", "Внимание", MB_YESNO | MB_ICONWARNING) != IDYES)
						{
							CComString comStrUrl = url;
							sprintf((char*)&text, "Ошибка загрузки файла: %s\n",comStrUrl.GetBuffer());
							pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
							return false;
						}
						else
						{
							sAttrValue.Delete(sAttrValue.GetLength() - 4, 4);
							v_fileSkip.push_back( sAttrValue);
						}
					}
				}
				sp_Resource->destroy();
				sp_Resource = NULL;

				//unlink(zipFilePath.GetBuffer());

				pDlg->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(20+40*dwCurentSize/dwAllSize), 0);
				
				wnd = pDlg->GetDlgItem( IDC_LABEL3);
				wnd.ShowWindow( SW_HIDE);

				wnd = pDlg->GetDlgItem( IDC_LABEL4);
				wnd.ShowWindow( SW_HIDE);
				break;
			}
		}
	}
	CWindow wnd = pDlg->GetDlgItem(IDC_PROGRESS2);
	wnd.ShowWindow( SW_HIDE);//скрываем статус бар загрузки файла
	
	sprintf((char*)&text, "Загрузка завершена");
	pDlg->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(80), 0);
	pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );

	fcloseall();
	try
	{
		pDlg->ReleaseLibrary();
	}
	catch (...)
	{
	}
	UpdateClient_CopyFile(aDataSize, aData, dwAllSize);
	return true;
}