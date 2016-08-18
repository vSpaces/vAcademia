#include "stdafx.h"
#include ".\commandlineparams.h"
#include <atlbase.h>
#include "StringFunctions.h"
#include "func.h"

inline bool IsSpace(char ach){
	if(ach == ' ' || ach == '\t') return true;
	return false;
}

inline bool IsDigit(char ach){
	if(ach>='0' && ach<='9') return true;
	return false;
}

bool IsDelim(char ach){
	switch(ach){
	case '.': case ',':	case ':': case ';': case '!': case '?': 
	case '-': case '+': case '*': case '/': case '=': case '<': case '>': 
	case '@': case '#': case '$': case '%': case '&': case '^': 
	case '\\': case '|': 
	case '\'': case '\"': 
	case '(': case ')': case '[': case ']':	case '{': case '}': 
		return true;
	}
	return false;
}

bool IsQuote(char ach){
	switch(ach){
	case '\'': case '"':	case '`': 
		return true;
	}
	return false;
}

bool IsQuote(wchar_t ach){
	switch(ach){
	case L'\'': case L'"':	case L'`': 
		return true;
	}
	return false;
}

CCommandLineParams::CCommandLineParams(void):
	MP_VECTOR_INIT(args),	
	MP_STRING_INIT(msAppName),	
	MP_STRING_INIT(msCrashType),	
	MP_WSTRING_INIT(msRoot),	
	MP_STRING_INIT(msUserName),
	MP_STRING_INIT(msPassword)	
{
	ResetParamsFlags();
}

CCommandLineParams::~CCommandLineParams(void)
{
}

void CCommandLineParams::SetCommandLine(LPCSTR alpcStr)
{
	ResetParamsFlags();

	TCHAR slog[ MAX_PATH_4];
	sprintf( slog, "Arguments: '%s'\n", alpcStr);
	WriteLog__( slog);

	// разбираем все аргументы (все что разделено пробелом, не считая того, что в двойных кавычках)
	// и складываем их в gvArgs
	bool bInQuotes = false; // пропускаем кавычки
	bool bPrevWasSpace = true; // предыдущий символ был пробельным
	LPCSTR pchArgBegin = alpcStr;
	LPCSTR pch;
	for(pch = alpcStr; *pch != '\0'; pch++)
	{
		// если находимся в кавычках, 
		if(bInQuotes){
			// то все пропускаем 
			if(*pch == '"')
				bInQuotes = false;
			bPrevWasSpace = false;
			continue;
		}
		if(*pch == '"')
			bInQuotes = true;
		// если пробел
		if(IsSpace(*pch)){
			// и до этого что-то было непробельное
			// то значит у нас "готов" новый аргумент
			if(!bPrevWasSpace)
			{
				SaveNextParam( pchArgBegin, pch);
			}
			pchArgBegin = pch;
			bPrevWasSpace = true;
		}else{
			if(bPrevWasSpace)
				pchArgBegin = pch;
			bPrevWasSpace = false;
		}
	}
	// если добрались до конца, то последний аргумент тоже надо сохранить
	if(!bPrevWasSpace)
	{
		SaveNextParam( pchArgBegin, pch);
	}

	ParseCommandLine();
}

void CCommandLineParams::SaveNextParam( LPCSTR pchArgBegin, LPCSTR pch)
{
	ATLASSERT( pchArgBegin);
	ATLASSERT( *pchArgBegin);
	ATLASSERT( pch);
	if( !pchArgBegin)
		return;
	if( !*pchArgBegin)
		return;
	if( !pch)
		return;

	int iLen = pch - pchArgBegin;
	if( iLen > 0)
	{
		std::string param;
		char tmp[2];
		tmp[1] = 0;
		for (int i = 0; i < iLen; i++)
		{
			tmp[0] = pchArgBegin[i];
			param += tmp;
		}
		args.push_back(MAKE_MP_STRING(param));
	}
}

