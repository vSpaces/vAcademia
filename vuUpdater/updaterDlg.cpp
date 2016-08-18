// updaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "updater.h"
#include "updaterDlg.h"
#include "FileSys.h"
#include "crmMemResourceFile.h"
#include ".\updaterdlg.h"
#include <shobjidl.h>

#include <shellapi.h>
#pragma comment(lib,"shell32")

#include "Tlhelp32.h"
#include "Psapi.h"

//#include "ProgressWnd.h"
/*#ifdef _DEBUG
#define new DEBUG_NEW
#endif*/

#define tmpBuffSize 65535

/*#define	LOG_SERVER_STATUS_BIT		0x4
#define	MAP_SERVER_STATUS_BIT		0x8
#define	SYNC_SERVER_STATUS_BIT		0x10
#define	SERVICE_SERVER_STATUS_BIT	0x20
#define	VOIP_SERVER_STATUS_BIT		0x40*/


CComModule _Module;

typedef HRESULT (__stdcall *TGetClassObjectProc)(REFCLSID rclsid, REFIID riid, LPVOID * ppv);


/*void CUpdaterCallback::onQueryComplete(int aiID, const wchar_t* apwcResponse, int* aiErrorCode)
{
	pUpdater->LoadingFiles(*aiErrorCode);
}*/


//CUpdaterDlg::CUpdaterDlg(CWnd* pParent /*=NULL*/)
CUpdaterDlg::CUpdaterDlg()
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//CComObject<CUpdaterNotifier>::CreateInstance( &notifier);
	notifier = new CUpdaterNotifier();
	//notifier->AddRef();
	//notifier->myDlg = this;
	hDownloadThread = NULL;
	iStep = 0;
	sp_Resource = NULL;
	sp_ComMan = NULL;
	sp_ResourceManager = NULL;
	pOMSContext = std::auto_ptr<oms::omsContext>(new oms::omsContext);
	hComModule = NULL;
	hServiceModule = NULL;
	/*pHandler = new CUpdaterCallback();
	pHandler->SetUpdater(this);*/
	countAuth = 0;
	hCheckLoginThread = NULL;
	m_hCheckLogin = CreateEvent( NULL, TRUE, FALSE, NULL);
	pUpdaterFiles = new CupdaterFiles( pOMSContext.get(), this);
}

CUpdaterDlg::~CUpdaterDlg()
{
	if( pUpdaterFiles != NULL)
		delete pUpdaterFiles;
	ATLASSERT( context);
	if( !context)
		return;
	ReleaseLibrary();
	CloseHandle(m_hCheckLogin);
}

void CUpdaterDlg::ReleaseLibrary()
{
	if( !hServiceModule)
		return;

	void (*lpDestroyServiceManager)( oms::omsContext* aContext);
	(FARPROC&)lpDestroyServiceManager = (FARPROC)GetProcAddress( (unsigned long)hServiceModule, "DestroyServiceWorldManager");
	if( lpDestroyServiceManager)
		lpDestroyServiceManager( context);
	::FreeLibrary( hServiceModule);

	//service::DestroyServiceWorldManager( pOMSContext.get());
	//cm::DestroyComManager( pOMSContext.get());
	if( !hComModule)
		return;

	void (*lpDestroyComManager)( oms::omsContext* aContext);
	(FARPROC&)lpDestroyComManager = (FARPROC)GetProcAddress( (unsigned long)hComModule, "DestroyComManager");
	if( lpDestroyComManager)
		lpDestroyComManager( context);
	::FreeLibrary( hComModule);
}

HWND ghwndProcessWindow = NULL;


