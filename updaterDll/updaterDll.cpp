// updaterDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include <windows.h>
//#include <atlbase.h>
#include "updaterDll.h"
#include "FileSys.h"
//#include "crmMemResourceFile.h"
#include "CommandLineParams.h"
#include <shobjidl.h>
#include "shlobj.h"
#include "PlatformDependent.h"

#include <shellapi.h>
#pragma comment(lib,"shell32")

#include "Tlhelp32.h"
#include "Psapi.h"
#include "../mdump/mdump.h"

#include <mmsystem.h>
#include "mutex.h"

CComModule _Module;

//bool (*pSetTextCurrentInfoFunc)( char *s) = NULL;
//TSetTextCurrentInfoFunc *pSetTextCurrentInfoFunc = NULL;
static char buff[512];
static wchar_t sServer[512];

CUpdaterDll::CUpdaterDll()
{
	hDownloadThread = NULL;
	iStep = 0;
	sp_Resource = NULL;
	sp_ComMan = NULL;
	sp_ResourceManager = NULL;
	context = MP_NEW(oms::omsContext);
	hComModule = NULL;
	//hServiceModule = NULL;
	countAuth = 0;
	hCheckLoginThread = NULL;
	m_hCheckLogin = CreateEvent( NULL, TRUE, FALSE, NULL);
	//MP_NEW_V2(pUpdaterFiles, CupdaterFiles, pOMSContext, this);
	status = 0;
	statusOneFile = 0;
	currentSize = 0;
	dwAllSize = 0;
	iErrorCodeLogin = 0;
	sInfoText = "123";
	m_EncPass = 0;
	m_isLoginSuccess = false;
	iErrorCode = -1;
	m_sServer = L"178.63.80.136";
	m_downloadURL = "http://www.vacademia.com/site/downloadClientExe";
	m_loadingSetupStatus = LOADING_SETUP_DEFAULT;
	m_isAlreadySiteOpened = false;
	pClientDownloader = NULL;
	//mpIResourceManager = NULL;
}

CUpdaterDll::~CUpdaterDll()
{	
	// при закрытии апдейтера и возможной критической ошибки отправляем на сайт	
	OpenSiteIfNeeded();
	/*if( pUpdaterFiles != NULL)
	{
		pUpdaterFiles->setDll( NULL);
		MP_DELETE( pUpdaterFiles);
		pUpdaterFiles = NULL;
	}*/
	if( pClientDownloader != NULL)
	{
		pClientDownloader->setDll( NULL);
		MP_DELETE( pClientDownloader);
		pClientDownloader = NULL;
	}
	ATLASSERT( context);
	if( !context)
		return;
	//ReleaseLibraryServiceMan();
	ReleaseLibraryCommanMan();
	::SetCurrentDirectoryW( msOldPath);
	::SetDllDirectory( NULL);;
	CloseHandle(m_hCheckLogin);
	if( context)
		MP_DELETE(context);
	context = NULL;
}

void CUpdaterDll::OnSiteOpened()
{
	m_isAlreadySiteOpened= true;
}

