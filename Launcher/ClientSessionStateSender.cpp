#include "stdafx.h"
#include <wininet.h> 
#include "shlobj.h"
#include <time.h>

#define LAUNCHER_SLN 1
#define MP_STRING std::string
#include "..\nengine\ncommon\IniFile.h"
#include "..\Cscl3DWS\RenderManager\Classes\UserSettings.h"
#include "ClientSessionStateSender.h"
#include "ProxyFinder.h"
#include <stdio.h>

CUserSettings m_userSettings;
static std::string sProxyAddr;
std::string sProxyUser;
std::string sProxyPass;
std::string sProxyType;
std::string sProxyTunnel;
std::vector<IProxySettings*> v_proxy;

int UrlEncode(const char *source, char *dest, unsigned max)  
{
	static const char *digits = "0123456789ABCDEF";
	unsigned char ch;
	unsigned len = 0;
	char *start = dest;

	while (len < max - 4 && *source)
	{
		ch = (unsigned char)*source;
		if (*source == ' ') {
			*dest++ = '+';
		}
		else if (isalnum(ch) || strchr("-_.!~*'()", ch)) {
			*dest++ = *source;
		}
		else {
			*dest++ = '%';
			*dest++ = digits[(ch >> 4) & 0x0F];
			*dest++ = digits[       ch & 0x0F];
		}  
		source++;
		len++;
	}
	*dest = 0;
	return start - dest;
}

std::string UrlEncodeString(const char *sz_decoded) 
{
	char * buf = new char[4096];
	UrlEncode(sz_decoded, buf, 1024);
	std::string result(buf);
	delete[] buf;
	return result;
}

std::wstring GetApplicationRootDirectory()
{
	wchar_t lpcIniFullPathName[MAX_PATH*2+2] = L"";

	wchar_t lpcStrBuffer[MAX_PATH*2+2];	lpcStrBuffer[0] = 0;
	::GetModuleFileNameW( NULL, lpcStrBuffer, MAX_PATH);

	if( lpcStrBuffer[0] == 0)
		return L"";
	
	PathRemoveFileSpecW( lpcStrBuffer);
	return std::wstring( lpcStrBuffer);	
}

DWORD WINAPI WatchThreadProc(LPVOID lpParam)
{
	while (true) 
	{ 
		ThreadInfo* info = (ThreadInfo*)lpParam;

		if (info!=NULL && info->sender!=NULL)
		{
			int errorCode = info->sender->GetErrorCode();
			if (errorCode == 1 && info->fileName == L"") // записать в файл;
			{
				std::wstring path = info->appPath;
				CreateDirectoryW(path.c_str(),NULL);

				path += L"\\";

				wchar_t dateStr [9];
				wchar_t timeStr [9];
				_wstrdate( dateStr);
				_wstrtime( timeStr );

				dateStr[2] = L'-';
				dateStr[5] = L'-';

				timeStr[2] = L'.';
				timeStr[5] = L'.';


				std::wstring fileName = dateStr;
				fileName += L" ";
				fileName += timeStr;
				fileName =  path + fileName + L".log";

				FILE* fl = _wfopen(fileName.c_str(), L"w");

				if (fl)
				{
					char status [33];
					itoa (info->status,status,10);

					char sessionID [33];
					itoa (info->sessionID,sessionID,10);

					char socketError [33];
					itoa (info->socketError,socketError,10);

					char socketStatus [33];
					itoa (info->socketStatus,socketStatus,10);

					fprintf( fl,"%s \n", sessionID);
					fprintf( fl,"%s \n", status);
					fprintf( fl,"%s \n", info->logFileName.c_str());
					fprintf( fl,"%s \n", info->login.c_str());

					if (info->version!="")
						fprintf( fl,"%s \n", info->version.c_str());
					else
						fprintf( fl,"0 \n");

					if (info->status != APP_HANG_SYSTEM)
					{
						fprintf( fl,"%s \n", info->proxySetting.c_str());
						fprintf( fl,"%s \n", socketError);
						fprintf( fl,"%s \n", socketStatus);

						if (info->allProcess!="")
							fprintf( fl,"%s \n", info->allProcess.c_str());
					}

			
					fclose(fl);
				}
				return 0;
			}	
			else if (errorCode == 0)
			{
				return 0;
			}
		
		}
		else
		{
			return 0;
		}

		Sleep(15000); 
	} 

	return 0;
}