void KillOtherPlayerProcesses()
{
	// получаем путь к exe-файлу
	TCHAR lpcStrBuffer[MAX_PATH*2+2];
	::GetModuleFileName( NULL, lpcStrBuffer, MAX_PATH);
	TCHAR lpcExeFullPathName[MAX_PATH*2+2];
	LPTSTR pTemp;
	::GetFullPathName((const TCHAR *)lpcStrBuffer, MAX_PATH*2, lpcExeFullPathName, &pTemp);
	//TCHAR lpcExeName[MAX_PATH];
	//_tcscpy(lpcExeName,pTemp);

	CComString sFullPath1 = lpcExeFullPathName;
	sFullPath1 = sFullPath1.Left(sFullPath1.ReverseFind('\\') + 1);
	CComString sFullPath2 = sFullPath1;
	sFullPath1+="vacademia.exe";
	sFullPath2+="player.exe";
	//DWORD dwMyProcessID = GetCurrentProcessId();

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

	//  Walk the snapshot of the processes, and for each process, 
	//  display information. 
	if (Process32First(hProcessSnap, &pe32)) { 
		DWORD         dwPriorityClass; 
		BOOL          bGotModule = FALSE; 
		MODULEENTRY32 me32; 
		//me32.dwSize = sizeof(me32);
		ZeroMemory(&me32, sizeof(me32));

		do{ 
			// если это мой процесс, то пропусаем
/*			if(pe32.th32ProcessID == dwMyProcessID)
				continue;*/
			// если название EXE-фала другое, то тоже пропускаем
			if(sFullPath1.Find(pe32.szExeFile) == -1 && sFullPath2.Find(pe32.szExeFile) == -1)
				continue;
			// открываем процесс, чтобы узнать побольше о его состоянии
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,
				FALSE, pe32.th32ProcessID);
			// если процесс открыть не смогли, то пропускаем его
			if (hProcess == NULL)
				continue;
			HMODULE hExeModule = NULL;
			DWORD cbNeeded;
			TCHAR szModulePath[MAX_PATH];
			LPTSTR pszProcessName = NULL;
			// получаем полный путь к EXE-файлу процесса
			if (EnumProcessModules(hProcess, &hExeModule, sizeof(HMODULE), &cbNeeded)){
				if (GetModuleFileNameEx(hProcess, hExeModule, szModulePath, MAX_PATH)){
					// если путь к exe-файлу разный, то пропускаем процесс
					int pos1 = sFullPath1.Find(szModulePath); 
					int pos2 = sFullPath2.Find(szModulePath); 
					if(pos1 == -1 && pos2 == -1){
				//_tcscmp(lpcExeFullPathName, szModulePath) != 0){
						CloseHandle(hProcess);
						continue;
					}
				}
			}
			CloseHandle(hProcess);
			// находим главное окно процесса
			ghwndProcessWindow = NULL;
			/*EnumWindows(EnumWindowsProc, (LPARAM)pe32.th32ProcessID);
			// если главное окно нашли
			if(ghwndProcessWindow != NULL){
				// и с ним все в порядке
				// (пока будем считать, что с окном все в порядке, если можно взять инфу о нем)
				if( IsWindowVisible( ghwndProcessWindow))
					continue;
			}*/

			// если главное окно процесса не найдено, значит с ним что-то не так. Можно убивать

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
		while (Process32Next(hProcessSnap, &pe32)); 
	} 

	// Do not forget to clean up the snapshot object. 
	CloseHandle (hProcessSnap); 
}

// CUpdaterDlg message handlers

