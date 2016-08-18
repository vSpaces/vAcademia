// updaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UpdaterFiles.h"
//#include "crmMemResourceFile.h"
#include "ComString.h"
#include "FileSys.h"
#include "gd/gd_XMLArchive.h"

#include <shellapi.h>

#include ".\updaterdll.h"
#include <shlobj.h>

#include "SumChecker.h"

#define tmpBuffSize 65535

//CComModule _Module;

void makeDir(const char* aDirPath)
{
	TCHAR temp[MAX_PATH2];
	memset(&temp, 0, MAX_PATH2);
	_tcscat(temp, aDirPath);

	LPTSTR start;
	LPTSTR point;
	for(start = point = temp; *point; point++)
	{
		if(*point == _T('/'))
		{
			*point = _T('\0'); 
			CreateDirectory(start, NULL);
			*point = _T('/');
		}
	}
}

void CupdaterFiles::setDll(  CUpdaterDll *apDll)
{
	pDll = apDll;
}

bool CupdaterFiles::CopyFileToClient( CWComString &sSrcFilePath, CWComString &sDestFilePath)
{
	sSrcFilePath.MakeLower();
	sDestFilePath.MakeLower();
	sSrcFilePath.Replace(L"\\",L"/");
	sDestFilePath.Replace(L"\\",L"/");

	//bool myUpdate = false;
	CComString moduleName = "";

	while (1)
	{
		int iError = 0;
		if( pBigFileManCommon != NULL && sDestFilePath.Find( sCommonPath)>-1)
		{
			bCommonChange = true;
			CComString sFileName = sDestFilePath.Right( sDestFilePath.GetLength() - sCommonPath.GetLength());
			iError = pBigFileManCommon->AddFile( sFileName.GetBuffer(), NULL, sSrcFilePath.GetBuffer());
			if(!iError)
				break;
		}
		else if( pBigFileManGeometry != NULL && sDestFilePath.Find( sGeometryPath)>-1)
		{
			bGeometryChange = true;
			CComString sFileName = sDestFilePath.Right( sDestFilePath.GetLength() - sGeometryPath.GetLength());
			iError = pBigFileManGeometry->AddFile( sFileName.GetBuffer(), NULL, sSrcFilePath.GetBuffer());
			if(!iError)
				break;
		}
		else if( pBigFileManUI != NULL && sDestFilePath.Find( sUIPath)>-1)
		{
			bUIChange = true;
			CComString sFileName = sDestFilePath.Right( sDestFilePath.GetLength() - sUIPath.GetLength());
			iError = pBigFileManUI->AddFile( sFileName.GetBuffer(), NULL, sSrcFilePath.GetBuffer());
			if(!iError)
				break;
		}
		else
		{
			CComString sNewFileDir = sDestFilePath;
			int pos = sNewFileDir.ReverseFind('/');
			sNewFileDir = sNewFileDir.Left( pos + 1);
			makeDir( sNewFileDir);
			if (CopyFileW( sSrcFilePath.GetBuffer(), sDestFilePath.GetBuffer(), false))//копируем куда надо
			{
				::DeleteFileW( sSrcFilePath.GetBuffer());			
				iError = 0;		
			}
			else
			{
				iError = 1;
			}
		}
		if(iError>0)
		{
			CComString log = "Cannot copy a file ";
			USES_CONVERSION;
			log += W2A(sSrcFilePath.GetBuffer());
			pDll->WriteLog(log, iError);

			return false;
		}
		else
			break;
	}

	return true;
}

