#include "stdafx.h"
#include ".\commandlineparams.h"
#include <atlbase.h>
#include "StringFunctions.h"

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


CCommandLineParams::CCommandLineParams(void):
	MP_VECTOR_INIT(args),
	MP_STRING_INIT(msModule),
	MP_STRING_INIT(msScene),
	MP_STRING_INIT(msUrl),
	MP_WSTRING_INIT(msRoot),
	MP_STRING_INIT(msLauncherEventName),
	MP_STRING_INIT(msUserName),
	MP_STRING_INIT(msPassword),
	MP_STRING_INIT(msTestResDir),
	MP_STRING_INIT(msUpdaterKey),
	MP_STRING_INIT(msVersion),
	MP_STRING_INIT(msLanguage)
	//MP_WSTRING_INIT(msClientPath)
{
	ResetParamsFlags();
}

CCommandLineParams::~CCommandLineParams(void)
{
}

void CCommandLineParams::SetCommandLine(LPCSTR alpcStr)
{
	ResetParamsFlags();

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

std::string FormatRoot(std::string& asRoot)
{
	std::string ret = asRoot;
	while( ret.size() > 0 && IsQuote( ret[0]))
		ret = ret.substr( 1, ret.size()-1);
	while( ret.size() > 0 && IsQuote( ret[ret.size()-1]))
		ret = ret.substr( 0, ret.size()-1);

	StringTrimLeft(ret);
	StringTrimRight(ret);	
	return ret;
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

std::string UnquoteParam( const std::string& asParam)
{
	std::string result = asParam;

	int auStart = 0;
	int auLength = result.size();

	if( auLength == 0)
		return asParam;

	// первый символ
	if( IsQuote( *result.begin()))
	{
		auStart = 1;
		auLength -= 1;
	}

	// последний символ
	if( IsQuote( *result.rbegin()))
	{
		auLength -= 1;
	}

	// кавычки убираем из пароля.
	result = result.substr( auStart, auLength);
	return result;
}

bool BoolParam( const std::string& asParam)
{
	std::string unquotedBoolean = UnquoteParam( asParam);

	if( unquotedBoolean == "1" || unquotedBoolean == "true")
		return true;
	return false;
}
// Разбор параметров
void CCommandLineParams::ParseCommandLine()
{
	USES_CONVERSION;
	for(int i = 0; i < args.size(); i++)
	{
		MP_STRING& arg = args[i];
		arg = StringToLower(arg);
		
		if( arg == "-url")
		{
			if( i < args.size() - 1)
			{
				i++;
				msUrl = args[i];
				mbUrlIsSet = true;
			}
		}
		else	if( arg == "-root")
		{
			if( i < args.size() - 1)
			{
				i++;
				msRoot = A2W(  args[i].c_str());
				msRoot = FormatRoot( msRoot);
				mbRootIsSet = true;
			}
		}
		else	if( arg == "-debug")
		{
			mbDebugIsSet = true;
		}
		else	if( arg == "-protocolaunch")
		{
			mbProtocolLaunchIsSet = true;
		}
		else	if( arg == "-launcher")
		{
			if( i < args.size() - 1)
			{
				i++;
				msLauncherEventName = ( args[i]);
				mbLauncherEventSet = true;
			}
		}
		else	if( arg == "-processid")
		{
			if( i < args.size() - 1)
			{
				i++;
				miLauncherProcessId = rtl_atoi(( args[i].c_str()));
				mbLauncherProcessIdIsSet = true;
			}
		}
		else	if( arg == "-user")
		{
			if( i < args.size() - 1)
			{
				i++;
				msUserName = UnquoteParam( args[i]);
				mbUserNameSet = true;
			}
		}
		else	if( arg == "-password")
		{
			if( i < args.size() - 1)
			{
				i++;
				msPassword = UnquoteParam( args[i]);
				mbPasswordSet = true;
			}
		}
		else	if( arg == "-language")
		{
			if( i < args.size() - 1)
			{
				i++;
				msLanguage = UnquoteParam( args[i]);
				mbLanguageSet = true;
			}
		}
		else	if( arg == "-required")
		{
			if( i < args.size() - 1)
			{
				i++;
				mbRequired = BoolParam( args[i]);
			}
		}
		else	if( arg == "-broken")
		{
			if( i < args.size() - 1)
			{
				i++;
				mbBroken = BoolParam( args[i]);
			}
		}
		else	if( arg == "-setup")
		{
			if( i < args.size() - 1)
			{
				i++;
				mbSetup = BoolParam( args[i]);
			}
		}
		else	if( arg == "-version")
		{
			if( i < args.size() - 1)
			{
				i++;
				msVersion = UnquoteParam( args[i]);
				mbVersionSet = true;				
			}
		}
		else	if( arg == "-encoding")
		{
			if( i < args.size() - 1)
			{
				i++;
				miPasswordEncodingMode = rtl_atoi( args[i].c_str());
				mbPasswordEncodingModeIsSet = true;
			}
		}
		else	if( arg == "-test")
		{
			if( i < args.size() - 1)
			{
				i++;
				miTestMode = rtl_atoi( args[i].c_str());
				mbTestModeIsSet = true;
			}
		}
		else	if( arg == "-testresdir")
		{
			if( i < args.size() - 1)
			{
				i++;
				msTestResDir = args[i];
				mbTestResDirIsSet = true;
			}
		}
		else	if( arg == "-updaterkey")
		{
			if( i < args.size() - 1)
			{
				i++;
				msUpdaterKey = UnquoteParam( args[i]);
				mbUpdaterKeyIsSet = true;				
			}
		}
		else	if( arg == "-openmodule")
		{
			if( i < args.size() - 1)
			{
				i++;
				msModule = UnquoteParam( args[i]);
				mbModuleIsSet = true;				
			}
		}
		else	if( arg == "-openscene")
		{
			if( i < args.size() - 1)
			{
				i++;
				msScene = args[i];
				msScene = UnquoteParam( args[i]);
				mbSceneIsSet = true;				
			}
		}
	}
}

void CCommandLineParams::ResetParamsFlags()
{
	mbRootIsSet = false;
	mbUrlIsSet = false;
	mbModuleIsSet = false;
	mbSceneIsSet = false;
	mbUserNameSet = false;
	mbPasswordSet = false;
	mbDebugIsSet = false;
	mbProtocolLaunchIsSet = false;
	mbLauncherProcessIdIsSet = false;
	mbTestModeIsSet = false;
	mbTestResDirIsSet = false;
	mbUpdaterKeyIsSet = false;	
	mbPasswordEncodingModeIsSet = false;
	mbVersionSet = false;
	mbLanguageSet = false;
	//mbClientPathSet = false;
	mbRequired = false;
	mbBroken = false;
	mbSetup = false;

	args.clear();
}