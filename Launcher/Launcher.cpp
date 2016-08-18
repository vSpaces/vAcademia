// Launcher.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <fstream>
#include "Launcher.h"
#include "Win7ApplicationIDFix.h"
#include <Shlwapi.h>
#include <mmsystem.h>
#include <stdio.h>
#include <Psapi.h>
#include "DialogError.h"
#include "ProcessFinder.h"
#include "mdump.h"
#include "mdump/dump.h"
#include "func.h"
#include "splashImage.h"
#include <time.h>
#include "ClientSessionStateSender.h"
#include "SocketConnectionTester.h"
#include <windows.h>
#include <winuser.h>
#include "wtsapi32.h"
#include "StartUpdater.h"
#include "LoadUpdater.h"
#include <io.h>
#include <ShlObj.h>
#include "launchererrors.h"
#include "../PlayerLib/notifies.h"
#include "FFMpeg_shield.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wtsapi32.lib")

#define MAX_LOADSTRING 100
#define MAX_TRY_RESTART 3

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HANDLE	hLauncherEvent = NULL;
HANDLE	hOMSPlayerProcessHandle = NULL;
HANDLE	hPlayerCreatingEvent = NULL; 
HBITMAP hBitmapSplash = 0;

HBRUSH hBrush;
HBRUSH hBrushOutline;

HANDLE g_hangMemoryObject = NULL;

//#define ANIMATED_ONEDIRECTION
HWND		hSplashWindow = 0;
UINT_PTR	hTimer = 0;
UINT_PTR	hDotsTimer = 0;

bool			processLaunched = false;
bool			processIsClosing = false;
bool			fadeInProgress = false;
bool			processHang = false;
bool			userActWait = false;
float			fadeOutDuration = 1000.0f;
unsigned long	startFadeOutTime = 0;
unsigned long	startClosingTime = 0;
unsigned long	startResponseTime = 0;
unsigned long	startAdditionalResponseTime = 0;
unsigned long	mdumpTimeExpired = 53000;
unsigned long	responseTimeExpired = 10000;
unsigned long	appHangTimeExpired = 63000;
volatile unsigned int	loadingPercent = 0;
volatile bool bGetPercentMsg = false;
CDialogError	dlgError;
CClientSessionStateSender clntSession;
PROCESS_INFORMATION playerProcessInfo = { 0 };
CSplashImage	*g_pSplashImage = NULL;
WCHAR			sRoot[2 * MAX_PATH];

long _sessionID = 0;
unsigned short session1 = 0;
unsigned short session2 = 0;

int	_internalState = -1;

unsigned long	timer = 0;

bool			 isPlayerNotRunninigStatusSend = false;
bool			 isPlayerAlreadyStartUp = false;
bool			 withProtocolaunchCmdLine = false;
int				 glIsEncodingPassword = 0;
std::wstring	 glPassword = L"";
std::wstring	 glLogin = L"";
std::wstring	 glLogPath = L"";
std::wstring	 glComment = L"";

bool isLiveTimerUsed = false;
int liveTimerStartTime = -1;


// Статусы запроса по сокету к серверу из player.ini по порту 13333
// 0 - запрос не послан
// 1 - connect вызван
// 2 - connect завершен
unsigned int			gSocketConnectionStatus = 0;
unsigned int			gSocketConnectionErrorCode = 0;
CSocketConnectionTester socketConnectionTester;
unsigned int			restartCnt = 0;
bool					isAlreadyShowRestartMess = false;
bool					isNeedToRestart = true;


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int, LPTSTR);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void SetSplashImage(HWND hwndSplash, HBITMAP hbmpSplash, float alpha);
void SendLogFile();
void SendStatusFromFile();
void Restart(bool isNeedToAutoLogin = true);
std::string GetLogin();
std::string GetClientVersion();
std::string GetClientLang();
bool IsClosedNotFromTaskManager();
CLoadUpdater getVersion;

std::wstring GetApplicationDataDirectory()
{
	wchar_t tmp[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}

void MakeSleep(CClientSessionStateSender* clntSession)
{
	Sleep(30000);
	if (!clntSession->IsLogWriteSuccess())
	{
		for (int i = 0; i < 60; i++)
		{
			Sleep(1000);
			if (clntSession->IsLogWriteSuccess())
			{
				break;
			}
		}
	}
}

std::wstring GetLogPath()
{
	return glLogPath;
}

void GetTempFileName( std::string &sTempFile)
{
	char sz[2024];
	GetTempPath(MAX_PATH, sz); // Получаем путь к TEMP-папке
	DWORD dwProcessId = GetCurrentProcessId();
	char ff[7];
	_itoa(dwProcessId, ff, 10);
	sTempFile = sz; sTempFile += "vacademia_"; sTempFile += ff; sTempFile += ".tmp";
}

bool IsReallyHang()
{
	std::wstring path = GetApplicationDataDirectory();
	path += L"\\start_video_capture";

	FILE* fl = _wfopen(path.c_str(), L"r");
	if (fl)
	{
		fclose(fl);
		return false;
	}

	if (!g_hangMemoryObject)
	{
		g_hangMemoryObject = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4, L"vAcademiaHangCheck");

		if (!g_hangMemoryObject)
		{			
			return true;
		}
	}
	
	unsigned int* mem = (unsigned int *)MapViewOfFile(g_hangMemoryObject, FILE_MAP_ALL_ACCESS, 0, 0, 4);							
	if (mem == NULL) 
	{					
		return false; 
	}

	bool res = false;

	if (GetTickCount() - *mem > 45000)
	{		
		res = true;

		if (*mem == 0x77777777)
		{
			res = false;
		}
	}
	else
	{
		res = false;
	}

	UnmapViewOfFile(mem);	

	return res;
}

void SetInternalState(int state)
{
	_internalState = state;
	bool isLoginSuccess = (state != -1) && (state != LOGIN_WND_NOT_CREATED_STATUS);
	clntSession.SetLoginSuccess(isLoginSuccess);
	if (isLoginSuccess)
	{
		isLiveTimerUsed = true;		
		liveTimerStartTime = GetTickCount();
	}
}

int GetInternalState()
{
	return _internalState;
}

HWND parentProcessHWND = NULL;
BOOL CALLBACK CheckWindow(HWND hwnd, LPARAM lParam)
{
	DWORD dwWinProcID = 0;
	GetWindowThreadProcessId(hwnd, &dwWinProcID);
	if (dwWinProcID == (DWORD)lParam)
	{
		TCHAR   className[MAX_PATH+1];
		if( GetClassName( hwnd, className, MAX_PATH) > 0)
		{
			if( _tcscmp( className, "LAUNCHER") == 0)
			{
				parentProcessHWND = hwnd;		
			}
		}
	}
	return TRUE;
}

void SendMessageToParentProcess(unsigned int parentProcessID)
{
	parentProcessHWND = NULL;
	EnumWindows(CheckWindow, (LPARAM)parentProcessID);
	if (parentProcessHWND != NULL)
	{		
		SendMessage(parentProcessHWND, WM_I_WANT_TO_KILL_VACADEMIA, 0, 0);
	}
}

void DelDir(std::wstring sDir)
{
	WIN32_FIND_DATAW fileData;
	std::wstring filePath = sDir + L"*.*";
	HANDLE	hSearch =  FindFirstFileW(filePath.c_str(), &fileData); 

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
				std::wstring s = sDir + fileData.cFileName;
				s+=L"\\";
				DelDir(s);
				bContinueSearch = FindNextFileW(hSearch, &fileData);
				::RemoveDirectoryW( s.c_str());
				continue;
			}

			std::wstring s = sDir+fileData.cFileName;
			bContinueSearch = FindNextFileW(hSearch, &fileData);

			::SetFileAttributesW( s.c_str(), FILE_ATTRIBUTE_NORMAL);
			::DeleteFileW( s.c_str());
			int code = GetLastError();
			code+=0;
		}
	}
	FindClose(hSearch);
}

void DeleteUpdaterTempCatIfNeeded()
{
	std::wstring updaterPath = GetApplicationDataDirectory();
	updaterPath +=L"\\updater_path.txt";

	FILE* fl = _wfopen(updaterPath.c_str() ,L"r");

	if (fl)
	{
		fseek(fl, 0, SEEK_END);
		int size = ftell(fl); 
		fseek(fl, 0, SEEK_SET);

		if( size != 0)
		{
			wchar_t *spPath = MP_NEW_ARR( wchar_t, size);
			ZeroMemory( spPath, size*sizeof(wchar_t));
			fread( &spPath[0], 1, size, fl);
			DelDir(spPath);
			RemoveDirectoryW(spPath);			
		}
	}

	if (fl)	
	{
		fclose(fl);
		DeleteFileW(updaterPath.c_str());
	}
}

