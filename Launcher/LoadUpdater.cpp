#include "stdafx.h"
#include "LoadUpdater.h"
#include <wininet.h> 
#include "shlobj.h"
#include <time.h>
#include "ProcessFinder.h"

#include "..\nengine\ncommon\IniFile.h"
#include "ClientSessionStateSender.h"
#include "ProxyFinder.h"
#include <stdio.h>

//объявляем буфер, для хранения возможной ошибки, размер определяется в самой библиотеке
static char errorBuffer[CURL_ERROR_SIZE];
//объялвяем буфер принимаемых данных
static string buffer;

ThreadInfoForVersion::ThreadInfoForVersion(std::string _serverIP, std::string _version, std::string _buildType)
{
	serverIP = _serverIP;
	curVersion = _version;
	buildType = _buildType;
}


DWORD WINAPI VersionThreadProc(LPVOID lpParam)
{
	ThreadInfoForVersion* info = (ThreadInfoForVersion*)lpParam;

	// определяет, какая программа запрашивает доступ 
	// (Заполняется переменная user_agent в заголовке HTTP), нужный вам тип доступа
	// возвращает вам дескриптор соединения.

	CURL *curl; 
	CURLcode result;  
	curl = curl_easy_init(); 
	std::string buffer;

	if(curl) 
	{  
		std::string url = "http://" + info->serverIP +"/vu/php_0_1/getVersion.php?version=" + info->curVersion + "&buildtype=" + info->buildType;
		//задаем все необходимые опции
		//определяем, куда выводить ошибки
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		//задаем опцию - получить страницу по адресу http://...
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		//указываем прокси сервер, если надо
		/*if ((info->proxySetting!="unknown") && (info->proxySetting!=""))
			curl_easy_setopt(curl, CURLOPT_PROXY, "192.168.0.62:80");*/
		//задаем опцию отображение заголовка страницы
		//curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		//указываем функцию обратного вызова для записи получаемых данных
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		//указываем куда записывать принимаемые данные
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		//запускаем выполнение задачи
		result = curl_easy_perform(curl);
		DWORD statusCode;
		CURLcode result1 = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&statusCode);
		//проверяем успешность выполнения операции
		if(result != CURLE_OK || result1 == CURLE_OK && CClientSessionStateSender::IsExistProxy() && (statusCode < 200 || statusCode > 299))
		{
			result = CClientSessionStateSender::CheckCurlProxys( curl, result);
		}
		if (result == CURLE_OK)     
		{
			bool hwndFound = false;
			WCHAR szPlayerDir[MAX_PATH] = { 0 };
			GetModuleFileNameW(NULL, szPlayerDir, MAX_PATH);
			PathRemoveFileSpecW(szPlayerDir);

			if (buffer.size() < 100)
			while (!hwndFound)
			{
				int i = 0;
				CProcessWindowFinder finder;		
				std::vector<SFindedProcess> processes = finder.GetPlayerWindow( "player.exe");
				for (int m = 0; m < processes.size(); m++)
				{
					HWND hWnd = processes[m].window;
					unsigned int parentProcessID = processes[m].parentProcessID;
					WINDOWPLACEMENT wndpl; wndpl.length = sizeof(WINDOWPLACEMENT);

					if ( IsWindowVisible(hWnd) && !IsHungAppWindow(hWnd) && GetWindowPlacement(hWnd, &wndpl) && wndpl.showCmd != SW_HIDE)
					{
						hwndFound = true;
						COPYDATASTRUCT ds;
						ds.dwData = 2;
						ds.cbData = (DWORD)buffer.length() + 1;
						ds.lpData = (char*)buffer.c_str();
						SendMessage( hWnd, WM_COPYDATA, 6, (LPARAM)(LPVOID) &ds);
						return 0;
					}
					else
					{
						if (i > 120)
						{
							return 0;
						}
						Sleep(1000);
						i++;
					}
				}
			}
		}  
	}


	//завершаем сессию
	curl_easy_cleanup(curl);

	return 0;
}

std::string CLoadUpdater::GetPropertyFromPlayerINI(std::string section, std::string key)
{
	// get folder of the current process
	WCHAR szPlayerDir[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szPlayerDir, MAX_PATH);
	PathRemoveFileSpecW(szPlayerDir);

	std::wstring fileName = szPlayerDir;
	fileName += L"\\player.ini";

	CIniFile iniFile(fileName);
	return iniFile.GetString( section.data(), key.data());
}

void CLoadUpdater::SendVersion()
{	
	ThreadInfoForVersion* infoWorldServer = NULL;
	CClientSessionStateSender::LoadProxyInfo();
	infoWorldServer = new ThreadInfoForVersion( GetPropertyFromPlayerINI( "paths", "server"), GetPropertyFromPlayerINI( "settings", "version"), GetPropertyFromPlayerINI( "settings", "buildtype"));
	m_threadWorldServer.SetStackSize(256 * 1024);
	m_threadWorldServer.SetParameter((void*)infoWorldServer);
	m_threadWorldServer.SetThreadProcedure(VersionThreadProc);
	m_threadWorldServer.Start();
}

void CLoadUpdater::SetErrorCode(int errorCode)
{
	m_errorCode = errorCode;
}
   