bool CupdaterFiles::UpdateClient_CopyFileRec( CWComString &sSrcDir, CWComString &sDestDir, int &aProgress)
{
	WIN32_FIND_DATAW fileData;
	HANDLE	hSearch =  FindFirstFileW(sSrcDir + L"*.*", &fileData);
	aProgress++;
	pDll->setOneFileStatus( aProgress);
	Sleep(1);

	if (hSearch != INVALID_HANDLE_VALUE )
	{
		BOOL bContinueSearch = TRUE;
		while (bContinueSearch)
		{
			if (wcscmp(fileData.cFileName, L".") == 0 || wcscmp(fileData.cFileName, L"..") == 0)
			{
				bContinueSearch = FindNextFileW(hSearch, &fileData);
				continue;
			}

			if((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				CWComString s1 = sSrcDir + fileData.cFileName + L"\\";
				CWComString s2 = sDestDir + fileData.cFileName + L"\\";
				bContinueSearch = FindNextFileW(hSearch, &fileData);
				try
				{
					UpdateClient_CopyFileRec( s1, s2, aProgress);
					//::RemoveDirectory( s.GetBuffer());
				}
				catch (...)
				{
				}
				continue;
			}

			CWComString sFile = fileData.cFileName;
			CWComString sFileArchivePath = sSrcDir + fileData.cFileName;
			bContinueSearch = FindNextFileW(hSearch, &fileData);
			if(sFileArchivePath.Find(fullPathFile, 0)>-1)
				continue;
			
			int pos = sFileArchivePath.Find(sArchivePathBase, 0);
			CWComString s = sFileArchivePath.Right( sFileArchivePath.GetLength() - sArchivePathBase.GetLength());
			if( pBigFileManUI != NULL && s.Find(L"ui\\")>-1)
				s = L"ui\\ui.pack";
			else if( pBigFileManCommon != NULL && s.Find(L"root\\files\\vu.common\\")>-1)
				s = L"root\\files\\vu.common\\vu.common.pack";
			else if( pBigFileManGeometry != NULL && s.Find(L"root\\files\\geometry\\")>-1)
				s = L"root\\files\\geometry\\geometry.pack";
			USES_CONVERSION;
			pDll->setInfoText( W2A(s.GetBuffer()));
			CWComString sFileDestPath = sDestDir + sFile;
			if(!CopyFileToClient( sFileArchivePath, sFileDestPath))
				return false;
		}
	}
	FindClose(hSearch);
	return true;
}

void CupdaterFiles::UpdateClient_CopyFile( CWComString &sFullPathDir,DWORD dwDowloadUpdateSize)
{
	pDll->NextStep(); // 6
	//pDll->setOneFileStatus( 0);

	Sleep(10);
	bool bRes = false;
	USES_CONVERSION;
	CComString sRes = "UpdateClient_CopyFile::ClientPath=="; sRes += W2A( m_ClientPath.GetBuffer());
	CComString sCode;
	pDll->WriteLog( sRes, sCode);
	if( sFullPathDir.GetLength()>0)
	{
		ClientPath = m_ClientPath;//glpcRootPath;
		ClientPath.MakeLower();

		LoadBigFileDll();
		CWComString sClientPath = ClientPath;
		sClientPath.Replace(L"\\", L"/");
		sUIPath = sClientPath + L"/ui";
		sUIPath.Replace(L"//", L"/");
		sCommonPath = sClientPath + L"/root/files/vu.common";
		sCommonPath.Replace(L"//", L"/");
		sGeometryPath = sClientPath + L"/root/files/geometry";
		sGeometryPath.Replace(L"//", L"/");
		
		int errorCode = CreateBigFileManagerImpl( &pBigFileManUI, sUIPath + L"/ui.pack");
		errorCode = CreateBigFileManagerImpl( &pBigFileManCommon, sCommonPath + L"/vu.common.pack");
		errorCode = CreateBigFileManagerImpl( &pBigFileManGeometry, sGeometryPath + L"/geometry.pack");
		sArchivePathBase = sFullPathDir;
		sRes = "CreateBigFileManagerImpl is successfull";		
		pDll->WriteLog( sRes, sCode);
		int progress = 0;
		bRes = UpdateClient_CopyFileRec( sFullPathDir, ClientPath, progress);
		//pDll->setOneFileStatus( 60);
		pDll->setStatus(75);

		// если не удалось скопировать файл, скачиваем клиент
		if (!bRes)
		{
			pDll->RequestDownloadSetup();
			return;
		}


		CWComString sFileName = ClientPath + L"/listdeletedfiles.ini";
		sFileName.Replace(L"\\", L"/");
		sFileName.Replace(L"//", L"/");

		std::vector<CWComString> dirs;
		HANDLE hFile = CreateFileW( sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			USES_CONVERSION;
			CComString sRes = "delete files from list "; sRes += W2A(sFileName.GetBuffer());
			CComString sCode;
			pDll->WriteLog( sRes, sCode);
			DWORD dwBufferSize = GetFileSize(hFile, NULL);
			DWORD dwReadSize = 0;
			BYTE *pBuffer = MP_NEW_ARR( BYTE, dwBufferSize + 1);
			if(::ReadFile( hFile, pBuffer, dwBufferSize, &dwReadSize, NULL) || dwReadSize == 0)
			{	
				int idx = 0;
				unsigned int count = 0;
				memparse_object( pBuffer, idx, count, dwBufferSize);
				//CComString sFilesDelete = pBuffer;
				int pos = 0;
				for(int i = 0; i< count; i++)
					//while( (pos = sFilesDelete.Find("\n"))>-1)
				{
					CWComString sFileDelete;
					memparse_string_unicode( pBuffer, idx, sFileDelete, dwBufferSize);
					sFileDelete = ClientPath + sFileDelete;
					sFileDelete.Replace(L"\\", L"/");
					sFileDelete.Replace(L"//", L"/");
					sFileDelete.MakeLower();
					//CComString sFileDelete = sFilesDelete.Left(pos);	
					CWComString sFilePath = sFileDelete.Left( sFileDelete.ReverseFind('/') + 1);
					//if(dirs.find(sFilePath) == dirs.end())				
					//CWComString sFileRelPath = sFileDelete;
					int  iError = 0;

					CComString sDel = "UpdateClient_CopyFile::del file: ";
					sDel += sFileDelete.GetBuffer();
					sCode.Empty();
					pDll->WriteLog( sDel, sCode);

					if( pBigFileManCommon != NULL && sFileDelete.Find( sCommonPath)>-1)
					{
						bCommonChange = true;
						CComString sDeleteFilePath =  W2A( sFileDelete.Right( sFileDelete.GetLength() - sCommonPath.GetLength()).GetBuffer());
						iError = pBigFileManCommon->DeleteFile( sDeleteFilePath.GetBuffer());						
					}
					else if( pBigFileManGeometry != NULL && sFileDelete.Find( sGeometryPath)>-1)
					{
						bGeometryChange = true;
						CComString sDeleteFilePath = W2A( sFileDelete.Right( sFileDelete.GetLength() - sGeometryPath.GetLength()).GetBuffer());
						iError = pBigFileManGeometry->DeleteFile( sDeleteFilePath.GetBuffer());						
					}					
					else if( pBigFileManUI != NULL && sFileDelete.Find( sUIPath)>-1)
					{
						bUIChange = true;
						CComString sDeleteFilePath = W2A( sFileDelete.Right( sFileDelete.GetLength() - sUIPath.GetLength()).GetBuffer());
						iError = pBigFileManUI->DeleteFile( sDeleteFilePath.GetBuffer());
						
					}
					else
					{
						std::vector<CWComString>::iterator iter = dirs.begin();
						bool bFind = false;					
						for( ;iter != dirs.end(); iter++)
						{
							if( sFilePath.Find(iter->GetBuffer()) == 0)
							{
								bFind = true;
								break;
							}
						}
						if(!bFind && sFilePath.GetLength()>1)
							dirs.push_back( sFilePath);						
						::DeleteFileW( sFileDelete);
					}
					//sFilesDelete = sFilesDelete.Right( sFilesDelete.GetLength() - pos - 1);
				}
			}
			MP_DELETE_ARR( pBuffer);
			CloseHandle( hFile);
			::DeleteFileW( sFileName);
			sRes = "delete file - "; sRes += W2A(sFileName.GetBuffer());
			pDll->WriteLog( sRes, sCode);
		}
		//pDll->setOneFileStatus( 67);
		pDll->setStatus(76);
		std::vector<CWComString>::iterator iter = dirs.begin();
		while( iter != dirs.end())
		{		
			WIN32_FIND_DATAW fileData;
			HANDLE hFind = FindFirstFileW((CWComString)(*iter) + L"*.*", &fileData);
			bool bFind = false;
			if (hFind != INVALID_HANDLE_VALUE )
			{
				BOOL bContinueSearch = TRUE;
				while (bContinueSearch)
				{
					if (wcscmp(fileData.cFileName, L".") == 0 || wcscmp(fileData.cFileName, L"..") == 0)
					{
						bContinueSearch = FindNextFileW(hFind, &fileData);
						continue;
					}
					bFind = true;
					break;
				}
				FindClose(hFind);
				
			}
			if (!bFind)
			{
				CComString sDel = "UpdateClient_CopyFile::del dir: ";
				USES_CONVERSION;
				sDel += W2A(iter->GetBuffer());
				sCode.Empty();
				pDll->WriteLog( sDel, sCode);
				::RemoveDirectoryW( iter->GetBuffer());
			}
			iter++;
		}
		dirs.clear();

		//pDll->setOneFileStatus( 72);
		pDll->setStatus(77);

		sRes = "Copies is successfull and destroying bigfile manager ";
		pDll->WriteLog( sRes, sCode);
		sRes = (bUIChange == true ? "UI has changed, " : "UI has not changed, ");
		sRes += (bCommonChange == true ? "Common has changed, " : "Common has not changed, ");
		sRes += (bGeometryChange == true ? "Geometry has changed" : "Geometry has not changed");
		pDll->WriteLog( sRes, sCode);

		if( pBigFileManUI != NULL && bUIChange)
			pBigFileManUI->ReSaveBigFile( NULL);
		//pDll->setOneFileStatus( 81);
		pDll->setStatus(82);
		if( pBigFileManCommon != NULL && bCommonChange)
			pBigFileManCommon->ReSaveBigFile( NULL);
		pDll->setStatus(85);
		//pDll->setOneFileStatus( 88);
		if( pBigFileManGeometry != NULL && bGeometryChange)
			pBigFileManGeometry->ReSaveBigFile( NULL);
		pDll->setStatus(88);
		//pDll->setOneFileStatus( 93);
		DestroyBigFileManagerImpl( pBigFileManUI, sUIPath);
		//pDll->setOneFileStatus( 95);
		pDll->setStatus(90);
		DestroyBigFileManagerImpl( pBigFileManCommon, sCommonPath);
		//pDll->setOneFileStatus( 97);
		DestroyBigFileManagerImpl( pBigFileManGeometry, sGeometryPath);
		//pDll->setOneFileStatus( 99);
		DestroyBigFileDll();
		pDll->setStatus(92);
		sRes = "DestroyBigFileDll is successfull";
		pDll->WriteLog( sRes, sCode);
		Sleep(10);
		//pDll->setOneFileStatus( 100);
	}
	else
		bRes = true;	

	if(!bRes)
	{
		pDll->RequestDownloadSetup();
		return;
	}

	sRes = "bat files is starting";
	sCode = "";
	pDll->setOneFileStatus( 0);
	pDll->WriteLog( sRes, sCode);
	pDll->NextStep();  // 7
	pDll->setStatus(94);
	pDll->setOneFileStatus( 30);
	Sleep(10);

	pDll->setStatus( 96);	

	// делаем проверку контрольной суммы обновленных файлов на основе мастер-файла, который идет с апдейтом. файл в папке с клиентом

	SumChecker* checker = new SumChecker();
	sRes = "ClientPath=="; sRes += W2A( m_ClientPath.GetBuffer());
	pDll->WriteLog( sRes, sCode);
	checker->CheckAllFiles(m_ClientPath.GetBuffer());

	// В случае несовпадения в переменную SumChecker:: sumStatus сгружается инфа о модифицированных файлах.
	// В случае неудачного обновления предлагать скачать нового клиента. При согласии скачивать и запускать его установку.
	std::string error = checker->sumStatus;

	if (error.length() > 0 )
	{
		sRes = "Check summ error: ";
		sRes +=checker->sumStatus;
		pDll->WriteLog( sRes, sCode);
		pDll->RequestDownloadSetup();
		return;
	}

	sRes = "WritePlayerIni is starting";
	pDll->WriteLog( sRes, sCode); 
	fcloseall();

	if (!pDll->WritePlayerIni())
	{
		pDll->RequestDownloadSetup();
		return;
	}

	pDll->setOneFileStatus( 0);
	pDll->SetStep( 79);	
	delCache( 0);

	pDll->RemoveStartFailFileMarker();
	pDll->ReleaseLibraries();

	pDll->setInfoText(m_NewNersion.GetBuffer());
	pDll->setStatus(100);
	pDll->Finish( true);	
}

bool CupdaterFiles::memparse_string_unicode(BYTE* buf, int& pos, CWComString& val, unsigned int iDataSize)
{
	unsigned short iLen;
	if (!memparse_length( buf, pos, iLen, iDataSize))
		return false;
	unsigned int iNewPos = iLen*2;
	iNewPos += pos;
	if(iNewPos > iDataSize)
	{
		return false;
	}

	wchar_t* temp = MP_NEW_ARR( wchar_t, (iLen + 1) * 2);
	if (temp == NULL)
	{
		return false;
	}
	memcpy(temp, buf + pos, iLen*2);	temp[iLen] = 0;
	val = temp;
	MP_DELETE_ARR( temp);

	pos = iNewPos;
	return true;
}

void CupdaterFiles::GetStringGUID( wchar_t* apwcGUID, unsigned short aSizeGUID)
{
	apwcGUID[ 0] = 0;
	GUID guid;
	if (SUCCEEDED( CoCreateGuid( &guid)))
		apwcGUID[ StringFromGUID2( guid, apwcGUID, aSizeGUID)] = 0;
	else
		wcscpy( apwcGUID, L"1234567890_1234567890");
}

bool CupdaterFiles::testCreateFile()
{
	//return false;
	CWComString tempPath = m_ClientPath;//glpcRootPath;
	wchar_t wsGUID[ 40];
	wchar_t *pGUID = wsGUID;
	GetStringGUID( pGUID, 40);
	CWComString sGuid = wsGUID;
	sGuid.Replace(L"{", L"");
	sGuid.Replace(L"}", L"");
	sGuid.Replace(L"-", L"");
	tempPath += L"\\";
	tempPath += sGuid.GetBuffer();
	tempPath += L"\\";
	tempPath.Replace(L"/", L"\\");
	tempPath.Replace(L"\\\\", L"\\");

	CComString s = "test creating test file";
	CComString sCode;
	pDll->WriteLog( s, sCode);

	//MessageBox( NULL, CachePath.GetBuffer(), "CachePath", MB_OK | MB_ICONINFORMATION);
	::CreateDirectoryW( tempPath.GetBuffer(), NULL);
	int code = GetLastError();	
	if( !PathFileExistsW(tempPath.GetBuffer()) && code != 0 && code != ERROR_ALREADY_EXISTS)
	{
		CComString s = "test create dir is failed";
		CComString sCode;
		pDll->WriteLog( s, sCode);
		return false;
	}
	CWComString tempFile = tempPath + sGuid + L"_2.txt";
	HANDLE hFile = ::CreateFileW( tempFile.GetBuffer(), GENERIC_WRITE, FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == NULL || hFile == INVALID_HANDLE_VALUE)
	{
		int code = GetLastError();
		//if( code == ERROR_ACCESS_DENIED || code != ERROR_ALREADY_EXISTS && code != ERROR_PATH_NOT_FOUND)
		if( code != 0)
		{
			CComString s = "test create file is failed";
			CComString sCode;
			pDll->WriteLog( s, sCode);
			return false;
		}
	}	
	::CloseHandle( hFile);
	int  progress = -1;
	pDll->DelDir(tempPath, progress);
	::RemoveDirectoryW( tempPath);
	return true;
}

void CupdaterFiles::delCache( int aProgress)
{
	CWComString CachePath = glpcRootPath;
	CachePath +=L"\\root\\files\\client\\";
	CachePath.Replace(L"/", L"\\");
	CachePath.Replace(L"\\\\", L"\\");

	//MessageBox( NULL, CachePath.GetBuffer(), "CachePath", MB_OK | MB_ICONINFORMATION);

	pDll->DelDir(CachePath, aProgress);
	pDll->setOneFileStatus( 100);
	::RemoveDirectoryW( CachePath);
}

void CupdaterFiles::Clear()
{
	char tmp[MAX_PATH];
	SHGetSpecialFolderPath(NULL, tmp, CSIDL_APPDATA, true);

	CComString sFileLog = tmp;
	sFileLog += "\\updater.log";
	sFileLog.Replace("\\\\", "\\");
	::DeleteFile( sFileLog);
}

void CupdaterFiles::LoadBigFileDll()
{

#ifdef DEBUG
#define	BIGFILEMAN_MANAGER_DLL "BigFileMan.dll"
#else
#define	BIGFILEMAN_MANAGER_DLL "BigFileMan.dll"
#endif

	if ( !hBigFileModule)
		hBigFileModule = pDll->LoadDLL( "", BIGFILEMAN_MANAGER_DLL);
}

int CupdaterFiles::CreateBigFileManagerImpl( bigfile::IBigFileManager **ppBigFileMan, const wchar_t *aModuleFile)
{
	int errorCode = -1;	
	if ( hBigFileModule)
	{
		unsigned int (*lpCreateBigFileManager)( bigfile::IBigFileManager **apBigFileMan, const wchar_t *asModuleFile);
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

	::FreeLibrary( (HMODULE)hBigFileModule);
	hBigFileModule = NULL;
}

void CupdaterFiles::DestroyBigFileManagerImpl( bigfile::IBigFileManager *apBigFileMan, const wchar_t* aModuleFile)
{
	ATLASSERT( pOMSContext);
	if( !pOMSContext)
		return;

	if( !hBigFileModule)
		return;

	void (*lpDestroyBigFileManager)( bigfile::IBigFileManagerBase *apBigFileMan, const wchar_t* asModuleFile);
	(FARPROC&)lpDestroyBigFileManager = (FARPROC)GetProcAddress( (HMODULE)hBigFileModule, "DestroyBigFileManager");
	if( lpDestroyBigFileManager)
		lpDestroyBigFileManager( ( bigfile::IBigFileManager *)apBigFileMan, aModuleFile);
}