void  ShowUpdateErrorMessage()
{
	std::string mess = "";
	std::string title = "";
	if ( clntSession.GetPropertyFromPlayerINI("settings","language") == "eng")
	{
		mess = "Administrator rights are required for update of vAcademia. Please, re-login in Windows as administator and try again.";
		title = "Update error";
	}
	else
	{
		mess = "У вас не хватает прав для выполнения обновления программы. Выполните вход в компьютер с правами администратора.";
		title = "Ошибка обновления";
	}

	MessageBox(0, mess.c_str(), title.c_str() , MB_OK);
}


void StartUpdater(std::string sCommand, std::wstring rootDir)
{
	std::wstring appPath = GetApplicationDataDirectory();

	CStartUpdater* stUpdate = new CStartUpdater();

	std::wstring updaterPath;

	unsigned int errorCode = 0;
	unsigned int systemErrorCode = 0;
	updaterPath = stUpdate->prepareUpdate(rootDir, appPath, errorCode, systemErrorCode);

	std::wstring updaterFileName = updaterPath + L"updater.exe";

	if( errorCode == LAUNCH_ERROR_NO_ERROR)
	{
		STARTUPINFOW si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_FORCEONFEEDBACK;
		si.wShowWindow = SW_SHOW;

		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		wchar_t cmd_line[1024] = {0};
		USES_CONVERSION;
		lstrcpynW(cmd_line, A2W(sCommand.c_str()), sizeof(cmd_line)/sizeof(cmd_line[0]));

		BOOL res = CreateProcessW( updaterFileName.c_str()
							, cmd_line
							, NULL
							, NULL
							, FALSE
							, CREATE_NO_WINDOW
							, NULL
							, updaterPath.c_str()
							, &si, &pi);

		int errorCode = GetLastError();
		if(res)
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		else
		{
			ShowUpdateErrorMessage();
			clntSession.SendStatus(_sessionID,UPDATER_NOT_RUNNING_PROCESS_STATUS, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());

		}
	}
	else
	{
		// ??
		ShowUpdateErrorMessage();
		clntSession.SendStatus(_sessionID,UPDATER_NOT_RUNNING_STATUS, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
	}
}

BOOL IsCorrectUpdate(std::wstring rootDir)
{
	std::wstring filePath = rootDir;
	filePath += L"updater_fail_start.txt";

	FILE* fl = _wfopen(filePath.c_str() ,L"r");

	if (fl)
	{
		fseek(fl, 0, SEEK_END);
		int size = ftell(fl); 
		fseek(fl, 0, SEEK_SET);

		if( size != 0)
		{
			std::string sCommand;
			sCommand.resize(size);
			fread(&sCommand[0], 1, size, fl);
			std::string mess = "";
			std::string title = "";

			if (sCommand.find("-try=\"1\"") != std::string::npos)
			{
				//Если апдейтер не смог обновиться 2 раза подряд - показываем только окно со ссылкой на скачивание версии и очень извиняемся ))

				/*std::string website = "http://" + clntSession.GetWorldServer()+ "/site/downloadclient";

				if ( clntSession.GetPropertyFromPlayerINI("settings","language") == "eng")
				{
					mess = "К сожалению, в процессе обновления произошла критическая ошибка.\nРабота \"Виртуальной Академии\" будет невозможна, если не загрузить и установить новую версию программы. Приносим свои извинения.\nМы можем скачать новую версию за Вас или Вы можете сами скачать ее с сайта.\nВыберите \"Да\" для автоматического скачивания или \"Нет\" для перехода на сайт.";
					title = "Information";
				}
				else
				{
					mess = "К сожалению, в процессе обновления произошла критическая ошибка.\nРабота \"Виртуальной Академии\" будет невозможна, если не загрузить и установить новую версию программы. Приносим свои извинения.\nМы можем скачать новую версию за Вас или Вы можете сами скачать ее с сайта.\nВыберите \"Да\" для автоматического скачивания или \"Нет\" для перехода на сайт.";
					title = "Информация";
				}

				if (MessageBox(0, mess.c_str(), title.c_str(), MB_YESNO | MB_ICONQUESTION) == IDNO)
				{
					HINSTANCE  res = ShellExecute( 0, "open", website.c_str(), 0, 0, SW_SHOWNORMAL);
					if( (int)res <= 32)
					{
						res = ShellExecute( 0, "open", "iexplore" , website.c_str(), 0, SW_SHOWNORMAL);	
					}
				}
				else
				{
					sCommand +="-setup=\"1\"";
					StartUpdater(sCommand, rootDir);
				}
					
				fclose(fl);
				return FALSE;*/
			}

			// если в процесс обновления произошла критическая ошибка и билд уже сломан, то скачиваем сетап
			// иначе обновляем еще раз
			if (sCommand.find("-broken \"1\"") != std::string::npos)
			{
				sCommand +=" -setup \"1\"";
			}
			
			/*if ( clntSession.GetPropertyFromPlayerINI("settings","language") == "eng")
			{
				mess = "К сожалению, предыдущее обновление завершилось неудачно.\nВы можете попробовать обновить клиент еще раз или сразу скачать новую версию.\nВыберите 'Yes' для повторного обновления или 'No' для скачивания новой версии.";
				title = "Information";
			}
			else
			{
				mess = "К сожалению, предыдущее обновление завершилось неудачно.\nВы можете попробовать обновить клиент еще раз или сразу скачать новую версию.\nВыберите \"Да\" для повторного обновления или \"Нет\" для скачивания новой версии.";
				title = "Информация";
			}

			if (MessageBox(0, mess.c_str(), title.c_str(), MB_YESNO| MB_ICONQUESTION) == IDNO)
			{
				sCommand +="-setup=\"1\"";
			}*/

			sCommand += " -version \"";
			sCommand += GetClientVersion()+"\"";
			sCommand += " -language \"";
			sCommand += GetClientLang()+"\"";
			sCommand += " /Lang=";
			sCommand += GetClientLang();

			StartUpdater(sCommand, rootDir);

			fclose(fl);
			return FALSE;
			
		}

		fclose(fl);
	}

	return TRUE;
}


//объявляем буфер, для хранения возможной ошибки, размер определяется в самой библиотеке
static char errorBuffer[CURL_ERROR_SIZE];
//объялвяем буфер принимаемых данных
static std::string buffer;

LONG WINAPI	LauncherFilterFunction( struct _EXCEPTION_POINTERS *pExceptionInfo );

LONG WINAPI LauncherFilterFunction( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	CoInitialize(NULL);

	LONG result = MDumpFilterFunction( pExceptionInfo, APP_LAUNCHER_ERROR_ID);
	if( result == EXCEPTION_VA_RUNTIME_MODULE)
	{		
		result = EXCEPTION_CONTINUE_SEARCH;
	}
	else
	{
		rtl_TerminateMyProcess( 1);
		CoUninitialize();
	}

	return result;
}

class CTestFailed
{
public:
	CTestFailed(){ x = 0;};
	~CTestFailed(){};
	void run(){ x++;};
	int x;
};

#include "CrashHandler.h"
CCrashHandler ch;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//StartUpdater("-root ../../../vu2208", "D:\\Program Files\\Virtual Academia\\");
	// Установим обработчик событий после загрузки всех модулей. Это нужно чтобы краши отправлялись стабильнее.
	// В дальнейшем после загрузки основных модулей поставим его еще раз.
	//StartExceptionHandler( &LauncherFilterFunction);
	//StartExceptionHandlers();
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();

	if ( clntSession.GetPropertyFromPlayerINI("settings", "buildtype") == "base")
	{
		std::wstring path = GetApplicationDataDirectory();
		path += L"\\new_va_ip";

		FILE* fl = _wfopen(path.c_str(), L"r");
		if (fl)
		{
			char buf[100];
			buf[0] = 0;
			fgets(buf, 100, fl);
			fclose(fl);
			std::string ip = buf;

			if ( clntSession.GetPropertyFromPlayerINI("paths","server") != ip)
			{
				if (ip.size() >= 7 && ip.size() <= 15)
				{
					bool isCorrect = true;
					for (int i = 0; i < ip.size(); i++)
					if ((ip[i] != '.') && ((ip[i] < '0') || (ip[i] > '9')))
					{
						isCorrect = false;
					}

					if (isCorrect)
					{
						clntSession.SetPropertyFromPlayerINI("paths", "server", ip);
					}
				}
			}
		}
	}

	// НАЧАЛО
	getVersion.SendVersion();
	
	//CTestFailed *p = new CTestFailed();
	//delete p;
	//p = NULL;
	//p->run();

	//MessageBox(0, "Запускаем Launcher", "Информация", MB_OK | MB_ICONINFORMATION);
	/* CURL *curl; 
	CURLcode result;  
	curl = curl_easy_init(); 
	std::string buffer;

	if(curl) 
	{  
		std::string version = clntSession.GetPropertyFromPlayerINI("settings", "version");
		std::string server = clntSession.GetPropertyFromPlayerINI("paths", "server");
		std::string url = "";
		if (server !="")
		 url = "http://" + server  +"/vu/php_0_1/GetClientVersion.php?version=" + version;
		
		//задаем все необходимые опции
		//определяем, куда выводить ошибки
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		//задаем опцию - получить страницу по адресу http://...
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		//указываем прокси сервер, если надо
	
		//curl_easy_setopt(curl, CURLOPT_PROXY, "192.168.0.62:80");

		//задаем опцию отображение заголовка страницы
		//curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		//указываем функцию обратного вызова для записи получаемых данных
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		//указываем куда записывать принимаемые данные
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		//запускаем выполнение задачи
		result = curl_easy_perform(curl);
		//проверяем успешность выполнения операции
		if (result == CURLE_OK)     
		{
			if (buffer.find("version:")!=-1 && buffer.find("update:")!=-1)
			{
				int pos = buffer.find("update:");
				std::string status = buffer.substr(pos + 7, 1);
			}
		}
	} */

	//MessageBox(0, "Запускаем Launcher", "Информация", MB_OK | MB_ICONINFORMATION);
	//-protocolaunch vacademia:///?x=-44780.0&y=26440.9&z=573.7&rn=42677&ra=0&rx=0&ry=0&rz=0&anonymousStart=1
	/* char dateStr [9];
	char timeStr [9];
	_strdate( dateStr);
	_strtime( timeStr );

	std::string fileName =timeStr;
	fileName =  "c:\\log_launcher"+fileName + ".txt";

	int ps = 15;

	while((ps = fileName.find(":",ps)) != std::string::npos){
		fileName.replace(ps, 1, "_");
	}

	FILE* fl = fopen(fileName.c_str(), "w");

	if (fl)
	{
		fprintf( fl,"Date: %s \n", dateStr);
		fprintf( fl,"Time: %s \n", timeStr);

		if (lpCmdLine == NULL)
		{
			fprintf( fl,"lpCmdLine == NULL \n");
		}
		else
		{
			std::string str = lpCmdLine;
			if (str == "")
			{
				int  ii = 0;
			}
			fprintf( fl,"lpCmdLine = %s\n", lpCmdLine);
		}
	}*/

	/*RegisterPowerSettingNotification(hSplashWindow, GUID_ACDC_POWER_SOURCE, 0);
	RegisterPowerSettingNotification(hSplashWindow, GUID_BATTERY_PERCENTAGE_REMAINING, 0);
	RegisterPowerSettingNotification(hSplashWindow, GUID_GLOBAL_USER_PRESENCE, 0);
	RegisterPowerSettingNotification(hSplashWindow, GUID_IDLE_BACKGROUND_TASK, 0);
	RegisterPowerSettingNotification(hSplashWindow, GUID_MONITOR_POWER_ON, 0);
	RegisterPowerSettingNotification(hSplashWindow, GUID_POWERSCHEME_PERSONALITY, 0);
	RegisterPowerSettingNotification(hSplashWindow, GUID_SESSION_DISPLAY_STATUS, 0);
	RegisterPowerSettingNotification(hSplashWindow, GUID_SESSION_USER_PRESENCE, 0);
	RegisterPowerSettingNotification(hSplashWindow, GUID_SYSTEM_AWAYMODE, 0);*/
	WTSRegisterSessionNotification(hSplashWindow, NOTIFY_FOR_THIS_SESSION);


	std::string sCommand = lpCmdLine;

	int posR = sCommand.find("-restart ");

	if (posR !=-1 )
	{
		restartCnt = rtl_atoi(sCommand.substr(posR+9, posR+10).c_str());
	}

	if (restartCnt >= MAX_TRY_RESTART)
	{
		if (isAlreadyShowRestartMess || restartCnt >MAX_TRY_RESTART)
			return 0;

		std::string mess;
		std::string title;

		if ( clntSession.GetPropertyFromPlayerINI("settings","language") == "eng")
		{

			mess = "Unfortunately, \"Virtual Academia\" can\'t be started due to fatal internal error.\nPlease, download the new version from www.vacademia.com \nor contact developers support@vacademia.com.";
			title = "Information";
		}
		else
		{
			mess = "К сожалению, \"Виртуальная Академия\" не может быть запущена вследствие критической ошибки.\nПожалуйста, скачайте новую версию с сайта www.vacademia.com\nили обратитесь к разработчикам support@vacademia.com.";
			title = "Информация";
		}

		isAlreadyShowRestartMess = true;
		if (MessageBox(NULL, mess.c_str(), title.c_str(), MB_OK|MB_TASKMODAL) == IDOK)
		{
			return 0;
		}	
	}

	std::vector<HWND> windowHandles;
	std::vector<unsigned int> parentProcessIDs;

	// get folder of the current process
	wchar_t szPlayerDir[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szPlayerDir, MAX_PATH);
	PathRemoveFileSpecW(szPlayerDir);


	std::wstring rootDir = szPlayerDir;
	rootDir+=L"\\";


	// Если мы обновлялись, то надо удалить временную папку с апдейтером, путь в файле updater_path.txt в App
	DeleteUpdaterTempCatIfNeeded();

	// Если предыдущий апдейт не завершился (смотрим файл updater_fail_start.txt в папке клиента), то вместо запуска плеера запускаем апдейтер.
	if (!IsCorrectUpdate(rootDir))
		return 0 ;

	// Если найдем запущенную копию плеера, то просто перешлем ей параметры
	if( lpCmdLine != NULL && strstr(lpCmdLine, "-protocolaunch") != NULL)
	{
		withProtocolaunchCmdLine = true;	
		
		CProcessWindowFinder	finder;		
		std::vector<SFindedProcess> processes = finder.GetPlayerWindow( "player.exe");
		for (int m = 0; m < processes.size(); m++)
		{
			HWND hWnd = processes[m].window;
			unsigned int parentProcessID = processes[m].parentProcessID;
			WINDOWPLACEMENT wndpl; wndpl.length = sizeof(WINDOWPLACEMENT);

			if ( IsWindowVisible(hWnd) && !IsHungAppWindow(hWnd) && GetWindowPlacement(hWnd, &wndpl) && wndpl.showCmd != SW_HIDE)
			{
				COPYDATASTRUCT ds;
				ds.dwData = 2;
				ds.cbData = (DWORD)strlen( lpCmdLine);
				ds.lpData = lpCmdLine;
				SendMessage( hWnd, WM_COPYDATA, 2, (LPARAM)(LPVOID) &ds);

				if (IsIconic(hWnd))
				{
					ShowWindow(hWnd, SW_RESTORE);
				}

				SetForegroundWindow(hWnd);

				return 0;
			}
			else
			{
				windowHandles.push_back(hWnd);
				parentProcessIDs.push_back(parentProcessID);
			}
		}
	}
	else
	{
		CProcessWindowFinder	finder;
		std::vector<SFindedProcess> processes = finder.GetPlayerWindow( "player.exe");
		for (int m = 0; m < processes.size(); m++)
		{
			HWND hWnd = processes[m].window;
			unsigned int parentProcessID = processes[m].parentProcessID;
		
			WINDOWPLACEMENT wndpl; wndpl.length = sizeof(WINDOWPLACEMENT);

			if ( IsWindowVisible(hWnd) && !IsHungAppWindow(hWnd) && GetWindowPlacement(hWnd, &wndpl) && wndpl.showCmd != SW_HIDE)
			{
				if (IsIconic(hWnd))
				{
					ShowWindow(hWnd, SW_RESTORE);
				}

				SetForegroundWindow(hWnd);

				return 0;
			}
			else
			{
				windowHandles.push_back(hWnd);
				parentProcessIDs.push_back(parentProcessID);
			}
		}
	}

	hPlayerCreatingEvent = CreateEvent( NULL, TRUE, FALSE, "VirtualAcademiaPlayerEvent");
	DWORD error = GetLastError();

	if ((error == ERROR_ALREADY_EXISTS) || (hPlayerCreatingEvent == NULL))
	{
		isPlayerAlreadyStartUp = true;
		return NULL;
	}

	for (int j = 0; j < windowHandles.size(); j++)
	{
		// уведомление parentProcessID об убийстве
		SendMessageToParentProcess(parentProcessIDs[j]);
	}

	Sleep(500);
				
	for (int j = 0; j < windowHandles.size(); j++)
	{
		DWORD dwWinProcID = 0;

		GetWindowThreadProcessId(windowHandles[j], &dwWinProcID);

		ShowWindow(windowHandles[j], SW_HIDE);

		HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, 0, dwWinProcID);

		if (processHandle != NULL)
		{
			rtl_TerminateOtherProcess( processHandle, 0);
					
			CloseHandle(processHandle);
		}
	}

	

	socketConnectionTester.StartSocketConnection();
	
	//if (fl)
	//	fclose(fl);

	//////////////////////////////////////////////////////////////////////////	
	//HWND hwndvAcademia = FindWindow( szWindowClass, NULL);
	/*DWORD dwWinProcID = 0;
	if( hwndvAcademia == NULL)
		GetWindowThreadProcessId(hwndvAcademia, &dwWinProcID);	*/
	//////////////////////////////////////////////////////////////////////////

	MSG msg;
	HACCEL hAccelTable;

	SetAppUserModelId();

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LAUNCHER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	//hBrushOutline = CreateSolidBrush(RGB(7,163,222));
	//hBrush = CreateSolidBrush( RGB(255,255,255));

	TCHAR *serverName = new TCHAR[MAX_PATH];
	TCHAR *appVersion = new TCHAR[MAX_PATH];
	sRoot[ 0] = '\0';
	
	USES_CONVERSION;
	std::wstring s = A2W(lpCmdLine);
	int pos = s.find( L"-root ");
	if ( pos > -1)
	{
		s = s.substr( pos + 6, s.length());
		pos = s.find( L" ");
		int q_pos1 = s.find( L"\"");
		int q_pos2 = s.find( L"\"", q_pos1 + 1);
		while( pos > q_pos1 && pos < q_pos2)
		{
			pos = s.find( L" ", pos + 1);
		}
		if ( pos > -1)
			s = s.substr( q_pos1 > -1 ? q_pos1 + 1 : 0, (q_pos2 > -1 && q_pos2 < pos) ? q_pos2 - 1 : pos);
		wcscpy( sRoot, s.c_str());
	}
	else
	{		
		GetModuleFileNameW(GetModuleHandleW(NULL), sRoot, 2 * MAX_PATH);
		PathRemoveFileSpecW(sRoot);
	}

	g_pSplashImage = new CSplashImage();
	g_pSplashImage->m_iLanguage = ENG;
	g_pSplashImage->SetRootPath( sRoot);
	getAppParams( sRoot, &serverName, &appVersion, &g_pSplashImage->m_iLanguage, NULL);
	SetAppVersionMDump( appVersion);
	dlgError.SetAppLanguage( g_pSplashImage->m_iLanguage);
	delete[] appVersion;
	delete[] serverName;

	//////////////////////////////////////////////////////////////////////////	
	/*if( hwndvAcademia == NULL)
	{		
		WIN32_FIND_DATA fileData;
		char sz[2024];
		GetTempPath(MAX_PATH, sz); // Получаем путь к TEMP-папке
		std::string sFiltr = sz; sFiltr += "vacademia_*.tmp";
		HANDLE hSearch = FindFirstFile(sFiltr.c_str(), &fileData); 
		if (hSearch != INVALID_HANDLE_VALUE )
		{
			BOOL bContinueSearch = TRUE;	
			//bool bShowMessage = true;
			while (bContinueSearch)
			{
				if (strcmp(fileData.cFileName, ".") == 0 || strcmp(fileData.cFileName, "..") == 0)
				{
					bContinueSearch = FindNextFile(hSearch, &fileData);
					continue;
				}			
				std::string sProcessID = fileData.cFileName;
				std::string sFilePath = sz; sFilePath += fileData.cFileName;
				bContinueSearch = FindNextFile(hSearch, &fileData);
				::DeleteFile( sFilePath.c_str());		
				sProcessID = sProcessID.substr(10);
				int pos = sProcessID.find(".tmp");
				sProcessID = sProcessID.substr(0, pos);
				int processID = rtl_atoi(sProcessID.c_str());
					//bool bFind = false;
					HANDLE hProcess = OpenProcess(READ_CONTROL, FALSE, processID);
					if( hProcess != NULL)
					{
						char fileName[1024];
						GetModuleFileNameEx( hProcess, NULL, fileName, 1024);
						CloseHandle( hProcess);
						if (strstr(fileName, "vacademia.exe") != NULL)
						{
							bShowMessage = false;
							bFind = true;
						}
					}
					/*if( bShowMessage && !bFind)
					{
						//bContinueSearch = false;
						bShowMessage = false;
						if ( g_pSplashImage->m_iLanguage == ENG)
							MessageBox(0, "The last attempt of an application launch was it is completed incorrectly. Check up Antivirus adjustments.", "Information", MB_OK | MB_ICONINFORMATION);
						else
							MessageBox(0, "Последняя попытка запуска приложения был завершена некорректно. Проверьте настройки Антивируса", "Информация", MB_OK | MB_ICONINFORMATION);
					}*				
				//}
			}
		}
		FindClose(hSearch);	
	}
	DWORD dwProcessId = GetCurrentProcessId();
	char ff[7];
	_itoa(dwProcessId, ff, 10);
	std::string sTempFile;
	GetTempFileName( sTempFile);
	HANDLE hFile = CreateFile((LPCTSTR)sTempFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, (HANDLE)NULL);
	//SetFileAttributes(sTempFile.c_str(), FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_HIDDEN);
	CloseHandle(hFile);	*/
	//////////////////////////////////////////////////////////////////////////

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow, lpCmdLine)) 
	{
		if (!isPlayerAlreadyStartUp && !withProtocolaunchCmdLine)
		{
			if( hSplashWindow != 0)
				ShowWindow(hSplashWindow, SW_HIDE);
			if ( g_pSplashImage->m_iLanguage == ENG)
				MessageBox(0, "Player.exe can’t start up. Probably, vAcademia is incorrectly installed or blocked by antivirus. Reinstall the program please.", "Setup error", MB_OK);
			else
				MessageBox(0, "Процесс player.exe не может быть запущен.\n Возможно vAcademia неправильно установлена или блокируется антивирусом. Переустановите программу или настройте антивирус.", "Ошибка установки", MB_OK);


			if (!isPlayerNotRunninigStatusSend)
			{
				socketConnectionTester.StopSocketConnection();
				clntSession.SendStatus(_sessionID,PLAYER_NOT_RUNNING_STATUS, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
				isPlayerNotRunninigStatusSend = true;
			}

			MakeSleep(&clntSession);
		}
		return FALSE;
	}
	
	// отсылаем статусы, которые по каким-либо причинам не смогли отослаться в предыдущую сессию

	SendStatusFromFile();
	
	// отсылаем логи, которые по каким-либо причинам не смогли отослаться в предыдущую сессию
	SendLogFile();

	///////////////

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LAUNCHER);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (msg.message == WM_SYSCOMMAND) 
		{ 
			if( (msg.wParam & 0xFFF0) == SC_CLOSE)
				continue;	
		}

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if ( hSplashWindow != NULL)
		DestroyWindow( hSplashWindow);

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LAUNCHER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_LAUNCHER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

