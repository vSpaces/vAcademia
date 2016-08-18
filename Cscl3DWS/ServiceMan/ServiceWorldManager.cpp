#include "StdAfx.h"
#include "ServiceWorldManager.h"
#include "atltime.h"

#include "GetServiceOut.h"
#include "ExecMethodOut.h"
#include "RegisterServiceOut.h"

#include "GetServiceIn.h"
#include "ExecMethodIn.h"
#include "LoginIn.h"
#include "LogoutIn.h"

#include "LoginIn2.h"
#include "LoginQueryOut2.h"

#include "SetSessionStateValueOut.h"
#include "SetSessionStateResultIn.h"

#include "ServiceLoadingFailedIn.h"

#include "TOInfoIn.h"

#include "ComputerConfigurationIn.h"
#include "ComputerStateOut.h"

#include "DumpParamsIn.h"
#include "DumpFileOut.h"

#include "logger.h"
#include "../../../Common/MD5.h"
#include "../../../Common/GetMacAddress.h"
#include "../../Cscl3DWS.h"
#include "CrashHandlerInThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "..\..\rm\rm.h"

#include "DataBuffer.h"

#pragma comment( lib, "Psapi.lib" )
//#include <windows.h>
#define DPSAPI_VERSION (1)
#include <Psapi.h>
// Send Type - типы посылаемых сообщений
// 1-3 зарезервированы для базового класса
#define ST_LoginServerRequest				4 // Запрос на логинацию
#define ST_LoginServerRequest2				6 // Запрос на логинацию
#define ST_SetSessionStateValue				7 // Установить флаг состояния сессии
#define ST_ComputerConfiguration			8 // Отправить статистику
#define ST_ComputerState					9 // Отправить состояние компьютера (fps, режим)
#define ST_MDumpParams						10 // Отправить параметры мини-дампа
#define ST_MDumpFile						11 // Отправить мини-дамп 
#define ST_ConnectionRestoredSuccessfully	12 // Отправить сообщение, что связь восстановилась
#define ST_DebuggerPresent					13 // Отправить сообщение, что заходили под отладкой

#define WM_VACADEMIA_WM_BASE				WM_APP
#define WM_SUCCES_LOGIN						(WM_VACADEMIA_WM_BASE + 110)
#define WM_EXIT_CODE						(WM_VACADEMIA_WM_BASE + 111)
#define WM_NEW_SESSION_AFTER_LOGOUT			(WM_VACADEMIA_WM_BASE + 112)

// 29 - ошибка получении версии
// 30 - showError()
// 32 - не прошли логинацию
// 33 - не соединились с ResServer
#define LG_ERROR_LOAD_MODULE				27 // ошибка загрузки модулей
#define LG_ERROR_TO							31 // то
#define LG_ERROR_LOAD_MODULE_TIMEOUT		28 // не может загрузить модуль по таймауту
#define LG_ERROR_CONNECT_SYNCH_SERVER		34 // не соединились с SynchServer
#define LG_ERROR_UNKNOWN					26 

using namespace ServiceManProtocol;

__loginParams::__loginParams():
	MP_WSTRING_INIT(login),
	MP_WSTRING_INIT(password),
	MP_WSTRING_INIT(encodingPassword),
	MP_WSTRING_INIT(computerJeyCode),
	MP_WSTRING_INIT(authKey),
	MP_WSTRING_INIT(updaterKey),
	MP_WSTRING_INIT(buildType)
{
}

CServiceWorldManager::CServiceWorldManager( omsContext* aContext):
											CServiceManagerBase(aContext)
{
	if (aContext != NULL)
		aContext->mpServiceWorldMan = this;
	bDestroying = false;
	bSendedExitCodeReceived = false;
	iComputerStateSending = 0;
	GetComputerConfigurationPacket();
	eventComputerStatsSend = CreateEvent( NULL, TRUE, FALSE, "eventComputerStatsSend");
	iDumpSendedCode = DUMP_NO_SENDED;
	hWnd = NULL;
}

CServiceWorldManager::~CServiceWorldManager()
{	
	pCallbacks = NULL;
	iComputerStateSending = 3;
	SetEvent( eventComputerStatsSend);	
	bDestroying = true;
	Disconnect();
	CloseHandle( eventComputerStatsSend);
}

void CServiceWorldManager::Destroy()
{
	MP_DELETE_THIS;
}

