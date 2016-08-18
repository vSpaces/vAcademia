// updaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UpdaterFiles.h"
//#include "crmMemResourceFile.h"
#include "ComString.h"
#include "FileSys.h"
#include "gd/gd_XMLArchive.h"

#include <shellapi.h>

#include ".\updaterdlg.h"

#define tmpBuffSize 65535

//CComModule _Module;

void CupdaterFiles::CreateBatFile( CComString &moduleName, CComString &myUpdateStrFrom, CComString &myUpdateStrTo)
{
	moduleName.Replace("/", "\\");
	myUpdateStrFrom.Replace("/", "\\");
	myUpdateStrTo.Replace("/", "\\");
	char text[MAX_PATH];//строка состояния
	char BATSTRING[] = ":Repeat\n\
					   @ren \"%s\" \"%s\"\n\
					   if exist \"%s\" (goto :RCopy)\n\
					   goto :Repeat\n\
					   :RCopy\n\
					   copy \"%s\" \"%s\"\n\
					   del \"%s\"\n\
					   del \"%s\"\n\
					   del \"%s\"";
	HANDLE hFile;

	char szFilePath[MAX_PATH*2] = {"\0"};
	char szFileBat[MAX_PATH*2] = {"\0"};
	char szFileExe[MAX_PATH*2] = {"\0"};
	char szFileFrom[MAX_PATH*2] = {"\0"};
	char szFileTo[MAX_PATH*2] = {"\0"};
	CComString sRenFile;
	CComString sRenFileName;

	char sz[2024]; // Строка-помошник...

	unsigned int iTmp;
	char* lpTmp;

	if (moduleName == "updater")
	{
		//GetModuleFileName(theApp.m_hInstance, szFileExe, sizeof(szFileExe)); // Получаем путь к запущенному ехешнику
		GetModuleFileName( NULL, szFileExe, sizeof(szFileExe)); // Получаем путь к запущенному ехешнику
		//strcpy(*glpcRootPath, szFileExe);
	}
	else
	{
		strcpy(szFileExe, myUpdateStrTo);
	}

	sRenFile = szFileExe;
	sRenFile.Replace(".", "_old.");

	sRenFileName = sRenFile.Right(sRenFile.GetLength() - sRenFile.ReverseFind('\\') - 1);

	for (iTmp = 0, lpTmp = szFileExe + strlen(szFileExe)-1; iTmp < strlen(szFileExe); lpTmp--, iTmp++)
	{
		if (!strncmp(lpTmp, "\\", 1))
			break;
	}

	strncpy(szFilePath, szFileExe, lpTmp - szFileExe);
	strcat(szFilePath, "\\");

	GetTempPath(MAX_PATH, sz); // Получаем путь к TEMP-папке
	sprintf(szFileBat, "%s%s.bat", (strlen(sz))?sz:szFilePath, moduleName.GetBuffer());

	DWORD dwBytesWritten;

	SetFileAttributes(szFileBat, FILE_ATTRIBUTE_NORMAL); // Если файл существует, то изменим его аттрибуты на нормальные

	hFile = CreateFile((LPCTSTR)szFileBat,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // Создаем по-новой файл ("зачем?" - ком. внутреннего голоса)

	if (!hFile)
	{
		pDlg->ExitWithError("Не удалось создать файл, error № 1005");
		return;
	}

	sprintf(sz, BATSTRING, szFileExe, sRenFileName.GetBuffer(), sRenFile.GetBuffer(), myUpdateStrFrom.GetBuffer(), myUpdateStrTo.GetBuffer(), myUpdateStrFrom.GetBuffer(), sRenFile.GetBuffer(), szFileBat);
	//sprintf(sz, BATSTRING, szFileExe, szFileExe, szFileExe, myUpdateStrFrom.GetBuffer(), myUpdateStrTo.GetBuffer(), myUpdateStrFrom.GetBuffer(), szFileBat);

	BOOL bError = (!WriteFile(hFile, sz, strlen(sz), &dwBytesWritten, NULL)
		|| strlen(sz) != dwBytesWritten);

	if (bError)
	{
		pDlg->ExitWithError( "Не удалось записать в файл, error № 1006");
		return ;
	}
	SetFileAttributes(szFileExe, FILE_ATTRIBUTE_ARCHIVE);
	SetFileAttributes(szFileBat, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_HIDDEN); // "прячем" файл
	CloseHandle(hFile); // Специально закрываем файл перед запуском батника, чтобы юзер не удалил файл раньше времени
	SetFileAttributes(szFileBat, FILE_ATTRIBUTE_NORMAL); // Делаем файл нормальным
	CComString BatFilePath = szFileBat;
	BatFiles.push_back(BatFilePath);
}

void CupdaterFiles::UpdateClient_CopyFile(DWORD aDataSize,BYTE *aData,DWORD dwDowloadUpdateSize)
{
	//MessageBox("закончили закачивать", "закончили закачивать", MB_OK | MB_ICONINFORMATION);
	int ff = 0;
	/*try
	{*/
	if (MutexBatExecuting != NULL || WaitForSingleObject(MutexBatExecuting, 100) == WAIT_OBJECT_0)
	{
		ReleaseMutex(MutexBatExecuting);
	}
	CWindow wnd = pDlg->GetDlgItem(IDCANCEL);
	wnd.EnableWindow( FALSE);

	//CoFreeLibrary(ghmComMan);
	//glpcRootPath - путь к файлам
	CComString ClientPath = glpcRootPath;
	ClientPath.MakeLower();
	ClientPath.Delete(ClientPath.Find("updater"),8 );

	LoadBigFileDll();
	CComString sUIPath = ClientPath + "/ui";
	sUIPath.Replace("\\", "/");
	sUIPath.Replace("//", "/");
	CComString sCommonPath = ClientPath + "/root/files/vu.common";
	sCommonPath.Replace("\\", "/");
	sCommonPath.Replace("//", "/");
	int errorCode = CreateBigFileManagerImpl( &pBigFileManUI, sUIPath + "/ui.pack");
	errorCode = CreateBigFileManagerImpl( &pBigFileManCommon, sCommonPath + "/vu.common.pack");

	char text[MAX_PATH];//строка состояния
	crmMemResourceFile* file;
	file = new crmMemResourceFile();
	file->Attach( aData, aDataSize);
	gd_xml_stream::CXMLArchive<crmMemResourceFile>	archive( file );
	
	bool myUpdate = false;
	CComString myUpdateStrFrom = "";
	CComString myUpdateStrTo = "";
	
	archive.SkipHeader();
	while ( archive.ReadNext() )
	{
		CComString sName = (const TCHAR*)archive.GetName(); sName.MakeLower();
		if ( sName == "file" )
		{
			CComString sAttrValue = archive.GetAttribute("value");
			sAttrValue.Delete(sAttrValue.GetLength() - 4, 4);
			std::vector<CComString>::iterator iter = v_fileSkip.begin();
			bool bSkip = false;
			for( ;iter!=v_fileSkip.end();iter++)
			{
				CComString s = *iter;
				if(s == sAttrValue)
				{
					bSkip = true;
					break;
				}
			}
			if(bSkip)
				continue;
			
			CComString sNewFileName = sAttrValue;
	//			sNewFileName.Delete(sNewFileName.GetLength() - 4, 4);
			sprintf((char*)&text, "Копирование\n%s",sAttrValue.GetBuffer());
			pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );	
			int pos = sNewFileName.Find("_");
			sNewFileName.Delete(0,pos+1);
			pos = sNewFileName.Find("/");
			if ( pos == -1 )
			{
				pos = sNewFileName.Find("\\");
			}
			sNewFileName.Delete(0,pos+1);
			
			//sAttrValue.Replace("/","\\");
			sAttrValue.Replace("\\","/");
			sAttrValue = glpcRootPath + sAttrValue;
			sNewFileName = ClientPath + sNewFileName;
			//sNewFileName.Replace("/","\\");
			sNewFileName.Replace("\\","/");
			sAttrValue.MakeLower();
			sNewFileName.MakeLower();

			SECFileSystem fs;
			CComString destPath = fs.GetPath(sNewFileName);
			fs.MakePath(destPath);

			CComString moduleName = "";
			if ( sAttrValue.Find("updater.exe") != -1)
			{
				myUpdate = true;
				myUpdateStrFrom = sAttrValue;
				myUpdateStrTo = ClientPath+"updater.exe";
				moduleName = "updater";
			}
			else if ( sAttrValue.Find("mdump.dll") != -1)
			{
				myUpdate = true;
				myUpdateStrFrom = sAttrValue;
				myUpdateStrTo = ClientPath+"mdump.dll";
				moduleName = "mdump";
			}
			else if ( sAttrValue.Find("serviceMan.dll") != -1)
			{
				myUpdate = true;
				myUpdateStrFrom = sAttrValue;
				myUpdateStrTo = ClientPath+"serviceMan.dll";
				moduleName = "serviceMan";
			}
			else if ( sAttrValue.Find("libcurl.dll") != -1)
			{
				myUpdate = true;
				myUpdateStrFrom = sAttrValue;
				myUpdateStrTo = ClientPath+"libcurl.dll";
				moduleName = "libcurl";
			}
			else if ( sAttrValue.Find("pathrefiner.dll") != -1)
			{
				myUpdate = true;
				myUpdateStrFrom = sAttrValue;
				myUpdateStrTo = ClientPath+"pathrefiner.dll";
				moduleName = "pathrefiner";
			}
			else if ( sAttrValue.Find("libxml2.dll") != -1)
			{
				myUpdate = true;
				myUpdateStrFrom = sAttrValue;
				myUpdateStrTo = ClientPath+"libxml2.dll";
				moduleName = "libxml2";
			}
			else if ( sAttrValue.Find("iconv.dll") != -1)
			{
				myUpdate = true;
				myUpdateStrFrom = sAttrValue;
				myUpdateStrTo = ClientPath+"iconv.dll";
				moduleName = "iconv";
			}
			else if ( sAttrValue.Find("zlib32.dll") != -1)
			{
				myUpdate = true;
				myUpdateStrFrom = sAttrValue;
				myUpdateStrTo = ClientPath+"zlib32.dll";
				moduleName = "zlib32";
			}
			else if ( sAttrValue.Find("logwriter.dll") != -1)
			{
				myUpdate = true;
				myUpdateStrFrom = sAttrValue;
				myUpdateStrTo = ClientPath+"logwriter.dll";
				moduleName = "logwriter";
			}
			else if ( sAttrValue.Find("bigfileman.dll") != -1)
			{
				myUpdate = true;
				myUpdateStrFrom = sAttrValue;
				myUpdateStrTo = ClientPath+"bigfileman.dll";
				moduleName = "bigfileman";
			}
			else
			{
				while (1)
				{
					bool bError = false;
					if( pBigFileManCommon != NULL && sNewFileName.Find( sCommonPath)>-1)
					{
						bError = pBigFileManCommon->AddFile( sNewFileName.Right( sNewFileName.GetLength() - sCommonPath.GetLength()), NULL, sNewFileName)>0?false:true;
						break;
					}
					else if( pBigFileManUI != NULL && sNewFileName.Find( sUIPath)>-1)
					{
						bError = pBigFileManUI->AddFile( sNewFileName.Right( sNewFileName.GetLength() - sUIPath.GetLength()), NULL, sNewFileName)>0?false:true;
						break;
					}
					else
					{
						if (CopyFile(sAttrValue.GetBuffer(),sNewFileName.GetBuffer() , false))//копирум куда надо
						{
							//DeleteFile(sAttrValue.GetBuffer());//удаляем файл
							bError = false;
						}
						else
							bError = true;
					}
					if(bError)
					{
						int iError = GetLastError();
						if (pDlg->MessageBox("Возможные причины этого:\n1. Установлены неверные права папки, где установлена Виртуальная Академия. (Следует сменить права.)\n2. В процессах висит программа Виртуальная Академия. (Попробовать вручную выгрузить ВУ из диспетчера задач.)\n3. Запущена копия программы Виртуальная Академия. (Завершить копию программы)\n4. У вас недостаточно прав. (Получить у администратора необходимые права.)\n\n После устранения причин попробуйте повторить попытку копирования", "Неудалось скопировать файл", MB_RETRYCANCEL | MB_ICONINFORMATION) != IDCANCEL)
							continue;
						else
						{
								//MessageBox("Выход", "Выход", MB_OK | MB_ICONINFORMATION);
							return;
						}
					}
					else
						break;
				}
			}
			if ( myUpdate )
			{
				CreateBatFile( moduleName, myUpdateStrFrom, myUpdateStrTo);
				myUpdate = false;
			}
		}
	}


	if( pBigFileManUI != NULL)
		pBigFileManUI->ReSaveBigFile( NULL);
	if( pBigFileManCommon != NULL)
		pBigFileManCommon->ReSaveBigFile( NULL);
	DestroyBigFileManagerImpl( pBigFileManUI, sUIPath);
	DestroyBigFileManagerImpl( pBigFileManCommon, sCommonPath);
	DestroyBigFileDll();

	//MessageBox("Запускаем батники", "Запускаем батники", MB_OK | MB_ICONINFORMATION);
	for (int i = 0; i < BatFiles.size(); ++i)
	{
		HINSTANCE h = ::ShellExecute(NULL, "open", BatFiles[i].GetBuffer(), NULL, NULL, SW_HIDE); // Запускаем батник
	}
	//}
//	ReleaseMutex(MutexBatExecuting);

	//MessageBox("Правим конфиги", "Правим конфиги", MB_OK | MB_ICONINFORMATION);

	pDlg->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(99), 0);
	sprintf((char*)&text, "Обновление информации о версии");
	pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );

	pDlg->WritePlayerIni( text);

	fcloseall();

		//MessageBox("закончили копировать", "закончили копировать", MB_OK | MB_ICONINFORMATION);

	CComString CachePath = glpcRootPath;
	CachePath +="\\root\\files\\client";
	CachePath.Replace("\\\\", "\\");

	delDir(CachePath);

	sprintf((char*)&text, "Обновление успешно завершено.\nКлиент обновлён до версии %s",m_NewNersion.GetBuffer());

	wnd = pDlg->GetDlgItem(IDC_CHECK1);
	wnd.EnableWindow( TRUE);
	wnd.ShowWindow( SW_SHOW);
	wnd.UpdateWindow();
	wnd.SendMessage( BM_SETCHECK ,BST_CHECKED,0);

	pDlg->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
	pDlg->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(100), 0);
	CloseHandle(hLoadingFilesThread);
	hLoadingFilesThread = NULL;
}