void CUpdaterDll::OpenSiteIfNeeded()
{
	if (m_isAlreadySiteOpened)
		return;

	// если закрываем на критической ошибке, на этапе копирования файлов, распаковки или скачивания клиента
	// то отправляем на сайт
	int step = GetStep();

	if (IsCriticalError(GetErrorCode()) || step == 6 || step == 7 ||
		(step == 9 || step == 10 && (m_loadingSetupStatus == LOADING_SETUP_PROCESS)) )
	{
		std::wstring sFileLog = GetApplicationDataDirectory();
		sFileLog += L"\\Vacademia\\updater_open_site.bat";

		std::string sCommand;
		
		sCommand = "start " + m_downloadSiteURL;

		FILE* file = _wfopen(sFileLog.c_str(), L"w");
		fwrite(sCommand.c_str(), sCommand.size() ,1, file);
		fclose(file);

		STARTUPINFOW si;
		ZeroMemory(&si,sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi,sizeof(pi));
		BOOL res = CreateProcessW(NULL, (LPWSTR)sFileLog.c_str(),NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

/*void CUpdaterDll::ReleaseLibraryServiceMan()
{
	try
	{
		if( !hServiceModule)
			return;
		void (*lpDestroyServiceManager)( oms::omsContext* aContext);
		(FARPROC&)lpDestroyServiceManager = (FARPROC)GetProcAddress( (unsigned long)hServiceModule, "DestroyServiceWorldManager");
		if( lpDestroyServiceManager)
		{
			lpDestroyServiceManager( context);
		}
		//service::DestroyServiceWorldManager( pOMSContext.get());
		//cm::DestroyComManager( pOMSContext.get());
		::FreeLibrary( hServiceModule);
		hServiceModule = NULL;
	}
	catch (...)
	{
	}
}*/

void CUpdaterDll::ReleaseLibraryCommanMan()
{
	try
	{
		if( !hComModule)
			return;
		void (*lpDestroyComManager)( oms::omsContext* aContext);;
		(FARPROC&)lpDestroyComManager = (FARPROC)GetProcAddress( (unsigned long)hComModule, "DestroyComManager");
		if( lpDestroyComManager)
		{
			lpDestroyComManager( context);
		}
		::FreeLibrary( hComModule);
		hComModule = NULL;
	}
	catch (...)
	{
	}
}


HWND ghwndProcessWindow = NULL;
//std::vector<HWND> v_WindowHandle;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam){
	DWORD dwWinProcID = 0;
	GetWindowThreadProcessId(hwnd, &dwWinProcID);
	if(dwWinProcID == (DWORD)lParam){
		TCHAR   className[MAX_PATH+1];
		if( GetClassName( hwnd, className, MAX_PATH) > 0)
		{
			if( _tcscmp( className, "VSpaces virtual worlds player") == 0)
			{
				ghwndProcessWindow = hwnd;
				//v_WindowHandle.push_back( hwnd);
				return FALSE; // нашли нужное окно. Можно не продолжать поиск
			}
		}
	}
	return TRUE;
}

bool CUpdaterDll::KillOtherPlayerProcesses( bool aFinsihAll)
{
	bool bRes = true;
	//CComString sFullPath1 = m_ClientPath;
	//sFullPath1.Replace("/", "\\");
	//sFullPath1.MakeLower();

	//надо прибивать все exe академии, иначе контрольные суммы не совпадут.
	//VoipMan.exe
	//vacademia.exe
	//tracer.exe
	//confchecker.exe
	//filter_control.exe
	//kazan_control.exe
	//Settings.exe
	//uninstall.exe
	//VacademiaReport.exe

	std::vector<CComString> terminateFiles;
	terminateFiles.push_back("vacademia.exe");
	terminateFiles.push_back("tracer.exe");
	terminateFiles.push_back("confchecker.exe");
	terminateFiles.push_back("filter_control.exe");
	terminateFiles.push_back("settings.exe");
	terminateFiles.push_back("uninstall.exe");
	terminateFiles.push_back("vacademiareport.exe");
	terminateFiles.push_back("voipman.exe");
	terminateFiles.push_back("player.exe");
	terminateFiles.push_back("kazan_control.exe");
	terminateFiles.push_back("server_switch.exe");
	
	//DWORD dwMyProcessID = GetCurrentProcessId();

	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32; 
	ZeroMemory(&pe32, sizeof(pe32));

	//  Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return bRes; 

	//  Fill in the size of the structure before using it. 
	pe32.dwSize = sizeof(PROCESSENTRY32); 

	//  Walk the snapshot of the processes, and for each process, 
	//  display information. 

	if (Process32First(hProcessSnap, &pe32)) { 
		DWORD         dwPriorityClass; 
		BOOL          bGotModule = FALSE; 
		MODULEENTRY32 me32; 
		//me32.dwSize = sizeof(me32);
		ZeroMemory(&me32, sizeof(me32));


		CComString processName;
		do{ 
			for (int i= 0; i < terminateFiles.size();i++)
			{
				processName = pe32.szExeFile;
				processName.MakeLower();
				if(terminateFiles[i].Find(processName) == -1)
					continue;
				// открываем процесс, чтобы узнать побольше о его состоянии
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,
					FALSE, pe32.th32ProcessID);
				// если процесс открыть не смогли, то пропускаем его
				if (hProcess == NULL)
					continue;

				CloseHandle(hProcess);

				// получаем описатель процесса
				hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
				if (hProcess == NULL)
					continue;

				DWORD dwError = ERROR_SUCCESS;

				// пытаемся завершить процесс
				if (!TerminateProcess(hProcess, (DWORD)-1))
					dwError = GetLastError();

				// закрываем описатель процесса
				CloseHandle(hProcess);
			}
			
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
	} 

	// Do not forget to clean up the snapshot object. 
	CloseHandle (hProcessSnap); 
	return bRes;
}


bool FileExists(const WCHAR* alpcFileName){
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(alpcFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);
	return true;
}

bool CUpdaterDll::ParseCmdLine(LPSTR alpCmdLine)
{
	CComString inLine = alpCmdLine;
	if(inLine.IsEmpty())
		return false;

	CCommandLineParams commandLineParams;
	commandLineParams.SetCommandLine(alpCmdLine);

	// Login password
	m_Login = I_AM_UPDATER;
	m_Password = "!@#$%^&*";

	if( commandLineParams.VersionIsSet())
		m_NewNersion = commandLineParams.GetVersion().c_str();

	if( commandLineParams.LanguageIsSet())
		m_LanguageIni = commandLineParams.GetLanguage().c_str();

	m_IsRequiredUpdate = commandLineParams.GetRequired();
	m_IsBrokenClient = commandLineParams.GetRequired();
	//m_IsNeedDownloadSetupOnly = commandLineParams.GetSetup();
}

bool CUpdaterDll::ParseINI(HINSTANCE ahInstance)
{
	m_parseINIError = 0;
//MessageBox(0, "Start parse ini", "", MB_OK);
	if(m_Login.IsEmpty())
	{
		m_parseINIError = 2;

		CComString sRes = "INI file parse error";
		CComString sCode( "m_Login.IsEmpty()");
		WriteLog( sRes, sCode);

		return false;
	}
	
	wchar_t lpcStrBuffer[MAX_PATH*2+2];
	wchar_t lpcExeFullPathName[MAX_PATH*2+2];
	wcscpy(lpcExeFullPathName, m_ClientPath.GetBuffer());
	wcscpy(lpcIniFullPathName, msPath.GetBuffer());

	//m_UpdaterPath = msPath;
	
	WCHAR rootPath[MAX_PATH * 2];
	wcscpy(rootPath, lpcExeFullPathName);

	wcscat(lpcIniFullPathName, L"player.ini");
	// если player.ini не найден,
//MessageBox(0, "Start parse ini", lpcIniFullPathName, MB_OK);
	if(!FileExists(lpcIniFullPathName))
	{
		m_parseINIError = 3;
		wcscpy(glpcRootPath, rootPath);

		CComString sRes = "INI file not found. Path = ";
		CComString sCode( lpcIniFullPathName);
		WriteLog( sRes, sCode);

		return false;
	}
	// Getting paths from INI-file
	if (GetPrivateProfileStringW( L"paths", L"root", L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		if (!wcscmp(lpcStrBuffer, L".\\"))
		{
			wcscpy(glpcRootPath, rootPath);
		}
		else
		{
			wcscpy(glpcRootPath,lpcStrBuffer);
		}
		int ii = 0;
		for(ii=0; glpcRootPath[ii]; ii++);
		{
			if(glpcRootPath[ii-1]!=_T('\\') && glpcRootPath[ii-1]!=_T('/'))
			{
				glpcRootPath[ii]=_T('\\'); 
				glpcRootPath[ii+1]=0;
			}
		}
	}
	if (GetPrivateProfileStringW(L"settings",L"version",L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		for(unsigned int i = 0; i < wcslen(lpcStrBuffer) && lpcStrBuffer[i] != ' '; ++i)
		{
			ucCurentVersion += lpcStrBuffer[i];
		}
	}

	if (GetPrivateProfileStringW(L"paths",L"server",L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		m_sServer = lpcStrBuffer;
	}
	
	if (GetPrivateProfileStringW(L"window",L"title",L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		m_sTitle = lpcStrBuffer;
		if( m_sTitle.Find(L"\"") == -1)
		{
			m_sTitle.Insert(0, L"\"");
			m_sTitle += L"\"";
		}
	}

	if (GetPrivateProfileStringW(L"settings",L"domain",L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		USES_CONVERSION;
		m_downloadURL = W2A(lpcStrBuffer);
		
		char sIsInt[2];
		sIsInt[0] = m_downloadURL[0];
		sIsInt[1] ='\0';
/*		if( m_downloadURL.find("www.") == std::string::npos && rtl_atoi( sIsInt) == 0)
			m_downloadURL =  "www." + m_downloadURL;*/
		if( m_downloadURL.find("http://") == std::string::npos)
			m_downloadURL =  "http://" + m_downloadURL;
		m_downloadSiteURL = m_downloadURL;
		m_downloadURL += "/site/downloadClientExe";
		m_downloadSiteURL += "/site/downloadclient";
		m_wdownloadSiteURL = A2W(m_downloadSiteURL.c_str());
	}

	if (GetPrivateProfileStringW(L"paths",L"server",L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		m_sServer = lpcStrBuffer;
	}
	CComString sRes = "ParseINI success";
	CComString sCode;
	WriteLog( sRes, sCode);
	return true;
}

void CUpdaterDll::ReleaseLibraries()
{
	CComString sRes;// = "ReleaseLibraryServiceMan is finish";
	CComString sCode;

	//ReleaseLibraryServiceMan();

	//WriteLog( sRes, sCode);

	ReleaseLibraryCommanMan();
	Sleep(10);

	sRes = "ReleaseLibraryCommanMan is finish";
	WriteLog( sRes, sCode);
}

void CUpdaterDll::WriteLog( CComString &sRes, int iCode)
{
	char ff[30];
	_itoa( iCode, ff, 10);
	CComString sCode = ff;
	WriteLog( sRes, sCode);
}

void CUpdaterDll::WriteLog( CComString &sRes, CComString &sCode)
{
	//mutex.lock();	
	CWComString sFileLog = GetApplicationDataDirectory().c_str();
	sFileLog += L"\\updater.log";
	sFileLog.Replace(L"\\\\", L"\\");
	FILE *m_file = _wfopen(sFileLog.GetBuffer(), L"ab");
	if( m_file != NULL)
	{		
		fprintf(m_file, "%s", sRes.GetBuffer());
		if(!sCode.IsEmpty())
			fprintf(m_file, " %s", sCode.GetBuffer());
		fprintf(m_file, "\n");		
		fflush(m_file);
		fclose( m_file);
	}	

	std::string logValue = sRes;

	if(!sCode.IsEmpty())
	{
		logValue +=" ";
		logValue += sCode;
	}

	if (m_isLoginSuccess)
	{
		//oms::omsContext* context = pOMSContext;
		if (context && context->mpResM)
			context->mpResM->WriteUpdaterLog(logValue.c_str());
	}
}

void CUpdaterDll::NextStep()
{
	iStep++;
	//bChangedState = true;
}

void CUpdaterDll::SetStep(int aStep)
{
	iStep = aStep;
	//bChangedState = true;
}

void CUpdaterDll::SetLoadingSetupStatus(int aStatus)
{
	m_loadingSetupStatus = aStatus;
}


std::wstring	GetApplicationRootDirectory()
{
	return glpcRootPath;
}

void CUpdaterDll::SetLang(char *aLang)
{
	m_Lang = aLang;
}

void CUpdaterDll::SetCurrentPath()
{	
	USES_CONVERSION;
	GetCurrentDirectoryW( MAX_PATH, msOldPath.GetBufferSetLength(MAX_PATH2));
	CComString sRes = "SetParams::msOldPath=="; sRes += msOldPath.GetBuffer();
	CComString sCode;
	WriteLog( sRes, sCode);
	if( msPath.IsEmpty())
		msPath = msOldPath;
	else
	{
		::SetCurrentDirectoryW( msPath);
		::SetDllDirectoryW( msPath.GetBuffer());
	}

	//CComString sCode;
	sRes = "SetCurrentPath = "; sRes += W2A(msPath.GetBuffer());
	WriteLog( sRes, sCode);
	
}

HMODULE CUpdaterDll::LoadDLL( LPCSTR alpcRelPath, LPCSTR alpcName)
{
	SECFileSystem fs;

	CComString sCode;
	CComString sRes = "Load library";
	WriteLog( sRes, sCode);
	CComString path = msPath;
	path += alpcName;
	path.Replace("/","\\");
	WriteLog( path, sCode);

	HMODULE handle = ::LoadLibraryEx( path.GetBuffer(), NULL, 0);
	return handle;
}

void* CUpdaterDll::GetProcAddress(unsigned long auHModule, const char* apcProcName){
	if(auHModule == 0)
		return NULL;
	return ::GetProcAddress((HMODULE)auHModule,apcProcName);
}

bool CUpdaterDll::Init()
{
	//MessageBox(0, "Init()", "", MB_OK);

	CComString sRes = "START UPDATE";
	CComString sCode;
	WriteLog( sRes, sCode);

	//sRes = "Loaded Comman dll";
	//WriteLog( sRes, sCode);

	*glpcRootPath=L'\0';
	*glpcRepositoryPath=L'\0';
	wcscpy(glpcUserPath, USER_PATH);
	//glpcUserPath=USER_PATH;
	*glpcUIPath='\0';
	*glpcDownloadManager='\0';
	*glpcComponentsPath='\0';
	*glpcComponentsPath='\0';
	// Путь, устанавливаемый в ComMan.QueryBase
	*glpcQueryBase='\0';
	// Путь, устанавливаемый в ComMan.RemoteBase
	*glpcRemoteBase='\0';
	// Путь в реестре для получения параметров
	*glpcRegPath='\0';
	char	text2[MAX_PATH];
	//char *p = &text2[0];
	int errorCode = 0;

	ParseINI(NULL);

	// тестируем создание папки и файла
	/*if( !pUpdaterFiles->testCreateFile())
	{
		m_IsNeedDownloadSetupOnly = TRUE;
	}*/

	//KillOtherPlayerProcesses();

	NextStep(); // 1

#ifdef DEBUG
#define	COMMAN_MANAGER_DLL	"ComMan.dll"
#else
#define	COMMAN_MANAGER_DLL	"ComMan.dll"
#endif

	if ( !hComModule)
		hComModule = LoadDLL( "", COMMAN_MANAGER_DLL);

	//context = pOMSContext;

	if ( hComModule)
	{
		omsresult (*lpCreateComManager)( oms::omsContext* aContext);

		(FARPROC&)lpCreateComManager = (FARPROC) GetProcAddress( (unsigned long) hComModule, "CreateComManager");

		if ( lpCreateComManager)
		{
			lpCreateComManager( context);
		}
		if ( !context->mpComMan)
		{
			::FreeLibrary( hComModule);
			hComModule = 0;
		}
		
	}

/*#ifdef DEBUG
#define	SERVICEMAN_MANAGER_DLL	"ServiceMan.dll"
#else
#define	SERVICEMAN_MANAGER_DLL	"ServiceMan.dll"
#endif

	if ( !hServiceModule)
		hServiceModule = LoadDLL( "", SERVICEMAN_MANAGER_DLL);



	//cm::CreateComManager( pOMSContext.get());
	if ( hServiceModule)
	{
		omsresult (*lpCreateServiceManager)( oms::omsContext* aContext);
		(FARPROC&)lpCreateServiceManager = (FARPROC) GetProcAddress( (unsigned long) hServiceModule, "CreateServiceWorldManager");
		if ( lpCreateServiceManager)
			lpCreateServiceManager( context);
		if ( !context->mpServiceWorldMan)
		{
			::FreeLibrary( hServiceModule);
			hServiceModule = 0;
		}
	}

	if ( !hServiceModule || !hComModule)
	{
		RequestDownloadSetupFromHTTP();
		//ExitWithError( 4);
		return false;
	}

	setResManager();
	USES_CONVERSION;
	context->mpComMan->PutPathIni( lpcIniFullPathName);

	if(*glpcRootPath != L'\0')
	{
		context->mpComMan->SetLocalBase(glpcRootPath);
	}
	else
	{
		WCHAR lpcModuleFileName[MAX_PATH2];
		//::GetModuleFileName( theApp.m_hInstance, lpcModuleFileName, MAX_PATH+150);
		::GetModuleFileNameW( NULL, lpcModuleFileName, MAX_PATH2);
		WCHAR lpcExeFullPathName[MAX_PATH2];
		LPWSTR lpFileName=NULL;
		::GetFullPathNameW(lpcModuleFileName,MAX_PATH2,lpcExeFullPathName,&lpFileName);
		if(lpFileName!=NULL)
		{
			*lpFileName=L'\0';
			context->mpComMan->SetLocalBase( lpcExeFullPathName);
		}
	}

	if (*glpcRepositoryPath == L'\0')
		wcscpy( glpcRepositoryPath, REPOSITORY_PATH);
	context->mpComMan->SetRepositoryBase( glpcRepositoryPath);
	if (*glpcUserPath == 0)
		wcscpy( glpcUserPath, USER_PATH);
	context->mpResM->SetUserBase( glpcUserPath);
	setStatus( 5);*/
	DWORD dwThreadID;	
	NextStep(); // 2

	

	hDownloadThread = CreateThread( NULL, 64000, (LPTHREAD_START_ROUTINE)StartDownload, this, 0, &dwThreadID);
	
	sRes = "Loaded Comman and ServiceMan dlls";

	////////////////////////////

	WriteLog( sRes, sCode);

	return true;
}

int CUpdaterDll::GetParseINIErrorCode()
{
	return m_parseINIError;
}

std::string CUpdaterDll::GetDownloadURL()
{
	return m_downloadURL;
}

DWORD WINAPI CUpdaterDll::StartDownload(LPVOID param)
{
	CUpdaterDll* apDll = (CUpdaterDll*)param;
	if(m_IsNeedDownloadSetupOnly)
	{
		apDll->RequestDownloadSetupFromHTTP();
	}
	// закомментировано, чтобы только грузит билд
	/*if(!apDll->Login( true))
	{
		apDll->ExitWithError( 5);		
	}
	else if (m_IsNeedDownloadSetupOnly)
	{
		apDll->StartDownloadClient();
	}
	else
	{
		int parseINIErrorCode = apDll->GetParseINIErrorCode();
		if (parseINIErrorCode != 0)
			apDll->ExitWithError(parseINIErrorCode);
	}*/

	return 0;
}

/*long CUpdaterDll::setResManager()
{
	ATLASSERT(context->mpComMan);
	mpIResourceManager = context->mpResM;
	mpIResourceManager->InitResourceManager();
	sp_ResourceManager = context->mpIResM;
	sp_ComMan = context->mpComMan;
	return S_OK;
	//return S_FALSE;
}

bool CUpdaterDll::CheckLoginDownload()
{
	while(true)
	{
		int res = WaitForSingleObject( m_hCheckLogin, 30000);
		ResetEvent(m_hCheckLogin);
		if(res == WAIT_TIMEOUT)
		{
			CComString sRes = "Login...WAIT_TIMEOUT";
			CComString sCode;
			WriteLog( sRes, sCode);
			if(countAuth==-1)
				break;
			//не спрашиваем, сразу качаем сетап по хттп

			return false;
		}
		else if (res == WAIT_OBJECT_0)
		{
			CComString sRes = "Login...WAIT_OBJECT_0";
			CComString sCode;
			WriteLog( sRes, sCode);
			char text[512];//строка состояния
			//HKEY	hkey;
			DWORD sizedata = 512;//
			//unsigned char acDataKey[512] = "1.0";//
			if ( iErrorCodeLogin!=0)
			{
				countAuth++;
				if( countAuth<3)
				{
					pOMSContext->mpServiceWorldMan->Disconnect();
					Sleep(3000);
					Login( false);
					continue;
				}				
				CComString s;
				switch (iErrorCodeLogin)
				{
					case -1:
					{
						s = m_Lang == "rus" ? "Неизвестное имя" : "Unknown login";
						break;
					}
					case -2:
					{
						s = m_Lang == "rus" ? "Неверный пароль" : "Wrong password";
						break;
					}
					case -3:
					{
						s = m_Lang == "rus" ? "Учетная запись повреждена. Зарегистрируйтесь заново" : "Your account is damaged. Please, register a new account.";
						break;
					}
					case -4:
					{
						s = m_Lang == "rus" ? "Пользователь с таким именем уже в сети" : "The user with such name is online right now";
						break;
					}
					case -5:
					{
						s = m_Lang == "rus" ? "Ваша регистрация не подтверждена. \nПройдите по ссылке, указанной в письме подтверждения регистрации." : "Your registration is not confirmed.\nClick on the link specified in the letter\n of confirmation of registration.";
						break;
					}
					case -6:
					{
						s = m_Lang == "rus" ? "Пользователь с таким именем больше не существует" : "This login does not exist more.";
						break;
					}
					case -7:
					{
						s = m_Lang == "rus" ? "Ваш аккаунт поврежден.\nСвяжитесь с разработчиками." : "Your account is broken.\nPlease contact with vAcademia Team.";
						break;
					}
					default:
					{
						char ff[5];
						_itoa( iErrorCodeLogin, ff, 10);
						s = m_Lang == "rus" ? "Ошибка подключения. Код ошибки: " : "Connection error.  Error status code: ";
						s += ff;
						break;
					}
				}				
				s+= m_Lang == "rus" ? "\nПовторить?" : "\nRepeat?";
				//setInfoText( "Ошибка соединения");
				CComString sTitle = m_Lang == "rus" ? "Внимание" : "Attention";
				if ( ::MessageBox( NULL, s.GetBuffer(), sTitle.GetBuffer(), MB_OKCANCEL | MB_ICONWARNING) != IDCANCEL)
				{
					pOMSContext->mpServiceWorldMan->Disconnect();
					setStatus(-1);
					//setInfoText( "Попытка переподключения");
					Sleep(1000);
					Login( false);
					continue;
				}
				else
				{
					ExitWithError( 1);
					return false;
				}
			}
			NextStep(); // 3

			int parseINIErrorCode = GetParseINIErrorCode();
			if (parseINIErrorCode == 0 && !m_IsNeedDownloadSetupOnly)
			{
				pUpdaterFiles->setEvent();
			}
				
			sRes = "Login...is success";
			WriteLog( sRes, sCode);
			break;
		}
	}	
}

bool CUpdaterDll::Login( bool bFirst)
{
	CComString sRes = "To Login...";
	CComString sCode;
	WriteLog( sRes, sCode);
	iErrorCodeLogin = 0;
	countAuth = 0;
	//CComString csLogin = m_Login;
	//CComString csPas = m_Password;
	char text[512];//строка состояния

	//setInfoText( "Установление соединения ...");
	//загрузка XML файла

	pOMSContext->mpServiceWorldMan->SetCallbacks( this);
	//pOMSContext->mpServiceWorldMan->Connect();

	USES_CONVERSION;
	wchar_t *wLogin = A2W(m_Login.GetBuffer());
	wchar_t *wPassword = A2W(m_Password.GetBuffer());

	CComString sLog = "wLogin = " + m_Login;
	WriteLog( sLog, sCode);

	int res = pOMSContext->mpServiceWorldMan->LoginAsync(0, wLogin, wPassword, false, 2, m_EncPass == 1 ? true : false);
	bool bRes = true;
	if(bFirst)
		bRes = CheckLoginDownload();

	m_isLoginSuccess = (bRes && res>-1)?true:false;
	
	return m_isLoginSuccess;
}


void CUpdaterDll::onMethodComplete(int aRequestID, int aRMMLID, const wchar_t* apwcResponse, bool isService, int aiErrorCode)
{
	iErrorCodeLogin = aiErrorCode;
	CComString sRes(apwcResponse);
	CComString sCode;

	WriteLog( sRes, sCode);
	sRes =	"Login result =";

	_itoa( iErrorCodeLogin, sCode.GetBufferSetLength(5), 10);	
	WriteLog( sRes, sCode);

	USES_CONVERSION;
	std::string params = W2A(apwcResponse);

	SetEvent(m_hCheckLogin);
}*/

bool CUpdaterDll::WritePlayerIni()
{
	WCHAR playerIniPath[MAX_PATH * 2];
	wcscpy(playerIniPath, m_ClientPath.GetBuffer());
	wcscat(playerIniPath, L"player.ini");

	if(!FileExists(playerIniPath))
	{
		ExitWithError(11);
		return false;
	}

	CWComString sVersionInNewFile;
	bool bSetVersion = true;
	USES_CONVERSION;
	int pos = m_NewNersion.Find("v_", 0);
	if(pos==0)
		m_NewNersion = m_NewNersion.Right( m_NewNersion.GetLength() - 2);
	if (GetPrivateProfileStringW(L"settings",L"version", L"", sVersionInNewFile.GetBufferSetLength( MAX_PATH), MAX_PATH, playerIniPath)) 
	{
		if( !sVersionInNewFile.IsEmpty() && (m_NewNersion.IsEmpty() || sVersionInNewFile.Find( A2W(m_NewNersion.GetBuffer()))>-1))
		{
			bSetVersion = false;
		}
	}
	if(bSetVersion)
	{
		if (!WritePrivateProfileStringW(L"settings", L"version", A2W(m_NewNersion.GetBuffer()), playerIniPath))
		{
			ExitWithError(12);
			return false;
		}
	}

	if (!WritePrivateProfileStringW(L"paths", L"server", m_sServer.GetBuffer(), playerIniPath))
	{
		ExitWithError(13);
		return false;
	}	
	if (m_Login.CompareNoCase(I_AM_UPDATER) != 0 && !WritePrivateProfileStringW(L"user", L"login", A2W(m_Login.GetBuffer()), playerIniPath))
	{
		ExitWithError(14);
		return false;
	}
	if (m_LanguageIni.GetLength()>0 && !WritePrivateProfileStringW( L"settings", L"language", A2W( m_LanguageIni.GetBuffer()), playerIniPath))
	{
		ExitWithError(15);
		return false;
	}
	
	WCHAR lpcStrBuffer[MAX_PATH*2+2];
	CWComString m_sTitle2;
	if (GetPrivateProfileStringW(L"window", L"title", L"", lpcStrBuffer, MAX_PATH, playerIniPath)) 
	{
		m_sTitle2 = lpcStrBuffer;
	}
	if( m_sTitle2.IsEmpty() || ( iswascii(m_sTitle2.GetAt(0)) && m_LanguageIni != "eng") || ( !iswascii(m_sTitle2.GetAt(0)) && m_LanguageIni == "eng"))
	{
		if (!WritePrivateProfileStringW(L"window", L"title", m_sTitle.GetBuffer(), playerIniPath))
		{
			ExitWithError(16);
			return false;
		}
	}
}

bool CUpdaterDll::Finish( bool bRun)
{
	if(!bRun)
		return true;
	SetStep( 80); // 80

	//DeleteFileW(msVacademiaFilePath.GetBuffer());

	return true;
}

int CUpdaterDll::GetStatus()
{
	return status;
}

int CUpdaterDll::GetOneFileStatus()
{
	return statusOneFile;
}

CComBSTR s;

LPCSTR CUpdaterDll::GetInfoText()
{
	return sInfoText.GetBuffer();
}

DWORD CUpdaterDll::GetCurrentLoaded()
{
	return currentSize;
}

DWORD CUpdaterDll::GetAllSize()
{
	return dwAllSize;
}

int CUpdaterDll::GetStep()
{
	return iStep;
}

LPCWSTR CUpdaterDll::GetCurrentPath()
{
	return msPath.GetBuffer();
}


LPCWSTR CUpdaterDll::GetClientPath()
{
	return m_ClientPath.GetBuffer();
}

LPCWSTR CUpdaterDll::GetServer()
{
	return m_sServer.GetBuffer();
}

LPCWSTR CUpdaterDll::GetDownloadSiteURL()
{
	return m_wdownloadSiteURL.GetBuffer();
}

int CUpdaterDll::GetErrorCode()
{
	return iErrorCode;
}

void CUpdaterDll::setStatus( int astatus)
{
	status = astatus;
	//bChangedState = true;
}

void CUpdaterDll::setOneFileStatus( int astatus)
{
	statusOneFile = astatus;
	//bChangedState = true;
}

void CUpdaterDll::setInfoText( char *apText)
{
	sInfoText = apText;
}

void CUpdaterDll::setCurrentLoaded( DWORD aCurrentSize)
{
	currentSize = aCurrentSize;
}

void CUpdaterDll::setAllSize( DWORD aAllSize)
{
	dwAllSize = aAllSize;
}

void CUpdaterDll::ExitWithError( int aErrorCode, char *p)
{
	CComString sRes;
	CComString sCode;

	switch(aErrorCode)
	{
	case 1:
		sRes = "Authorisation is not passed";
		break;
	case 2:
		sRes = "Parametres are absent. Login is empty";
		break;
	case 3:
		sRes = "Version information getting error: file player.ini is not found";
		break;
	case 4:
		sRes = "Update manager initialization error";
		break;
	case 5:
		sRes = "The server is not accessible";
		break;
	case 8:
		{
			sRes = "Download error of file: ";
			sRes += p;
		}
		break;
	case 9:
		sRes = "File creation error (1005)";
		break;
	case 10:
		sRes = "File write error (1006)";
		break;
	case 11:
		sRes = "Version updating error: player.ini is not found";
		break;
	case 12:
		sRes = "Version updating error";
		break;
	case 13:
		sRes = "Server information updating error";
		break;
	case 14:
		sRes = "Login information updating error";
		break;
	case 15:
		sRes = "Language information updating error";
		break;
	case 16:
		sRes = "Title information updating error";
		break;
	case 19:
		sRes = "File copy error";
		break;
	case 20:
		sRes = "Unknown error";
		break;
	case 21:
		sRes = "Not Enough Disk Space. Setup is interrupted by the user";
		break;
	case 22:
		sRes = "Not enough rights";
		break;
	case 23:
		sRes = "Check summ error";
		break;
	case 24:
		sRes = "File write error";
		break;
	case 25:
		sRes = "Curl init error";
		break;
	case 26:
		sRes = "Curl result error";
		break;
	case 27:
		sRes = "Curl unauthorized ...";
		break;
	case 28:
		sRes = "Curl file download is not success...";
		break;
	}

	WriteLog( sRes, sCode);

	CheckStartFailFileMarker(false);


	if (aErrorCode == 3)
	{
		//if (!RequestDownloadSetup())
		if (!RequestDownloadSetupFromHTTP())
			setStatus(102);

		return;
	}

	iErrorCode = aErrorCode;
	setStatus(101);
	if(p != NULL)
		setInfoText(p);

	Finish(false);
}

unsigned int CUpdaterDll::SetParams( char *aLang, char *apLine)
{
	CComString sPath = apLine;	
	CComString sCode;
	CComString sRes = "SetParams::apLine=="; sRes += sPath.GetBuffer();
	WriteLog( sRes, sCode);
	int pos = sPath.Find(".exe", 0) + 4;
	CComString sLine = sPath.Right( sPath.GetLength() - pos - 1);
	// здесь делаем чтение ??? параметры надо читать из файла
	sPath = sPath.Left( pos);
	sPath.Replace("/", "\\");
	sPath.Replace("\\\\", "\\");
	pos = sPath.ReverseFind('\\');
	sPath = sPath.Left( pos);
	pos = sPath.Find(':', 0);
	while( pos>-1 && sPath[pos] != ' ' && sPath[pos] != ',')
		pos--;
	if(pos > -1)
		sPath = sPath.Right( sPath.GetLength() - pos - 1);
	SetLang( aLang);
	sPath += "/";
	sPath.Replace("\\", "/");
	sPath.Replace("//", "/");

	ReadUpdaterPathFromFile();
	
	SetCurrentPath();
	ParseCmdLine( sLine.GetBuffer());

	msVacademiaFilePath = msPath.GetBuffer();
	msVacademiaFilePath += L"\\vacademia_path.txt";

	sRes = "SetParams::msVacademiaFilePath=="; sRes +=  msVacademiaFilePath.GetBuffer();
	WriteLog( sRes, sCode);

	FILE* fl = _wfopen( msVacademiaFilePath.GetBuffer() ,L"r");

	if (fl)
	{
		fseek(fl, 0, SEEK_END);
		int size = ftell(fl); 
		fseek(fl, 0, SEEK_SET);

		if( size != 0)
		{
			//int wsize = size % 2 == 0 ? size/2 : (size/2 + 1);		
			WCHAR sPath[MAX_PATH2] = {L'\0'};
			fread( sPath, size, 1, fl);
			m_ClientPath = sPath;
			m_ClientPath += L"\\";
			m_ClientPath.Replace( L"\\\\", L"\\");
			m_ClientPath.MakeLower();			
			//wcscpy( sPathStatic, m_ClientPath.GetBuffer());			
		}
		fclose(fl);
		USES_CONVERSION;
		sRes = "SetClientPath = "; sRes += W2A(m_ClientPath.GetBuffer());
		WriteLog( sRes, sCode);
	}

	return 1;
}

/*void CUpdaterDll::DelCache()
{
	if( pUpdaterFiles != NULL)
		pUpdaterFiles->delCache();
}*/

// пишем файл, который удалим после успешного обновления
// Если предыдущий апдейт не завершится, то вместо запуска плеера запустим апдейтер в ланчере.

void CUpdaterDll::InitStartFailFileMarker(char* apLine)
{
	CComString sPath = apLine;
	int pos = sPath.Find(".exe", 0) + 4;
	CComString sLine = sPath.Right( sPath.GetLength() - pos - 1);

	CWComString sFileStart = m_ClientPath;
	sFileStart += L"\\updater_fail_start.txt";
	sFileStart.Replace(L"\\\\", L"\\");

	FILE* fl = _wfopen(sFileStart.GetBuffer(), L"r");
	if (fl){
		sLine += " ";
		sLine += "-try=\"1\"";
	}

	fl = _wfopen(sFileStart.GetBuffer(), L"w");
	if (fl)
	{
		// -login="nap" -password="698d51a19d8a121ce581499d7b701668" -bencpass="1" -version="0.1.3.8140" -language="eng" -updaterkey="a9f4e4d2-359c-4312-b7c0-7bb40d6a00eb" -clientPath="C:\VAcademia_p\release_P\vu2008\" /Lang=eng 
		fwrite(sLine.GetBuffer(),sLine.GetLength(),1, fl);
		fclose(fl);
	}
}

void CUpdaterDll::ReadUpdaterPathFromFile()
{
	// файл нужен, чтобы потом при следующем старте академии удалить updater.exe
	std::wstring path = GetApplicationDataDirectory();
	path += L"\\updater_path.txt";
	FILE*  fl = _wfopen(path.c_str(), L"r");
	if (fl)
	{	
		WCHAR sPath[MAX_PATH2] = {L'\0'};
		fread( sPath, MAX_PATH2, 1, fl);
		msPath = sPath;
		CComString sCode;
		CComString sRes = "ReadUpdaterPathFromFile=="; sRes += msPath.GetBuffer();
		WriteLog( sRes, sCode);
		fclose(fl);
	}
}

void CUpdaterDll::RemoveStartFailFileMarker()
{
	CWComString sFileStart = m_ClientPath;
	sFileStart += L"\\updater_fail_start.txt";
	sFileStart.Replace(L"\\\\", L"\\");
	int res = _wremove(sFileStart.GetBuffer());

	if (res!=0)
	{
		FILE* fl = _wfopen(sFileStart.GetBuffer(), L"w");
		if (fl)
		{
			fclose(fl);
		}
	}
}

void CUpdaterDll::AddStatusToStartFailFile(bool isBroken)
{
	CWComString sFileStart = m_ClientPath;
	sFileStart += L"\\updater_fail_start.txt";
	sFileStart.Replace(L"\\\\", L"\\");

	FILE* fl = _wfopen(sFileStart.GetBuffer(), L"a");
	if (fl)
	{
		if (isBroken)
			fprintf(fl," -broken \"1\"");
		else
			fprintf(fl," -broken \"0\"");
		fclose(fl);
	}
}

// скачиваем и запускаем установку
void CUpdaterDll::StartDownloadClient()
{
	MP_NEW_V2(pClientDownloader, CClientDowloader, context, this);
}

bool  CUpdaterDll::DelDir( CWComString &sDir, int &aProgress)
{	
	WIN32_FIND_DATAW fileData;
	HANDLE	hSearch =  FindFirstFileW(sDir + L"*.*", &fileData); 

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
				CWComString s = sDir + fileData.cFileName;
				s+=L"\\";				
				bContinueSearch = FindNextFileW(hSearch, &fileData);
				DelDir( s, aProgress);
				if( aProgress > -1 && aProgress < 100)
				{
					aProgress++;
					setOneFileStatus( aProgress);
				}
				continue;
			}

			CWComString s = sDir+fileData.cFileName;
			bContinueSearch = FindNextFileW(hSearch, &fileData);
			try
			{
				::SetFileAttributesW( s.GetBuffer(), FILE_ATTRIBUTE_NORMAL);
				::DeleteFileW( s.GetBuffer());				
				int code = GetLastError();
				if( code != 0)
				{
					CComString sDel = "DelDir::error del file: ";
					sDel += s.GetBuffer();
					WriteLog( sDel, code);
				}
			}
			catch (...)
			{
			}
		}
	}
	FindClose(hSearch);
	try
	{
		::RemoveDirectoryW( sDir.GetBuffer());
		int code = GetLastError();
		if( code != 0)
		{
			CComString sDel = "DelDir::error del dir: ";
			USES_CONVERSION;
			sDel += W2A(sDir.GetBuffer());
			WriteLog( sDel, code);
		}
	}
	catch (...)
	{
	}
	return true;
}

void CUpdaterDll::CheckStartFailFileMarker(bool onCancel)
{
	BOOL isCritical = IsCriticalError(GetErrorCode());

	if (isCritical)
	{
		AddStatusToStartFailFile(true);
		return;
	}

	// если ошибка некритическая и до начала копирования файлов
	if (GetStep() < 6)
	{	
		if (m_IsRequiredUpdate)
		{
			// нажали отмена
			if ( onCancel == true)
				RemoveStartFailFileMarker();
			else
				AddStatusToStartFailFile(false);
		}
		else
		{
			RemoveStartFailFileMarker();
		}
	}
	else
	{
		AddStatusToStartFailFile(true);
	}
}

bool CUpdaterDll::IsCriticalError(int aErrorCode)
{
	if( aErrorCode == 3 || aErrorCode == 19  || aErrorCode == 4 ||
		(aErrorCode > 11 && aErrorCode < 16) || (aErrorCode >= 23 && aErrorCode <= 26))
		return true;

	return false;
}

double download_total_size = 0;
double download_total_size_done = 0;
//CMutex mutexProgress;
CMutex mutexFile;
HANDLE hFile = NULL;
int countThread = 8;
bool bProxy = false;
CComString sHeaderData;

size_t write_header_data(void *ptr, size_t size, size_t nmemb, ThreadInfo *info) {
	sHeaderData += (char*)ptr;
	return size*nmemb;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, ThreadInfoPart *info) {
	DWORD dWritten;
	mutexFile.lock();
	SetFilePointer( hFile, info->curPos, 0, FILE_BEGIN);
	WriteFile( hFile, ptr, size*nmemb, &dWritten, NULL);
	download_total_size_done += dWritten;
	info->sender->ReportProgressHTTP( download_total_size, download_total_size_done);
	mutexFile.unlock();
	info->curPos += dWritten;	
	return dWritten;
}

DWORD WINAPI CUpdaterDll::TaskThreadProcPart(LPVOID lpParam)
{
	ThreadInfoPart* info = (ThreadInfoPart*)lpParam;
	return info->sender->TaskDownloadProcPart( info);
}

DWORD CUpdaterDll::TaskDownloadProcPart( ThreadInfoPart* apParam)
{
	ThreadInfoPart* info = apParam;

	CURL *curl; 
	curl = curl_easy_init(); 
	//char errorBuffer[CURL_ERROR_SIZE];
	if(curl) 
	{  
		std::string url = info->url;

		//curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);		
		curl_easy_setopt(curl, CURLOPT_RANGE, info->range.c_str());
		curl_easy_setopt(curl, CURLOPT_HEADER,0);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,1);
		//задаем опцию - получить страницу по адресу http://...
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		//указываем функцию обратного вызова для записи получаемых данных
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		//указываем куда записывать принимаемые данные
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, info);
		curl_easy_setopt(curl, CURLOPT_MAXCONNECTS, 4);				
		//запускаем выполнение задачи
		if( !bProxy)
			info->result = curl_easy_perform(curl);
		else
		{
			if ( !info->proxy.empty()) //указываем прокси сервер, если надо
			{
				//curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
				curl_easy_setopt(curl, CURLOPT_PROXY, info->proxy.c_str());
				if ( !info->proxyuserpwd.empty()) 
				{
					curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, info->proxyuserpwd.c_str());				
				}
			}
			else
			{
				curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 0);
			}
			info->result = curl_easy_perform(curl);
		}
		int result = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&info->statusCode);
	}
	else
	{
		info->result = -25;
	}
	//завершаем сессию
	curl_easy_cleanup(curl);
	return 0;
}