// ----------------------------------------------------	
// Реадизация ф-й интерфейса IBaseServiceManager
// ----------------------------------------------------	
// Подключись к серверу сервисов
void CServiceWorldManager::Connect()
{
	CServiceManagerBase::ConnectWorldServer(this, this);
	if( IsConnected() && iComputerStateSending == 0)
	{		
		iComputerStateSending = 1;
		SendOnMyClientSession(EST_SendQuery, ST_ComputerConfiguration, computerInfoPacket.GetData(), computerInfoPacket.GetDataSize());	
	}
}

// Отключись от сервера сервисов
void CServiceWorldManager::Disconnect()
{
	CServiceManagerBase::Disconnect();
}

// Удалить менеджер синхронизации
void CServiceWorldManager::Release()
{
	CServiceManagerBase::Release();
}

// Получить описание 
omsresult CServiceWorldManager::GetJSONService(unsigned int aRmmlID, const wchar_t* apwcServiceName)
{
	return CServiceManagerBase::GetJSONService(aRmmlID, apwcServiceName);
}

// Вызвать метод
omsresult CServiceWorldManager::ExecMethod(int requestID, unsigned int rmmlID, const wchar_t* apwcJSONMethodDescription)
{
	return CServiceManagerBase::ExecMethod(requestID, rmmlID, apwcJSONMethodDescription);
}

// Установить интерфейс, принимающий обратные вызовы от SyncManager-а
bool CServiceWorldManager::SetCallbacks(IServiceManCallbacks* apCallbacks)
{
	return CServiceManagerBase::SetCallbacks(apCallbacks);
}
// Подтвердить регистрацию сервиса на клиенте
omsresult CServiceWorldManager::RegisterService(int pendingID, unsigned int rmmlID)
{
	return CServiceManagerBase::RegisterService(pendingID, rmmlID);
}

/*// отослать на сервер, не отосланные сообщения сервисов
void CServiceWorldManager::SendAllNotSendingServiceMessage()
{
	CServiceManagerBase::SendAllNotSendingServiceMessage();
}*/


int CServiceWorldManager::Login(const wchar_t* apwcLogin, const wchar_t* apwcPassword, bool isEncodingPassword, const wchar_t* apwcEncodingPassword, const wchar_t* apwcComputerJeyCode, bool isAnonymous, int requestID, int source, const wchar_t* apwcAuthKey, const wchar_t* apwcUpdaterKey, const wchar_t *apwcBuildType)
{
	bsLogin = apwcLogin;
	// логин нужно знать уже в этот момент, чтобы настройки и пр. загрузить
	if( pContext->mpSM != NULL)
		pContext->mpSM->SetClientStringID(bsLogin);

	pContext->mpComMan->LogLoginBeginTime();

	if(!IsConnected())
		Connect();

	if (!IsConnected())
		return -1;

	wchar_t wcsLanguage[100] = L"";
	wchar_t wcsVersion[100] = L"";
	if( pContext->mpApp != NULL)
	{
		pContext->mpApp->GetLanguage((wchar_t*)wcsLanguage, 99);
		pContext->mpApp->GetVersion((wchar_t*)wcsVersion, 99);
	}
	unsigned char auProxiesMask = pContext->mpComMan->GetProxiesMask();
	std::wstring wIP = pContext->mpComMan->GetMyIP();	
	CLoginQueryOut2 queryOut(requestID, apwcLogin, apwcPassword, apwcEncodingPassword, apwcComputerJeyCode, source, isAnonymous, wcsLanguage, wIP.c_str(), apwcAuthKey, computerInfoPacket.GetData(), computerInfoPacket.GetDataSize(), wcsVersion, auProxiesMask, apwcUpdaterKey, apwcBuildType);
	SendOnMyClientSession(EST_SendSignalQuery, ST_LoginServerRequest2, queryOut.GetData(), queryOut.GetDataSize());
	return requestID;
}