// Creates the splash owner window and the splash window.
HWND CreateSplashWindow()
{
	HWND hwndOwner = CreateWindow(szWindowClass, NULL, WS_POPUP,
		0, 0, 0, 0, NULL, NULL, hInst, NULL);	
	return CreateWindowEx(WS_EX_LAYERED, szWindowClass, NULL, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
		0, 0, 0, 0, hwndOwner, NULL, hInst, NULL);
}

// Calls UpdateLayeredWindow to set a bitmap (with alpha) as the content of the splash window.
void SetSplashImage(HWND hwndSplash, float alpha)
{	
	if ( g_pSplashImage != NULL)
		g_pSplashImage->SetSplashImage( hwndSplash, alpha, loadingPercent);	
}


HANDLE CreateLauncherEvent(TCHAR * szLaunchEventName)
{
	TCHAR mutexName[MAX_PATH];
	TCHAR* mutexInitName = "VirtualAcademiaLauncherEvent";
	TCHAR strNum[MAX_PATH];
	HANDLE hEvent = NULL;
	int i = 0;
	DWORD error;
	do
	{
		strcpy(mutexName, mutexInitName);
        itoa(i++, strNum, 10);
		strcat(mutexName, strNum);
		hEvent = CreateEvent( NULL, TRUE, FALSE, mutexName);
		error = GetLastError();
	}
	while ( (error == ERROR_ALREADY_EXISTS || hEvent == NULL) && (i < 10));

	strcpy(szLaunchEventName, mutexName);

	return hEvent;
}