HANDLE CUpdaterDll::RunThreadProcPart( ThreadInfoPart *aParam)
{	
	aParam->downloadThread.SetStackSize(256 * 1024 / countThread);
	aParam->downloadThread.SetParameter((void*)aParam);
	aParam->downloadThread.SetThreadProcedure(TaskThreadProcPart);
	aParam->downloadThread.Start();
	return aParam->downloadThread.GetHandle();
}

DWORD WINAPI CUpdaterDll::TaskThreadProc(LPVOID lpParam)
{
	ThreadInfo* info = (ThreadInfo*)lpParam;
	return info->sender->TaskDownloadProc( info);
}

DWORD CUpdaterDll::TaskDownloadProc( ThreadInfo *aParam)
{
	ThreadInfo* info = aParam;

	CURL *curl; 
	CURLcode result;  
	curl = curl_easy_init(); 
	//char errorBuffer[CURL_ERROR_SIZE];
	DWORD dwFileSize = 0;

	if(curl) 
	{
		// скачать первый килобайт, узнать общий размер и запустить потоки.
		std::string url = info->url;

		hFile = CreateFileW(info->outputfile.c_str(), GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if( hFile == NULL || hFile == INVALID_HANDLE_VALUE)
		{
			info->sender->ExitWithError(24);
			//завершаем сессию
			curl_easy_cleanup(curl);
			return 0;
		}

		//задаем все необходимые опции
		//определяем, куда выводить ошибки
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		//curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_header_data);
		//указываем куда записывать принимаемые данные
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, info);
		//задаем опцию - получить страницу по адресу http://...
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		CComString sRes = "curl url "; sRes += url.c_str();
		CComString sCode;
		WriteLog( sRes, sCode);

		result = curl_easy_perform(curl);

		DWORD statusCode = 0;
		char ff[10];
		int result1 = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&statusCode);
		_itoa( statusCode, ff, 10);
		sRes = "curl and http status code "; sRes += ff;
		WriteLog( sRes, sCode);
		_itoa( result1, ff, 10);
		sRes = "curl result1 == "; sRes += ff;
		WriteLog( sRes, sCode);

		if (result != CURLE_OK || result1 == CURLE_OK && !info->proxy.empty() && (statusCode < 200 || statusCode > 299))
		{			
			sRes = "curl set proxy: "; sRes += info->proxy.c_str(); sRes += ", ";sRes += info->proxyuserpwd.c_str();
			if ( !info->proxy.empty()) //указываем прокси сервер, если надо
			{
				bProxy = true;
				//curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
				curl_easy_setopt(curl, CURLOPT_PROXY, info->proxy.c_str());
				if (!info->proxyuserpwd.empty()) //указываем прокси сервер, если надо
				{
					curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, info->proxyuserpwd.c_str());	
				}
			}
			else
			{
				curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 0);
			}
			result = curl_easy_perform(curl);
		}
		if (result == CURLE_OK)     
		{
			statusCode = 0;
			int result1 = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&statusCode);			
			curl_easy_cleanup(curl);			
			_itoa( statusCode, ff, 10);
			sRes = "curl success and http status code "; sRes += ff;
			WriteLog( sRes, sCode);
		}
		else
		{
			ExitWithError(26);
			return 0;
		}
	}
	else
	{
		ExitWithError(25);
		return 0;
	}
	int pos = sHeaderData.Find("Content-Length:", 0) + 15;
	int posEnd = sHeaderData.Find( "\n", pos + 1);
	CComString sFileSize = sHeaderData.Mid( pos, posEnd - pos);
	sFileSize.TrimLeft();
	sFileSize.TrimRight();
	dwFileSize = rtl_atoi( sFileSize.GetBuffer());
	if (dwFileSize < 10*1024)
	{
		ExitWithError(27); 
		return 0;
	}
	CComString sRes = " size of file "; sRes += sFileSize.GetBuffer();
	CComString sCode;
	WriteLog( sRes, sCode);
	download_total_size = dwFileSize;
	int partSize = (int)floor( (double)dwFileSize/countThread);
	HANDLE *phThreads = new HANDLE[ countThread];
	ThreadInfoPart* infoParts = new ThreadInfoPart[ countThread];
	for( int i = 0; i < countThread; i++)
	{
		int begin = i*partSize;
		int end = begin + partSize - 1;
		if( i == countThread - 1)
			end = 0;

		char sBegin[20];
		char sEnd[20];
		_itoa( begin, sBegin, 10);
		_itoa( end, sEnd, 10);
		std::string sRange = sBegin;		
		sRange+="-"; 
		if( end > 0)
			sRange += sEnd;
		infoParts[i].SetParams( info->sender, info->url, info->proxy, info->proxyuserpwd, begin, sRange);
		phThreads[ i] = RunThreadProcPart( &infoParts[i]);		
	}
	WaitForMultipleObjects( countThread, phThreads, TRUE, INFINITE);
	delete[] phThreads;
	int countSuccess = 0;	
	
	int try1 = 0;
	while( try1 < 3 && countSuccess < countThread)
	{
		countSuccess = 0;
		for( int i = 0; i < countThread; i++)
		{
			if(infoParts[i].result == CURLE_OK)
			{
				countSuccess++;
			}
		}
		if(countSuccess >= countThread)
			break;
		HANDLE *phThreads = new HANDLE[ countThread];
		int j = 0;
		DWORD dwStatusCode = 0;
		for( int i = 0; i < countThread; i++)
		{
			if(infoParts[i].result == CURLE_OK)
				continue;			
			if ( j == 0 && infoParts->statusCode >= 400 && infoParts->statusCode < 500)
				break;
			phThreads[ j] = RunThreadProcPart( &infoParts[i]);
			j++;			
		}
		if (j == 0 && dwStatusCode >= 400 && dwStatusCode < 500)
			break;
		WaitForMultipleObjects( j, phThreads, TRUE, INFINITE);
		delete[] phThreads;
		try1++;
	}
	
	DWORD sizeH = 0;
	DWORD size = GetFileSize(hFile, &sizeH);
	CloseHandle(hFile);
	if ( try1 > 2 && size < dwFileSize)
	{
		ExitWithError(28); 
		return 0;
	}

			
	//info->sender->setStatus(100);
	info->sender->SetLoadingSetupStatus(LOADING_SETUP_FINISH);
	sRes = "loading setup file is finish";
	info->sender->WriteLog( sRes, sCode);
	info->sender->OpenSetup(info->outputfile);
	return 0;
}