LRESULT CUpdaterDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	/*SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon*/


	// TODO: Add extra initialization here
	CoInitialize( NULL);

	HINSTANCE hInst = (HINSTANCE) GetWindowLong(GWL_HINSTANCE);
	m_hIcon = LoadIcon( hInst, MAKEINTRESOURCE( IDR_MAINFRAME));
	int ff = GetLastError();
	SetIcon(m_hIcon, TRUE);	
	ff = GetLastError();
	RedrawWindow();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUpdaterDlg::OnPaint() 
{
	/*if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}*/
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUpdaterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool FileExists(const TCHAR* alpcFileName){
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(alpcFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);
	return true;
}

bool CUpdaterDlg::ParseCmdLine(LPSTR alpCmdLine)
{
	CComString inLine = alpCmdLine;
	if(inLine.IsEmpty())
	{
		return false;
	}

	int pos = inLine.Find(" ");
	m_Login = inLine.Mid(0,pos);
	inLine.Delete(0,pos+1);
	pos = inLine.Find(" ");
	m_Password = inLine.Mid(0,pos);
	inLine.Delete(0,pos+3);
	/*inLine.Delete(0,pos+1);
	pos = inLine.Find(" ");
	if(pos>-1)
	{
		m_NewNersion = inLine.Mid(0,pos);
		inLine.Delete(0,pos+1);
		m_PathINI = inLine;

	}
	else*/
	m_NewNersion = inLine;

}
bool CUpdaterDlg::ParseINI(HINSTANCE ahInstance, char **pptext)
{
	if(m_Login.IsEmpty())
	{
		sprintf((char*)(*pptext), "Отсутсвуют параметры");
		return false;
	}
	// Getting execute full name
	TCHAR lpcStrBuffer[MAX_PATH*2+2];
	::GetModuleFileName( ahInstance, lpcStrBuffer, MAX_PATH);
	TCHAR lpcExeFullPathName[MAX_PATH*2+2];
	_tcscpy(lpcExeFullPathName, lpcStrBuffer);
	// INI-file FullPathName
	_tcscpy(lpcIniFullPathName,lpcExeFullPathName);
	TCHAR * pTemp = _tcsstr(lpcIniFullPathName, "updater");
	// удалим поддиректорию \updater\... из пути
	for (int i = 0; i < _tcslen(lpcIniFullPathName); ++i)
	{
		if (!_tcscmp(&lpcIniFullPathName[i], pTemp))
		{
			lpcIniFullPathName[i] = '\0';
			break;
		}
	}
	TCHAR rootPath[MAX_PATH * 2];
	_tcscpy(rootPath, lpcIniFullPathName);

	_tcscat(lpcIniFullPathName,_T("player.ini"));
	// если player.ini не найден,
	if(!FileExists(lpcIniFullPathName))
	{
		sprintf((char*)(*pptext), "Ошибка взятия информации о версии, файл player.ini не найден");
		return false;
	}

	// Getting paths from INI-file
	if (GetPrivateProfileString( _T("paths"), _T("root"), _T(""), lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		if (!_tcscmp(lpcStrBuffer, _T(".\\")))
		{
			_tcscpy(glpcRootPath, rootPath);
		}
		else
		{
			_tcscpy(glpcRootPath,lpcStrBuffer);
		}
		for(int ii=0; glpcRootPath[ii]; ii++);
		{
			if(glpcRootPath[ii-1]!=_T('\\') && glpcRootPath[ii-1]!=_T('/'))
			{
				glpcRootPath[ii]=_T('\\'); 
				glpcRootPath[ii+1]=0;
			}
		}
		// если это не полный путь, т.е. путь относительно exe-шника
		if(*glpcRootPath != _T('\0') && 
			((glpcRootPath[0] != _T('\\') || glpcRootPath[1] != _T('\\')) 
			&& glpcRootPath[1] != _T(':')))
		{
			TCHAR lpcRootPath[MAX_PATH*2+2];
			_tcscpy(lpcRootPath, lpcExeFullPathName);
			_tcscat(lpcRootPath, glpcRootPath);
			::GetFullPathName((const TCHAR *)lpcRootPath,
				MAX_PATH*2,
				glpcRootPath,
				&pTemp);
			for(int ii=0; glpcRootPath[ii]; ii++);
			if(glpcRootPath[ii-1]!=_T('\\') && glpcRootPath[ii-1]!=_T('/'))
			{
				glpcRootPath[ii]=_T('\\'); glpcRootPath[ii+1]=0;
			}
		}
	}
	if (GetPrivateProfileString( _T("settings"), _T("version"), _T(""), lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		for(unsigned int i = 0; i < _tcslen(lpcStrBuffer) && lpcStrBuffer[i] != ' '; ++i)
		{
			ucCurentVersion += lpcStrBuffer[i];
		}
	}

	if (GetPrivateProfileString( _T("paths"), _T("server"), _T(""), lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		m_sServer = lpcStrBuffer;
	}
}

void CUpdaterDlg::onMethodComplete(int aRequestID, int aRMMLID, const wchar_t* apwcResponse, bool isService, int aiErrorCode)
{
	SetEvent(m_hCheckLogin);
	CloseHandle(hCheckLoginThread);
	hCheckLoginThread = NULL;
	char text[512];//строка состояния
	//HKEY	hkey;
	DWORD sizedata = 512;//
	//unsigned char acDataKey[512] = "1.0";//
	if ( aiErrorCode!=0)
	{
		countAuth++;
		if( countAuth<5)
		{
			Sleep(3000);
			Login();
			return;
		}
		char ff[10];
		_itoa( aiErrorCode, ff, 10);
		CComString s = "Ошибка подключения: ";
		if( aiErrorCode == 4)
			s+="Пользователь с таким именем уже в сети";
		s+="\nПовторить?";
		sprintf((char*)&text, "Ошибка соединения");
		this->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
		if (MessageBox(s.GetBuffer(), "Внимание", MB_OKCANCEL | MB_ICONINFORMATION) != IDCANCEL)
		{
			//pOMSContext->mpServiceWorldMan->Disconnect();
			sprintf((char*)&text, "Попытка переподключения");
			this->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
			Sleep(1000);
			countAuth = 0;
			Login();
			return;
		}
		else
		{
			ExitWithError( "Не могу авторизоваться");
			return;
		}
	}

	pUpdaterFiles->setEvent();
}


bool CUpdaterDlg::SetupResManParams()
{
	context->mpComMan->SetConnectType(2);

	setResManager();
	USES_CONVERSION;
	long res = 9;
	res = context->mpComMan->PutPathIni(A2W(lpcIniFullPathName));
	if(res==E_FAIL)
	{
		MessageBox(_T("Некорректная установка программы.\nНеобходима регистрация параметров сервера."), _T("Error!"), MB_OK);
		//ATLASSERT( FALSE);
		return false;
	}
	return true;
}

HMODULE LoadDLL( LPCSTR alpcRelPath, LPCSTR alpcName)
{
	CComString sModuleDirectory;
	GetModuleFileName( (HMODULE)_Module.GetTypeLibInstance(), sModuleDirectory.GetBufferSetLength(MAX_PATH), MAX_PATH);

	CComString sCurrentDirectory;
	GetCurrentDirectory( MAX_PATH, sCurrentDirectory.GetBufferSetLength(MAX_PATH));

	SECFileSystem fs;
	SetCurrentDirectory( fs.GetPath(sModuleDirectory) + alpcRelPath);
	HMODULE handle = ::LoadLibrary( alpcName);
	SetCurrentDirectory( sCurrentDirectory);
	return handle;
}

void* CUpdaterDlg::GetProcAddress(unsigned long auHModule, const char* apcProcName){
	if(auHModule == 0)
		return NULL;
	return ::GetProcAddress((HMODULE)auHModule,apcProcName);
}

bool CUpdaterDlg::OnOK()
{
	return false;
}

LRESULT CUpdaterDlg::OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	bHandled = true;
	if ( iStep != 0)
	{
		//HWND _Item;
		CWindow wnd = GetDlgItem(IDC_CHECK1);
		if ( (int)wnd.SendMessage( BM_GETCHECK, 0, 0) == 1 )
		{
			CComString ClientPath = glpcRootPath;
			ClientPath.MakeLower();
			if( ClientPath.Find("updater.exe")>-1)
			ClientPath.Replace("updater.exe", "vacademia.exe");
			else
			{
				ClientPath+="\\vacademia.exe";
				ClientPath.Replace("\\\\", "\\");
			}
			ClientPath.Replace("/", "\\");
			LPSTR apwcSysCommand = LPSTR(ClientPath.GetBuffer());
			STARTUPINFO si;
			ZeroMemory(&si,sizeof(si));
			si.cb = sizeof(si);
			PROCESS_INFORMATION pi;
			ZeroMemory(&pi,sizeof(pi));
			BOOL res = CreateProcess(NULL,LPSTR(apwcSysCommand),NULL,NULL,TRUE,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi);
			Sleep(500);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		EndDialog(0);
		return 0;
	}
	iStep = 1;
	HWND _Item;
	//делаем неактивной кнопку далее

	//скрываем начальнцю картинку
	CWindow wnd = GetDlgItem(IDC_PIC1);
	wnd.ShowWindow( SW_HIDE);

	//показываем статус бар
	wnd = GetDlgItem(IDC_PROGRESS1);
	wnd.ShowWindow( SW_SHOW);
	
	//полказываем строку состояния
	wnd = GetDlgItem(IDC_LABEL);
	wnd.ShowWindow( SW_SHOW);

	//устанавливаем размеры статус бара
	this->SendDlgItemMessage(IDC_PROGRESS1, 0, MAKELPARAM(0, 100));
	char text[MAX_PATH];//текст для строки состояния

	wnd = GetDlgItem(IDOK1);
	wnd.EnableWindow( FALSE);

	sprintf((char*)&text, "Инициализация менеджера загрузки файлов ...");
	this->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
	*glpcRootPath='\0';
	*glpcRepositoryPath='\0';
	strcpy(glpcUserPath, USER_PATH);
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
	// Тип соединения менеджера ресурсов 
	*glpcResServerConnectType='\0';	

	this->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(1), 0);
	//узнаём текущую установленную версию
	//ParseINI(theApp.m_hInstance);
	char	text2[MAX_PATH];
	char *p = &text2[0];
	if(!ParseINI(NULL, &p))
	{
		ExitWithError( text2);
		return 0;
	}
	//подключаем ComMan

	KillOtherPlayerProcesses();

	#ifdef DEBUG
	#define	COMMAN_MANAGER_DLL	"ComMan.dll"
	#else
	#define	COMMAN_MANAGER_DLL	"ComMan.dll"
	#endif

	if ( !hComModule)
		hComModule = ::LoadDLL( "", COMMAN_MANAGER_DLL);

	context = pOMSContext.get();
	//cm::CreateComManager( pOMSContext.get());
	if ( hComModule)
	{
		omsresult (*lpCreateComManager)( oms::omsContext* aContext);
		(FARPROC&)lpCreateComManager = (FARPROC) GetProcAddress( (unsigned long) hComModule, "CreateComManager");
		if ( lpCreateComManager)
			lpCreateComManager( context);
		if ( !context->mpComMan)
		{
			::FreeLibrary( hComModule);
			hComModule = 0;
		}
	}

	#ifdef DEBUG
	#define	SERVICEMAN_MANAGER_DLL	"ServiceMan.dll"
	#else
	#define	SERVICEMAN_MANAGER_DLL	"ServiceMan.dll"
	#endif

	if ( !hServiceModule)
		hServiceModule = ::LoadDLL( "", SERVICEMAN_MANAGER_DLL);

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

	//context->mpComMan->SetCallbacks(pHandler);

	SetupResManParams();
	
	if(*glpcRootPath)
	{
		CComBSTR bstr(glpcRootPath);
		context->mpComMan->SetLocalBase(bstr);
	}
	else
	{
		TCHAR lpcModuleFileName[MAX_PATH+200];
		//::GetModuleFileName( theApp.m_hInstance, lpcModuleFileName, MAX_PATH+150);
		::GetModuleFileName( NULL, lpcModuleFileName, MAX_PATH+150);
		TCHAR lpcExeFullPathName[MAX_PATH+200];
		LPSTR lpFileName=NULL;
		::GetFullPathName(lpcModuleFileName,MAX_PATH+150,lpcExeFullPathName,&lpFileName);
		if(lpFileName!=NULL)
		{
			*lpFileName='\0';
			CComBSTR bstr(lpcExeFullPathName);
			context->mpComMan->SetLocalBase(bstr);
		}
	}
	if (*glpcRepositoryPath == 0)
		_tcscpy( glpcRepositoryPath, REPOSITORY_PATH);
	context->mpComMan->SetRepositoryBase( CComBSTR( glpcRepositoryPath));
	if (*glpcUserPath == 0)
		_tcscpy( glpcUserPath, USER_PATH);
	context->mpResM->SetUserBase( CComBSTR( glpcUserPath));

	this->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(10), 0);
	DWORD dwThreadID;
	hDownloadThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)StartDownload, this, 0, &dwThreadID);
	return 0;
}



