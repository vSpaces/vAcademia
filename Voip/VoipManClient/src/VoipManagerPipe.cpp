#include "StdAfx.h"
#include "..\include\voipManagerPipe.h"
#include "Tlhelp32.h"
#include "Psapi.h"
#include "include/PipeProtocol/InitVoipSystemOut.h"
#include "include/PipeProtocol/SetVoiceGainLevelOut.h"
#include "include/PipeProtocol/HandleTransmissionStateChangedOut.h"
#include "include/PipeProtocol/SetVoiceActivationLevelOut.h"
#include "include/PipeProtocol/SetAutoGaneControlEnabledOut.h"
#include "include/PipeProtocol/SetEnableDenoisingOut.h"
#include "include/PipeProtocol/GetDevicesOut.h"
#include "include/PipeProtocol/GetInputDevicesOut.h"
#include "include/PipeProtocol/GetOutputDevicesOut.h"
#include "include/PipeProtocol/SetUserGainLevelOut.h"
#include "include/PipeProtocol/ConnectToEVoipOut.h"
#include "include/PipeProtocol/AcceptingOut.h"
#include "include/PipeProtocol/DisconnectOut.h"
#include "include/PipeProtocol/DoLoginOut.h"
//#include "include/PipeProtocol/GetUserOut.h"
#include "include/PipeProtocol/SetUserVolumeOut.h"
#include "../nengine/ncommon/ThreadAffinity.h"
#include "SimpleThreadStarter.h"
#include "include/PipeProtocol/ConfirmationOut.h"

CVoipManagerPipe::CVoipManagerPipe(oms::omsContext* aContext, CVoipManagerConaito *aVoipManagerConaito)
{
	eventConnectPipes = CreateEvent( NULL, FALSE, FALSE, "eventConnectPipes");

	m_voipManProcessID = 0;
	context = aContext;
	bRunning = true;
	lastCommandReceivedTime = 0;
	maxDelayBetweenCommands = 180;
	pVoipManagerConaito = aVoipManagerConaito;
	bDisConnected = true;
	pipesID = 0;

	bRestoringEVO = true;
	MP_NEW_V( pVoipManagerConaitoServer, CVoipManagerConaitoServer, pVoipManagerConaito->GetOMSContext());
	pVoipManagerPipeEVO = NULL;
	bRunningCheckTime = true;
	bReChangePipes = false;

	m_readPipeServer.SetContext(aContext);
	m_writePipeServer.SetContext(aContext);
}

void CVoipManagerPipe::ReleaseThread( HANDLE &aThread)
{
	if( aThread == NULL)
		return;
	if( WaitForSingleObject( aThread, 2000) == WAIT_TIMEOUT)
	{
		mutexClose.lock();
		if(aThread!=NULL)
		{
			//09.04.2014 лишь при закрытии приложения - иначе риск повисаний
			::TerminateThread( aThread, 0);
			CloseHandle(aThread);
			aThread = NULL;
		}
		mutexClose.unlock();
	}
}

CVoipManagerPipe::~CVoipManagerPipe(void)
{
	bRunning = false;
	if( pVoipManagerPipeEVO != NULL)
		MP_DELETE( pVoipManagerPipeEVO);
	if( pVoipManagerConaitoServer != NULL)
		MP_DELETE( pVoipManagerConaitoServer);
	DestroyPipes();
	std::wstring sCommand = GetPathVoipManServer();
	int iCountNormalProcess = FindVoipManServerProcesses( sCommand, true);	

	WriteLog( "FindVoipManServerProcesses ended");

	context = NULL;

	ReleaseThread( m_hTimeThread);
	ReleaseThread( m_hWritePipeThread);	
	ReleaseThread( m_hReadPipeThread);
	ReleaseThread( m_hProcessThread);	
	
}

HWND ghwndProcessWindow = NULL;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam){
	DWORD dwWinProcID = 0;
	GetWindowThreadProcessId(hwnd, &dwWinProcID);
	if(dwWinProcID == (DWORD)lParam){
		TCHAR   className[MAX_PATH+1];
		if( GetClassName( hwnd, className, MAX_PATH) > 0)
		{
			if( strcmp( className, "VOIPMAN") == 0)
			{
				ghwndProcessWindow = hwnd;
				//v_WindowHandle.push_back( hwnd);
				return FALSE; // нашли нужное окно. Можно не продолжать поиск
			}
		}
	}
	return TRUE;
}

bool FindVoipManServerWindow( int aProcessId)
{
	ghwndProcessWindow = NULL;
	EnumWindows(EnumWindowsProc, (LPARAM)aProcessId);
	// если главное окно нашли
	if(ghwndProcessWindow != NULL){
		// и с ним все в порядке
		// (пока будем считать, что с окном все в порядке, если можно взять инфу о нем)
		/*if( IsWindowVisible( ghwndProcessWindow))
			continue;*/
		WINDOWINFO wi;
		ZeroMemory(&wi, sizeof(WINDOWINFO));
		if( GetWindowInfo( ghwndProcessWindow, &wi))
			return true;
	}
	return false;
}