//объявляем буфер, для хранения возможной ошибки, размер определяется в самой библиотеке
static char errorBuffer[CURL_ERROR_SIZE];
//объялвяем буфер принимаемых данных
static string buffer;

bool CClientSessionStateSender::IsExistProxy()
{
	return !sProxyAddr.empty();
}

CURLcode CClientSessionStateSender::CheckCurlProxys( CURL *curl, CURLcode result)
{
	if( !sProxyAddr.empty())
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, sProxyAddr.c_str());
		//curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 0);
		if( sProxyType == "3")
			curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
		if( sProxyType == "4")
			curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
		if(!sProxyUser.empty())
		{
			std::string sProxyUserPass = sProxyUser.c_str(); sProxyUserPass += ":"; sProxyUserPass += sProxyPass.c_str();
			curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, sProxyUserPass.c_str());
		}
		result = curl_easy_perform(curl);
		/*if( result != CURLE_OK)
		{
			curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
			result = curl_easy_perform(curl);
		}*/
		if( result == CURLE_OK)
			return result;		
	}
	USES_CONVERSION;
	std::vector<IProxySettings*>::iterator iter = v_proxy.begin();
	bool bPrevUseUser = false;
	for( ;iter != v_proxy.end(); iter++)
	{
		IProxySettings *proxy = *iter;
		sProxyAddr = W2A(proxy->getIP()); sProxyAddr += ":";
		char ff[7];
		_itoa( (int)proxy->getPort(), ff, 10);
		sProxyAddr += ff;
		curl_easy_setopt(curl, CURLOPT_PROXY, sProxyAddr.c_str());
		//curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 0);
		if(proxy->isUseUser())
		{			
			std::string sProxyUserPass = W2A(proxy->getUser()); sProxyUserPass += ":"; sProxyUserPass += W2A(proxy->getPassword());
			curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, sProxyUserPass.c_str());
			bPrevUseUser = true;
		}
		else if(bPrevUseUser)
		{
			curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, "");
			bPrevUseUser = false;
		}
		result = curl_easy_perform(curl);
		/*if( result != CURLE_OK)
		{
			curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
			result = curl_easy_perform(curl);
		}*/
	}
	return result;
}