bool CUpdaterDlg::CreateEmptyFile(CComString fullFilePath)
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

/*FILE * CUpdaterDlg::CreateFileWithPath(CComString fullFilePath)
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

	FILE * pFile; 
	pFile = fopen(fullFilePath.GetBuffer(), "wb");

	return pFile;
}*/

void CUpdaterDlg::StartDownload( CUpdaterDlg *apDlg)
{
	if(!apDlg->Login())
	{
		apDlg->ExitWithError( "Сервер не доступен");
	}
}

long CUpdaterDlg::setResManager()
{

	ATLASSERT(context->mpComMan);
	mpIResourceManager = context->mpResM;
	mpIResourceManager->InitResourceManager();
	sp_ResourceManager = context->mpIResM;
	sp_ComMan = context->mpComMan;
	return S_OK;
	//return S_FALSE;
}

DWORD WINAPI CheckLoginDownload(LPVOID lpParameter)
{
	ATLASSERT( lpParameter);
	((CUpdaterDlg*)lpParameter)->CheckLoginDownload();
	return 0;
}

void CUpdaterDlg::CheckLoginDownload()
{
	while(true)
	{
		if(WaitForSingleObject( m_hCheckLogin, 30000)==WAIT_TIMEOUT)
		{
			if(countAuth==-1)
				break;
			char text[512];//строка состояния
			ResetEvent(m_hCheckLogin);
			CComString s = "Соединение не установлено. Проверьте сеть.";
			s+="\nПовторить?";
			//bool bRes = true;
			if (MessageBox(s.GetBuffer(), "Ошибка", MB_OKCANCEL | MB_ICONERROR) != IDCANCEL)
			{
				sprintf((char*)&text, "Попытка переподключения");
				this->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );
				Sleep(1000);
				countAuth = 0;
				Login();
			}
			else
			{
				ExitWithError( "Авторизация не пройдена");
			}				
		}
	}
}