bool CVoipManagerPipe::KillVoipManServerProcess( int aProcessId)
{	
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, aProcessId);
	if (hProcess == NULL)
		return false;

	DWORD dwError = ERROR_SUCCESS;

	// пытаемся завершить процесс
	if (!rtl_TerminateOtherProcess(hProcess, (DWORD)-1))
	{
		dwError = GetLastError();
		CComString sLog;	sLog.Format("[VOIP] TerminateProcess is failed errorCode=%u", dwError);
		WriteLog( sLog.GetBuffer());
	}
	else
	{
		WriteLog("[VOIP] TerminateProcess is success");
	}

	// закрываем описатель процесса
	CloseHandle(hProcess);		
	return true;
}

int CVoipManagerPipe::FindVoipManServerProcesses( std::wstring &sPathFile, bool abClose)
{
	CWComString sFullPath1 = sPathFile.c_str();
	sFullPath1.MakeLower();
	int iRes = 0;
	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32W pe32; 
	ZeroMemory(&pe32, sizeof(pe32));

	CComString sLog;
	sLog = "[VOIP_PIPE] FindVoipManServerProcesses sPathFile==";
	USES_CONVERSION;
	sLog += W2A(sPathFile.c_str());
	WriteLog( sLog.GetBuffer());

	//  Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 	

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return -1; 

	//  Fill in the size of the structure before using it. 
	pe32.dwSize = sizeof(PROCESSENTRY32W); 

	//  Walk the snapshot of the processes, and for each process, 
	//  display information. 
	//v_WindowHandle.clear();
	if (Process32FirstW(hProcessSnap, &pe32)) { 
		//DWORD         dwPriorityClass; 
		BOOL          bGotModule = FALSE; 
		MODULEENTRY32 me32; 
		//me32.dwSize = sizeof(me32);
		ZeroMemory(&me32, sizeof(me32));

		do{
			CWComString sProcessName = pe32.szExeFile;
			sProcessName.MakeLower();
			if(sFullPath1.Find( sProcessName.GetBuffer()) == -1)
				continue;
			// открываем процесс, чтобы узнать побольше о его состоянии
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,
				FALSE, pe32.th32ProcessID);
			// если процесс открыть не смогли, то пропускаем его
			if (hProcess == NULL)
				continue;
			HMODULE hExeModule = NULL;
//			DWORD cbNeeded;
			wchar_t szModulePath[MAX_PATH*2];
			LPTSTR pszProcessName = NULL;
			// получаем полный путь к EXE-файлу процесса
			if (GetModuleFileNameExW(hProcess, hExeModule, szModulePath, MAX_PATH*2)){
				// если путь к exe-файлу разный, то пропускаем процесс
				CWComString sFullPath2 = szModulePath;
				sFullPath2.MakeLower();
				int pos1 = sFullPath1.Find(sFullPath2.GetBuffer()); 
				//int pos2 = sFullPath2.Find(szModulePath); 
				//if(pos1 == -1 && pos2 == -1){
				if(pos1 == -1){					
					CloseHandle(hProcess);
					continue;
				}
			}
			//}
			CloseHandle(hProcess);
			CComString sLog;
			sLog.Format("[VOIP_PIPE] FindVoipManServerProcesses::found is success  pe32.th32ProcessID==%d", pe32.th32ProcessID);
			WriteLog( sLog.GetBuffer());
			// получаем описатель процесса
			if( !abClose && FindVoipManServerWindow( pe32.th32ProcessID))
			{
				iRes++;
				continue;
			}
			CComString sLog1;
			sLog1.Format("[VOIP_PIPE] FindVoipManServerProcesses::KillProcess  pe32.th32ProcessID==%d", pe32.th32ProcessID);
			WriteLog( sLog1.GetBuffer());
			KillVoipManServerProcess( pe32.th32ProcessID);
		} 
		while (Process32NextW(hProcessSnap, &pe32)); 
	}
	return iRes;
}

std::wstring CVoipManagerPipe::GetPathVoipManServer()
{
	std::wstring sCommand;
	wchar_t	moduleFileName[ 10*MAX_PATH] = {L'\0'};
	GetModuleFileNameW( NULL, moduleFileName, MAX_PATH);
	wchar_t *lpFileName = NULL;
	wchar_t	modulePath[ 10*MAX_PATH] = {L'\0'};
	::GetFullPathNameW(moduleFileName,sizeof(modulePath)/sizeof(modulePath[0]),modulePath,&lpFileName);
	sCommand = modulePath;
	int nPos = sCommand.find_last_of(L"\\");
	if(nPos > -1)
		sCommand = sCommand.substr( 0, nPos+1);
	sCommand += L"VoipMan.exe";
	nPos = sCommand.find(L"\\\\");
	if(nPos > -1)
		sCommand.replace(nPos, 2, L"\\");
	return sCommand;
}