DWORD WINAPI TaskThreadProc(LPVOID lpParam)
{
	ThreadInfo* info = (ThreadInfo*)lpParam;

	// определяет, какая программа запрашивает доступ 
	// (Заполняется переменная user_agent в заголовке HTTP), нужный вам тип доступа
	// возвращает вам дескриптор соединения.

	CURL *curl; 
	CURLcode result = CURLE_OK;  
	curl = curl_easy_init(); 
	std::string buffer;

	if(curl) 
	{  
		char status [33];
		itoa (info->status,status,10);

		char sessionID [33];
		itoa (info->sessionID,sessionID,10);

		char socketError [33];
		itoa (info->socketError,socketError,10);

		char socketStatus [33];
		itoa (info->socketStatus,socketStatus,10);

		std::string url = "http://" + info->serverIP +"/vu/php_0_1/update_session_state.php?status=" + status + "&sessionID=" +sessionID + "&login=" +info->login + "&computerName=" + UrlEncodeString(info->computerName.c_str());

		if (info->logFileName!=L"")
		{
			url += "&logFileName=";
			USES_CONVERSION;
			url += UrlEncodeString( W2A(info->logFileName.c_str()));
		}


		if (info->version!="")
		{
			url += "&version=";
			url += UrlEncodeString( info->version.c_str());
		}

		if (info->status != APP_HANG_SYSTEM)
		{
			url += "&socketError=";
			url += socketError;
			url += "&socketStatus=";
			url += socketStatus;
			//url +="&proxySetting=" + UrlEncodeString(info->proxySetting.c_str());
			url +="&proxySetting=" + UrlEncodeString( (sProxyAddr + " " + sProxyUser + ":" + sProxyPass + " type:" + sProxyType).c_str());

			if (info->allProcess!="")
			{
				url+="&allProcess=" + UrlEncodeString(info->allProcess.c_str());
			}
		}
		if (info->comment.size() > 0)
		{			
			url += "&comment=" + UrlEncodeString(info->comment.c_str());
		}

		//задаем все необходимые опции
		//определяем, куда выводить ошибки
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		//задаем опцию - получить страницу по адресу http://...
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		//указываем прокси сервер, если надо
		//if ((info->proxySetting!="unknown") && (info->proxySetting!=""))		
		//задаем опцию отображение заголовка страницы
		//curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		//указываем функцию обратного вызова для записи получаемых данных
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		//указываем куда записывать принимаемые данные
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		//запускаем выполнение задачи
		CURLcode result = curl_easy_perform(curl);
		
		DWORD statusCode;
		CURLcode result1 = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&statusCode);
		//проверяем успешность выполнения операции
		if(result != CURLE_OK || result1 == CURLE_OK && !sProxyAddr.empty() && (statusCode < 200 || statusCode > 299))
		{
			result = CClientSessionStateSender::CheckCurlProxys( curl, result);
		}
		//проверяем успешность выполнения операции
		if (result == CURLE_OK)     
		{
			if (buffer=="file")
			{
				info->sender->SetErrorCode(0);
				// отослали нужный статус с перезахода, теперь нужно удалить файл
				if (info->fileName!=L"")
				{
					std::wstring path = info->appPath + L"\\" + info->fileName;
					_wremove(path.c_str());
				}
			}
			else
			{
				info->sender->SetErrorCode(1);
			}
		}
		else
		{
			info->sender->SetErrorCode(1); 
		} 
	}
	else
	{
		info->sender->SetErrorCode(1);
	}

	//завершаем сессию
	curl_easy_cleanup(curl);

	/* if (info!=NULL && info->sender!= NULL)
	{
		HINTERNET hInternetSession;   
		hInternetSession = InternetOpen(
			"Microsoft Internet Explorer",  // agent
			INTERNET_OPEN_TYPE_PRECONFIG,   // access
			NULL, NULL, 0);					 // defaults

		if (hInternetSession)
		{
			char status [33];
			itoa (info->status,status,10);

			char sessionID [33];
			itoa (info->sessionID,sessionID,10);

			char socketError [33];
			itoa (info->socketError,socketError,10);

			char socketStatus [33];
			itoa (info->socketStatus,socketStatus,10);

			std::string url = "http://" + info->serverIP +"/vu/php_0_1/update_session_state.php?status=" + status + "&sessionID=" +sessionID + "&login=" +info->login;

			if (info->status != APP_HANG_SYSTEM)
			{
				url += "&socketError=";
				url += socketError;
				url += "&socketStatus=";
				url += socketStatus;
				url +="&proxySetting=" +info->proxySetting;

				if (info->allProcess!="")
				{
					url+="&allProcess=" +info->allProcess;
				}
			}

	

			HINTERNET hURL = InternetOpenUrl(
				hInternetSession,               // session handle
				url.c_str(),   // URL to access
				NULL, 0, 0, 0);                 // defaults

			if (hURL)
			{
				BOOL bResult;
				char cBuffer[32];          // I'm only going to access 1K of info.
				DWORD dwBytesRead;

				bResult = InternetReadFile(
					hURL,            // handle to URL
					(LPSTR)cBuffer,  // pointer to buffer
					(DWORD)32,     // size of buffer
					&dwBytesRead);   // pointer to var to hold return value

				if (bResult)
				{
					if (cBuffer[0]== 'f' && cBuffer[1]== 'i' && cBuffer[2]== 'l' && cBuffer[3]== 'e')
					{
						info->sender->SetErrorCode(0);
						// отослали нужный статус с перезахода, теперь нужно удалить файл
						if (info->fileName!="")
						{
							std::string path = info->appPath + "\\" + info->fileName;
							remove(path.c_str());
						}
					}
					else
					{
						info->sender->SetErrorCode(1);
					}
				}
				else
				{
					info->sender->SetErrorCode(1);
				}

				InternetCloseHandle(hURL);
			}
			else
			{
				info->sender->SetErrorCode(1);
			}

			InternetCloseHandle(hInternetSession);
		}
		else
		{
			info->sender->SetErrorCode(1);
		}
	}*/


	return 0;
}