int CServiceWorldManager::Logout( int iCodeError)
{
	if (iCodeError == 7)
		SetSessionStateValue(CSS_EXIT_CODE, LG_ERROR_LOAD_MODULE);
	else if (iCodeError == 21)
		SetSessionStateValue(CSS_EXIT_CODE, LG_ERROR_TO);
	else if (iCodeError >=LG_ERROR_LOAD_MODULE_TIMEOUT && iCodeError!=31 && iCodeError <= LG_ERROR_CONNECT_SYNCH_SERVER)
		SetSessionStateValue(CSS_EXIT_CODE, iCodeError);
	else
		SetSessionStateValue(CSS_EXIT_CODE, LG_ERROR_UNKNOWN);

	if( pContext->mpSM != NULL)
		pContext->mpSM->SetClientStringID(L"");

	csFactory.Lock();
	if( pClientSession != NULL)
		pClientSession->RemoveQueryByType( ST_LoginServerRequest2);
	csFactory.Unlock();

	Disconnect();
	pContext->mpComMan->CloseAllSessions();
	if(pContext->mpSM != NULL && iCodeError != 0)
		pContext->mpSM->OnLogout( iCodeError);


	if (hWnd)
		::PostMessage(hWnd, WM_NEW_SESSION_AFTER_LOGOUT, 0, 0);
	return 1;
}

bool CServiceWorldManager::IsConnected()
{
	if (pClientSession == NULL)
		return false;
	return true;
}

std::wstring PwdSeparator(const wchar_t *aSourcePwd, int part)
{
	std::wstring sourcePwd = aSourcePwd;
	std::wstring::size_type pos = sourcePwd.find(L"|");
	if(pos == std::wstring::npos)
		return sourcePwd;
	if(part == 0) //первые 32 значения
	{
		return sourcePwd.substr(0, pos);
	}else{
		return sourcePwd.substr(pos+1);
	}
}

int CServiceWorldManager::LoginAsync( int requestID, const wchar_t* apwcLogin, const wchar_t* apwcPassword, bool isAnonymous, int source, bool isEncodingPassword, const wchar_t* apwcAuthKey, const wchar_t* apwcUpdaterKey)
{
	std::string pass;

	_loginParams *loginParams = MP_NEW( _loginParams);
	loginParams->server = this;
	loginParams->login = apwcLogin;
	// теперь всегда отправлял пароль зашифрованным
	loginParams->isEncodingPassword = isEncodingPassword;
	loginParams->authKey = apwcAuthKey != NULL ? apwcAuthKey : L"";
	loginParams->updaterKey = apwcUpdaterKey != NULL ? apwcUpdaterKey : L"";

	std::wstring wsPassword = apwcPassword;

	if ( isEncodingPassword)
	{
		loginParams->password = L"";
		CComString sMacAddress = GetMacAddress();
		USES_CONVERSION;
		
		loginParams->computerJeyCode = A2W( sMacAddress.GetBuffer());
		loginParams->encodingPassword = PwdSeparator(wsPassword.c_str(),0);		

		loginParams->encodingPassword +=loginParams->computerJeyCode;
		LPTSTR sPassword = W2A(loginParams->encodingPassword.c_str());
		char *sEncodingPassword = MP_NEW_ARR( char, md5::MD5_BUFFERSIZE );
		md5::CalculateMD5( (BYTE*) sPassword, &sEncodingPassword, md5::MD5_BUFFERSIZE);
		loginParams->encodingPassword = A2W( sEncodingPassword);
		loginParams->encodingPassword += L"|"; 
		//LPTSTR sSPassword = W2A(PwdSeparator(apwcPassword,1).c_str());
		//char *sSEncodingPassword = MP_NEW_ARR(char,100);
		//md5::CalculateMD5((BYTE*)sSPassword,&sSEncodingPassword);
		loginParams->encodingPassword += PwdSeparator(wsPassword.c_str(),1);
	
		MP_DELETE_ARR( sEncodingPassword);
	}
	loginParams->buildType = pContext->mpComMan->GetBuildType();
	//else
	//{
	//	loginParams->password = apwcPassword;
	//	wsPassword = loginParams->password + L"|0";
	//}

	{
		COPYDATASTRUCT CopyData;
		CopyData.dwData=0;
		CopyData.cbData=wcslen(apwcLogin)*sizeof(wchar_t)+2;
		CopyData.lpData= (PVOID)apwcLogin;

		if (hWnd)
			::SendMessage( hWnd, WM_COPYDATA, COPY_DATA_CODE_LOGIN, (LPARAM)(LPVOID) &CopyData);
	}
    
	COPYDATASTRUCT CopyData;
	CopyData.dwData=0;
	CopyData.cbData=wsPassword.length()*sizeof(wchar_t)+2;
	CopyData.lpData= (PVOID) wsPassword.c_str();

	if (hWnd)
		::SendMessage( hWnd, WM_COPYDATA, COPY_DATA_CODE_PASSWORD, (LPARAM)(LPVOID) &CopyData);

	loginParams->requestID = requestID;
	loginParams->source = source;
	loginParams->anonymous = isAnonymous;

	CSimpleThreadStarter stateThreadStarter(__FUNCTION__);
	stateThreadStarter.SetRoutine(loginThread_);
	stateThreadStarter.SetParameter(loginParams);
	stateThreadStarter.Start();	

	return 0;//(idRequest + 1);
}