bool CVoipManagerPipe::RunVoipManagerServer( bool abClose, bool& abIntoPlayer)
{
	CComString sLog;
	WriteLog( "[VOIP_PIPE] RunVoipManagerServer");

	if( !CreatePipes())
	{
		WriteLog( "[VOIP_PIPE] Fatal error. Pipes was not created.");
		return false;
	}

	std::wstring sCommand = GetPathVoipManServer();
	int iCountNormalProcess = 0;
	mutexRun.lock();
	iCountNormalProcess = FindVoipManServerProcesses( sCommand, abClose);
	mutexRun.unlock();
	if( iCountNormalProcess > 0)
	{
		sLog.Format( "[VOIP_PIPE] Fatal error. VoipMax.exe found that can`t be closed. iCountNormalProcess = %u", iCountNormalProcess);
		WriteLog( sLog.GetBuffer());
		return false;
	}	

	STARTUPINFOW si;
	ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(si);	
	ZeroMemory(&pi,sizeof(pi));
	sCommand += L" -piperead \"";
	//sCommand += sPipeWriteToEVOIP;
	USES_CONVERSION;
	CWComString s = A2W( sPipeWriteToEVOIP.GetBuffer()); s.Replace(L"\\", L"/");
	sCommand += s;
	sCommand += L"\" -pipewrite \"";
	s = A2W(sPipeReadFromEVOIP.GetBuffer()); s.Replace(L"\\", L"/");
	sCommand += s;
	//sCommand += sPipeReadFromEVOIP;
	sCommand += L"\"";
	sLog.Format("sCommand.c_str() = %s", W2A(sCommand.c_str()));
	WriteLog( sLog.GetBuffer());
	BOOL res = FALSE;
	if( !abIntoPlayer)
		res = CreateProcessW(NULL, (LPWSTR)sCommand.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	m_voipManProcessID = pi.dwProcessId;
	
	_SYSTEM_INFO nfo;
	GetSystemInfo(&nfo);
	int coreCount = nfo.dwNumberOfProcessors;
	if (pi.hProcess != NULL) {
		if (coreCount > 1)
		{
			SetProcessAffinityMask(pi.hProcess, 2);//0x10 - 2е ядро
		}
		SetPriorityClass(pi.hProcess, NORMAL_PRIORITY_CLASS);
	}

	if(res)
	{		
		WriteLog( "[VOIP_PIPE] VoipMan launched!");
	}
	else
	{
		if( !abIntoPlayer)
		{
			sLog.Format( "[VOIP_PIPE] Critical error. CreateProcess failed. Error code is %u", GetLastError());
			WriteLog( sLog.GetBuffer());
		}
		abIntoPlayer = true;

		if( pVoipManagerPipeEVO == NULL)
			MP_NEW_V4( pVoipManagerPipeEVO , CVoipManagerPipeEVO, pVoipManagerConaito->GetOMSContext(), pVoipManagerConaitoServer, sPipeReadFromEVOIP.GetBuffer(), sPipeWriteToEVOIP.GetBuffer());


		if( !pVoipManagerConaitoServer->IsVoipSystemInitialized())
			pVoipManagerConaitoServer->Initialize( false);

		CSimpleThreadStarter serverThreadStarter(__FUNCTION__);
		serverThreadStarter.SetRoutine(ThreadServerRunner);
		serverThreadStarter.SetParameter(this);
		HANDLE hThread = serverThreadStarter.Start();	
		if (hThread == NULL)
		{
			sLog.Format( "[VOIP_PIPE] Critical error. CreateThread failed. Error code is %u", GetLastError());
			WriteLog( sLog.GetBuffer());
		}
	}

	// Ожидаем подключения клиентских пайпов
	if( !ConnectPipes())
	{
		WriteLog( "[VOIP_PIPE] Fatal error. Pipes was not connected.");
		return false;
	}
	bReChangePipes = false;

	WriteLog( "[VOIP_PIPE] Initialize VoipSystem");
	maxDelayBetweenCommands = 60;
	if( !pVoipManagerConaito->IsGetInputDevices())
	{			
		SendGetDevices( false, false);
	}

	if( bRestoringEVO)
	{
		WriteLog( "[VOIP_PIPE] ReInitializeVoipSystem");
		pVoipManagerConaito->ReInitializeVoipSystem();
		pVoipManagerConaito->ConnectImpl( false);
		pVoipManagerConaito->UpdateVoipConnection();		
		pVoipManagerConaito->StartCheckAudioDevices();
	}
	else
	{
		WriteLog( "[VOIP_PIPE] ReInitializeVoipSystem");
	}
	//SetEvent( mutexTimeOut);
	return true;
}

DWORD WINAPI CVoipManagerPipe::ThreadServerRunner(LPVOID lpvParam)
{
	((CVoipManagerPipe*)lpvParam)->ServerRunnerThread();

	return 0;
}

void CVoipManagerPipe::ServerRunnerThread()
{	
	Sleep(100);
	if( pVoipManagerPipeEVO != NULL)
		pVoipManagerPipeEVO->Run( false);
}

void CVoipManagerPipe::Run()
{
	CSimpleThreadStarter pipeThreadStarter(__FUNCTION__);
	pipeThreadStarter.SetRoutine(ThreadWritePipeRunner);
	pipeThreadStarter.SetParameter(this);
	m_hWritePipeThread = pipeThreadStarter.Start();
	m_writePipeThreadId = pipeThreadStarter.GetThreadID();
	
	if (m_hWritePipeThread == NULL)
	{
		WriteLog("[VOIP_PIPE] Can't create ThreadWritePipeRunner");
	}
	 
	CSimpleThreadStarter pipeReadThreadStarter(__FUNCTION__);
	pipeReadThreadStarter.SetRoutine(ThreadReadPipeRunner);
	pipeReadThreadStarter.SetParameter(this);
	m_hReadPipeThread = pipeReadThreadStarter.Start();
	m_readPipeThreadId = pipeReadThreadStarter.GetThreadID();

	if (m_hReadPipeThread == NULL)
	{
		WriteLog("[VOIP_PIPE] Can't create ThreadReadPipeRunner");
	}

	CSimpleThreadStarter commandThreadStarter(__FUNCTION__);
	commandThreadStarter.SetRoutine(ThreadProcessCommand);
	commandThreadStarter.SetParameter(this);
	m_hProcessThread = commandThreadStarter.Start();
	m_processThreadId = commandThreadStarter.GetThreadID();

	if (m_hProcessThread == NULL)
	{
		WriteLog("[VOIP_PIPE] Can't create ThreadProcessCommand");
	}

	CSimpleThreadStarter timeThreadStarter(__FUNCTION__);
	timeThreadStarter.SetRoutine(ThreadTimeCommand);
	timeThreadStarter.SetParameter(this);
	m_hTimeThread = timeThreadStarter.Start();
	m_timeThreadId = timeThreadStarter.GetThreadID();

	if (m_hTimeThread == NULL)
	{
		WriteLog("[VOIP_PIPE] Can't create ThreadTimeCommand");
	}
	if( m_hTimeThread != NULL)
		SetThreadPriority(m_hTimeThread, THREAD_PRIORITY_ABOVE_NORMAL);
}

bool CVoipManagerPipe::IsRestoringConnect()
{
	return bRestoringEVO;
}

void CVoipManagerPipe::SetRestoredConnect()
{
	bRestoringEVO = false;
}

void CVoipManagerPipe::IncNumPipe( CComString &sPipe, char *aBaseName, int &iNumPipe)
{
	iNumPipe++;
	if( iNumPipe > 999)
		iNumPipe = 0;
	char ff[5];
	_itoa_s( iNumPipe, ff, 5, 10);
	sPipe = aBaseName;
	sPipe += ff;
}

DWORD WINAPI CVoipManagerPipe::ThreadWritePipeRunner(LPVOID lpvParam)
{
	((CVoipManagerPipe*)lpvParam)->WritePipeServerThread();

	return 0;
}

void CVoipManagerPipe::WritePipeServerThread()
{	
	int  iNumPipe = 0;
	while( bRunning)
	{
		// Ожидаем создания пайпа
		if(!m_writePipeServer.IsCreated() || !m_writePipeServer.IsConnected())
		{
			Sleep(200);
			continue;
		}

		while ( bRunning && m_writePipeServer.IsConnected())
		{
			std::vector<CDataStorage*> commands;
			CDataStorage* outCommandData = m_commandsData.GetOutCommand();
			if (outCommandData == NULL)
			{
				//??
			}
			else while (outCommandData != NULL)
			{
				commands.push_back(outCommandData);
				outCommandData = m_commandsData.GetOutCommand();
			}

			bool writeError = false;
			if( commands.size() > 0)
			{
				m_writePipeServer.WriteData(commands, writeError); // отправка команды
			}

			if(writeError)
			{
				CComString sLog; sLog.Format("[VOIP_PIPE] Write to pipe %s error", sPipeWriteToEVOIP.GetBuffer());
				WriteLog( sLog.GetBuffer());
				DestroyPipes();
				maxDelayBetweenCommands = 0;
				bRestoringEVO = true;
				break;
			}

			Sleep(50);
			continue;			
		}		
	}
	mutexClose.lock();
	m_hWritePipeThread = NULL;
	mutexClose.unlock();
}

DWORD WINAPI CVoipManagerPipe::ThreadReadPipeRunner(LPVOID lpvParam)
{
	((CVoipManagerPipe*)lpvParam)->ReadPipeServerThread();

	return 0;
}

void CVoipManagerPipe::ReadPipeServerThread()
{	
	int  iNumPipe = 0;
	while( bRunning)
	{
		// Ожидаем создания и подключения пайпа
		if(!m_readPipeServer.IsCreated() || !m_readPipeServer.IsConnected())
		{
			Sleep(200);
			continue;
		}

		// Начинаем читать данные
		while ( bRunning && m_readPipeServer.IsConnected())
		{
			if( bReChangePipes)
			{
				Sleep(100);
				break;
			}
			bool readError = false;
			int count = m_readPipeServer.ReadCount( readError);
			if( readError)
			{
				if(  bRunning && !bReChangePipes)
				{
					CComString sLog; sLog.Format("[VOIP_PIPE] Read from  pipe %s error", sPipeReadFromEVOIP.GetBuffer());
					WriteLog( sLog.GetBuffer());
					DestroyPipes();
				}				
				maxDelayBetweenCommands = 0;
				bRestoringEVO = true;
				break;
			}
			
			for (int packetNum = 0; packetNum < count; packetNum++)
			{
				if(bReChangePipes)
				{
					break;
				}
				CDataStorage* inCommandData = m_readPipeServer.ReadData( readError);
				if(readError)
				{
					break;
				}
				if( inCommandData == NULL)
				{
					packetNum--;
					Sleep(50);
					continue;
				}
				if(inCommandData != NULL)
					m_commandsData.AddInCommand( inCommandData); // чтение команды
			}

			if( readError)
			{
				if(  bRunning && !bReChangePipes)
				{
					CComString sLog; sLog.Format("[VOIP_PIPE] Read from  pipe %s error", sPipeReadFromEVOIP.GetBuffer());
					WriteLog( sLog.GetBuffer());
					DestroyPipes();
				}				
				maxDelayBetweenCommands = 0;
				bRestoringEVO = true;
				break;
			}
			else
			{
			//	lastCommandReceivedTime = GetTickCount()/1000;
			}

			Sleep(50);
		}
		Sleep(100);
	}
	mutexClose.lock();
	m_hReadPipeThread = NULL;
	mutexClose.unlock();
}

bool CVoipManagerPipe::CreatePipes()
{
	CComString sLog;
	WriteLog( "[VOIP_PIPE] Create pipes");

	DestroyPipes();

	int triesCount = 10;
	while( triesCount > 0)
	{
		pipesID++;

		sLog.Format("[VOIP_PIPE] Try to create pipes pipesID = %u", pipesID);
		WriteLog(  sLog.GetBuffer());

		sPipeWriteToEVOIP.Format("\\\\.\\pipe\\voipmanpipetoevoip%u", pipesID);
		sPipeReadFromEVOIP.Format("\\\\.\\pipe\\voipmanpipefromevoip%u", pipesID);

		int errorCode = 0;
		if( (errorCode = m_writePipeServer.CreatePipe( sPipeWriteToEVOIP.GetBuffer())) == 0)
		{
			if( (errorCode = m_readPipeServer.CreatePipe( sPipeReadFromEVOIP.GetBuffer())) == 0)
			{
				WriteLog( "[VOIP_PIPE] Pipes was created.");
				return true;
			}
			else
			{
				sLog.Format("[VOIP_PIPE] Failed to create read pipe %s. Error code = %u", sPipeWriteToEVOIP.GetBuffer(), errorCode);
				WriteLog( sLog.GetBuffer());
			}
		}
		else
		{
			sLog.Format("[VOIP_PIPE] Failed to create write pipe %s. Error code = %u", sPipeWriteToEVOIP.GetBuffer(), errorCode);
			WriteLog( sLog.GetBuffer());
		}
		triesCount--;
		Sleep( 100);
	}
	WriteLog( "[VOIP_PIPE] Pipes was not created.");
	return false;
}

void CVoipManagerPipe::DestroyPipes()
{
	CComString sLog;
	WriteLog( "[VOIP_PIPE] Destroy pipes");
	m_commandsData.ClearInCommands();
	m_commandsData.ClearOutCommands();
	// Удаляем пишуший пайп
	int errorCode = 0;
	if( m_writePipeServer.IsCreated())
	{
		errorCode = m_writePipeServer.DestroyPipe();
		sLog.Format("[VOIP_PIPE] m_writePipeServer was disconnected. Disconnect result of %s is %u", sPipeWriteToEVOIP.GetBuffer(), errorCode);
	}
	else
	{
		sLog.Format("[VOIP_PIPE] m_writePipeServer is not connected. Destroy is not needed.");
	}
	WriteLog( sLog.GetBuffer());

	// Удаляем читающий пайп
	if( m_readPipeServer.IsCreated())
	{
		bReChangePipes = true;
		errorCode = m_readPipeServer.DestroyPipe();
		sLog.Format("[VOIP_PIPE] m_readPipeServer was disconnected. Disconnect result of %s is %u", sPipeReadFromEVOIP.GetBuffer(), errorCode);
	}
	else
	{
		sLog.Format("[VOIP_PIPE] m_readPipeServer is not connected. Destroy is not needed.");
	}
	WriteLog( sLog.GetBuffer());

	WriteLog( "[VOIP_PIPE] Pipes was destroyed");
}


DWORD WINAPI CVoipManagerPipe::ThreadConnectPipes(LPVOID lpvParam)
{
	((CVoipManagerPipe*)lpvParam)->ConnectPipesAsync();
	return 0;
}

void CVoipManagerPipe::ConnectPipesAsync()
{
	CComString sLog;
	// Ждем подключения пишушего пайпа
	bConnectError = false;
	if( m_writePipeServer.IsCreated())
	{
		int errorCode = 0;
		if( (errorCode = m_writePipeServer.WaitForConnection()) == 0)
		{
			sLog.Format("[VOIP_PIPE] m_writePipeServer is connected.");
		}
		else
		{
			bConnectError = true;
			sLog.Format("[VOIP_PIPE] m_writePipeServer is not connected. WaitForConnection result of %s is %u", sPipeWriteToEVOIP.GetBuffer(), errorCode);
		}
	}
	else
	{
		bConnectError = true;
		sLog.Format("[VOIP_PIPE] m_writePipeServer is not connected. Failed to connect.");
	}
	WriteLog( sLog.GetBuffer());

	// Ждем подключения читающего пайпа
	if( !bConnectError)
	{
		if( m_readPipeServer.IsCreated())
		{
			int errorCode = 0;
			if( (errorCode = m_readPipeServer.WaitForConnection()) == 0)
			{
				sLog.Format("[VOIP_PIPE] m_readPipeServer is connected.");
			}
			else
			{
				bConnectError = true;
				sLog.Format("[VOIP_PIPE] m_readPipeServer is not connected. WaitForConnection result of %s is %u", sPipeReadFromEVOIP.GetBuffer(), errorCode);
			}			
		}
		else
		{
			bConnectError = true;
			sLog.Format("[VOIP_PIPE] m_readPipeServer is not connected. Failed to connect.");
		}
	}
	WriteLog( sLog.GetBuffer());
	::SetEvent(eventConnectPipes);
}

bool CVoipManagerPipe::ConnectPipes()
{	
	WriteLog( "[VOIP_PIPE] Connect pipes");

	CSimpleThreadStarter connectPipesThreadStarter(__FUNCTION__);
	connectPipesThreadStarter.SetRoutine(ThreadConnectPipes);
	connectPipesThreadStarter.SetParameter(this);
	HANDLE hConnectPipesThread = connectPipesThreadStarter.Start();
	DWORD connectPipesThreadId = connectPipesThreadStarter.GetThreadID();

	if (hConnectPipesThread == NULL)
	{
		WriteLog("[VOIP_PIPE] Can't create ThreadConnectPipes");
	}
	if(hConnectPipesThread != NULL)
		SetThreadPriority(hConnectPipesThread, THREAD_PRIORITY_ABOVE_NORMAL);

	int waitRes = ::WaitForSingleObject( eventConnectPipes, 30000);
	if( waitRes == WAIT_TIMEOUT)
	{
		WriteLog( "[VOIP_PIPE] time out pipes connect");
		DestroyPipes();
		CloseHandle(hConnectPipesThread);
		bConnectError = true;
	}

	if( bConnectError)
		WriteLog( "[VOIP_PIPE] Pipes was not connected");
	else
		WriteLog( "[VOIP_PIPE] Pipes was connected");

	return !bConnectError;
}

void CVoipManagerPipe::WriteLog( LPCSTR alpcLogData)
{
	if( alpcLogData && *alpcLogData && context != NULL && context->mpLogWriter != NULL)
	{
		context->mpLogWriter->WriteLnLPCSTR( alpcLogData);
	}
}

DWORD WINAPI CVoipManagerPipe::ThreadProcessCommand(LPVOID lpvParam)
{
	((CVoipManagerPipe*)lpvParam)->ProcessCommand();

	return 0;
}

// чтение данных от сервера менеджера и передача их на исполнению клиент менеджера
void CVoipManagerPipe::ProcessCommand()
{
	while ( bRunning)
	{
		if (!m_readPipeServer.IsCreated() || !m_readPipeServer.IsConnected())
		{
			Sleep(500);
			continue;
		}

		byte cmdID = 0;
		CDataStorage* commandData = m_commandsData.GetInCommand();

		while (commandData != NULL)
		{
			if ((commandData->Read(cmdID) && cmdID != 0 /*&& cmdID != 1*/))
			{
				break;
			}

			WriteLog("analyse number protocol message is false");
			MP_DELETE(commandData);
			commandData = m_commandsData.GetInCommand();
		}

		if (commandData != NULL)
		{
			if (cmdID == CMD_IDLE)
			{
				//printf("ProcessCommand::CMD_IDLE\n");				
				continue;
			}

			ProcessCommandById(cmdID, commandData);
			MP_DELETE(commandData);
		}	
		if (m_commandsData.GetInCommandSize() == 0)
			Sleep(50);
		else
			Sleep(1);
	}
	mutexClose.lock();
	m_hProcessThread = NULL;
	mutexClose.unlock();
}

DWORD WINAPI CVoipManagerPipe::ThreadTimeCommand(LPVOID lpvParam)
{
	((CVoipManagerPipe*)lpvParam)->CheckTime();

	return 0;
}

void CVoipManagerPipe::CheckTime()
{
	lastCommandReceivedTime = 0;
	HANDLE hProcess = NULL;
	int countTryRun = 0;
	int maxCountTryRun = 4;
	unsigned int maxIntervalTryRun = 240;
	unsigned int lastTryRunTime = 0;
	bool bVoipIntoPlayer = false;
	while( bRunning && bRunningCheckTime)
	{
		unsigned int currentTime = GetTickCount()/1000;
		unsigned int currentDelay = (currentTime - lastCommandReceivedTime);	
		bool canSendAliveMessage = true;
		if( m_voipManProcessID != 0)
		{
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_voipManProcessID);			
			if( hProcess == NULL || hProcess == INVALID_HANDLE_VALUE)
			{	
				CComString sLog;
				int errorCode = GetLastError();
				if( errorCode == ERROR_ACCESS_DENIED)
				{
					hProcess = NULL;
					sLog.Format("[VOIP_PIPE] Get Process voipMan Info is not access, errorCode==%d", errorCode);
				}
				else
				{
					hProcess = INVALID_HANDLE_VALUE;
					sLog.Format("[VOIP_PIPE] Get Process voipMan Info is halt, errorCode==%d", errorCode);
				}
				WriteLog( sLog.GetBuffer());

				canSendAliveMessage = false;
			}
		}
		else if (!bVoipIntoPlayer)
		{
			canSendAliveMessage = false;
		}

		if (canSendAliveMessage)
		{
			CConfirmationOut packetOut(CMD_CONFIRMATION);
			m_commandsData.AddOutCommand( packetOut.GetDataStorage());
		}

		if( !bVoipIntoPlayer && hProcess == INVALID_HANDLE_VALUE || currentDelay > maxDelayBetweenCommands)
		{			
			CComString sLog;	sLog.Format("[VOIP_PIPE] Max delay between commands exceeded, maxDelayBetweenCommands = %u, current delay = %u, process handle = %u, m_voipManProcessID = %u. Restarting VoipMan.exe", maxDelayBetweenCommands, currentDelay, hProcess, m_voipManProcessID);
			WriteLog( sLog.GetBuffer());
			if (bVoipIntoPlayer)
			{
				//внутри академии не запускаем больше 1го раза(не реализована такая логика в RunVoipManagerServer и память течь будет)
				lastCommandReceivedTime = GetTickCount()/1000;
				WriteLog( "[VOIP_PIPE] VoipMan into vacademia was not restarted");
				Sleep( 1500);
				continue;
			}

			m_voipManProcessID = 0;

			maxDelayBetweenCommands = 60;
			if( countTryRun > 0 && (currentTime - lastTryRunTime) > maxIntervalTryRun)
				countTryRun = 0;
			countTryRun++;
			lastTryRunTime = currentTime;
			bVoipIntoPlayer = (countTryRun > maxCountTryRun);
			//bVoipIntoPlayer = true;
			bRestoringEVO = true;
			if( RunVoipManagerServer( true, bVoipIntoPlayer))
			{
				if(bVoipIntoPlayer)
					WriteLog( "[VOIP_PIPE] voip system start into player");
				else
					WriteLog( "[VOIP_PIPE] VoipMan.exe was restarted");
				lastCommandReceivedTime = currentTime;
			}
			else
			{
				WriteLog( "[VOIP_PIPE] VoipMan.exe was not restarted");
				lastCommandReceivedTime = currentTime - 5;
				Sleep( 5000);
			}
		}
		if (hProcess != INVALID_HANDLE_VALUE && hProcess != NULL)
		{
			CloseHandle(hProcess);
			hProcess = INVALID_HANDLE_VALUE;
		}
		Sleep( 1500);
	}
	mutexClose.lock();
	m_hTimeThread = NULL;
	mutexClose.unlock();
}