DWORD WINAPI UploadThreadProc(LPVOID lpParam)
{
	UploadThreadInfo* info = (UploadThreadInfo*)lpParam;	
	std::wstring fileName = info->logFileName.c_str();
	int pos = fileName.find_last_of(L"\\");
	if (pos != -1)
	{
		fileName = fileName.substr(pos + 1, fileName.size() - pos - 1);
	}

	std::wstring path = info->appPath;
	CreateDirectoryW(path.c_str(),NULL);

	path += L"\\";
	
	std::wstring logFileName = path + fileName + L".txt";

	CURL *curl;
	CURLcode res;
 
	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;
	struct curl_slist *headerlist=NULL;
	static const char buf[] = "Expect:";
 
	curl_global_init(CURL_GLOBAL_ALL);
	USES_CONVERSION;
 
	/* Fill in the file upload field */ 
	curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "sendfile",
			   CURLFORM_FILE, W2A(info->logFileName.c_str()),
               CURLFORM_END);
 
	/* Fill in the filename field */ 
	curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "filename",
               CURLFORM_COPYCONTENTS, W2A(fileName.c_str()),
               CURLFORM_END);
 
 
	/* Fill in the submit field too, even if this is rarely needed */ 
	curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "submit",
               CURLFORM_COPYCONTENTS, "send",
               CURLFORM_END);
 
	curl = curl_easy_init();
	/* initalize custom header list (stating that Expect: 100-continue is not
		 wanted */ 
	headerlist = curl_slist_append(headerlist, buf);
	if (curl) 
	{
		/* what URL that receives this POST */ 
		std::string url = "http://" + info->serverIP + "/vu/php_0_1/upload_log.php";
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());		
		/*if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )
		/* only disable 100-continue header if explicitly requested */ 
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		res = curl_easy_perform(curl);

		DWORD statusCode;
		CURLcode result1 = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&statusCode);
		//проверяем успешность выполнения операции
		if(res != CURLE_OK || result1 == CURLE_OK && !sProxyAddr.empty() && (statusCode < 200 || statusCode > 299))
		{
			res = CClientSessionStateSender::CheckCurlProxys( curl, res);
		}
 
		/* then cleanup the formpost chain */ 
		curl_formfree(formpost);
		/* free slist */ 
		curl_slist_free_all (headerlist);

		/* always cleanup */ 
		curl_easy_cleanup(curl);


		if (res == CURLE_OK)     
		{
			_wremove(logFileName.c_str());
			info->sender->SetWriteLogSuccess(true);
		}
	}

	return 0;
}

ThreadInfo::ThreadInfo(int _sessionID, unsigned char _status, std::string _serverIP, std::string _login, std::wstring _appPath, std::wstring _fileName, std::string _allProcess, unsigned int _socketError, unsigned int _socketStatus, std::string _proxySetting,	std::string _computerName, std::wstring _logFileName, CClientSessionStateSender* _sender, std::wstring _comment, std::string _version)
{
	sessionID = _sessionID;
	status = _status;
	serverIP = _serverIP;
	login = _login;
	appPath = _appPath;
	fileName = _fileName;
	allProcess = _allProcess;
	socketError = _socketError;
	socketStatus = _socketStatus;
	proxySetting = _proxySetting;
	computerName = _computerName;
	logFileName = _logFileName;
	sender = _sender;
	USES_CONVERSION;
	comment = W2A(_comment.c_str());	
	version = _version;
}

