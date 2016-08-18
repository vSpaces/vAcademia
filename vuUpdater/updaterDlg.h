// updaterDlg.h : header file
//
#pragma once

#include "StdAfx.h"
#include <vector>
#include "ProgressWnd.h"
//#include <windowsx.h>
//#include <mmsystem.h>
#include "crmMemResourceFile.h"
#include "ComString.h"
#include "oms_context.h"
#include "res.h"
//#include "ComManRes.h"
#include "../cscl3dws/ServiceMan/ServiceMan.h"
#include "ComMan.h"
#include ".\updaterFiles.h"
//#include "Mutex.h"
#define REPOSITORY_PATH _T("root\\files\\")
#define USER_PATH _T("user\\")





using namespace std;
//using namespace oms;
using namespace res;
class CUpdaterNotifier;
class CUpdaterCallback;

#include "InfoMan/Dialog/DialogLayout.h"

// CUpdaterDlg dialog
class CUpdaterDlg : //public CDialog, 
	public CDialogImpl<CUpdaterDlg>,
	public CUpdateUI<CUpdaterDlg>,
	public CDialogLayout<CUpdaterDlg>,
	public service::IServiceManCallbacks
{
// Construction
public:
	//CUpdaterDlg(CWnd* pParent = NULL);	// standard constructor
	CUpdaterDlg();	// standard constructor
	~CUpdaterDlg();
	void UpdateClient_CopyFile(DWORD aDataSize,BYTE *aData,DWORD dwDowloadUpdateSize);
// Dialog Data
	enum { IDD = IDD_UPDATER_DIALOG };

	BEGIN_LAYOUT_MAP()
		//LAYOUT_CONTROL(IDC_EDIT_PROP, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP)
	END_LAYOUT_MAP()


	BEGIN_UPDATE_UI_MAP(CUpdaterDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CUpdaterDlg)
		//MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK1, OnBnClickedOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnBnClickedCancel)
		COMMAND_ID_HANDLER(IDC_LABEL, OnStnClickedLabel)
		CHAIN_MSG_MAP(CDialogLayout<CUpdaterDlg>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	static void StartDownload( CUpdaterDlg *apDlg);
	void CheckLoginDownload();
	bool OnOK();
public:
	long onResourceNotify(IResource *aResource, DWORD aNotifyCode);
	long setResManager();
	bool LoadingFiles();
	bool ParseCmdLine(LPSTR alpCmdLine);
	void WritePlayerIni(char* text);
	void ExitWithError( const char *pText); // выход с ошибкой 
	void ReleaseLibrary();

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	bool CreateEmptyFile(CComString fullFilePath);
	//FILE * CreateFileWithPath(CComString fullFilePath);
	bool  delDir( CComString &sDir);

	CProgressWnd progressWnd;
	int countAuth;
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

	TCHAR glpcResServerConnectType[7];		
	bool bNext;
	HANDLE hDownloadThread;
	HANDLE hCheckLoginThread;
	CComString m_Login;
	//CUpdaterCallback* pHandler;
	CComString m_Password;
	CComString m_PathINI;
	CComString m_sServer;
	int iStep;
	// получить адрес функции в dll
	void* GetProcAddress(unsigned long auHModule, const char* apwcProcName);
	// освободить dll
	bool SetupResManParams();
	bool MakePathToComponents(CHAR* lpcModuleFileName, CHAR* lpcExeFullPathName, LPSTR &lpFileName, BOOL &bByPathOnly);
	bool ParseINI(HINSTANCE ahInstance, char **pptext);
	bool Login();

	HMODULE	hComModule;
	HMODULE	hServiceModule;
	std::auto_ptr<oms::omsContext> pOMSContext;
	oms::omsContext* context;
	res::resIResourceManager * mpIResourceManager;

	HANDLE m_hCheckLogin;

	CupdaterFiles *pUpdaterFiles;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//virtual BOOL OnInitDialog();
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnPaint();
	HCURSOR OnQueryDragIcon();	
	CUpdaterNotifier* notifier;
public:
	LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStnClickedLabel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	virtual void onGetService(unsigned int rmmlID, const wchar_t* apwcServiceName, const wchar_t* apwcServiceDescription, int* apiErrorCode=NULL ){};
	virtual void onMethodComplete(int aRequestID, int aRMMLID, const wchar_t* apwcResponse=NULL, bool isService=false, int aiErrorCode=0);
	virtual void onServiceNotify(int aiID, const wchar_t* apwcResponse, int* apiErrorCode){};
	virtual void onConnectRestored(){};
};

class CUpdaterNotifier : public IResourceNotify
{
	friend class CUpdaterDlg;
public:

public:
	CUpdaterNotifier( )
	{
	}

	~CUpdaterNotifier()
	{
	}

	//IResourceNotify
public:
	long onResourceNotify(IResource *aResource, DWORD aNotifyCode)
	{
		myDlg->onResourceNotify( aResource, aNotifyCode);
		return 0;
	}

	CUpdaterDlg*	myDlg;
};

/*class CUpdaterCallback : public cm::cmIComManCallbacks
{
	CUpdaterDlg *pUpdater;
public:
	void onQueryComplete(int aiID, const wchar_t* apwcResponse=NULL, int* aiErrorCode=NULL);

	void SetUpdater(CUpdaterDlg *apUpdater)
	{
		pUpdater = apUpdater;
	};
};*/