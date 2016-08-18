#pragma once

#include "ComString.h"
#include <vector>

class CCommandLineParams
{
public:
	CCommandLineParams(void);
	~CCommandLineParams(void);

	void SetCommandLine(LPCSTR alpcStr);

	bool AppNameIsSet(){ return mbAppNameIsSet;}
	bool CrashTypeIsSet(){ return mbCrashTypeIsSet;}
	bool RootIsSet(){ return mbRootIsSet;}	
	bool UserNameIsSet(){ return mbUserNameSet;}
	bool PasswordIsSet(){ return mbPasswordSet;}
	bool PasswordEncodingModeIsSet(){ return mbPasswordEncodingModeIsSet;}
	bool AllowAppRestartIsSet(){ return mbAllowRestartAppIsSet;}
	bool SendCrashShowIsSet(){ return mbSendCrashShowIsSet;}
	
	const std::string& GetAppName(){ return msAppName;}
	const std::string& GetCrashType(){ return msCrashType;}
	const std::wstring& GetRoot(){ return msRoot;}
	const std::string& GetUserName() {return msUserName;}
	const std::string& GetPassword() {return msPassword;}	
	
	const int& GetPasswordEncodingMode(){return miPasswordEncodingMode;}
	const int& GetAllowAppRestart(){return miAllowRestartApp;}
	const int& GetSendCrashShow(){return miSendCrashShow;}

private:
	void ParseCommandLine();
	void ResetParamsFlags();
	void SaveNextParam( LPCSTR pchArgBegin, LPCSTR pch);

	MP_VECTOR<MP_STRING> args;

	// Application Name
	bool		mbAppNameIsSet;
	MP_STRING	msAppName;

	// Crash Type
	bool		mbCrashTypeIsSet;
	MP_STRING	msCrashType;

	// root
	bool		mbRootIsSet;
	MP_WSTRING	msRoot;

	// User Name
	bool		mbUserNameSet;
	MP_STRING	msUserName;

	// Password
	bool		mbPasswordSet;
	MP_STRING	msPassword;
		
	// password encoding
	bool		mbPasswordEncodingModeIsSet;
	int			miPasswordEncodingMode;	

	bool		mbAllowRestartAppIsSet;
	int			miAllowRestartApp;

	bool		mbSendCrashShowIsSet;
	int			miSendCrashShow;
};