CClientSessionStateSender::CClientSessionStateSender()
{
	SetLogDirectory();
	lastSendedStatus = -1;
	fileName = L"";
	m_isLoginSuccess = false;
	m_isWriteLogSuccess = false;
}

bool CClientSessionStateSender::LoadProxyInfo()
{
	m_userSettings.LoadGlobal();	
	std::string proxyMode = m_userSettings.GetSetting("proxy_mode");
	if( !proxyMode.empty() && proxyMode != "none")
	{
		if( proxyMode == "system")
		{
			CProxyFinder proxyFinder;	
			proxyFinder.getAllProxySettings( &v_proxy, PT_HTTP);
			/*if( !v_proxy.empty())		
				return true;*/
		}
		else
		{
			sProxyType = "2";
			sProxyAddr = m_userSettings.GetSetting("proxy_address");
			sProxyAddr += ":";
			sProxyAddr += m_userSettings.GetSetting("proxy_port");		

			std::string use_user = m_userSettings.GetSetting("proxy_use_user");
			if (use_user == "yes" || use_user == "1")
			{
				sProxyUser = m_userSettings.GetSetting("proxy_username");
				//std::string password = m_userSettings.GetPassSetting("ls_pass");
				sProxyPass = m_userSettings.GetSetting("proxy_pass");
			}
			return true;
		}
	}
	std::string enabled = m_userSettings.GetSetting("ls_enabled");
	if (enabled == "1")
	{
		sProxyAddr = m_userSettings.GetSetting("ls_addres");
		sProxyAddr += ":";
		sProxyAddr += m_userSettings.GetSetting("ls_port");		

		std::string use_user = m_userSettings.GetSetting("ls_use_user");
		if (use_user == "yes" || use_user == "1")
		{
			sProxyUser = m_userSettings.GetSetting("ls_username");
			//std::string password = m_userSettings.GetPassSetting("ls_pass");
			sProxyPass = m_userSettings.GetSetting("ls_pass");
		}
		sProxyType =  m_userSettings.GetSetting("ls_type");
		sProxyTunnel =  m_userSettings.GetSetting("ls_tunnel");
		return true;
	}
	return false;
}

std::string CClientSessionStateSender::GetWorldServer()
{
	std::string worldServer = GetPropertyFromPlayerINI("paths", "server");
	if (worldServer == "")
	{
		//worldServer = "178.63.80.136";		
		worldServer = "176.9.41.35";
	}
	return worldServer;
}

CClientSessionStateSender::~CClientSessionStateSender()
{
}

void CClientSessionStateSender::SetLoginSuccess(bool isLoginSuccess)
{
	m_isLoginSuccess = isLoginSuccess;
}

void CClientSessionStateSender::SetWriteLogSuccess(bool isWriteLogSuccess)
{
	m_isWriteLogSuccess = isWriteLogSuccess;
}

void CClientSessionStateSender::CreateUploadThread(std::wstring logPath)
{
	std::string worldServer = GetWorldServer();

	m_uploadThread.SetStackSize(256 * 1024);
	UploadThreadInfo* infoUpload = new UploadThreadInfo(logPath, worldServer,GetLogDirectory(), this);
	m_uploadThread.SetParameter((void*)infoUpload);
	m_uploadThread.SetThreadProcedure(UploadThreadProc);
	m_uploadThread.Start();
}