DWORD WINAPI CServiceWorldManager::loginThread_(LPVOID param)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();

	_loginParams* aLoginParams = (_loginParams*)param;

	return aLoginParams->server->Login( aLoginParams->login.c_str(), aLoginParams->password.c_str(), aLoginParams->isEncodingPassword, aLoginParams->encodingPassword.c_str(), aLoginParams->computerJeyCode.c_str(), aLoginParams->anonymous, aLoginParams->requestID, aLoginParams->source, aLoginParams->authKey.c_str(), aLoginParams->updaterKey.c_str(), aLoginParams->buildType.c_str());
}

void CServiceWorldManager::OnExecLoginMethodResults( BYTE* aData, int aDataSize)
{	
	CLoginIn2 serviceInfo(aData, aDataSize);
	serviceInfo.Analyse();

	if( pContext == NULL || pContext->mpComMan == NULL)
	{
		ATLASSERT(pContext != NULL && pContext->mpComMan != NULL);
		return;
	}
	if(bDestroying)
		return;

	int iErrorCode = pContext->mpComMan->SetLoginData(serviceInfo.GetMethodResultSize(), (const wchar_t*)serviceInfo.GetMethodResult());

	if( iErrorCode == 0 && iComputerStateSending < 2)
	{
		iComputerStateSending = 2;		

		CSimpleThreadStarter stateThreadStarter(__FUNCTION__);
		stateThreadStarter.SetRoutine(computerStateThread_);
		stateThreadStarter.SetParameter(this);
		stateThreadStarter.Start();	
	}

	const wchar_t* pwcLogin = pContext->mpComMan->GetClientStringID();
	if(pwcLogin != NULL && *pwcLogin != L'\0')
		bsLogin = pwcLogin;

	if(pContext->mpSM != NULL)
		pContext->mpSM->SetClientStringID(bsLogin);

	if(iErrorCode==0)
	{
		if(pContext->mpLogger!=NULL)
			pContext->mpLogger->SetClientSession(pClientSession);

		if(pContext->mp3DWS != NULL)
		{
			long sessionID = pContext->mpComMan->GetSessionID();		
			pContext->mp3DWS->onLoginFinished( sessionID);
		}
	}

	if (IsDebuggerPresent())
	{
		SendOnMyClientSession(EST_SendSignalQuery, ST_DebuggerPresent, NULL,0);
	}

	if (hWnd)
		::PostMessage(hWnd, WM_SUCCES_LOGIN,0, 0);

	// Возвращаемся в rmml
	if (pCallbacks != NULL){
		std::wstring wsResponse;
		wsResponse += L"{login:'";
		wsResponse += bsLogin;

		/*wsResponse += L"', downloadURL:'";
		CComString sDownLoadURL = pContext->mpComMan->getHost();
		CComString sDownLoadURL = serviceInfo.GetDownloadURL();
		wsResponse += sDownLoadURL.AllocSysString();*/

		CComString sVersion = serviceInfo.GetLastVersion();
		if( sVersion.GetLength()>0)
		{
			wsResponse += L"', version:'";
			wsResponse += sVersion.AllocSysString();
			wsResponse += L"', update:";
			wchar_t ff[20] = {L'\0'};
			_itow_s( serviceInfo.GetUpdateParam(), ff, 20, 10);
			wsResponse += ff;
			CComString sUpdaterKey = serviceInfo.GetUpdaterKey();
			wsResponse += L", updaterKey:'";
			wsResponse += sUpdaterKey.AllocSysString();
			wsResponse += L"', isTO:";
			wchar_t ff2[20] = {L'\0'};
			_itow_s( serviceInfo.GetTOParam(), ff2, 20, 10);
			wsResponse += ff2;
			wsResponse += L"}";
			USES_CONVERSION;
			if( pContext->mpSM != NULL)
				pContext->mpSM->SetUpdaterKeyByThread( A2W(sUpdaterKey.GetBuffer()));
		}
		else
		{
			wsResponse += L"'}";
		}

		pCallbacks->onMethodComplete(serviceInfo.getRmmlID(), 0, wsResponse.c_str(), false, iErrorCode);
	}
}

