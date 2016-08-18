// updaterDll.h : header file
//
#pragma once

#include "StdAfx.h"
#include <vector>
//#include "crmMemResourceFile.h"
#include "ComString.h"
#include "oms_context.h"
#include "res.h"
//#include "../cscl3dws/ServiceMan/ServiceMan.h"
#include "ComMan.h"
//#include ".\updaterFiles.h"
#include ".\ClientDownloader.h"
#include "..\nengine\ncommon\Thread.h"


typedef unsigned int socklen_t;
#include "../curl/include/curl/curl.h"

#define REPOSITORY_PATH L"root\\files\\"
#define USER_PATH L"user\\"
#define I_AM_UPDATER "iamupdater"

#define LOADING_SETUP_DEFAULT		-1
#define LOADING_SETUP_PROCESS		1
#define LOADING_SETUP_FINISH		2

#undef UPDATERDLL_API
#ifdef _USRDLL
#define UPDATERDLL_API __declspec(dllexport)
#else
#define UPDATERDLL_API
#endif

class CUpdaterDll;

typedef bool (TSetTextCurrentInfoFunc)( char *s);

struct ThreadInfo
{
	std::string url;
	std::wstring outputfile;
	std::string proxy;
	std::string proxyuserpwd;	

	CUpdaterDll* sender;

	ThreadInfo( CUpdaterDll* _sender, std::string _url, std::wstring _outputfile, std::string _proxy, std::string _proxyuserpwd)
	{
		sender = _sender;
		url = _url;
		outputfile =_outputfile;
		proxy =_proxy;
		proxyuserpwd =_proxyuserpwd;
	};	
};

class ThreadInfoPart
{
public:
	std::string url;
	std::string proxy;
	std::string proxyuserpwd;
	std::string range;
	unsigned int begin;
	int curPos;
	int result;
	DWORD statusCode;
	CThread downloadThread;

	CUpdaterDll* sender;
	ThreadInfoPart()
	{
		result = -1;
		statusCode = 0;
	}

	/*void ResetPos()
	{
		curPos = begin;
	}*/

	void SetParams( CUpdaterDll* _sender, std::string _url, std::string _proxy, std::string _proxyuserpwd, unsigned int _begin, std::string _range)
	{
		sender = _sender;
		url = _url;		
		proxy =_proxy;
		proxyuserpwd =_proxyuserpwd;
		begin = _begin;
		range =_range;
		curPos = begin;
	};	
};

class CUpdaterDll //: public service::IServiceManCallbacks

{
	// Construction
public:
	CUpdaterDll();	// standard constructor
	~CUpdaterDll();

	void UpdateClient_CopyFile(DWORD aDataSize,BYTE *aData,DWORD dwDowloadUpdateSize);

	static DWORD WINAPI StartDownload(LPVOID param);
	bool CheckLoginDownload();

	long setResManager();
	bool LoadingFiles();
	bool ParseCmdLine(LPSTR alpCmdLine);
	bool WritePlayerIni();
	void ExitWithError( int aErrorCode, char *p = ""); // выход с ошибкой 
	void ReleaseLibraryCommanMan();
	//void ReleaseLibraryServiceMan();
	void ReleaseLibraries();
	int  GetParseINIErrorCode();

	void SetCurrentPath();
	void SetLang(char *aLang);
	void NextStep();
	void SetStep(int aStep);
	void SetLoadingSetupStatus(int aStatus);
	HMODULE LoadDLL( LPCSTR alpcRelPath, LPCSTR alpcName);
	bool KillOtherPlayerProcesses( bool aFinsihAll);
	void WriteLog( CComString &sRes, int iCode);
	void WriteLog( CComString &sRes, CComString &sCode);
	void InitStartFailFileMarker(char* apLine);
	void RemoveStartFailFileMarker();
	void StartDownloadClient();
	void Close();
	bool RequestDownloadSetup();
	void CheckStartFailFileMarker(bool onCancel);
	bool RequestDownloadSetupFromHTTP();
	void ReportProgressHTTP( double download_total_size, double download_total_size_done);
	void OpenSetup(std::wstring setupPath);
	std::string GetDownloadURL();
	void OpenSiteIfNeeded();
	void OnSiteOpened();

protected:	
	bool CreateEmptyFile(CComString fullFilePath);
	void ReadUpdaterPathFromFile();
	bool IsCriticalError(int aErrorCode);
	void AddStatusToStartFailFile(bool isBroken);