void CClientSessionStateSender::SendStatus(int auSessionID, unsigned char aStatus, std::string login, std::wstring logPath, unsigned int socketError, unsigned int socketStatus, std::string proxySetting, std::string allProcess, std::wstring comment, std::string version, bool isLoginSended)
{	
	// Если выходим с формы логина, но логин получили (т.е. пользователь логинился), то меняем статус на ошибочный
	if ((aStatus == 23) && (isLoginSended))
	{
		aStatus = 46;
	}

	if (lastSendedStatus == aStatus)
			return;

	// Пользователь вышел, когда ему выдало ошибку соединения с сервером
	// и предложило настроить сеть или сообщение про ТО или
	// сообщение про отсутствие драйверов
	// После этого никаких новых статусов посылать не надо
	if (((lastSendedStatus >= 20)  && (lastSendedStatus <= 25)) ||
		((lastSendedStatus >= 36)  && (lastSendedStatus <= 39)) ||
		((lastSendedStatus >= 44)  && (lastSendedStatus <= 48)) ||
		((lastSendedStatus >= 2) && (lastSendedStatus <= 8)))
	{
		return;
	}

	lastSendedStatus = aStatus;
	SetErrorCode(-1);

	if (login == "")    
		login = "no_login";

	/*if (proxySetting == "unknown")
	{
		CProxyFinder proxyFinder;
		ProxySettings* _proxySetting = new ProxySettings();  

		if (proxyFinder.getIEProxySettings(_proxySetting, PT_HTTP))
		{
			proxySetting = _proxySetting->toString();
		}
	}*/	

	bool isLinux = false;

	// список процессов не нужен, если корректный выход или сами вырубили комп и есть логин 
	if (allProcess == "" && ((!m_isLoginSuccess || (aStatus != 1 && aStatus != INCORRECT_EXIT_STATUS)) ||
		((aStatus >= 14) && (aStatus <= 16))))
	{
		// список процессов
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

				if (name == "winedevice.exe")
				{
					isLinux = true;
				}

				int p = name.find(" ");

				while (p != -1) 
				{ 
					name.replace(p, 1, "_"); 
					p = name.find(" ");
				}

				std::map<std::string, unsigned short>::iterator iter = m_processName.find(name);

				if (iter == m_processName.end())
				{
					m_processName.insert(std::map<std::string, unsigned short>::value_type(name,1));
				}
			}
			isOk = Process32Next(pSnap, &ProcEntry);
		}	
		CloseHandle(pSnap);
		// формируем строку из имен процессов
		std::map<std::string, unsigned short>::iterator it = m_processName.begin();
		std::map<std::string, unsigned short>::iterator itEnd = m_processName.end();

		std::string _allProcess = "";
		for ( ; it != itEnd; it++)
		{
			if (it!= m_processName.begin())
				_allProcess +="*";
			_allProcess += (*it).first;
		}
		//

		allProcess = _allProcess;
		if (allProcess.size() > 3072)
		{
			allProcess = allProcess.substr(0,3072);
			allProcess += "...";
		}
	}

	if (isLinux)
	{
		if ((aStatus >= 14) && (aStatus <= 16))
		{
			aStatus = 40;
			lastSendedStatus = 40;
		}
	}

	LoadProxyInfo();

	char computerName[500];
	unsigned long size = 250;
	GetComputerName((LPTSTR)&computerName, &size);

	std::string worldServer = GetWorldServer();
	std::string dumpServer = GetPropertyFromPlayerINI("settings", "dumpserver");

	ThreadInfo* infoWorldServer = NULL;
	ThreadInfo* infoWatch = NULL;

	if (((aStatus != 1 && aStatus !=2 && aStatus !=6) || (!m_isLoginSuccess)) && (logPath.size() > 0) && (logPath != L"no_log"))
	{
		CreateUploadThread(logPath);

		int pos = logPath.find_last_of(L"\\");
		if (pos != -1)
		{
			logPath = logPath.substr(pos + 1, logPath.size() - pos - 1);
		}

		infoWorldServer = new ThreadInfo(auSessionID, aStatus, worldServer, login, appPath, fileName, allProcess, socketError, socketStatus, proxySetting, computerName, logPath, this, comment, version);
		infoWatch = new ThreadInfo(auSessionID, aStatus, "", login, appPath, fileName, allProcess, socketError, socketStatus, proxySetting, computerName, logPath, this, comment, version);
	}
	else
	{
		infoWorldServer = new ThreadInfo(auSessionID, aStatus, worldServer, login, appPath, fileName, allProcess, socketError, socketStatus, proxySetting, computerName, L"", this, comment, version);
		infoWatch = new ThreadInfo(auSessionID, aStatus, "", login, appPath, fileName, allProcess, socketError, socketStatus, proxySetting, computerName, L"no_log", this, comment, version);
		SetWriteLogSuccess(true);
	}

	m_threadWorldServer.SetStackSize(256 * 1024);
	//ThreadInfo* infoWorldServer = new ThreadInfo(auSessionID, aStatus, worldServer, login, appPath, fileName, allProcess, socketError, socketStatus, proxySetting, computerName, this);
	m_threadWorldServer.SetParameter((void*)infoWorldServer);
	m_threadWorldServer.SetThreadProcedure(TaskThreadProc);
	m_threadWorldServer.Start();

	m_threadWatch.SetStackSize(256 * 1024);
	//ThreadInfo* infoWatch = new ThreadInfo(auSessionID, aStatus, "", login, appPath, fileName, allProcess, socketError, socketStatus, proxySetting, computerName, this);
	m_threadWatch.SetParameter((void*)infoWatch);
	m_threadWatch.SetThreadProcedure(WatchThreadProc);
	m_threadWatch.Start();

	/*if (worldServer != dumpServer)
	{
		m_threadDumpServer.SetStackSize(256 * 1024);
		ThreadInfo* infoDump = new ThreadInfo(auSessionID, aStatus, worldServer, login, appPath, fileName, allProcess, socketError, socketStatus, proxySetting, this);
		m_threadDumpServer.SetParameter((void*)infoDump);
		m_threadDumpServer.SetThreadProcedure(TaskThreadProc);
		m_threadDumpServer.Start();
	}

	/*if (((aStatus != 1 && aStatus !=2 && aStatus !=6) || (!m_isLoginSuccess)) && (logPath.size() > 0))
	{
		m_uploadThread.SetStackSize(256 * 1024);
		UploadThreadInfo* infoUpload = new UploadThreadInfo(logPath, worldServer);
		m_uploadThread.SetParameter((void*)infoUpload);
		m_uploadThread.SetThreadProcedure(UploadThreadProc);
		m_uploadThread.Start();
	}*/
}