bool CUpdaterDlg::Login()
{
	CComString csLogin = m_Login;
	CComString csPas = m_Password;
	char text[512];//строка состояния

	sprintf((char*)&text, "Установление соединения ...");
	this->SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text );	
	//загрузка XML файла

	CComString ClientPath = glpcRootPath;
	ClientPath.MakeLower();
	USES_CONVERSION;
	sp_ResourceManager->setCacheBase(CComBSTR(A2W(ClientPath.GetBuffer())));

	pOMSContext->mpServiceWorldMan->SetCallbacks( this);
	//pOMSContext->mpServiceWorldMan->Connect();
	wchar_t *wLogin = A2W(csLogin.GetBuffer());
	wchar_t *wPassword = A2W(csPas.GetBuffer());
	int res = pOMSContext->mpServiceWorldMan->Login(wLogin, wPassword, 0);
	DWORD dwThreadID;
	if(hCheckLoginThread == NULL)
		hCheckLoginThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)::CheckLoginDownload, this, 0, &dwThreadID);
	return res>-1?true:false;
}

void CUpdaterDlg::ExitWithError( const char *pText)
{
	/*HINSTANCE hInst = (HINSTANCE) GetWindowLong(GWL_HINSTANCE);
	m_hIcon = LoadIcon( hInst, MAKEINTRESOURCE( IDR_MAINFRAME));
	int ff = GetLastError();
	SetIcon(m_hIcon, TRUE);	
	ff = GetLastError();
	RedrawWindow();*/
	char text[MAX_PATH];
	CComString sError = pText;
	sError += "\n1.	Обновление Виртуальной академии завершилось неуспешно\n2.	Вы можете скачать последний клиент с сайта Виртуальной академии. Для скачивания откройте ссылку: \n 'http://beta.vacademia.com/site/downloadclient'";
	//sprintf((char*)&text, "Ошибка загрузки информации о файле: %s\n",sAttrValue.GetBuffer());
	DWORD k;
	k = 250 << 2;
	k += 350;
	SendDlgItemMessage( IDC_LABEL, WM_SIZE, SIZE_RESTORED, (LPARAM)&k);
	SendDlgItemMessage( IDC_LABEL, WM_SETTEXT, 0, (LPARAM)sError.GetBuffer() );
	try
	{
		CWindow wnd = GetDlgItem( IDC_LABEL);
		LOGFONT lf;//структура для создания шрифта
		//далее - ее заполнение
		ZeroMemory(&lf, sizeof(LOGFONT));//обнуление
		lf.lfHeight = 14;
		lf.lfWidth = 10;
		lf.lfWeight = FW_NORMAL;//==600; от 0 до 1000(жирнющщий =) ).
		lf.lfItalic = false;//наклонный
		lf.lfCharSet = DEFAULT_CHARSET;//кодировка, набор
		lf.lfPitchAndFamily = FF_DONTCARE; //название шрифта
		//strcpy(lf.lfFaceName, "Microsoft Sans Serif");
		HFONT hfont = CreateFontIndirect(&lf);
		wnd.SendMessage( WM_SETFONT, (WPARAM)hfont, MAKELPARAM(true, 0)); 
		//CWnd* pWnd = GetDlgItem( IDC_LABEL);
		//pWnd->SetLayeredWindowAttributes()
	}
	catch (...)
	{
	}

	CWindow wnd = GetDlgItem(IDOK1);
	wnd.EnableWindow( TRUE);

	sprintf((char*)&text, "Закрыть");
	wnd.SendMessage( WM_SETTEXT, 0, (LPARAM)&text);

	SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(0), 0);

	wnd = GetDlgItem(IDC_PROGRESS1);
	wnd.ShowWindow( SW_HIDE);//скрываем статус бар загрузки файла

	wnd = GetDlgItem(IDC_PROGRESS2);
	wnd.ShowWindow( SW_HIDE);//скрываем статус бар загрузки файла

	wnd = GetDlgItem(IDC_PIC1);
	wnd.ShowWindow( SW_HIDE);
	
	wnd = GetDlgItem(IDC_PIC2);
	wnd.ShowWindow( SW_HIDE);

	wnd = GetDlgItem(IDCANCEL);
	wnd.EnableWindow( FALSE);
}