	int countAuth;
	CWComString msPath;
	CWComString msOldPath;
protected:
	//IDispatch* gpDispComMan;

	// Путь, устанавливаемый в ComMan.QueryBase
	TCHAR glpcQueryBase[MAX_PATH*2+2];
	// Путь, устанавливаемый в ComMan.RemoteBase
	TCHAR glpcRemoteBase[MAX_PATH*2+2];
	// Путь в реестре для получения параметров
	TCHAR glpcRegPath[MAX_PATH*2+2];
	// Тип соединения менеджера ресурсов 
	cm::cmICommunicationManager		*sp_ComMan;
	IResMan		*sp_ResourceManager;
	IResource		*sp_Resource;
	IResource	*sp_LocalResource;
	DWORD			dwLoadedSize;
	TCHAR			szFullLocal[MAX_PATH];
	TCHAR			szTempDir[MAX_PATH];
	TCHAR			szTempBase[MAX_PATH];
	
	bool bNext;
	HANDLE hDownloadThread;
	HANDLE hCheckLoginThread;
	CComString m_Login;
	//CUpdaterCallback* pHandler;
	CComString m_Password;
	CComString m_UpdaterKey;
	int m_EncPass;
	CComString m_PathINI;
	CWComString m_sServer;
	CWComString m_sTitle;
	bool m_isLoginSuccess;
	int m_parseINIError;
	std::string m_downloadURL;
	std::string m_downloadSiteURL;
	CWComString m_wdownloadSiteURL;
	int m_loadingSetupStatus;
	int iStep;
	bool m_isAlreadySiteOpened;
	// получить адрес функции в dll
	void* GetProcAddress(unsigned long auHModule, const char* apwcProcName);
	// освободить dll
	bool MakePathToComponents(CHAR* lpcModuleFileName, CHAR* lpcExeFullPathName, LPSTR &lpFileName, BOOL &bByPathOnly);
	bool ParseINI(HINSTANCE ahInstance);
	bool Login( bool bFirst);

	HMODULE	hComModule;
	//HMODULE	hServiceModule;
	//oms::omsContext* pOMSContext;
	oms::omsContext* context;
	//res::resIResourceManager * mpIResourceManager;

	HANDLE m_hCheckLogin;

	//CupdaterFiles *pUpdaterFiles;
	CClientDowloader *pClientDownloader;
	HWND updaterHandle;
	//CUpdaterNotifier* notifier;
public:

	//virtual void onGetService(unsigned int rmmlID, const wchar_t* apwcServiceName, const wchar_t* apwcServiceDescription, int* apiErrorCode=NULL ){};
	//virtual void onMethodComplete(int aRequestID, int aRMMLID, const wchar_t* apwcResponse=NULL, bool isService=false, int aiErrorCode=0);
	//virtual void onServiceNotify(int aiID, const wchar_t* apwcResponse, int* apiErrorCode){};
	//virtual void onConnectRestored(){};

public:

	void setStatus( int astatus);
	void setOneFileStatus( int astatus);
	void setInfoText( char *apText);
	void setCurrentLoaded( DWORD aCurrentSize);
	void setAllSize( DWORD aAllSize);

public:
	int GetStatus();
	int GetOneFileStatus();
	//wchar_t *GetInfoText();
	//bool GetInfoText( char *p);
	LPCSTR GetInfoText();
	DWORD GetCurrentLoaded();
	DWORD GetAllSize();
	int GetStep();
	int GetErrorCode();

	bool Init();
	bool Finish( bool bRun);
	LPCWSTR GetCurrentPath();
	LPCWSTR GetClientPath();
	LPCWSTR GetServer();
	LPCWSTR GetDownloadSiteURL();
	unsigned int SetParams( char *aLang, char *apLine);
	//void DelCache();
	bool DelDir( CWComString &sDir, int &aProgress);

	DWORD TaskDownloadProc( ThreadInfo *aParam);
	DWORD TaskDownloadProcPart( ThreadInfoPart *aParam);

private:	
	HANDLE RunThreadProcPart( ThreadInfoPart *aParam);

	static DWORD WINAPI TaskThreadProc(LPVOID lpParam);
	static DWORD WINAPI TaskThreadProcPart(LPVOID lpParam);

protected:
	int status;
	int statusOneFile;
	CComString sInfoText;
	DWORD currentSize;
	DWORD dwAllSize;
	int iErrorCode;
	int iErrorCodeLogin;
	CThread m_downloadThread;
	CWComString msVacademiaFilePath;
};