bool CUpdaterDll::RequestDownloadSetupFromHTTP()
{
	SetStep(10);

	CComString sRes = "Setup loading file from http is begin";
	CComString sCode;
	WriteLog( sRes, sCode);

	setStatus( 0);
	setOneFileStatus(0);
	setAllSize(0);
	setCurrentLoaded(0);
	SetLoadingSetupStatus(LOADING_SETUP_PROCESS);


	std::string url = GetDownloadURL();
	std::wstring outfilename = GetCurrentPath();
	outfilename += L"setup.exe";

	BYTE aProxyUsing;
	BSTR aProxy;
	BSTR aProxyUser;
	BSTR aProxyPassword;


	if (context!=NULL && context->mpComMan!=NULL)
		context->mpComMan->getProxy(&aProxyUsing, &aProxy, &aProxyUser, &aProxyPassword);


	ThreadInfo* info = NULL;

	if (aProxyUsing == 1)
	{
		CComString sProxy(aProxy);
		CComString sProxyuser(aProxyUser);
		CComString sProxypass(aProxyPassword);

		CComString sProxyuserpwd =  sProxyuser;
		sProxyuserpwd +=":";
		sProxyuserpwd +=sProxypass;

		info = new ThreadInfo(this, url, outfilename, sProxy.GetBuffer(), sProxyuserpwd.GetBuffer());

		sRes = "Setup loading file from http with proxy setting";
		WriteLog( sRes, sCode);
	}
	else	
		info = new ThreadInfo(this, url, outfilename, "", "");


	m_downloadThread.SetStackSize(256 * 1024);
	m_downloadThread.SetParameter((void*)info);
	m_downloadThread.SetThreadProcedure(TaskThreadProc);
	m_downloadThread.Start();

	return true;	
}