void CServiceWorldManager::OnExecLogout( BYTE* aData, int aDataSize)
{	
	CLogoutIn serviceInfo(aData, aDataSize);
	serviceInfo.Analyse();
	Logout( serviceInfo.getCode());
}

void CServiceWorldManager::OnNotifyTOInfo( BYTE* aData, int aDataSize)
{	
	CTOInfoIn serviceInfo(aData, aDataSize);
	serviceInfo.Analyse();
	if(pContext->mpSM != NULL)
		pContext->mpSM->OnNotifyServerTOInfo( serviceInfo.getMinutes());
}

void CServiceWorldManager::OnExecSetSessionState( BYTE* aData, int aDataSize)
{	
	CSetSessionStateResultIn serviceInfo(aData, aDataSize);
	serviceInfo.Analyse();
	if(serviceInfo.getType() == CSS_EXIT_CODE)
	{
		if(serviceInfo.getCode() == exitCodeLast)
			bSendedExitCodeReceived = true;
	}
}

void CServiceWorldManager::OnExecDumpParams( BYTE* aData, int aDataSize)
{	
	CDumpParamsIn dumpParams(aData, aDataSize);
	if ( dumpParams.Analyse())
	{
		iDumpSendedCode = dumpParams.getCode();
	}
	else
		iDumpSendedCode = DUMP_NO_SENDED;
}

void CServiceWorldManager::OnExecDumpFile( BYTE* aData, int aDataSize)
{	
	CDumpParamsIn dumpParams(aData, aDataSize);
	if ( dumpParams.Analyse())
	{
		iDumpSendedCode = dumpParams.getCode();
	}
	else
		iDumpSendedCode = DUMP_NO_SENDED;
}

int CServiceWorldManager::SendMDumpParams( BYTE* aData, int aDataSize)
{
	if ( !IsConnected())
		Connect();
	if ( !IsConnected() )
		return -1;
	
	iDumpSendedCode = DUMP_SENDING_STARTING;
	SendOnMyClientSession(EST_Send, ST_MDumpParams, (const unsigned char *) aData, aDataSize);
	return 0;//requestID;
}

int CServiceWorldManager::SendMDumpFile( BYTE* aData, int aDataSize, int aDataTotalSize)
{
	if ( !IsConnected())
		Connect();
	if ( !IsConnected() )
		return -1;

	iDumpSendedCode = DUMP_SENDING_STARTING;
	CDumpFileOut queryOut(aData, aDataSize, aDataTotalSize);
	SendOnMyClientSession(EST_Send, ST_MDumpFile, (const unsigned char *) queryOut.GetData(), queryOut.GetDataSize());
	return 0;//requestID;
}

int CServiceWorldManager::IsDumpSendedCode()
{
	return iDumpSendedCode;
}

bool CServiceWorldManager::IsExitCodeReceived()
{	
	return bSendedExitCodeReceived;
}

const wchar_t* CServiceWorldManager::GetClientStringID()
{
	return bsLogin;
}

void CServiceWorldManager::SetSessionStateValue( E_SESSION_STATE_TYPES auState, unsigned short auValue, const wchar_t* apwcComment)
{
	if( auState == CSS_EXIT_CODE)
	{
		bSendedExitCodeReceived = false;
		exitCodeLast = auValue;

		long sessionID = -1;

		if (pContext && pContext->mpComMan)
			 sessionID = pContext->mpComMan->GetSessionID();		

		if ((apwcComment) && (hWnd))
		{
			COPYDATASTRUCT CopyData;
			CopyData.dwData=0;
			CopyData.cbData=wcslen(apwcComment)*sizeof(wchar_t)+2;
			CopyData.lpData= (PVOID)apwcComment;
		
			::SendMessage(hWnd, WM_COPYDATA, COPY_DATA_CODE_COMMENT, (LPARAM)(LPVOID) &CopyData);
		}

		if ((hWnd) && (((auValue >= 2) && (auValue <= 8)) || ((auValue >= 44) && (auValue <= 48)) || ((auValue >= 20) && (auValue <= 25)) || ((auValue >= 36) && (auValue <= 38)) || (sessionID!=-1 && (auValue >= 27 && auValue <= 35))))
		{			
			::PostMessage(hWnd, WM_EXIT_CODE, auValue, 0);
		}		
	}
	ServiceManProtocol::CSetSessionStateValueOut outData( auState, auValue);
	SendOnMyClientSession(EST_SendSignalQuery, ST_SetSessionStateValue, outData.GetData(), outData.GetDataSize());
}