std::wstring FormatRoot(std::wstring& asRoot)
{
	std::wstring ret = asRoot;
	while( ret.size() > 0 && IsQuote( ret[0]))
		ret = ret.substr( 1, ret.size()-1);
	while( ret.size() > 0 && IsQuote( ret[ret.size()-1]))
		ret = ret.substr( 0, ret.size()-1);

	StringTrimLeft(ret);
	StringTrimRight(ret);	
	return ret;
}

// Разбор параметров
void CCommandLineParams::ParseCommandLine()
{
	TCHAR slog[ MAX_PATH_4];
	
	for(int i = 0; i < args.size(); i++)
	{
		MP_STRING& arg = args[i];
		arg = StringToLower(arg);
		
		if( arg == "-app_name")
		{
			if( i < args.size() - 1)
			{
				i++;
				msAppName = args[i];
				if ( msAppName.find( '"') == 0 || msAppName.find( '\'') == 0)
				{
					// кавычки убираем из пароля.
					msAppName = msAppName.substr( 1, msAppName.length() - 2);
				}
				mbAppNameIsSet = true;
				sprintf( slog, "-app_name = '%s'\n", msAppName.c_str());
				WriteLog__( slog);
			}
		}
		else	if( arg == "-crash_type")
		{
			if( i < args.size() - 1)
			{
				i++;
				msCrashType = args[i];
				mbCrashTypeIsSet = true;
				sprintf( slog, "-crash_type = '%s'\n", msCrashType.c_str());
				WriteLog__( slog);
			}
		}
		else	if( arg == "-root")
		{
			if( i < args.size() - 1)
			{
				i++;
				USES_CONVERSION;
				msRoot = A2W(args[i].c_str());
				msRoot = FormatRoot( msRoot);
				mbRootIsSet = true;				
				sprintf( slog, "-root = '%s'\n", W2A(msRoot.c_str()));
				WriteLog__( slog);
			}
		}		
		else	if( arg == "-user")
		{
			if( i < args.size() - 1)
			{
				i++;
				msUserName = ( args[i]);
				mbUserNameSet = true;
				sprintf( slog, "-user = '%s'\n", msUserName.c_str());
				WriteLog__( slog);
			}
		}
		else	if( arg == "-password")
		{
			if( i < args.size() - 1)
			{
				i++;
				msPassword = ( args[i]);
				if ( msPassword.find( '"') == 0 || msPassword.find( '\'') == 0)
				{
					// кавычки убираем из пароля.
					msPassword = msPassword.substr( 1, msPassword.length() - 2);
				}
				mbPasswordSet = true;
				sprintf( slog, "-password = '%s'\n", msPassword.c_str());
				WriteLog__( slog);
			}
		}
		else	if( arg == "-encoding")
		{
			if( i < args.size() - 1)
			{
				i++;
				miPasswordEncodingMode = rtl_atoi( args[i].c_str());
				mbPasswordEncodingModeIsSet = true;
				sprintf( slog, "-encoding = '%d'\n", miPasswordEncodingMode);
				WriteLog__( slog);
			}
		}
		else	if( arg == "-allow_restart_app")
		{
			if( i < args.size() - 1)
			{
				i++;
				miAllowRestartApp = rtl_atoi( args[i].c_str());
				mbAllowRestartAppIsSet = true;
				sprintf( slog, "-allow_restart_app = '%d'\n", miAllowRestartApp);
				WriteLog__( slog);
			}
		}
		else	if( arg == "-send_crash_show")
		{
			if( i < args.size() - 1)
			{
				i++;
				miSendCrashShow = rtl_atoi( args[i].c_str());
				mbSendCrashShowIsSet = true;
				sprintf( slog, "-send_crash_show = '%d'\n", miSendCrashShow);
				WriteLog__( slog);
			}
		}

		
	}
}

void CCommandLineParams::ResetParamsFlags()
{
	mbAppNameIsSet = false;
	mbCrashTypeIsSet = false;
	mbRootIsSet = false;	
	mbUserNameSet = false;
	mbPasswordSet = false;	
	mbPasswordEncodingModeIsSet = false;
	mbAllowRestartAppIsSet = false;
	mbSendCrashShowIsSet = false;

	args.clear();
}