void CUpdaterDll::ReportProgressHTTP( double download_total_size, double download_total_size_done)
{
	setAllSize( download_total_size/1024);
	setCurrentLoaded(download_total_size_done/1024);
	setOneFileStatus((100*download_total_size_done)/download_total_size);
	//setStatus(20 + (60*download_total_size_done/download_total_size));
	setStatus(95*download_total_size_done/download_total_size);
}

void CUpdaterDll::OpenSetup(std::wstring setupPath)
{
	SHELLEXECUTEINFOW sei;
	ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = L"open";
	sei.lpFile = setupPath.c_str();
	USES_CONVERSION;
	CWComString sParams = L"/lang="; sParams += A2W(m_Lang);
	CWComString sPath = m_ClientPath;
	while( sPath[ sPath.GetLength() - 1] == L'\\')
		sPath = sPath.Left( sPath.GetLength() - 1);
	sParams += L" /dir=\""; sParams += sPath; sParams += L"\"";
	sParams += L" /silent";
	sei.lpParameters = sParams.GetBuffer();
	sei.nShow = SW_SHOWNORMAL;

	// run setup.exe
	if (ShellExecuteExW(&sei))
	{
		CComString sRes = "start setup.exe";
		CComString sCode;
		WriteLog( sRes, sCode);
		CloseHandle(sei.hProcess);
		RemoveStartFailFileMarker();
	}

	ReleaseLibraries();

	// можно удалять сетап и папки
	CWComString path = GetCurrentPath();
	/*if (path != m_ClientPath)
	{
		SetStep( 79);
		int  progress = 0;
		DelDir(path, progress);
		setOneFileStatus( 100);
	}*/
	SetStep( 99);
	setOneFileStatus( 100);

	char tmp[MAX_PATH];
	SHGetSpecialFolderPath(NULL, tmp, CSIDL_APPDATA, true);

	CComString sFileLog = tmp;
	sFileLog += "\\updater.log";
	sFileLog.Replace("\\\\", "\\");
	::DeleteFile( sFileLog);

	Finish( true);
	Close();
}

