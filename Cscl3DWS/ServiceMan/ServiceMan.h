#ifndef __SERVICEMAN_H_
#define __SERVICEMAN_H_

#ifdef SERVICEMAN_EXPORTS
#define SERVICEMAN_API __declspec(dllexport)
#else
#define SERVICEMAN_API __declspec(dllimport)
#endif

#ifdef WIN32
#define SERVICEMAN_NO_VTABLE __declspec(novtable)
#else
#define SERVICEMAN_NO_VTABLE
#endif

#include "oms_context.h"
#include "rmml.h"

// ---------------------
// Принимаемые сообщения
// ---------------------

#define RT_GetService					1  // Получено JSON описание сервиса
#define RT_ExecMethodResults  			2  // Результат выполнения метода
#define RT_GetServiceByMethod			3  // Получено JSON описание сервиса из метода
#define RT_Notification					4  // Нотификация с сервера
#define RT_ServiceLoadingFailed			7 // ошибка загрузки сервиса
#define RT_PreviewServerRequest			5 // получение имени preview сервера
#define RT_VoipServerRequest			6 // получение имени voip сервера
#define RT_VoipServerForRecordRequest	8 // получение имени voip сервера по номеру записи
#define RT_SharingServerRequest			9 // получение имени sharing сервера

#define RT_IPadSessionStarted			10 // начата сессия на IPad
#define RT_IPadSessionStoped			11 // закрыта сессия на IPad
#define RT_IPadSessionStatus			12 // статус сессии на IPad
#define RT_IPadSessionBinState			13 // получено состояние объекта
#define RT_IPadSessionChangeSlide		14 // изменить слайд

//#define RT_PreviewServersRequest		10 // получение имен preview серверов
//#define RT_VoipServersRequest			11 // получение имен voip серверов

// -------------------------------------
// Send Type - типы посылаемых сообщений
// -------------------------------------
#define ST_GetService					1  // Получить с сервера JSON описалово сервиса
#define ST_ExecMethod					2  // Выполнить метод
#define ST_RegisterService				3  // Подтвердить регистрацию сервиса на клиенте
#define ST_PreviewServerRequest			4 // Запросить адрес Preview сервер 
#define ST_VoipServerRequest			5 // Запросить адрес Voip сервер 
#define ST_RegisterRecord				6 // Зарегистрировать запись на сервере
// устарела #define ST_UnregisterRecord				7 // Удалить данные о записи 
#define ST_VoipServerForRecordRequest	8 // Получить инфу о сервере на котором можно воспроизвести новую запись
#define ST_SetSplashSrc					9 // Установить splashSrc
#define ST_SharingServerRequest			11 // Запросить адрес Sharing сервер 
#define ST_SendErrorMessageToAdmin		12
#define ST_SendIPadChangeToken			13
#define ST_SendIPadObjectFullState		14
#define ST_SendIPadSlideSrcChangedOut	15
#define ST_SendIPadLogoutOut			16

//#define ST_PreviewServersRequest		12 // Запросить адрес Preview сервер 
//#define ST_VoipServersRequest			13 // Послать администратору сообщение 

namespace service
{

	// Received Type - типы принемаемых сообщений

	struct SERVICEMAN_NO_VTABLE IServiceManCallbacks
	{
		// получена json реализация с сервера
		virtual void onGetService(unsigned int rmmlID, const wchar_t* apwcServiceName, const wchar_t* apwcServiceDescription, int* apiErrorCode=NULL ) = 0;
		
		// Получен ответ от метода
		virtual void onMethodComplete(int aRequestID, int aRMMLID, const wchar_t* apwcResponse=NULL, bool isService=false, int aiErrorCode=0) = 0;
		virtual void onServiceNotify(int aiID, const wchar_t* apwcResponse, int* apiErrorCode) = 0;
		virtual void onConnectRestored() = 0;
	};

	struct SERVICEMAN_NO_VTABLE IBaseServiceManager
	{
		// Подключись к серверу сервисов
		virtual void Connect() = 0;
		// Отключись от сервера сервисов
		virtual void Disconnect() = 0;
		// Удалить менеджер синхронизации
		virtual void Release() = 0;
		// Получить описание 
		virtual omsresult GetJSONService(unsigned int aRmmlID, const wchar_t* apwcServiceName) = 0;
		// Вызвать метод
		virtual omsresult ExecMethod(int requestID, unsigned int rmmlID, const wchar_t* apwcJSONMethodDescription);
		// Установить интерфейс, принимающий обратные вызовы от SyncManager-а
		virtual bool SetCallbacks(IServiceManCallbacks* apCallbacks)=0;
		// Подтвердить регистрацию сервиса на клиенте
		virtual omsresult RegisterService(int pendingID, unsigned int rmmlID)=0;
		/*// отослать на сервер, не отосланные сообщения сервисов
		virtual void SendAllNotSendingServiceMessage()=0;*/
		// удалить самого себя
		virtual void Destroy() = 0;
	};

