// BasePopupDialog.h: interface for the CBasePopupDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEPOPUPDIALOG_H__08FC7E3F_FDF6_4255_B9A8_2B459C87B881__INCLUDED_)
#define AFX_BASEPOPUPDIALOG_H__08FC7E3F_FDF6_4255_B9A8_2B459C87B881__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlddx.h>
#include "../resource.h"
#include "../Cscl3DWS/RenderManager/Classes/UserSettings.h"

#pragma comment(lib,"wsock32.lib")

class CBasePopupDialog :  public CDialogImpl<CBasePopupDialog>, public CWinDataExchange<CBasePopupDialog>
							
{
public:
	CBasePopupDialog();
	virtual ~CBasePopupDialog();
	void InitializeControls();

	enum Idd{IDD = IDD_DIALOGBAR};

	BEGIN_MSG_MAP(CBasePopupDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnBnClickedButton1)
		COMMAND_HANDLER(IDC_BUTTON2, BN_CLICKED, OnBnClickedButton2)
		COMMAND_HANDLER(IDC_CHECK_USE_PROXY_USER, BN_CLICKED, OnBnClickedUseProxyUser)

		COMMAND_HANDLER(IDC_CHECK_USE_SOCKS_USER, BN_CLICKED, OnBnClickedUseSocksUser)
		COMMAND_HANDLER(IDC_RADIO_SOCKS4, BN_CLICKED, OnSocks4Clicked)
		COMMAND_HANDLER(IDC_RADIO_SOCKS5, BN_CLICKED, OnSocks5Clicked)

		COMMAND_HANDLER(IDC_CHECK_USE_HTTPS_USER, BN_CLICKED, OnBnClickedUseHttpsUser)

		COMMAND_HANDLER(IDC_RADIO_PROXY_MANUAL, BN_CLICKED, OnProxyManualClicked)
		COMMAND_HANDLER(IDC_RADIO_PROXY_AS_SYSTEM, BN_CLICKED, OnProxySystemClicked)
		COMMAND_HANDLER(IDC_RADIO_PROXY_NOT_USE, BN_CLICKED, OnProxyNoClicked)

		COMMAND_HANDLER(IDC_RADIO_SOCKS_MANUAL, BN_CLICKED, OnSocksManualClicked)
		COMMAND_HANDLER(IDC_RADIO_SOCKS_AS_SYSTEM, BN_CLICKED, OnSocksSystemClicked)
		COMMAND_HANDLER(IDC_RADIO_SOCKS_NOT_USE, BN_CLICKED, OnSocksNoClicked)

		COMMAND_HANDLER(IDC_RADIO_HTTPS_MANUAL, BN_CLICKED, OnHttpsManualClicked)
		COMMAND_HANDLER(IDC_RADIO_HTTPS_AS_SYSTEM, BN_CLICKED, OnHttpsSystemClicked)
		COMMAND_HANDLER(IDC_RADIO_HTTPS_NOT_USE, BN_CLICKED, OnHttpsNoClicked)

	END_MSG_MAP()