void CServiceWorldManager::SendConnectionRestoredStatus()
{
	SendOnMyClientSession(EST_SendSignalQuery, ST_ConnectionRestoredSuccessfully, NULL,0);
}

void CServiceWorldManager::OnConnectLost()
{
	if(pContext!=NULL && pContext->mpSM!=NULL)
		pContext->mpSM->OnServerDisconnected(rmml::ML_SERVER_MASK_WORLD_SERVICE, NULL);
}

void CServiceWorldManager::OnConnectRestored()
{
	if(pCallbacks!=NULL)
		pCallbacks->onConnectRestored();
	
	if(pContext!=NULL && pContext->mpSM!=NULL) 
		pContext->mpSM->OnServerConnected(rmml::ML_SERVER_MASK_WORLD_SERVICE, true);
}

void CServiceWorldManager::GetComputerConfigurationPacket()
{
	if(pContext==NULL || pContext->mpRM == NULL) 
		return;

	std::wstring compName;
	std::string processName;
	unsigned int processorCount; 
	unsigned int realCoreCount;
	bool hyperthreadingEnabled;
	unsigned __int64 physicalMemorySize;
	unsigned __int64 virtualMemorySize; 
	std::string osVersion;
	unsigned char osBits;

	pContext->mpRM->GetComputerInfo( compName, processName, processorCount, realCoreCount, hyperthreadingEnabled, physicalMemorySize, virtualMemorySize, osVersion, osBits);

	if( pContext->mpLogWriter)
	{
		pContext->mpLogWriter->WriteLn("GetComputerInfo");
		pContext->mpLogWriter->WriteLn("Computer name: ", compName.c_str());
		pContext->mpLogWriter->WriteLn("Physical memory size: ", physicalMemorySize, " bytes");
		pContext->mpLogWriter->WriteLn("Virtual memory size: ", virtualMemorySize, " bytes");
		pContext->mpLogWriter->WriteLn("Processor name: ", processName.c_str());
		pContext->mpLogWriter->WriteLn("Virtual cores count: ", processorCount);
		pContext->mpLogWriter->WriteLn("Physical cores count: ", realCoreCount);
		pContext->mpLogWriter->WriteLn("Hyperthreading support: ", hyperthreadingEnabled ? "yes" : "no");
		pContext->mpLogWriter->WriteLn("OS version: ", osVersion);
		pContext->mpLogWriter->WriteLn("OS bits: x", osBits);
	}

	std::string vendorName;
	std::string deviceName;
	unsigned __int64 videoMemorySize;
	int major;
	int minor;
	pContext->mpRM->GetGPUInfo( vendorName, deviceName, videoMemorySize, major, minor);

	if( pContext->mpLogWriter)
	{
		pContext->mpLogWriter->WriteLn("GetGPUInfo");
		pContext->mpLogWriter->WriteLn("GPU vendor: ", vendorName.c_str());
		pContext->mpLogWriter->WriteLn("GPU device: ", deviceName.c_str());
		pContext->mpLogWriter->WriteLn("Video memory size: ", videoMemorySize, " bytes");
		pContext->mpLogWriter->WriteLn("Driver version: ", major, ".", minor);
	}

	computerInfoPacket.AddComputerInfo( compName, processName, processorCount, realCoreCount, hyperthreadingEnabled, physicalMemorySize, virtualMemorySize, osVersion, osBits);
	computerInfoPacket.AddGPUInfo( vendorName, deviceName, videoMemorySize, major, minor);	
}