bool  CupdaterFiles::delDir( CComString &sDir)
{
	WIN32_FIND_DATA fileData;
	HANDLE	hSearch =  FindFirstFile(sDir + "*.*", &fileData); 

	if (hSearch != INVALID_HANDLE_VALUE )
	{
		BOOL bContinueSearch = TRUE;
		while (bContinueSearch)
		{
			if (strcmp(fileData.cFileName, ".") == 0 || strcmp(fileData.cFileName, "..") == 0)
			{
				bContinueSearch = FindNextFile(hSearch, &fileData);
				continue;
			}

			if((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				CComString s = sDir + fileData.cFileName;
				s+="\\";
				delDir( s);
				bContinueSearch = FindNextFile(hSearch, &fileData);
				try
				{
				::RemoveDirectory( s.GetBuffer());
				}
				catch (...)
				{
				}
				continue;
			}

			CComString s = sDir+fileData.cFileName;
			bContinueSearch = FindNextFile(hSearch, &fileData);
			try
			{
				::SetFileAttributes( s.GetBuffer(), FILE_ATTRIBUTE_NORMAL);
			::DeleteFile( s.GetBuffer());
		}
			catch (...)
			{
	}
		}
	}
	FindClose(hSearch);
	return true;
}

void CupdaterFiles::LoadBigFileDll()
{

#ifdef DEBUG
#define	BIGFILEMAN_MANAGER_DLL "BigFileMan.dll"
#else
#define	BIGFILEMAN_MANAGER_DLL "BigFileMan.dll"
#endif

	if ( !hBigFileModule)
		hBigFileModule = loadDLL( "", BIGFILEMAN_MANAGER_DLL);
}

int CupdaterFiles::CreateBigFileManagerImpl( bigfile::IBigFileManager **ppBigFileMan, LPCTSTR aModuleFile)
{
	int errorCode = -1;
	if ( hBigFileModule)
	{
		unsigned int (*lpCreateBigFileManager)( bigfile::IBigFileManager **apBigFileMan, LPCTSTR asModuleFile);
		(FARPROC&)lpCreateBigFileManager = (FARPROC) GetProcAddress( (HMODULE) hBigFileModule, "CreateBigFileManager");
		if ( lpCreateBigFileManager)
			errorCode = lpCreateBigFileManager( ppBigFileMan, aModuleFile);
	}
	return errorCode;
}

void CupdaterFiles::DestroyBigFileDll()
{
	ATLASSERT( pOMSContext);
	if( !pOMSContext)
		return;

	if( !hBigFileModule)
		return;

	FreeLibrary( (HMODULE)hBigFileModule);
	hBigFileModule = NULL;
}

void CupdaterFiles::DestroyBigFileManagerImpl( bigfile::IBigFileManager *apBigFileMan, LPCTSTR aModuleFile)
{
	ATLASSERT( pOMSContext);
	if( !pOMSContext)
		return;

	if( !hBigFileModule)
		return;

	void (*lpDestroyBigFileManager)( bigfile::IBigFileManagerBase *apBigFileMan, LPCTSTR asModuleFile);
	(FARPROC&)lpDestroyBigFileManager = (FARPROC)GetProcAddress( (HMODULE)hBigFileModule, "DestroyBigFileManager");
	if( lpDestroyBigFileManager)
		lpDestroyBigFileManager( ( bigfile::IBigFileManager *)apBigFileMan, aModuleFile);
}