bool CUpdaterDll::RequestDownloadSetup()
{
	StartDownloadClient();
	return true;
}

void CUpdaterDll::Close()
{
		HANDLE         hProcessSnap = NULL; 
		BOOL           bRet      = FALSE; 
		PROCESSENTRY32 pe32; 
		ZeroMemory(&pe32, sizeof(pe32));

		//  Take a snapshot of all processes in the system. 
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

		if (hProcessSnap == INVALID_HANDLE_VALUE) 
			return; 

		//  Fill in the size of the structure before using it. 
		pe32.dwSize = sizeof(PROCESSENTRY32); 
		// получаем описатель процесса

		if (Process32First(hProcessSnap, &pe32)) { 
			DWORD         dwPriorityClass; 
			BOOL          bGotModule = FALSE; 
			MODULEENTRY32 me32; 
			//me32.dwSize = sizeof(me32);
			ZeroMemory(&me32, sizeof(me32));

			do{
				if (strcmp(pe32.szExeFile, "updater.tmp") == 0)  
				{
					HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
					if (hProcess == NULL)
						return;

					DWORD dwError = ERROR_SUCCESS;

					// пытаемся завершить процесс
					if (!TerminateProcess(hProcess, (DWORD)-1))
						dwError = GetLastError();

					// закрываем описатель процесса
					CloseHandle(hProcess);

					SetLastError(dwError);
				}		
			}
			while (Process32Next(hProcessSnap, &pe32)); 
		}
		CloseHandle (hProcessSnap); 
}