	BEGIN_DDX_MAP(CBasePopupDialog)
		DDX_CONTROL_HANDLE(IDC_PROXY_ADDRESS, mEditProxyAddress)
		DDX_CONTROL_HANDLE(IDC_SOCKS_ADDRESS, mEditSocksAddress)
		DDX_CONTROL_HANDLE(IDC_HTTPS_ADDRESS, mEditHttpsAddress)
		DDX_TEXT(IDC_PROXY_ADDRESS, msEditProxyAddress);
		DDX_TEXT(IDC_SOCKS_ADDRESS, msEditSocksAddress);
		DDX_TEXT(IDC_HTTPS_ADDRESS, msEditHttpsAddress);
		DDX_CONTROL_HANDLE(IDC_PROXY_PORT, mEditProxyPort)
		DDX_CONTROL_HANDLE(IDC_SOCKS_PORT, mEditSocksPort)
		DDX_CONTROL_HANDLE(IDC_HTTPS_PORT, mEditHttpsPort)
		DDX_TEXT(IDC_PROXY_PORT, msEditProxyPort);
		DDX_TEXT(IDC_SOCKS_PORT, msEditSocksPort);
		DDX_TEXT(IDC_HTTPS_PORT, msEditHttpsPort);
		DDX_CONTROL_HANDLE(IDC_STATIC, mStaticProxyAddress)
		DDX_CONTROL_HANDLE(IDC_STATIC8, mStaticSocksAddress)
		DDX_CONTROL_HANDLE(IDC_STATIC5, mStaticHttpsAddress)
		DDX_CONTROL_HANDLE(IDC_STATIC2, mStaticProxyPort)
		DDX_CONTROL_HANDLE(IDC_STATIC39, mStaticSocksPort)
		DDX_CONTROL_HANDLE(IDC_STATIC6, mStaticHttpsPort)
		DDX_CONTROL_HANDLE(IDC_PROXY_USER, mEditProxyUser)
		DDX_CONTROL_HANDLE(IDC_SOCKS_USER, mEditSocksUser)
		DDX_CONTROL_HANDLE(IDC_HTTPS_USER, mEditHttpsUser)
		DDX_TEXT(IDC_PROXY_USER, msEditProxyUser);
		DDX_TEXT(IDC_SOCKS_USER, msEditSocksUser);
		DDX_TEXT(IDC_HTTPS_USER, msEditHttpsUser);
		DDX_CONTROL_HANDLE(IDC_PROXY_PASS, mEditProxyPass)
		DDX_CONTROL_HANDLE(IDC_SOCKS_PASS, mEditSocksPass)
		DDX_CONTROL_HANDLE(IDC_HTTPS_PASS, mEditHttpsPass)
		DDX_TEXT(IDC_PROXY_PASS, msEditProxyPass);
		DDX_TEXT(IDC_SOCKS_PASS, msEditSocksPass);
		DDX_TEXT(IDC_HTTPS_PASS, msEditHttpsPass);
		DDX_CONTROL_HANDLE(IDC_STATICPROXY, mStaticProxy)
		DDX_CONTROL_HANDLE(IDC_STATICSOCKS, mStaticSocks)
		DDX_CONTROL_HANDLE(IDC_STATICHTTPS, mStaticHttps)
		DDX_CONTROL_HANDLE(IDC_STATIC_USER, mStaticProxyUser)
		DDX_CONTROL_HANDLE(IDC_STATIC_USER_SOCKS, mStaticSocksUser)
		DDX_CONTROL_HANDLE(IDC_STATIC_USER_HTTPS, mStaticHttpsUser)
		DDX_CONTROL_HANDLE(IDC_STATIC_PASS, mStaticProxyPass)
		DDX_CONTROL_HANDLE(IDC_STATIC_PASS_SOCKS, mStaticSocksPass)
		DDX_CONTROL_HANDLE(IDC_STATIC_PASS_HTTPS, mStaticHttpsPass)


		DDX_CONTROL_HANDLE(IDC_RADIO_SOCKS4, mRadioSocks4)
		DDX_CHECK(IDC_RADIO_SOCKS4, miRadioSocks4)

		DDX_CONTROL_HANDLE(IDC_RADIO_SOCKS5, mRadioSocks5)
		DDX_CHECK(IDC_RADIO_SOCKS5, miRadioSocks5)


		DDX_CONTROL_HANDLE(IDC_CHECK_USE_PROXY_USER, mCheckProxyUser)
		DDX_CHECK(IDC_CHECK_USE_PROXY_USER, miCheckProxyUser)

		DDX_CONTROL_HANDLE(IDC_CHECK_USE_SOCKS_USER, mCheckSocksUser)
		DDX_CHECK(IDC_CHECK_USE_SOCKS_USER, miCheckSocksUser)

		DDX_CONTROL_HANDLE(IDC_CHECK_USE_HTTPS_USER, mCheckHttpsUser)
		DDX_CHECK(IDC_CHECK_USE_HTTPS_USER, miCheckHttpsUser)
		DDX_CHECK(IDC_CHECK_USE_HTTPS, miCheckUseHttps)

		DDX_CONTROL_HANDLE(IDC_RADIO_PROXY_NOT_USE, mRadioProxyNo)
		DDX_CONTROL_HANDLE(IDC_RADIO_PROXY_MANUAL, mRadioProxyManual)
		DDX_CONTROL_HANDLE(IDC_RADIO_PROXY_AS_SYSTEM, mRadioProxySystem)

		DDX_CONTROL_HANDLE(IDC_RADIO_SOCKS_NOT_USE, mRadioSocksNo)
		DDX_CONTROL_HANDLE(IDC_RADIO_SOCKS_MANUAL, mRadioSocksManual)
		DDX_CONTROL_HANDLE(IDC_RADIO_SOCKS_AS_SYSTEM, mRadioSocksSystem)

		DDX_CONTROL_HANDLE(IDC_RADIO_HTTPS_NOT_USE, mRadioHttpsNo)
		DDX_CONTROL_HANDLE(IDC_RADIO_HTTPS_MANUAL, mRadioHttpsManual)
		DDX_CONTROL_HANDLE(IDC_RADIO_HTTPS_AS_SYSTEM, mRadioHttpsSystem)

