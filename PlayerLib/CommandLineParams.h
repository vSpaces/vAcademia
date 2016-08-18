#pragma once

#include "ComString.h"
#include <vector>

class CCommandLineParams
{
public:
	CCommandLineParams(void);
	~CCommandLineParams(void);

	void SetCommandLine(LPCSTR alpcStr);
	bool UrlIsSet(){ return mbUrlIsSet;}
	bool RootIsSet(){ return mbRootIsSet;}
	bool ModuleIsSet(){ return mbModuleIsSet;}
	bool SceneIsSet(){ return mbSceneIsSet;}
	bool UserNameIsSet(){ return mbUserNameSet;}
	bool PasswordIsSet(){ return mbPasswordSet;}
	bool DebugIsSet(){ return mbDebugIsSet;}
	bool ProtocolLaunchIsSet(){ return mbProtocolLaunchIsSet;}
	bool LauncherProcessIdIsSet(){ return mbLauncherProcessIdIsSet;}
	bool PasswordEncodingModeIsSet(){ return mbPasswordEncodingModeIsSet;}
	bool TestModeIsSet(){ return mbTestModeIsSet;}
	bool TestResDirIsSet(){ return mbTestResDirIsSet;}
	bool UpdaterKeyIsSet(){ return mbUpdaterKeyIsSet;}
	bool LanguageIsSet(){ return mbLanguageSet;}
	bool VersionIsSet(){ return mbVersionSet;}
	//bool ClientPathIsSet(){ return mbClientPathSet;}

	const std::string& GetUrl(){ return msUrl;}
	const std::wstring& GetRoot(){ return msRoot;}
	const std::string& GetModule(){ return msModule;}
	const std::string& GetScene(){ return msScene;}
	const std::string& GetLauncherEventName() {return msLauncherEventName;}
	const std::string& GetUserName() {return msUserName;}
	const std::string& GetPassword() {return msPassword;}
	const std::string& GetTestResDir() {return msTestResDir;}
	const std::string& GetUpdaterKey() {return msUpdaterKey;}
	const bool GetRequired() {return mbRequired;}
	const bool GetBroken() {return mbBroken;}
	const bool GetSetup() {return mbSetup;}
	const std::string& GetVersion() {return msVersion;}
	//const std::wstring& GetClientPath() {return msClientPath;}
	const std::string& GetLanguage() {return msLanguage;}

	const int& GetLauncherProcessID() {return miLauncherProcessId;}
	const int& GetPasswordEncodingMode(){return miPasswordEncodingMode;}
	const int& GetTestMode() {return miTestMode;}

private:
	void ParseCommandLine();
	void ResetParamsFlags();
	void SaveNextParam( LPCSTR pchArgBegin, LPCSTR pch);

	MP_VECTOR<MP_STRING> args;

	// module
	bool		mbModuleIsSet;
	MP_STRING	msModule;

	// scene
	bool		mbSceneIsSet;
	MP_STRING	msScene;

	// url
	bool		mbUrlIsSet;
	MP_STRING	msUrl;

	// root
	bool		mbRootIsSet;
	MP_WSTRING	msRoot;

	// Launcher Event Name
	bool		mbLauncherEventSet;
	MP_STRING	msLauncherEventName;

	// User Name
	bool		mbUserNameSet;
	MP_STRING	msUserName;

	// Password
	bool		mbPasswordSet;
	MP_STRING	msPassword;

	// Client Version
	// -version	0.12.0.9220
	bool		mbVersionSet;
	MP_STRING	msVersion;

	// Language
	// -language rus
	bool		mbLanguageSet;
	MP_STRING	msLanguage;

	// Client path
	// -clientpath "d:\program files\virtual academia"
	//bool		mbClientPathSet;
	//MP_WSTRING	msClientPath;

	// Required
	// -required	[0|1]
	bool		mbRequired;

	// Broken client
	// -broken	[0|1]
	bool		mbBroken;

	// Setup for client needed after update
	// -setup	[0|1]
	bool		mbSetup;

	// External launch flag
	bool		mbProtocolLaunchIsSet;

	// Debug mode. Sleep every frame for 10 ms
	bool		mbDebugIsSet;

	// Launcher process identifier
	bool		mbLauncherProcessIdIsSet;
	int			miLauncherProcessId;

	// password encoding
	bool		mbPasswordEncodingModeIsSet;
	int			miPasswordEncodingMode;

	// test mode
	bool		mbTestModeIsSet;
	int			miTestMode;

	// test resource directory
	bool mbTestResDirIsSet;
	MP_STRING msTestResDir;

	bool mbUpdaterKeyIsSet;
	MP_STRING msUpdaterKey;	
};