CUpdaterDll *pDll = NULL;

HANDLE  hUpdaterProcessHandle = 0;

/*LONG WINAPI UpdaterFilterFunction( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	/*if ( hUpdaterProcessHandle)
		::MessageBox( NULL, "UpdaterFilterFunction hUpdaterProcessHandle != NULL", "Debug Message", MB_OK);
	else
		::MessageBox( NULL, "UpdaterFilterFunction hUpdaterProcessHandle == NULL", "Debug Message", MB_OK);*/

	//LONG result = MDumpFilterFunction( pExceptionInfo);

	//if ( hUpdaterProcessHandle)
	//{
	//	TerminateProcess( hUpdaterProcessHandle, 1);
	//}
	
	/*return result;
}*/


class CTestFailed
{
public:
	CTestFailed(){ x = 0;};
	~CTestFailed(){};
	void run(){ x++;};
	int x;
};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{	
			//LPTOP_LEVEL_EXCEPTION_FILTER exp = StartExceptionHandler( &UpdaterFilterFunction);

			//if ( exp)
			//	::MessageBox( NULL, "DllMain exp != NULL", "Debug Message", MB_OK);
			//else
			//	::MessageBox( NULL, "DllMain exp == NULL", "Debug Message", MB_OK);

			//hUpdaterProcessHandle = GetCurrentProcess();

			CoInitialize(0);
			
			if(pDll == NULL)
				pDll = MP_NEW( CUpdaterDll);
			break;
		}
	case DLL_PROCESS_DETACH:
		CoUninitialize();
		break;
	}
    return TRUE;
}