LRESULT CUpdaterDlg::OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	pUpdaterFiles->ReleaseThread();
	bHandled = true;
	pUpdaterFiles->wait();

	TCHAR lpcStrBuffer[MAX_PATH*2+2];
	//::GetModuleFileName( theApp.m_hInstance, lpcStrBuffer, MAX_PATH);
	::GetModuleFileName( NULL, lpcStrBuffer, MAX_PATH);
	TCHAR lpcExeFullPathName[MAX_PATH*2+2];
	LPTSTR pTemp;
	::GetFullPathName((const TCHAR *)lpcStrBuffer,
		MAX_PATH*2,
		lpcExeFullPathName,
		&pTemp);
	TCHAR lpcExeName[MAX_PATH];
	_tcscpy(lpcExeName,pTemp);
	// cut '.exe'
	for(int ii=0; lpcExeName[ii]; ii++);
	for(; ii>0 && lpcExeName[ii]!=_T('.'); ii--);
	if(ii>0) lpcExeName[ii]=0;

	*pTemp=0;
	CComString CachePath = lpcExeFullPathName;
	CachePath +="\\root\\files\\client\\";
	CachePath.Replace("\\\\", "\\");
	int res;
	SHFILEOPSTRUCT fo;
	ZeroMemory(&fo, sizeof(fo));
	fo.hwnd   = NULL;  // хэндл окна-владельца прогресс-диалога
	//fo.pFrom  = CachePath.GetBuffer();
	fo.pFrom  = CachePath.GetBuffer();
	fo.wFunc  = FO_DELETE;
	fo.fFlags = FOF_NOERRORUI | FOF_NOCONFIRMATION;
	res = SHFileOperation(&fo);

	pUpdaterFiles->releaseMutex();
	// TODO: Add your control notification handler code here
	//OnCancel();
	return EndDialog(0);
}

