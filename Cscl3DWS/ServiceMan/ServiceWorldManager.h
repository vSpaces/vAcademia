#pragma once
#include "serviceman.h"
#include "imessagehandler.h"
#include "iclientsession.h"
#include "ServiceManager.h"

#include "ComputerConfigurationOut.h"

using namespace service;

#define RT_ExecLoginMethodResults		5  // результат авторизации
#define RT_ExecLogout					6  // отмена авторизации
#define RT_ExecSetSessionState			8  // результат сессии установлен
#define RT_ExecComputerConfiguration	9  // конфигурация компьтюера сохранена на сервере
#define RT_NotifyTOInfo					10 // информация о ТО сервера
#define RT_ExecDumpParams				11 // информация о параметрах дампа сохранена на сервере
#define RT_ExecDumpFile					12 // информация о файле дампа 9части или всего) сохранена на сервере

class CServiceWorldManager;
class CReportMan;

typedef struct __loginParams
{
	CServiceWorldManager *server;
	MP_WSTRING login;
	MP_WSTRING password;
	MP_WSTRING encodingPassword;
	MP_WSTRING computerJeyCode;
	MP_WSTRING authKey;
	MP_WSTRING updaterKey;
	MP_WSTRING buildType;
	int requestID;
	int source;
	bool anonymous;
	bool isEncodingPassword;

	__loginParams();
} _loginParams;

class CServiceWorldManager : 
	public IServiceWorldManager,
	public CServiceManagerBase,
	public cs::imessagehandler,
	public cs::isessionlistener
{
public:
	CServiceWorldManager( omsContext* aContext);
	virtual ~CServiceWorldManager();

	void GetComputerConfigurationPacket();

	// ----------------------------------------------------	
	// Реадизация ф-й интерфейса IBaseServiceManager
	// ----------------------------------------------------	
	// Подключись к серверу сервисов
	virtual void Connect();
	// Отключись от сервера сервисов
	virtual void Disconnect();
	// Удалить менеджер синхронизации
	virtual void Release();
	// Получить описание 
	virtual omsresult GetJSONService(unsigned int aRmmlID, const wchar_t* apwcServiceName);
	// Установить интерфейс, принимающий обратные вызовы от SyncManager-а
	virtual bool SetCallbacks(IServiceManCallbacks* apCallbacks);
	// Вызвать метод
	virtual omsresult ExecMethod(int requestID, unsigned int rmmlID, const wchar_t* apwcJSONMethodDescription);
	// Подтвердить регистрацию сервиса на клиенте
	virtual omsresult RegisterService(int pendingID, unsigned int rmmlID);
	/*// отослать на сервер, не отосланные сообщения сервисов
	virtual void SendAllNotSendingServiceMessage();*/
	// удалить самого себя
	virtual void Destroy();

	bool IsConnected();

	// ----------------------------------------------------	
	// Реадизация ф-й интерфейса isessionlistener
	// ----------------------------------------------------	
	virtual void OnConnectLost();
	virtual void OnConnectRestored();


	BEGIN_IN_MESSAGE_MAP()
		IN_MESSAGE2( RT_ExecLoginMethodResults, OnExecLoginMethodResults)
		IN_MESSAGE2( RT_ExecLogout, OnExecLogout)
		IN_MESSAGE2( RT_ExecSetSessionState, OnExecSetSessionState)		

		IN_MESSAGE2( RT_GetService, OnGetService)
		IN_MESSAGE2( RT_ExecMethodResults, OnExecMethodResults)
		IN_MESSAGE2( RT_GetServiceByMethod, OnGetServiceByMethod)
		IN_MESSAGE2( RT_Notification, OnServiceSendNotify)
		IN_MESSAGE2( RT_ServiceLoadingFailed, OnServiceLoadingFailed)
		IN_MESSAGE2( RT_NotifyTOInfo, OnNotifyTOInfo)
		IN_MESSAGE2( RT_ExecComputerConfiguration, OnExecComputerConfiguration)
		IN_MESSAGE2( RT_ExecDumpParams, OnExecDumpParams)
		IN_MESSAGE2( RT_ExecDumpFile, OnExecDumpFile)
	END_IN_MESSAGE_MAP()

	// ----------------------------------------------------	
	// Реадизация ф-й интерфейса IServiceWorldManager
	// ----------------------------------------------------	
	int Login(const wchar_t* apwcLogin, const wchar_t* apwcPassword, bool isEncodingPassword, const wchar_t* apwcEncodingPassword, const wchar_t* apwcComputerJeyCode, bool isAnonymous, int requestID, int source, const wchar_t* apwcAuthKey, const wchar_t* apwcUpdaterKey, const wchar_t *apwcBuildType);
	int Logout( int iCodeError);
	int LoginAsync( int requestID, const wchar_t* apwcLogin, const wchar_t* apwcPassword, bool isAnonymous = false, int source = -1, bool isEncodingPassword = false, const wchar_t* apwcAuthKey = NULL, const wchar_t* apwcUpdaterKey = NULL);
	const wchar_t* GetClientStringID();
	bool IsExitCodeReceived();
	void SetSessionStateValue( E_SESSION_STATE_TYPES auState, unsigned short auValue, const wchar_t* apwcComment = NULL);
	void SendConnectionRestoredStatus();
	void OnExecLoginMethodResults( BYTE* aData, int aDataSize);
	void OnExecLogout( BYTE* aData, int aDataSize);
	void OnNotifyTOInfo( BYTE* aData, int aDataSize);
	void OnExecSetSessionState( BYTE* aData, int aDataSize); 
	void OnExecComputerConfiguration( BYTE* aData, int aDataSize){};
	void OnExecDumpParams( BYTE* aData, int aDataSize);
	void OnExecDumpFile( BYTE* aData, int aDataSize);

	int SendMDumpParams( BYTE* aData, int aDataSize);
	int SendMDumpFile( BYTE* aData, int aDataSize, int aDataTotalSize);
	int IsDumpSendedCode();
	
	void computerStateThread();

	void SetWndHandle(HWND ahWnd);
	bool IsDebuggerPresent();
protected:
	CComBSTR bsLogin;
	bool bSendedExitCodeReceived;
	int exitCodeLast;
	int iDumpSendedCode;
//	int idRequest;
	ServiceManProtocol::ComputerConfigurationOut computerInfoPacket;

private:
	static DWORD WINAPI loginThread_(LPVOID param);	
	static DWORD WINAPI computerStateThread_(LPVOID param);
	bool bDestroying;
	int iComputerStateSending;
	HANDLE eventComputerStatsSend;
	HWND hWnd;
};

