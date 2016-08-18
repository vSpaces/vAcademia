// Errors.h
#pragma once

#define	BEGIN_ERROR_MAP		switch( a_dwErrorCode)	{	
#define	END_ERROR_MAP		}return SERROR_UNKNOWN;

#define	DESKTOPMAN_ERROR	0x80000000

#define	DECLARE_ERROR( b, c, v, s)	\
	static const DWORD		ERROR_##c	=	b | v;	\
	static const wchar_t*	SERROR_##c	=	s;

#define	HANDLE_ERROR( c)		case ERROR_##c: return SERROR_##c;

//////////////////////////////////////////////////////////////////////////
// Common errors
DECLARE_ERROR( DESKTOPMAN_ERROR, NOERROR,				0, L"No error")							// Нет ошибки
DECLARE_ERROR( DESKTOPMAN_ERROR, UNKNOWN,				1, L"Unknown error")					// Неизвестная ошибка
DECLARE_ERROR( DESKTOPMAN_ERROR, CONNECTION_FAILED,		2, L"Connect to server failed")			// Не удалось подключиться к серверу
DECLARE_ERROR( DESKTOPMAN_ERROR, NO_SELECTOR,			3, L"No selector")						// Не установлен интерфейс получения изображения
DECLARE_ERROR( DESKTOPMAN_ERROR, RUNNING_ALREADY,		4, L"Running already")					// Получение изображения уже идет

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

const wchar_t*	GetExportErrorString(DWORD a_dwErrorCode)
{
	return CError::GetErrorString(a_dwErrorCode);
}
//////////////////////////////////////////////////////////////////////////
// error CComStrings