HANDLE LaunchWpfApplication(const TCHAR * szLaunchMutexName, LPCSTR lpCmdLine)
{
	// get folder of the current process
	WCHAR szCurrentFolder[2 * MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szCurrentFolder, MAX_PATH);
	PathRemoveFileSpecW(szCurrentFolder);

	// add the application name to the path
	WCHAR szCommandLine[2 * MAX_PATH] = { 0 };
	wcscat(szCommandLine, L"-launcher ");
	USES_CONVERSION;
	wcscat(szCommandLine, A2W(szLaunchMutexName));
	wcscat(szCommandLine, L" -processID ");
	WCHAR szProcessID[ MAX_PATH];
	_itow( GetCurrentProcessId(), szProcessID, 10);
	wcscat(szCommandLine, szProcessID);
	if (strlen(lpCmdLine) != 0)
	{
		wcscat(szCommandLine, L" ");
		wcscat(szCommandLine, A2W(lpCmdLine));
	}

	WCHAR szExecPath[2 * MAX_PATH] = L"player.exe ";
	wcscat(szExecPath, szCommandLine);

	// start the application
	STARTUPINFOW si = { 0 };
	si.cb = sizeof(si);
	
	if( CreateProcessW(NULL, szExecPath, NULL, NULL, FALSE, 0, NULL, szCurrentFolder, &si, &playerProcessInfo))
		return playerProcessInfo.hProcess;

	//Следящее приложение не смогло запустить player - код=13
	if (!isPlayerNotRunninigStatusSend)
	{
		socketConnectionTester.StopSocketConnection();
		clntSession.SendStatus(_sessionID,PLAYER_NOT_RUNNING_STATUS, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
		isPlayerNotRunninigStatusSend = true;

		if (hPlayerCreatingEvent)
			CloseHandle(hPlayerCreatingEvent);
	}
	else
	{
		/*if(loadingPercent == 0)
			loadingPercent = 1;*/
	}

	int ff = GetLastError();
	return NULL;
	
	//return new HANDLE();	
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, LPTSTR lpCmdLine)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateSplashWindow();

	if (!hWnd)
	{
		return FALSE;
	}


	hSplashWindow = hWnd;
	//SetLayeredWindowAttributes(hSplashWindow, RGB(255, 0, 0), 70, ULW_ALPHA);

	g_pSplashImage->Load();

	SetSplashImage( hWnd, 1);

	SetWindowPos( hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	if ( g_pSplashImage->IsLoaded())
		SetWindowRgn( hWnd, g_pSplashImage->GetRegion(), TRUE);

	char mutexName[MAX_PATH];
	hLauncherEvent = CreateLauncherEvent(&mutexName[0]);


	/*
	Vladimir
	регистрация сообщений для общения между двумя сотрудничающими приложений
	*/
	TCHAR wndMsgName[ MAX_PATH];
	strcpy( wndMsgName, mutexName);
	strcat( wndMsgName, "_player");
	UINT msgId = RegisterWindowMessage( wndMsgName);
	if ( msgId == 0)
	{
		int errID = GetLastError();
		if ( errID > 0)
		{
		}
	}

	hTimer = SetTimer(hWnd, 0, 50, (TIMERPROC) NULL);
	hDotsTimer = SetTimer(hWnd, 1, 100, (TIMERPROC) NULL);
	hOMSPlayerProcessHandle = LaunchWpfApplication(&mutexName[0], lpCmdLine);
	if( hOMSPlayerProcessHandle != 0)
	{
		return TRUE;
	}
	KillTimer( hWnd, hTimer);
	KillTimer( hWnd, hDotsTimer);
	hTimer = -1;
	hDotsTimer = -1;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// Code is written by Vladimir (2011.02.10)

BOOL bSendSignalNeed = TRUE;
HWND hMyPlayerWnd = 0;

BOOL SendSignal2Player( int processId, int msgCode)
{
	if ( hMyPlayerWnd == 0 && processId > 0)
	{
		CProcessWindowFinder processWindowFinder;
		hMyPlayerWnd = processWindowFinder.GetPlayerWindow( processId);
		if ( hMyPlayerWnd != 0)
			return PostMessage( hMyPlayerWnd, WM_LAUNCHER_WM_MESSAGE, GetCurrentProcessId(), msgCode);
	}
	else if ( hMyPlayerWnd != 0)
	{
		return PostMessage( hMyPlayerWnd, WM_LAUNCHER_WM_MESSAGE, GetCurrentProcessId(), msgCode);
	}

	return FALSE;
}

LPCWSTR lpModules[] = {L"EvoVoIP.dll", L"vu.exe", L"ntdll.dll", L"Unknown", L"kernel.dll"};

BOOL skipHangForModule()
{
	CONTEXT context;
	SuspendThread( playerProcessInfo.hThread);	
	int error = ::GetLastError();
	if ( error > 0)
	{
		return FALSE;
	}
	Sleep( 100); // подождем 100 мс, чтобы точно быть уверенным, что поток приостановлен
	context.ContextFlags = CONTEXT_CONTROL;
	BOOL b = GetThreadContext( playerProcessInfo.hThread, &context);
	ResumeThread( playerProcessInfo.hThread);
	error = ::GetLastError();

	MEMORY_BASIC_INFORMATION MemInfo;
	WCHAR szCrashModulePathName[MAX_PATH*2];
	ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));						

	WCHAR *pszCrashModuleFileName = L"Unknown";

	if ( VirtualQueryEx(hOMSPlayerProcessHandle, (void *) context.Eip, &MemInfo, sizeof(MemInfo)))
	{							
		if (GetModuleFileNameExW(hOMSPlayerProcessHandle, (HINSTANCE)MemInfo.AllocationBase,
			szCrashModulePathName,
			sizeof(szCrashModulePathName)-2) > 0)
		{
			pszCrashModuleFileName = GetFilePart(szCrashModulePathName);
		}

		if ( wcscmp( pszCrashModuleFileName, L"Unknown") == 0 || wcscmp( pszCrashModuleFileName, L"") == 0)
		{
			WCHAR		Module_Name[MAX_PATH];
			BYTE		Module_Addr[MAX_PATH];
			// If module with E.ExceptionAddress found - save its path and date.
			if (Get_Module_By_Ret_Addr( playerProcessInfo.dwProcessId, (PBYTE)MemInfo.AllocationBase, Module_Name, Module_Addr))
			{			
				pszCrashModuleFileName = GetFilePart(Module_Name);				
			}
		}
	}

	for ( int i = 0; i < sizeof( lpModules) / sizeof( lpModules[ 0]); i++)
	{
		if ( wcscmp( pszCrashModuleFileName, lpModules[ i]) == 0)
		{			
			return TRUE;
		}
	}

	return FALSE;
}

