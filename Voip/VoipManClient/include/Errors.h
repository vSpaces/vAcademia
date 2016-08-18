// Errors.h
#pragma once

#define	BEGIN_ERROR_MAP		switch( a_dwErrorCode)	{	
#define	END_ERROR_MAP		}return SERROR_UNKNOWN;

#define	VOIPMAN_ERROR	0x80000000
#define	PTLIB_ERROR		0x40000000
#define	OPENH323_ERROR	0x20000000

#define	DECLARE_ERROR( b, c, v, s)	\
	static const DWORD		ERROR_##c	=	b | v;	\
	static const wchar_t*	SERROR_##c	=	s;

#define	HANDLE_ERROR( c)		case ERROR_##c: return SERROR_##c;

//////////////////////////////////////////////////////////////////////////
// Common errors
DECLARE_ERROR( VOIPMAN_ERROR, NOERROR,				0, L"No error")							// Нет ошибки
DECLARE_ERROR( VOIPMAN_ERROR, UNKNOWN,				1, L"Unknown error")					// Неизвестная ошибка
DECLARE_ERROR( VOIPMAN_ERROR, NOTINITIALIZED,		3, L"VOIP system is not initialized")	// Система не инициализирована
DECLARE_ERROR( VOIPMAN_ERROR, CONNECTION_FAILED,	4, L"Connect to server failed")			// Не удалось подключиться к серверу
DECLARE_ERROR( VOIPMAN_ERROR, CONTEXT_MISSED,		5, L"Context not defined")				// Не установлен контекст
DECLARE_ERROR( VOIPMAN_ERROR, ALREADYINITIALIZED,	6, L"Already initialized")				// Система уже инициализирована
DECLARE_ERROR( VOIPMAN_ERROR, DEFAULTS_USED,		7, L"Default devices used")				// Установлены текущие устройства воспроизведения
DECLARE_ERROR( VOIPMAN_ERROR, SR_NOT_AVAILABLE_DURING_RECORDING,		8, L"Self voice recording is not available during class recording")
DECLARE_ERROR( VOIPMAN_ERROR, SR_MODE_NOT_INITIALIZED,		9, L"Self voice recording mode is not active")
DECLARE_ERROR( VOIPMAN_ERROR, SR_CHANNEL_NOT_CONNECTED,		10, L"VOIP is not connected to self recording channel")
DECLARE_ERROR( VOIPMAN_ERROR, SR_CLIENT_NOT_LOADED,		11, L"VOIP server side client is not loaded")


//////////////////////////////////////////////////////////////////////////
// PtLib errors

//////////////////////////////////////////////////////////////////////////
// OpenH323 errors
class CError
{
public:
	CError()
	{ 
		ErrorCode = ERROR_NOERROR;
	}
	~CError(){}

public:
	static const wchar_t*	GetErrorString(DWORD a_dwErrorCode)
	{
		BEGIN_ERROR_MAP
			// Common errors
			HANDLE_ERROR( NOERROR);
			// PtLib errors
			// OpenH323 errors
			// Default errors
			HANDLE_ERROR( UNKNOWN);
		END_ERROR_MAP
	}
	void	SetErrorCode(DWORD a_dwCode)
	{
		ErrorCode = a_dwCode;
	}

public:
	DWORD	ErrorCode;
};

/*const wchar_t*	GetExportErrorString(DWORD a_dwErrorCode)
{
	return CError::GetErrorString(a_dwErrorCode);
}*/
//////////////////////////////////////////////////////////////////////////
// error CComStrings