void CVoipManagerPipe::SendGetDevices(bool abNeedAnswerToRmml, bool abForce)
{
	CGetDevicesOut packetOut( CMD_GETDEVICES, abNeedAnswerToRmml, abForce);	
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
	WriteLog( "[VOIP] SendGetDevices");		
}

void CVoipManagerPipe::SendGetInputDevices(bool abNeedAnswerToRmml, bool abForce)
{
	CGetInputDevicesOut packetOut( CMD_GETINPUTDEVICES, abNeedAnswerToRmml, abForce);	
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
	WriteLog( "[VOIP] SendGetInputDevices");		
}

void CVoipManagerPipe::SendGetOutputDevices(bool abNeedAnswerToRmml, bool abForce)
{
	CGetOutputDevicesOut packetOut( CMD_GETOUTPUTDEVICES, abNeedAnswerToRmml, abForce);	
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
	WriteLog( "[VOIP] SendGetOutputDevices");		
}

void CVoipManagerPipe::SendInitVoipSystem( int aiVasDevice,  int aiToIncludeSoundDevice, const wchar_t *alpcDeviceName, const wchar_t *alpcOutDeviceName)
{
	CInitVoipSystemOut packetOut( CMD_INITVOIPSYSTEM, aiVasDevice, aiToIncludeSoundDevice, alpcDeviceName, alpcOutDeviceName);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendSetVoiceGainLevel( float aVal)
{
	CSetVoiceGainLevelOut packetOut( CMD_SETVOICEGAINLEVEL, aVal);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendHandleTransmissionStateChanged( bool aMicrophoneEnabled, bool aVoiceActivate)
{
	CHandleTransmissionStateChangedOut packetOut( CMD_TRANSMISSIONSTATE, aMicrophoneEnabled, aVoiceActivate);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendSetVoiceActivationLevel( float aVal)
{
	CSetVoiceActivationLevelOut packetOut( CMD_SETVOICEACTIVATIONLEVEL, aVal);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendSetAutoGaneControlEnabled( bool abEnabled)
{
	CSetAutoGaneControlEnabledOut packetOut( CMD_SETAUTOGANECONTROL, abEnabled);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendEnableDenoising( bool abEnabled)
{
	CSetEnableDenoisingOut packetOut( CMD_SETDENOISING, abEnabled);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendSetUserGainLevel( const wchar_t* alwpcUserLogin, double adUserGainLevel)
{
	CSetUserGainLevelOut packetOut( CMD_SETUSERGAINLEVEL, alwpcUserLogin, adUserGainLevel);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendConnectImpl(  const char *apwServerIP, std::vector<IProxySettings*>* aProxySettingsVec, int aTcpPort, int anUdpPort, bool aCheckConnecting)
{
	CConnectToEVoipOut packetOut( CMD_CONNECTTOEVOIP, apwServerIP, aProxySettingsVec, aTcpPort, anUdpPort, aCheckConnecting);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendJoinChannel( const char *aRoomName)
{
	CJoinChannelOut packetOut( CMD_JOINCHANNEL, aRoomName);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendDisconnect()
{
	CDisconnectOut packetOut( CMD_DISCONNECTEVOIP);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendDoLogin( const wchar_t *aUserName, const wchar_t *aUserPassword, const wchar_t *aRoomName)
{
	CDoLoginOut packetOut( CMD_DOLOGIN, aUserName, aUserPassword, aRoomName);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

/*void CVoipManagerPipe::SendGetUser( int aMsgID, int aUserID)
{
	MP_NEW_P3( packetOut, CGetUserOut, CMD_GETUSER, aMsgID, aUserID);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}*/

void CVoipManagerPipe::SendSetUserVolume( const wchar_t *aUserName, int aVolume)
{
	CSetUserVolumeOut packetOut( CMD_SETUSERVOLUME, aUserName, aVolume);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendIncomingVoiceVolume( int aVolume)
{
	//чтобы не создавать еще 1 класс пакета
	CSetVoiceGainLevelOut packetOut( CMD_SETMASTERVOLUME, (float)aVolume);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendEchoCancellation( bool anEnabled)
{
	//чтобы не создавать еще 1 класс пакета
	CSetEnableDenoisingOut packetOut( CMD_SETECHOCANCELLATION, anEnabled);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendMixerEchoCancellation( bool anEnabled)
{
	//чтобы не создавать еще 1 класс пакета
	CSetEnableDenoisingOut packetOut( CMD_SETMIXERECHOCANCELLATION, anEnabled);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendStartFindRealMicrophon( )
{
	CCommonPipePacketOut packetOut( CMD_STARTFINDMICDEVICE);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::SendEndFindRealMicrophon( )
{
	CCommonPipePacketOut packetOut( CMD_ENDFINDMIVDEVICE);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipe::CloseVoipManServer()
{
	WriteLog( "[VOIP_PIPE] CloseVoipManServer");
	bRunningCheckTime = false;
	CCommonPipePacketOut packetOut( CMD_CLOSEVOIPMANSERVER);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

/*CDataStorage* CVoipManagerPipe::GetIdleData()
{
	MP_NEW_P(idleData, CDataStorage, 4);
	BYTE idleByte = CMD_IDLE;
	idleData->Add(idleByte);

	return idleData;
}*/