		DDX_CONTROL_HANDLE(IDC_BUTTON2, mOk);
		DDX_CONTROL_HANDLE(IDC_BUTTON1, mCancel);

	END_DDX_MAP()


protected:

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DoDataExchange(FALSE);
		InitializeControls();
		bHandled = TRUE;
		return TRUE;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
	CEdit mEditProxyAddress;
	CEdit mEditProxyPort;
	CEdit mEditProxyUser;
	CEdit mEditProxyPass;
	CStatic mStaticProxy;
	CStatic mStaticProxyAddress;
	CStatic mStaticProxyPort;
	CStatic mStaticProxyUser;
	CStatic mStaticProxyPass;
	int  miRadioProxy;
	CButton mCheckProxyUser;
	int miCheckProxyUser;
	CString msEditProxyAddress;
	CString msEditProxyPort;
	CString msEditProxyUser;
	CString msEditProxyPass;
	CUserSettings	m_userSettings;
	int	m_notifiedPlayersCount;

	CEdit mEditSocksUser;
	CEdit mEditSocksPass;
	CEdit mEditSocksAddress;
	CEdit mEditSocksPort;
	CStatic mStaticSocks;
	CStatic mStaticSocksPass;
	CStatic mStaticSocksUser;
	CStatic mStaticSocksAddress;
	CStatic mStaticSocksPort;
	CButton mCheckSocksUser;
	CString msEditSocksAddress;
	CString msEditSocksPort;
	CString msEditSocksUser;
	CString msEditSocksPass;
	int miCheckSocksUser;

	CButton mRadioSocks4;
	CButton mRadioSocks5;
	int miRadioSocks4;
	int miRadioSocks5;

	CEdit mEditHttpsUser;
	CEdit mEditHttpsPass;
	CEdit mEditHttpsAddress;
	CEdit mEditHttpsPort;
	CStatic mStaticHttps;
	CStatic mStaticHttpsPass;
	CStatic mStaticHttpsUser;
	CStatic mStaticHttpsAddress;
	CStatic mStaticHttpsPort;
	CButton mCheckHttpsUser;
	CString msEditHttpsAddress;
	CString msEditHttpsPort;
	CString msEditHttpsUser;
	CString msEditHttpsPass;
	int miCheckHttpsUser;
	int miCheckUseHttps;

	CButton mRadioProxyNo;
	CButton mRadioProxyManual;
	CButton mRadioProxySystem;

	CButton mRadioSocksNo;
	CButton mRadioSocksManual;
	CButton mRadioSocksSystem;

	CButton mRadioHttpsNo;
	CButton mRadioHttpsManual;
	CButton mRadioHttpsSystem;

	CButton mOk;
	CButton mCancel;

	std::string proxyMode;
	std::string socksMode;
	std::string httpsMode;

	std::string lsServer;
	std::string lsPort;
	std::string lsUser;
	std::string lsPassword;
	int lsUseUser;
	int lsType;

public:
	void NotifyPlayer(HANDLE ahProcess, HWND ahWnd);

public:
	LRESULT OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedAutoProxy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedUseProxyUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCheckClickedUseProxy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnBnClickedUseSocksUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnSocks4Clicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSocks5Clicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnBnClickedUseHttpsUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnProxyManualClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnProxyNoClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnProxySystemClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnSocksManualClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSocksNoClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSocksSystemClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnHttpsManualClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnHttpsNoClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnHttpsSystemClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);


private:
	// 0 - ошибка при сохранении
	// 1 - все ок
	// 2 - ошибка данных
	int SaveProxyParams();
	bool isPortValid(std::string asPort);
	bool isIPValid(std::string asIP);
	bool IsIPAddress( std::string& asProxy);
	void EnabledProxyUserItem(bool val);
	void EnabledSocksUserItem(bool val);
	void EnabledHttpsUserItem(bool val);
	void EnabledSocksAuthentificate(bool val);
	void EnabledProxyItem(bool val);
	void EnabledSocksItem(bool val);
	void EnabledHttpsItem(bool val);
	void SetupIWS3DParams();
	bool InitWS3D(HWND ahWnd);
	void DestroyWS3D();
	void Destroy();
	void ChangeLanguage(std::string& asLang);
	void InformExternalPlayer();
	bool IsLSProxySetting( std::string &asProxyIP, std::string &asProxyPort, int aProxyUseUser, std::string &asProxyUser, std::string &asProxyPassword);
};

#endif // !defined(AFX_BASEPOPUPDIALOG_H__08FC7E3F_FDF6_4255_B9A8_2B459C87B881__INCLUDED_)