	struct SERVICEMAN_NO_VTABLE IServiceManager : public IBaseServiceManager
	{
		// Получить адрес Preview сервера
		virtual omsresult SendPreviewServerRequest( unsigned int aiRecordID)=0;
		// Получить адрес Voip сервера
		virtual omsresult SendVoipServerRequest( const char* alpcRoomName)=0;
		// Получить адрес Sharing сервера
		virtual omsresult SendSharingServerRequest( const char* alpcRoomName)=0;
		// Зарегистировать запись голосового сервера
		virtual omsresult SendVoipRecordRegisterRequest( const char* alpcServerIP, const char* alpcRoomName, unsigned int aiRecordID)=0;
		// Получить параметры голосового сервера, на котором была запись aiRecordID и 
		// параметры голосового сервера, на котором можно воспроизвести эту запись
		virtual omsresult SendVoipGetPlayServerRequest( unsigned int aiPlayRealityID,
														unsigned int aiPlayingRecordID)=0;
		// установить splashSrc
		virtual void SetSplashSrc( unsigned int aiRecordID, const char* alpcFileName)=0;

		virtual void SendErrorMessageToAdmin( const wchar_t *apErrorText)=0;

		virtual void SendIPadChangeToken( unsigned int aToken)=0;
		virtual void SendIPadObjectFullState(  unsigned int aPacketID, unsigned int aToken, BYTE* aBinState, int aBinStateSize)=0;
		virtual void SendIPadSlideSrcChangedOut( unsigned int aPacketID, unsigned int aToken, float aProportion, float aScaleX, float aScaleY, const wchar_t *aSrc)=0;
		virtual void SendIPadLogoutOut( int aCode)=0;

		// Получить адрес Preview сервера
		//virtual omsresult SendPreviewServersRequest()=0;
		// Получить адрес Voip сервера
		//virtual omsresult SendVoipServersRequest()=0;
	};

	struct SERVICEMAN_NO_VTABLE IIPadManager
	{
		virtual void StartSession(  wchar_t *aName, unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID, void *aImgObject) = 0;
		virtual void DestroySession() = 0;
		virtual void OnIPadMessage( BYTE aType, BYTE* aData, int aDataSize) = 0;
		virtual bool IsIPadObject( wchar_t *aName, unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID);		
		virtual void SendForceFullState() = 0;
		virtual void SetIPadImageParam( const wchar_t *apSrc, float aProportion, float aScaleX, float aScaleY) = 0;
		virtual void OnConnectRestored() = 0;
		virtual void Logout() = 0;
		virtual void UpdateIPadObjects() = 0;
	};

	enum E_SESSION_STATE_TYPES
	{ 
		CSS_CLIENT_WORK_STATE = 0,
		CSS_DEADLOCKS_COUNT,
		CSS_CRASH_WINDOW_SHOWN,
		CSS_EXIT_CODE
	};

	enum E_DUMP_SENDING_STATE
	{
		DUMP_SENDING_STARTING = 0,
		DUMP_SENDED,
		DUMP_NO_SENDED,
		DUMP_ERROR,
	};

#define COPY_DATA_CODE_PASSWORD	3
#define COPY_DATA_CODE_LOGIN	4
#define COPY_DATA_CODE_COMMENT	5
#define COPY_DATA_CODE_VERSION	6
#define COPY_DATA_VJS			7
#define COPY_DATA_EXIT			8

	struct SERVICEMAN_NO_VTABLE IServiceWorldManager: public IBaseServiceManager 
	{
		virtual int Login( const wchar_t* apwcLogin, const wchar_t* apwcPassword, bool isEncodingPassword, const wchar_t* apwcEncodingPassword, const wchar_t* apwcComputerJeyCode, bool isAnonymous, int requestID, int source, const wchar_t* apwcAuthKey, const wchar_t* apwcUpdaterKey, const wchar_t* apwcBuildType) = 0;
		virtual int Logout( int iCodeError) = 0;
		virtual int LoginAsync( int requestID, const wchar_t* apwcLogin, const wchar_t* apwcPassword, bool isAnonymous, int source = -1, bool isEncodingPassword = false, const wchar_t* apwcAuthKey = NULL, const wchar_t* apwcUpdaterKey = NULL) = 0;
		virtual const wchar_t* GetClientStringID()=0;
		virtual bool IsExitCodeReceived()=0;
		virtual bool IsDebuggerPresent()=0;

		/** Передает текущий статус клиентской сессии пользователя на сервер 
		\param unsigned int auState - идентификатор состояния
		CSS_ - client session state
		CSS_CLIENT_WORK_STATE = 0 - этап загрузки
		CSS_DEADLOCKS_COUNT = 1 - количество подвисаний
		CSS_CRASH_WINDOW_SHOWN = 2 - показалось окно краша
		CSS_CLOSE_CODE = 3 - корректный выход

		\param unsigned int auValue - значение состояния
		1 байт - номер этапа загрузки
		2 байт - количество подвисаний
		3 байт - флаг краша / показалось окно
		4 байт - флаг корректного закрытия сессии
		\return void
		\details
		*/
		virtual void SetSessionStateValue( E_SESSION_STATE_TYPES auState, unsigned short auValue, const wchar_t* apwcComment = NULL)=0;
		virtual void SendConnectionRestoredStatus()=0;
		virtual int SendMDumpParams( BYTE* aData, int aDataSize) = 0;
		virtual int SendMDumpFile( BYTE* aData, int aDataSize, int aDataTotalSize) = 0;
		virtual int IsDumpSendedCode() = 0;
		virtual void SetWndHandle(HWND ahWnd) = 0;
	};


	SERVICEMAN_API omsresult CreateServiceManager( oms::omsContext* aContext);
	SERVICEMAN_API void DestroyServiceManager( oms::omsContext* aContext);
	SERVICEMAN_API omsresult CreateServiceWorldManager( oms::omsContext* aContext);
	SERVICEMAN_API void DestroyServiceWorldManager( oms::omsContext* aContext);
}
#endif