void CServiceWorldManager::computerStateThread()
{
	int *pCounter;
	int fpss_qlt_0[60];
	int fpss_qlt_1[60];
	int fpss_qlt_2[60];
	int fpss_qlt_3[60];
	int counter_0 = 0;
	int counter_1 = 0;
	int counter_2 = 0;
	int counter_3 = 0;
	int *fpss;
	std::string sFPS_qlt_0;
	std::string sFPS_qlt_1;
	std::string sFPS_qlt_2;
	std::string sFPS_qlt_3;
	std::string *psFPS;
	int quality = -1;
	while( iComputerStateSending == 2)
	{
		if(pContext==NULL || pContext->mpRM == NULL) 
			return;
		
		char sQuality[255];
		pContext->mpRM->GetUserSetting( L"quality", sQuality, 255);
		int quality = rtl_atoi(sQuality);		
		switch( quality)
		{
		case 0:
			{
				psFPS = &sFPS_qlt_0;
				fpss = (int*)&fpss_qlt_0;
				pCounter = &counter_0;
			}
			break;
		case 1:
			{
				psFPS = &sFPS_qlt_1;
				fpss = (int*)&fpss_qlt_1;
				pCounter = &counter_1;
			}
			break;
		case 2:
			{
				psFPS = &sFPS_qlt_2;
				fpss = (int*)&fpss_qlt_2;
				pCounter = &counter_2;
			}
			break;
		case 3:
			{
				psFPS = &sFPS_qlt_3;
				fpss = (int*)&fpss_qlt_3;
				pCounter = &counter_3;
			}
			break;
		default:
			{
				psFPS = NULL;
				fpss =  NULL;
				pCounter = NULL;
			}
			break;
		}
		if( psFPS == NULL || psFPS->size()>=256)
		{			
			WaitForSingleObject( eventComputerStatsSend, 5000);	
			continue;
		}
		fpss[*pCounter] = pContext->mpRM->GetCurrentFPS();
		(*pCounter)++;
		if( (*pCounter) > 59)
		{											
			int fps = 0;
			for(int i = 0; i < (*pCounter); i++)
				fps += fpss[ i];
			fps /= *pCounter;
			char oneFps[10];
			_itoa_s(fps, oneFps, 10, 10);	

			if( psFPS->size()>0)
				(*psFPS) += ", ";
			(*psFPS) += oneFps;

			DWORD dwProcessMainID = GetCurrentProcessId();	
			HANDLE hProcess = OpenProcess(PROCESS_VM_READ  | PROCESS_QUERY_INFORMATION , false, dwProcessMainID);
			unsigned int currentPhysicalMemory = 0;
			unsigned int currentVirtualMemory = 0;
			if (hProcess != INVALID_HANDLE_VALUE && hProcess != NULL)
			{
				//GetProcessWorkingSetSizeEx(hProcess, &dwMin, &dwMax, QUOTA_LIMITS_HARDWS_MAX_ENABLE | QUOTA_LIMITS_HARDWS_MIN_DISABLE));
				PROCESS_MEMORY_COUNTERS psmemCounters;
				GetProcessMemoryInfo(hProcess, &psmemCounters, sizeof(PROCESS_MEMORY_COUNTERS));
				currentPhysicalMemory = psmemCounters.WorkingSetSize;
				currentVirtualMemory = psmemCounters.PagefileUsage;
				CloseHandle(hProcess);
			}
			
				
								
			ServiceManProtocol::ComputerStateOut outData( psFPS->c_str(), quality, currentPhysicalMemory, currentVirtualMemory);
			SendOnMyClientSession(EST_Send, ST_ComputerState, outData.GetData(), outData.GetDataSize());
			*pCounter = 0;
			//if( sFPS.size() >= 256)
			//{
			//	bComputerStateSending = false;
			//	break;
			//}
		}
		WaitForSingleObject( eventComputerStatsSend, 1000);		
	}
}

void CServiceWorldManager::SetWndHandle(HWND ahWnd)
{ 
	hWnd = ahWnd;
}

DWORD WINAPI CServiceWorldManager::computerStateThread_(LPVOID param)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();

	CServiceWorldManager* aParams = (CServiceWorldManager*)param;

	if(aParams != NULL)
		aParams->computerStateThread();	
	return 0;}

// Заходили под отладкой.
bool CServiceWorldManager::IsDebuggerPresent()
{
	HMODULE hDll = ::LoadLibrary( "kernel32.dll");
	bool (*IsDebuggerPresent)();
	(FARPROC&)IsDebuggerPresent = GetProcAddress(hDll, "IsDebuggerPresent");
	return IsDebuggerPresent();
}