// end Vladimir
//////////////////////////////////////////////////////////////////////////

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

#include "../cscl3dws/oms/oms_messages.h"
#define APP_BY_VREPORT				1024
#define APP_BY_VREPORT				1024

//BOOL bSessionIDStart = FALSE;
//BOOL bSessionIDEnded = FALSE;
//BOOL bPercentNotifyStart = FALSE;
//BOOL bPercentNotifyEnded = FALSE;

BOOL bPlayerFinishing = FALSE;

void UpdateSession()
{
	unsigned short* a = (unsigned short*)&_sessionID;
	a[0] = session1;
	a[1] = session2;
	setSessionID(_sessionID);			
	//getAppLogin();
}

void OnApplicationGoToSleep()
{
	clntSession.SendStatus(_sessionID, 41, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
	Sleep(15000);
}

void OnWindowsSessionLogOff()
{
	clntSession.SendStatus(_sessionID, 42, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
	Sleep(15000);
}

void OnWindowsSessionLocked()
{
	clntSession.SendStatus(_sessionID, 43, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
	Sleep(15000);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

#define WM_WTSSESSION_CHANGE			0x02B1
#define WTS_SESSION_LOGOFF              0x6
#define WTS_SESSION_LOCK                0x7

	switch (message) 
	{
	case WM_WTSSESSION_CHANGE:
		if (wParam == WTS_SESSION_LOGOFF)
		{
			OnWindowsSessionLogOff();
		}
		else if (wParam == WTS_SESSION_LOCK)
		{
			OnWindowsSessionLocked();
		}
		break;
	case WM_POWERBROADCAST:
		switch (wParam)
		{
			case (PBT_APMBATTERYLOW):
				// to do				
				break;

			case (PBT_APMQUERYSUSPEND):				
				return BROADCAST_QUERY_DENY;
				break;

			case (PBT_APMRESUMECRITICAL):
				OnApplicationGoToSleep();
				break;

			case (PBT_APMSUSPEND):
				OnApplicationGoToSleep();
				break;
		}		
		break;
	case WM_COPYDATA:{
			COPYDATASTRUCT* pCopyData=(COPYDATASTRUCT*)lParam;
			wchar_t* data = (wchar_t*)pCopyData->lpData;
			if (wParam == APP_NOTIFY)
			{
				glComment = data;
			}
			else if (wParam == APP_WORK)
			{
				glPassword = data;
				glIsEncodingPassword = 1;
				//int pos = glPassword.find( L"|");
				//if ( pos > -1)
				//{
				//	glIsEncodingPassword = _wtoi( glPassword.substr( pos + 1, glPassword.length() - pos - 1).c_str());
				//	glPassword = glPassword.substr( 0, pos);
				//}
			}
			else if (wParam == APP_STATUS)
			{
				glLogin = data;
			}
			else
			{
				glLogPath = data;
			}
		}break;
	case WM_I_WANT_TO_KILL_VACADEMIA:
		{
//			isLauncherDestroyedByAnotherLauncher = true;
			bool res = rtl_TerminateMyProcess( 0);
			int ii = 0;
		}
		break;
	case WM_LAUNCHER_WM_MESSAGE:{
		int x = lParam;
		int lId = LOWORD(lParam);
		int lIdH = HIWORD(lParam);
		int wIdH = HIWORD(wParam); 
		int wIdL = LOWORD(wParam);
		int wId = (wIdH << 16) | wIdL;

		if ( lIdH == APP_BY_VREPORT)
		{
			int x = lId;
			int y = 10;
			x = x + y;
			y = y + x;

			isNeedToRestart = false;
		}

		if ( GetProcessId( hOMSPlayerProcessHandle) != wId)
			return 0;

		if ( lIdH == APP_NOTIFY)
		{			
			bSendSignalNeed = TRUE;
			dlgError.CloseDialog();
		}
		else if ( lIdH == APP_FINISHING)
		{
			responseTimeExpired = 60000;
			mdumpTimeExpired = 172000;
			appHangTimeExpired = 180000;
			bSendSignalNeed = TRUE;
			bPlayerFinishing = TRUE;
			dlgError.CloseDialog();
			socketConnectionTester.StopSocketConnection();
			clntSession.SendStatus(_sessionID, 1, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
			bGetPercentMsg = false;
		}
		else if ( lIdH == APP_SESSION)
		{
			session1 = lId;			

			UpdateSession();
		}
		else if ( lIdH == APP_SESSION_2)
		{
			session2 = lId;
			
			UpdateSession();
		}
		else if ( lIdH == APP_PERCENT)
		{
			bGetPercentMsg = true;
			if(loadingPercent < lId)
			{
				loadingPercent = lId;
			}		
			if (lId >= 100)
			{
				if (hPlayerCreatingEvent)
				{
					CloseHandle(hPlayerCreatingEvent);
					hPlayerCreatingEvent = NULL;
				}
				fadeInProgress = true;
			}
		}
		else if ( lIdH == APP_END)
		{
			int errorCode = rtl_TerminateOtherProcess( hOMSPlayerProcessHandle, 0);
			hOMSPlayerProcessHandle = NULL;
			bGetPercentMsg = false;
		}
		else if ( lIdH == APP_WORK)
		{
			if (lId == INCORRECT_EXIT_STATUS)
			{
				socketConnectionTester.StopSocketConnection();
				clntSession.SendStatus(_sessionID, INCORRECT_EXIT_STATUS, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
			}
			else
			{
				SetInternalState(lId);
			}
		}
		else if ( lIdH == APP_STATUS)
		{
			socketConnectionTester.StopSocketConnection();
			clntSession.SendStatus(_sessionID, lId, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion(), (glLogin.size() > 0) && (glPassword.size() > 0));

			if (((lId >= 20) && (lId <= 38)) || ((lId >= 44) && (lId <= 46)))
			{
				MakeSleep(&clntSession);
			}
		}
		else if ( lIdH == APP_CLOSED_BY_OTHER_APP)
		{
			socketConnectionTester.StopSocketConnection();
			clntSession.SendStatus(_sessionID, 39, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
			bGetPercentMsg = false;
		}
		else if ( lIdH == APP_NEED_HIDE)
		{
			if (hSplashWindow!=NULL)
				DestroyWindow(hSplashWindow);
		}		

		}break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		//case IDM_ABOUT:
		//	DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
		//	break;
		//case IDM_EXIT:
		//	DestroyWindow(hWnd);
		//	break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		{
			if(hTimer != -1)
				KillTimer( hWnd, hTimer);
			if(hDotsTimer != -1)
				KillTimer( hWnd, hDotsTimer);
			PostQuitMessage(0);
		}
		break;

	case WM_TIMER:
		{ 
			timer +=50;

			if (isLiveTimerUsed)
			if (GetTickCount() - liveTimerStartTime > 1000 * 60 * 4)
			{
				restartCnt = 0;
			}

			if( wParam == 0)
			{
				if (hLauncherEvent == NULL || hOMSPlayerProcessHandle == NULL)
					exit(0);

				// Проверяем, не запустился ли наш процесс
				if (!processLaunched)
				{
					if (timer == 180000)
					{
						socketConnectionTester.StopSocketConnection();
						clntSession.SendStatus(_sessionID,PLAYER_NOT_RUNNING_LONG_TIME, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
					}
					else
					{
						if ( !bGetPercentMsg && timer > 5000 && loadingPercent < 90)
						{
							loadingPercent = (int)floor(((double)(timer))/5000);
						}
						if( WaitForSingleObject(hLauncherEvent, 0) == WAIT_OBJECT_0)
						{
							::ResetEvent( hLauncherEvent);
							fadeInProgress = true;
							startFadeOutTime = timeGetTime();
							processLaunched = true;
						}
					}
				}
				else
				{
					// Process is going to closing state
					if( WaitForSingleObject(hLauncherEvent, 0) == WAIT_OBJECT_0)
					{
						// wait for 10 seconds
						if( !processIsClosing)
						{
							processIsClosing = true;
							startClosingTime = timeGetTime();
						}
						else
						{
							if( (timeGetTime() - startClosingTime) > 10000/*10 sec*/)
							{
								if (WaitForSingleObject(hOMSPlayerProcessHandle, 0) != WAIT_OBJECT_0)
								{
									rtl_TerminateOtherProcess( hOMSPlayerProcessHandle, 0);
									hOMSPlayerProcessHandle = NULL;
								}
							}
						}
					}
				}

				// Проверяет, не закрылся ли наш процесс
				if (WaitForSingleObject(hOMSPlayerProcessHandle, 0) == WAIT_OBJECT_0)
				{
					KillFFMpeg();

					if ( !processIsClosing && !bPlayerFinishing)
					{
						// перезапускаем академию, если в процессах нет диспетчера задач
						bool isClosedNotFromTaskManager = IsClosedNotFromTaskManager();
						if (GetInternalState() == -1) // отвалилось до создания окна логинации
						{
							socketConnectionTester.StopSocketConnection();
							clntSession.SendStatus(_sessionID,LOGIN_WND_NOT_CREATED_STATUS, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
							
							if (isClosedNotFromTaskManager)
								Restart(false);
						}
						else if (GetInternalState() != LOGIN_PROCESS_FAILED_STATUS) // отвалилось после создания окна, но до успешной логинации
						{
							socketConnectionTester.StopSocketConnection();
							clntSession.SendStatus(_sessionID,LOGIN_PROCESS_FAILED_STATUS, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
							
							if (isClosedNotFromTaskManager)
								Restart(false);
						}
						else if (!userActWait && !processHang)
						{
							socketConnectionTester.StopSocketConnection();
							clntSession.SendStatus(_sessionID,NOT_NORMAL_CLOSED_STATUS, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
							DWORD dwProcessId = GetProcessId( hOMSPlayerProcessHandle);
							wchar_t ff[30];
							_itow(dwProcessId, ff, 10);

							std::wstring path = clntSession.GetLogDirectory() + L"\\run_";
							path +=ff;
							path +=L".log";
							_wremove(path.c_str());

							// перезапуск
							if (isClosedNotFromTaskManager)
								Restart();
						}
						else // непонятная ситуация
						{
							// перезапуск
							if (isClosedNotFromTaskManager)
								Restart();
						}
						
						// процесса нет - перезапустили академию
						// перезапускаем таймер, иначе может уйти пустой дамп на повисание по таймауту
						startResponseTime = timeGetTime();
						startAdditionalResponseTime = startResponseTime;

						MakeSleep(&clntSession); //максимальный sleep 90сек

						if (!isClosedNotFromTaskManager || restartCnt >= MAX_TRY_RESTART)
						{
							// restartCnt >= MAX_TRY_RESTART - академия не перезапустится
							// есть диспетчер задач - перезапуска академии не было
							// перезапускаем таймер, чтобы не было краша с повисанием
							startResponseTime = timeGetTime();
							startAdditionalResponseTime = startResponseTime;
						}
					}
					

					if ( !processIsClosing)
					{
						// процесс завершился не нормально, отправляем дамп
						if ( !userActWait && processHang)
						{
							// процесс до завершения завис
							// отправляем дамп
							startResponseTime = timeGetTime();
							startAdditionalResponseTime = startResponseTime;
							processHang = false;
							send_report();							
						}						
					}

					SAFE_DELETE( g_pSplashImage);
					DestroyWindow( hSplashWindow);
				}

				if ( processLaunched && !fadeInProgress && !processIsClosing)
				{
					if ( bSendSignalNeed)
					{
						BOOL bSucc = SendSignal2Player( GetProcessId( hOMSPlayerProcessHandle), APP_NOTIFY);
						if ( bSucc)
						{
							startResponseTime = timeGetTime();
							startAdditionalResponseTime = startResponseTime;
							processHang = false;
							bSendSignalNeed = FALSE;
						}
					}

					
					if ( !userActWait && !processHang && (timeGetTime() - startAdditionalResponseTime) > responseTimeExpired/*9,5 sec*/ && hOMSPlayerProcessHandle != NULL)
					{
						// процесс не отвечает в течение 10 сек.
						// отправляем снова запрос
						BOOL bSucc = SendSignal2Player( GetProcessId( hOMSPlayerProcessHandle), APP_NOTIFY);
						startAdditionalResponseTime = timeGetTime();
					}
					
					if ( !userActWait && !processHang && (timeGetTime() - startResponseTime) > mdumpTimeExpired/*53 sec*/ && hOMSPlayerProcessHandle != NULL && (IsReallyHang()))
					{		
						//при окошке показыаемом при MAX_TRY_RESTART снимается пустой краш
						if (restartCnt >= MAX_TRY_RESTART)
						{
							// процесс не отвечает в течение 53 сек.
							// создаем дамп

							socketConnectionTester.StopSocketConnection();
							clntSession.SendStatus(_sessionID,MAKE_DUMP_STATUS, GetLogin(), GetLogPath(), gSocketConnectionErrorCode, gSocketConnectionStatus, "", "", glComment, GetClientVersion());
							if ( glPassword.length() > 0)
							{
								USES_CONVERSION;						
								SetUserMDump( (const char *) GetLogin().c_str(), (const char *) W2A( glPassword.c_str()), glIsEncodingPassword);
							}
							make_dump( sRoot, hOMSPlayerProcessHandle, FALSE, APP_HANG_ID);
							processHang = true;
						}
					}
					else if ( !userActWait && processHang && (timeGetTime() - startResponseTime) > appHangTimeExpired/*63 sec*/  && (IsReallyHang()))
					{
						// процесс не отвечает в течение 63 сек.
						// отправляем дамп						
						processHang = false;						
						userActWait = true;

						if ( bPlayerFinishing && skipHangForModule())
						{
							rtl_TerminateOtherProcess( hOMSPlayerProcessHandle, 0);
							hOMSPlayerProcessHandle = NULL;
							bGetPercentMsg = false;
						}
						else
						{
							rtl_TerminateOtherProcess( hOMSPlayerProcessHandle, 0);
							hOMSPlayerProcessHandle = NULL;
							bGetPercentMsg = false;
							if ( glPassword.length() > 0)
							{
								USES_CONVERSION;
								SetUserMDump( (const char *) GetLogin().c_str(), (const char *) W2A( glPassword.c_str()), glIsEncodingPassword);
							}
							if ( send_report())
							{							
							}
							else
							{
								/*
								int errorID = ::GetLastError();
								TCHAR ch[ MAX_PATH];
								if ( g_pSplashImage->m_iLanguage == ENG)
								{
									strcpy( ch, "VacademiaReport.exe can’t start up: error code is ");
									strcat( ch, itoa( errorID, ch, 10));
									strcat( ch, " ; run command ");
									strcat( ch, szCommandLine);
									MessageBox(0, ch, "Error", MB_OK | MB_TOPMOST);
								}
								else
								{
									strcpy( ch, "Программа VacademiaReport.exe не запущена: код ошибки равен ");
									strcat( ch, itoa( errorID, ch, 10));
									strcat( ch, " ; команда запуска: ");
									strcat( ch, szCommandLine);
									MessageBox(0, ch, "Ошибка", MB_OK | MB_TOPMOST);
								}
								*/
							}
						}
						startResponseTime = timeGetTime();
						startAdditionalResponseTime = startResponseTime;
						processHang = false;
						bSendSignalNeed = FALSE;
						userActWait = false;
					}
				}
				
				if( fadeInProgress)
				{
					unsigned long fadeOutTime = timeGetTime() - startFadeOutTime;
					float alpha = fadeOutTime / fadeOutDuration;
					if( alpha < 1)
					{
						SetSplashImage( hSplashWindow, 1 - alpha);
					}
					else
					{
						SetSplashImage( hSplashWindow, 0);

						KillTimer(hSplashWindow, hTimer);
						hTimer = SetTimer(hSplashWindow, 0, 2000, (TIMERPROC) NULL);

						ShowWindow(hSplashWindow, SW_HIDE);
						UpdateWindow(hSplashWindow);

						fadeInProgress = false;
						SAFE_DELETE( g_pSplashImage);
						std::string sTempFile;
						GetTempFileName(sTempFile);
						::DeleteFile( sTempFile.c_str());
					}
				}
				else
					SetSplashImage( hSplashWindow, 1);
			}
			else if( wParam == 1 && g_pSplashImage != NULL)
			{
#ifdef ANIMATED_ONEDIRECTION
				g_pSplashImage->m_animatedDotIndex++;
				if( g_pSplashImage->m_animatedDotIndex == g_pSplashImage->m_animatedDotsCount)
					g_pSplashImage->m_animatedDotIndex = 0;
#else
				if( g_pSplashImage->m_animatedDotsDirectionRight)
				{
					g_pSplashImage->m_animatedDotIndex++;
					if( g_pSplashImage->m_animatedDotIndex == g_pSplashImage->m_animatedDotsCount)
					{
						g_pSplashImage->m_animatedDotsDirectionRight = false;
						g_pSplashImage->m_animatedDotIndex--;
					}
				}
				else
				{
					g_pSplashImage->m_animatedDotIndex --;
					if( g_pSplashImage->m_animatedDotIndex == 0)
					{
						g_pSplashImage->m_animatedDotsDirectionRight = true;
					}
				}
#endif
				InvalidateRect( hSplashWindow, NULL, TRUE);
				UpdateWindow( hSplashWindow);
				
				/*
				if ( loadingPercent > 100)
				{
					SetSplashImage( hSplashWindow, 0);
					ShowWindow(hSplashWindow, SW_HIDE);
					UpdateWindow(hSplashWindow);
					SAFE_DELETE( g_pSplashImage); 
				}
				else
					loadingPercent++;
				//*/
			}
		}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void SendLogFile()
{
	std::wstring logDir = clntSession.GetLogDirectory();
	std::wstring path = logDir + L"\\*.txt";

	WIN32_FIND_DATAW FileInfo;

	HANDLE Search = FindFirstFileW(path.c_str(),&FileInfo);

	if (Search != INVALID_HANDLE_VALUE){
		do{
			std::wstring fileName = FileInfo.cFileName;

			int pos = fileName.find(L".txt");
			if (pos != -1)
			{
				fileName = fileName.substr(0,pos);
			}

			std::wstring logPath = GetApplicationDataDirectory();

			logPath += L"\\Vacademia\\Log\\" + fileName;

			CClientSessionStateSender* loc_sender = new CClientSessionStateSender();
			loc_sender->CreateUploadThread(logPath);

		}while (FindNextFileW(Search, &FileInfo));

		FindClose(Search);
	}
}

void SendStatusFromFile()
{
	std::wstring logDir = clntSession.GetLogDirectory();
	std::wstring path = logDir + L"\\*.log";

	WIN32_FIND_DATAW FileInfo;

	HANDLE Search = FindFirstFileW(path.c_str(),&FileInfo);

	if (Search != INVALID_HANDLE_VALUE){
		do{
			std::wstring fileName = FileInfo.cFileName;
			std::wstring filePath = logDir + L"\\" + fileName;
			FILE* fl = _wfopen(filePath.c_str(), L"r");
			if (fl)
			{
				if (FileInfo.nFileSizeLow == 0)
				{
					fclose(fl);
					_wremove(filePath.c_str());
				}
				else
				{
					DWORD dwProcessId = -1;
					int sesID = -1; 
					int status = -1;
					char allProcess[16384];
					char login[500];

					if ( fileName.find(L"run_")!=-1)
					{
						fscanf(fl, "%i\n", &dwProcessId);
						fscanf(fl, "%i\n", &sesID); 
						fscanf(fl, "%s\n", &login);

						HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_TERMINATE, 0, dwProcessId);

						TCHAR processName[MAX_PATH];
						DWORD code; 

						if (processHandle)
						{
							GetProcessImageFileName( processHandle, processName, sizeof(processName)/sizeof(*processName));
							GetExitCodeProcess(processHandle, &code);
						}

				
						if (sesID !=-1 && login!="" && (processHandle == NULL || 
														strstr(processName, "player.exe") == NULL) ||
														(strstr(processName, "player.exe") != NULL && code != STILL_ACTIVE))
						{
							CClientSessionStateSender* loc_sender = new CClientSessionStateSender();
							loc_sender->SetFileName(FileInfo.cFileName);
							fclose(fl);

							socketConnectionTester.StopSocketConnection();
							loc_sender->SendStatus(sesID, APP_HANG_SYSTEM, login, L"", gSocketConnectionErrorCode, gSocketConnectionStatus, login, "", glComment, GetClientVersion());
						}
						else if (sesID == -1 || login == "")
						{
							fclose(fl);
							_wremove(filePath.c_str());
						}

						CloseHandle(processHandle);
					}
					else
					{
						int socketError = 0;
						int socketStatus = 0;
						char proxySetting[1000];
						char  logPath[500];
						char  version[500];

						fscanf(fl, "%i\n", &sesID); 
						fscanf(fl, "%i\n", &status); 
						fgets(logPath, 500, fl); 
						fscanf(fl, "%s\n", &login); 
						fscanf(fl, "%s\n", &version); 
						fscanf(fl, "%s\n", &proxySetting); 
						fscanf(fl, "%i\n", &socketError); 
						fscanf(fl, "%i\n", &socketStatus); 

						USES_CONVERSION;
						std::wstring logFileName = A2W(logPath);
						logFileName = logFileName.substr(0, logFileName.length()-1);

						if (!feof(fl))
						{
							fscanf(fl, "%s\n", &allProcess); 
						}

						if (sesID !=-1 && status!=-1)
						{
							CClientSessionStateSender* loc_sender = new CClientSessionStateSender();
							loc_sender->SetFileName(FileInfo.cFileName);
							fclose(fl);

							socketConnectionTester.StopSocketConnection();
							loc_sender->SendStatus(sesID, status, login, logFileName, gSocketConnectionErrorCode, gSocketConnectionStatus, proxySetting, allProcess, glComment, version);
						}
						else
						{
							fclose(fl);
							_wremove(filePath.c_str());
						}
					}


				}

			}
		}while (FindNextFileW(Search, &FileInfo));

		FindClose(Search);
	}
}

void Restart(bool isNeedToAutoLogin)
{
	if( hSplashWindow != 0)
		ShowWindow(hSplashWindow, SW_HIDE);

	if (!isNeedToRestart)
		return;

	restartCnt++;

	wchar_t sAppName[ MAX_PATH + 1];
	GetCurrentDirectoryW(MAX_PATH, &sAppName[0]);	
	wchar_t szRoot[MAX_PATH + 1];
	wcscpy(szRoot, sAppName);
	//_tcscat( sAppName, "\\player.exe");
	wcscat( sAppName, L"\\vacademia.exe");

	wchar_t szCommandLine[MAX_PATH];

	if (isNeedToAutoLogin)
	{
		USES_CONVERSION;
		std::wstring szUserName =  A2W(clntSession.GetPropertyFromPlayerINI("user","login").c_str());
		std::wstring glSettingFileName = clntSession.GetSettingsDirectory();
		glSettingFileName += L"\\va_global_user_settings.xml";

		// находи пароль
		std::string password = "";
		std::ifstream file;
		file.open(glSettingFileName.c_str(), std::ios::in);
		if (file.good())
		{
			int pos = -1;

			while (!file.eof())
			{
				if (!getline(file, password))
				{
					file.close();
					break;
				}

				pos = password.find("name=\"password\"");

				if (pos!=-1)
				{
					pos = password.find("value=\"");
					password = password.substr(pos+7); 
					pos = password.find('"');
					password = password.substr(0,pos);
					break;
				}
			}
		}
		file.close();

		if (password == "")
		{			
			password = W2A(glPassword.c_str());	
		}

		if (password != "")
		{
			lstrcpyW(szCommandLine, L" -user ");
			lstrcatW(szCommandLine, szUserName.c_str());
			lstrcatW(szCommandLine, L" -password ");
			lstrcatW(szCommandLine, A2W(password.c_str()));
		}
		else
		{
			lstrcpyW(szCommandLine, L"");
		}
	}
	else
	{
		lstrcpyW(szCommandLine, L"");

		if( hSplashWindow != 0)
			ShowWindow(hSplashWindow, SW_HIDE);

		if (hPlayerCreatingEvent)
		{
			CloseHandle(hPlayerCreatingEvent);
			hPlayerCreatingEvent = NULL;
		}
	}

	wchar_t ff[30];
	_itow(restartCnt, ff, 10);

	lstrcatW(szCommandLine, L" -restart ");
	lstrcatW(szCommandLine,ff);

	// get folder of the current process
	wchar_t szCurrentFolder[2 * MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szCurrentFolder, MAX_PATH);
	PathRemoveFileSpecW(szCurrentFolder);

	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_FORCEONFEEDBACK;
	si.wShowWindow = SW_SHOW;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	CreateProcessW(sAppName,	szCommandLine,	NULL, NULL,	FALSE , 0,NULL,szCurrentFolder,	&si,&pi);


	//int lastError = GetLastError();
}

std::string GetLogin()
{
	std::string login = clntSession.GetPropertyFromPlayerINI("user","login");

	if (login.size() == 0)
	{
		USES_CONVERSION;
		login = W2A(glLogin.c_str());
	}

	return login;
}

std::string GetClientVersion()
{
	std::string version = clntSession.GetPropertyFromPlayerINI("settings","version");
	return version;
}

std::string GetClientLang()
{
	std::string lang = clntSession.GetPropertyFromPlayerINI("settings","language");	return lang;
}

// перезапускаем академию, если в процессах нет диспетчера задач
bool IsClosedNotFromTaskManager()
{
	// ищем диспетчер задач
	HANDLE pSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	bool isOk = false;	
	PROCESSENTRY32 ProcEntry;	
	ProcEntry.dwSize = sizeof(ProcEntry);
	isOk = Process32First(pSnap, &ProcEntry);	

	while (isOk)
	{
		if (strcmp(ProcEntry.szExeFile, "[System Process]") != 0)
		{
			std::string name = ProcEntry.szExeFile;
			name = StringToLower(name); 

			if (name == "taskmgr.exe")
			{
				return false;
			}
		}
		isOk = Process32Next(pSnap, &ProcEntry);
	}	
	CloseHandle(pSnap);

	return true;
}