std::string CClientSessionStateSender::GetPropertyFromPlayerINI(std::string section, std::string key)
{
	// get folder of the current process
	wchar_t szPlayerDir[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szPlayerDir, MAX_PATH);
	PathRemoveFileSpecW(szPlayerDir);

	std::wstring fileName = szPlayerDir;
	fileName += L"\\player.ini";

	CIniFile iniFile(fileName);

	return iniFile.GetString(section,key);
}

void CClientSessionStateSender::SetPropertyFromPlayerINI(std::string section, std::string key, std::string value)
{
	// get folder of the current process
	wchar_t szPlayerDir[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szPlayerDir, MAX_PATH);
	PathRemoveFileSpecW(szPlayerDir);

	std::wstring fileName = szPlayerDir;
	fileName += L"\\player.ini";

	CIniFile iniFile(fileName);
	iniFile.SetString(section, key, value);
}

void CClientSessionStateSender::SetFileName(std::wstring _fileName)
{
	fileName = _fileName;
}

void CClientSessionStateSender::SetLogDirectory()
{
	wchar_t tmp[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
	appPath = tmp;

	settingPath = tmp;
	settingPath += L"\\Vacademia";

	appPath += L"\\Vacademia\\LauncherLog";
}

std::wstring CClientSessionStateSender::GetLogDirectory()
{
	return appPath;
}

std::wstring CClientSessionStateSender::GetSettingsDirectory()
{
	return settingPath;
}

void CClientSessionStateSender::SetErrorCode(int errorCode)
{
	m_errorCode = errorCode;
}

int CClientSessionStateSender::GetErrorCode()
{
	return m_errorCode;
}