long CUpdaterDlg::onResourceNotify(IResource *aResource, DWORD aNotifyCode)
{
	if ( aNotifyCode == RN_ASYNCH_DOWNLOADED )
	{
		DWORD aSize = 0;
		DWORD aSize2 = 0;
		sp_Resource->getDownloadedSize(&aSize);
		sp_Resource->getSize(&aSize2);
		if  ( aSize2 == aSize )
		{
			//sp_Resource->close();
			sp_Resource->destroy();
			//sp_Resource.Release();
			//delete sp_Resource;
			sp_Resource = NULL;
			bNext = true;
		}	
	}
	// ??
	return 0;
}

LRESULT CUpdaterDlg::OnStnClickedLabel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	return 0;
}

void CUpdaterDlg::WritePlayerIni(char* text)
{
	CWindow wnd = GetDlgItem(IDOK1);
	wnd.EnableWindow( TRUE);
	sprintf((char*)&text, "Закрыть");
	wnd.SendMessage( WM_SETTEXT, 0, (LPARAM)&text);
	/*  добавить изменение версии в файл player.ini */
	if(!FileExists(lpcIniFullPathName))
	{
		ExitWithError( "Ошибка обновления информации о версии, файл player.ini не найден");
		return;
	}
	if (!WritePrivateProfileString((LPCTSTR)"settings", (LPCTSTR)"version", (LPCTSTR)m_NewNersion.GetBuffer(), (LPCTSTR)lpcIniFullPathName))
	{
		ExitWithError( "Ошибка обновления информации о версии");
		return;
	}

	if (!WritePrivateProfileString((LPCTSTR)"paths", (LPCTSTR)"server", (LPCTSTR)m_sServer.GetBuffer(), (LPCTSTR)lpcIniFullPathName))
	{
		ExitWithError( "Ошибка обновления информации о сервере");
		return;
	}
	if (!WritePrivateProfileString((LPCTSTR)"user", (LPCTSTR)"login", (LPCTSTR)m_Login.GetBuffer(), (LPCTSTR)lpcIniFullPathName))
	{
		ExitWithError( "Ошибка обновления информации о логине");
		return;
	}
}