#include <exception>

/*unsigned int StartExceptionHandler_export()
{
	StartExceptionHandlers();*/
	/*LPTOP_LEVEL_EXCEPTION_FILTER exp = StartExceptionHandler( &UpdaterFilterFunction);

	if ( exp)
		::MessageBox( NULL, "StartExceptionHandler_export exp != NULL", "Debug Message", MB_OK);
	else
		::MessageBox( NULL, "StartExceptionHandler_export exp == NULL", "Debug Message", MB_OK);

	hUpdaterProcessHandle = GetCurrentProcess();
	*/
/*	return 0;
}*/

UPDATERDLL_API  unsigned int __stdcall SetParamsUpdater_export( char *aLang, char *apLine)
{
	if(pDll == NULL)
	{
		return 1;
	}

	pDll->SetParams( aLang, apLine);
	pDll->InitStartFailFileMarker(apLine);

	return 0;
}

UPDATERDLL_API  unsigned int __stdcall StartUpdater_export( )
{
	//StartExceptionHandler( &UpdaterFilterFunction);
	//hUpdaterProcessHandle = GetCurrentProcess();
	if(pDll == NULL)
	{
		return 1;
	}
	pDll->Init();
	return 1;
}

UPDATERDLL_API  int __stdcall GetStatus_export()
{
	if( !pDll)
		return 0;

	return pDll->GetStatus();
}

UPDATERDLL_API  int __stdcall GetStep_export()
{
	if( !pDll)
		return 0;

	return pDll->GetStep();
}

UPDATERDLL_API  int __stdcall GetOneFileStatus_export()
{
	if( !pDll)
		return 0;

	return pDll->GetOneFileStatus();
}

//UPDATERDLL_API wchar_t *GetInfoText_export()
//UPDATERDLL_API bool GetInfoText_export( char *p)
UPDATERDLL_API  LPCSTR __stdcall GetInfoText_export()
{
	if( !pDll)
		return "";
	
	CComString s = pDll->GetInfoText();
	//static char buff2[512];
	strcpy( buff, s.GetBuffer());
	return buff;
}

UPDATERDLL_API int __stdcall GetClientDir_export( LPWSTR path, int aSize)
{
	if( !pDll)
		return 0;

	CWComString s = pDll->GetClientPath();
	int size = s.GetLength();
	if( aSize < size)
		return aSize - size;
	wcscpy( path, s.GetBuffer());	
	return size;
}

UPDATERDLL_API int __stdcall GetVacademiaPath_export( LPWSTR path, int aSize)
{
	if( !pDll)
		return 0;

	CWComString s = pDll->GetClientPath(); s += L"vacademia.exe";
	int size = s.GetLength();
	if( aSize < size)
		return aSize - size;
	wcscpy( path, s.GetBuffer());	
	return size;
}

UPDATERDLL_API int __stdcall GetServer_export( LPWSTR server, int aSize)
{
	if( !pDll)
		return 0;

	CWComString s = pDll->GetServer();
	int size = s.GetLength();
	if( aSize < size)
		return aSize - size;
	wcscpy( server, s.GetBuffer());
	return size;
}

UPDATERDLL_API int __stdcall GetDownloadSiteURL_export( LPWSTR server, int aSize)
{
	if( !pDll)
		return 0;

	CWComString s = pDll->GetDownloadSiteURL();
	int size = s.GetLength();
	if( aSize < size)
		return aSize - size;
	wcscpy( server, s.GetBuffer());
	return size;
}

UPDATERDLL_API  unsigned int __stdcall GetCurrentLoaded_export()
{
	if( !pDll)
		return 0;

	return pDll->GetCurrentLoaded();
}

UPDATERDLL_API  unsigned int __stdcall GetAllSize_export()
{
	if( !pDll)
		return 0;

	return pDll->GetAllSize();
}

UPDATERDLL_API  int __stdcall GetErrorCode_export()
{
	if( !pDll)
		return 0;

	return pDll->GetErrorCode();
}

UPDATERDLL_API  bool __stdcall KIllVA_export()
{
	if( !pDll)
		return false;

	return pDll->KillOtherPlayerProcesses( false);
}

UPDATERDLL_API  bool __stdcall KIllVAAll_export()
{
	if( !pDll)
		return false;

	return pDll->KillOtherPlayerProcesses( true);
}

UPDATERDLL_API  bool __stdcall Destroy_export()
{
	if(pDll != NULL)
	{
		MP_DELETE( pDll);
	}
	pDll = NULL;
	return true;
}

UPDATERDLL_API  LPSTR __stdcall MakeIntResource_export( DWORD dw)
{
	char *p = MAKEINTRESOURCE(IDI_WARNING);
	return MAKEINTRESOURCE(IDI_WARNING);
}

UPDATERDLL_API  DWORD __stdcall LoadIconWarning_export( DWORD dw)
{
	static HICON hIcon = LoadIcon( NULL, MAKEINTRESOURCE(IDI_WARNING));
	return (DWORD)hIcon;
}

UPDATERDLL_API  bool __stdcall OnCancel_export()
{
	if(pDll != NULL)
	{
		pDll->CheckStartFailFileMarker(true);
	}

	return true;
}

UPDATERDLL_API  bool __stdcall OnSiteOpened_export()
{
	if(pDll != NULL)
	{
		pDll->OnSiteOpened();
	}

	return true;
}

UPDATERDLL_API  bool __stdcall Quit_export()
{
	if(pDll != NULL)
	{
		pDll->OpenSiteIfNeeded();
	}

	return true;
}

