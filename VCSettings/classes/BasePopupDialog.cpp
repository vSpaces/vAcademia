// BasePopupDialog.cpp: implementation of the CBasePopupDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BasePopupDialog.h"
#include "Common.h"
#include "fileSys.h"
#include "ProcessFinder.h"
#include "../PlayerLib/notifies.h"
#include <shellapi.h>
#include <Tlhelp32.h>
#include <Psapi.h>
#include "../../Comman/ProxySettings.h"

#define	DEFAULT_LANGUAGE	"eng"
#define	LOCALProxy_ADDRESS	"127.0.0.1"
#define	SERVER_ADDRESS "www.virtyola.ru"
#define REPOSITORY_PATH _T("root\\files\\")
#define WM_UPDATE_SETTINGS			(WM_USER + 111)

// Global Variables:
extern CAppModule _AppModule;

#define MAX_LOADSTRING 100
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

#define MSG_PASSWORDS_DONOT_MATCH			1
#define MSGTITLE_ERROR						2
#define MSG_SAVE_OK							3
#define MSG_SAVE_OK_ACTIVE_RUN_FOUND		4
#define MSG_SAVE_FAILED						5
#define MSG_ATTENTION						6
#define MSG_SOCKS_PASSWORDS_DONOT_MATCH		7

#define MSG_SOCKS_PORT_NOT_VALID			8
#define MSG_SOCKS_IP_NOT_VALID				9
#define MSG_PROXY_PORT_NOT_VALID			10
#define MSG_PROXY_IP_NOT_VALID				11
#define MSG_HTTPS_PORT_NOT_VALID			12
#define MSG_HTTPS_IP_NOT_VALID				13

#define MSG_SOCKS_USER_NOT_VALID				14
#define MSG_SOCKS_PASS_NOT_VALID				15
#define MSG_PROXY_USER_NOT_VALID				16
#define MSG_PROXY_PASS_NOT_VALID				17
#define MSG_HTTPS_USER_NOT_VALID				18
#define MSG_HTTPS_PASS_NOT_VALID				19


bool FileExists(const TCHAR* alpcFileName){
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(alpcFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);
	return true;
}

bool FileExists(const WCHAR* alpcFileName){
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(alpcFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);
	return true;
}

std::wstring GetApplicationDataDirectory()
{
	wchar_t tmp[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}

std::wstring GetApplicationRootDirectory()
{
	wchar_t lpcIniFullPathName[MAX_PATH*2+2] = L"";

	wchar_t lpcStrBuffer[MAX_PATH*2+2];	lpcStrBuffer[0] = 0;
	::GetModuleFileNameW( _AppModule.m_hInst, lpcStrBuffer, MAX_PATH);

	if( lpcStrBuffer[0] == 0)
		return L"";

	SECFileSystem fs;
	CWComString sExePath = fs.GetPath( CWComString( lpcStrBuffer), TRUE);
	return std::wstring( sExePath.GetBuffer());
}

std::string	GetLanguage()
{
	WCHAR lpcStrBuffer[MAX_PATH*2+2];	lpcStrBuffer[0] = 0;

	SECFileSystem fs;
	CWComString sIniFilePath = fs.AppendWildcard( CWComString(GetApplicationRootDirectory().c_str()), L"player.ini");
	if( !FileExists(sIniFilePath))
		return DEFAULT_LANGUAGE;

	WCHAR lpcIniFullPathName[MAX_PATH*2+2]= L"";
	wcscpy_s( lpcIniFullPathName, MAX_PATH*2+2, sIniFilePath.GetBuffer());

	USES_CONVERSION;
	if (GetPrivateProfileStringW( L"settings", L"language", L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName))
		return W2A(lpcStrBuffer);

	return DEFAULT_LANGUAGE;
}

LPWSTR GetMessageString( int auMessageID)
{
	if( GetLanguage() == "rus")
	{
		switch( auMessageID)
		{
		case MSG_PASSWORDS_DONOT_MATCH:
			return L"Пароли http proxy не совпадают.";
		case MSGTITLE_ERROR:
			return L"Ошибка";
		case MSG_SAVE_OK:
			return L"Параметры успешно сохранены.";
		case MSG_SAVE_OK_ACTIVE_RUN_FOUND:
			return L"Параметры успешно сохранены. Вы можете выполнить вход в vAcademia без ее перезапуска.";
		case MSG_SAVE_FAILED:
			return L"Во время сохранения параметров произошла ошибка. Настройки не были соранены.";
		case MSG_ATTENTION:
			return L"Внимание";
		case MSG_SOCKS_PASSWORDS_DONOT_MATCH:
			return L"Пароли socks proxy не совпадают.";
		case MSG_SOCKS_PORT_NOT_VALID:
			return L"Порт socks proxy указан не верно.";
		case MSG_SOCKS_IP_NOT_VALID:
			return L"IP-адресс socks proxy указан не верно.";
		case MSG_PROXY_PORT_NOT_VALID:
			return L"Порт http proxy указан не верно.";
		case MSG_PROXY_IP_NOT_VALID:
			return L"IP-адресс http proxy указан не верно.";
		case MSG_HTTPS_PORT_NOT_VALID:
			return L"Порт https proxy указан не верно.";
		case MSG_HTTPS_IP_NOT_VALID:
			return L"IP-адресс https proxy указан не верно.";
		case MSG_PROXY_USER_NOT_VALID:
			return L"Имя пользователя для http proxy не указано.";
		case MSG_PROXY_PASS_NOT_VALID:
			return L"Пароль для http proxy не указан.";
		case MSG_SOCKS_USER_NOT_VALID:
			return L"Имя пользователя для SOCKS proxy не указано.";
		case MSG_SOCKS_PASS_NOT_VALID:
			return L"Пароль для SOCKS proxy не указан.";
		case MSG_HTTPS_USER_NOT_VALID:
			return L"Имя пользователя для https proxy не указано.";
		case MSG_HTTPS_PASS_NOT_VALID:
			return L"Пароль для https proxy не указан.";
		}
		return L"";
	}
	switch( auMessageID)
	{
	case MSG_PASSWORDS_DONOT_MATCH:
		return L"Passwords http proxy does not match.";
	case MSGTITLE_ERROR:
		return L"Error";
	case MSG_SAVE_OK:
		return L"Proxy settings were saved successfully.";
	case MSG_SAVE_OK_ACTIVE_RUN_FOUND:
		return L"Proxy settings were saved successfully. You can now login to vAcademia without restarting.";
	case MSG_SAVE_FAILED:
		return L"Error occured during saving. Settings were not saved.";
	case MSG_ATTENTION:
		return L"Attention";
	case MSG_SOCKS_PASSWORDS_DONOT_MATCH:
		return L"Passwords of socks proxy does not match.";
	case MSG_SOCKS_PORT_NOT_VALID:
		return L"Socks proxy port is not valid.";
	case MSG_SOCKS_IP_NOT_VALID:
		return L"Socks proxy IP-adress is not valid.";
	case MSG_PROXY_PORT_NOT_VALID:
		return L"Http proxy port is not valid.";
	case MSG_PROXY_IP_NOT_VALID:
		return L"Http proxy IP-adress is not valid.";
	case MSG_HTTPS_PORT_NOT_VALID:
		return L"Https proxy port is not valid.";
	case MSG_HTTPS_IP_NOT_VALID:
		return L"Https proxy IP-adress is not valid.";
	case MSG_PROXY_USER_NOT_VALID:
		return L"User name for http proxy not specified.";
	case MSG_PROXY_PASS_NOT_VALID:
		return L"Password for http proxy not specified.";
	case MSG_SOCKS_USER_NOT_VALID:
		return L"User name for SOCKS proxy not specified.";
	case MSG_SOCKS_PASS_NOT_VALID:
		return L"Password for SOCKS proxy not specified.";
	case MSG_HTTPS_USER_NOT_VALID:
		return L"User name for https proxy not specified.";
	case MSG_HTTPS_PASS_NOT_VALID:
		return L"Password for https proxy not specified.";
	}
	return L"";
}

CBasePopupDialog::CBasePopupDialog()
{
	m_notifiedPlayersCount = 0;
}

CBasePopupDialog::~CBasePopupDialog()
{

}

void CBasePopupDialog::ChangeLanguage(std::string& asLang)
{
	if(asLang=="eng")
	{
		SetWindowTextW( this->m_hWnd, L"Network settings");
		SetWindowTextW(mStaticProxy.m_hWnd, L"HTTP Proxy server settings");
		SetWindowTextW(mStaticSocks.m_hWnd, L"SOCKS Proxy server settings");
		SetWindowTextW(mStaticHttps.m_hWnd, L"HTTPS Proxy server settings");
		SetWindowTextW(mStaticProxyAddress.m_hWnd, L"Address:");
		SetWindowTextW(mStaticSocksAddress.m_hWnd, L"Address:");
		SetWindowTextW(mStaticHttpsAddress.m_hWnd, L"Address:");
		SetWindowTextW(mStaticProxyPort.m_hWnd, L"Port:");
		SetWindowTextW(mStaticSocksPort.m_hWnd, L"Port:");
		SetWindowTextW(mStaticHttpsPort.m_hWnd, L"Port:");
		SetWindowTextW(mStaticProxyUser.m_hWnd, L"Username:");
		SetWindowTextW(mStaticSocksUser.m_hWnd, L"Username:");
		SetWindowTextW(mStaticHttpsUser.m_hWnd, L"Username:");
		SetWindowTextW(mStaticProxyPass.m_hWnd, L"Password:");
		SetWindowTextW(mStaticSocksPass.m_hWnd, L"Password:");
		SetWindowTextW(mStaticHttpsPass.m_hWnd, L"Password:");
		SetWindowTextW(mRadioSocks4.m_hWnd, L"SOCKS 4");
		SetWindowTextW(mRadioSocks5.m_hWnd, L"SOCKS 5");
		SetWindowTextW(mCheckProxyUser.m_hWnd, L"Use username and password");
		SetWindowTextW(mCheckSocksUser.m_hWnd, L"Use username and password");
		SetWindowTextW(mCheckHttpsUser.m_hWnd, L"Use username and password");
		SetWindowTextW(mRadioProxyNo.m_hWnd, L"No proxy");
		SetWindowTextW(mRadioProxySystem.m_hWnd, L"Use the system proxy settings");
		SetWindowTextW(mRadioProxyManual.m_hWnd, L"Manually configure the proxy service");
		SetWindowTextW(mRadioSocksNo.m_hWnd, L"No proxy");
		SetWindowTextW(mRadioSocksSystem.m_hWnd, L"Use the system proxy settings");
		SetWindowTextW(mRadioSocksManual.m_hWnd, L"Manually configure the proxy service");
		SetWindowTextW(mRadioHttpsNo.m_hWnd, L"No proxy");
		SetWindowTextW(mRadioHttpsSystem.m_hWnd, L"Use the system proxy settings");
		SetWindowTextW(mRadioHttpsManual.m_hWnd, L"Manually configure the proxy service");
		SetWindowTextW(mOk.m_hWnd, L"OK");
		SetWindowTextW(mCancel.m_hWnd, L"Cancel");
	}
	else
	{
		SetWindowTextW( this->m_hWnd, L"Настройки сети");
		SetWindowTextW(mStaticProxy.m_hWnd, L"Настройки HTTP прокси-сервера");
		SetWindowTextW(mStaticSocks.m_hWnd, L"Настройки SOCKS прокси-сервера");
		SetWindowTextW(mStaticHttps.m_hWnd, L"Настройки HTTPS прокси-сервера");
		SetWindowTextW(mStaticProxyAddress.m_hWnd, L"Адрес:");
		SetWindowTextW(mStaticSocksAddress.m_hWnd, L"Адрес:");
		SetWindowTextW(mStaticHttpsAddress.m_hWnd, L"Адрес:");
		SetWindowTextW(mStaticProxyPort.m_hWnd, L"Порт:");
		SetWindowTextW(mStaticSocksPort.m_hWnd, L"Порт:");
		SetWindowTextW(mStaticHttpsPort.m_hWnd, L"Порт:");
		SetWindowTextW(mStaticProxyUser.m_hWnd, L"Имя пользователя:");
		SetWindowTextW(mStaticSocksUser.m_hWnd, L"Имя пользователя:");
		SetWindowTextW(mStaticHttpsUser.m_hWnd, L"Имя пользователя:");
		SetWindowTextW(mStaticSocksPass.m_hWnd, L"Пароль:");
		SetWindowTextW(mStaticProxyPass.m_hWnd, L"Пароль:");
		SetWindowTextW(mStaticHttpsPass.m_hWnd, L"Пароль:");
		SetWindowTextW(mRadioSocks4.m_hWnd, L"SOCKS 4");
		SetWindowTextW(mRadioSocks5.m_hWnd, L"SOCKS 5");
		SetWindowTextW(mCheckProxyUser.m_hWnd, L"Использовать имя пользователя и пароль");
		SetWindowTextW(mCheckSocksUser.m_hWnd, L"Использовать имя пользователя и пароль");
		SetWindowTextW(mCheckHttpsUser.m_hWnd, L"Использовать имя пользователя и пароль");
		SetWindowTextW(mRadioProxyNo.m_hWnd, L"Без прокси");
		SetWindowTextW(mRadioProxySystem.m_hWnd, L"Использовать системные настройки прокси");
		SetWindowTextW(mRadioProxyManual.m_hWnd, L"Ручная настройка сервиса прокси");
		SetWindowTextW(mRadioSocksNo.m_hWnd, L"Без прокси");
		SetWindowTextW(mRadioSocksSystem.m_hWnd, L"Использовать системные настройки прокси");
		SetWindowTextW(mRadioSocksManual.m_hWnd, L"Ручная настройка сервиса прокси");
		SetWindowTextW(mRadioHttpsNo.m_hWnd, L"Без прокси");
		SetWindowTextW(mRadioHttpsSystem.m_hWnd, L"Использовать системные настройки прокси");
		SetWindowTextW(mRadioHttpsManual.m_hWnd, L"Ручная настройка сервиса прокси");
		SetWindowTextW(mOk.m_hWnd, L"ОК");
		SetWindowTextW(mCancel.m_hWnd, L"Отмена");
	}
}

void CBasePopupDialog::SetupIWS3DParams()
{
	// Setup proxy params
	std::string sProxyServer;
	std::string sProxyPort;
	std::string sProxyUser;
	std::string sProxyPassword;
	std::string sProxyMode;
	std::string sProxyUseUser;

	std::string sSocksServer;
	std::string sSocksPort;
	std::string sSocksUser;
	std::string sSocksPassword;
	std::string sSocksMode;
	std::string sSocksUseUser;
	std::string sSocksVer;

	std::string sHttpsServer;
	std::string sHttpsPort;
	std::string sHttpsUser;
	std::string sHttpsPassword;
	std::string sHttpsMode;
	std::string sHttpsUseUser;

	std::string sLang = GetLanguage();
	ChangeLanguage(sLang);

	m_userSettings.LoadGlobal();
	sProxyServer = m_userSettings.GetSetting("proxy_address");
	sProxyPort = m_userSettings.GetSetting("proxy_port");
	sProxyUser = m_userSettings.GetSetting("proxy_username");
	//sProxyPassword = m_userSettings.GetPassSetting("proxy_pass");
	sProxyPassword = m_userSettings.GetSetting("proxy_pass");
	sProxyMode = m_userSettings.GetSetting("proxy_mode");
	sProxyUseUser = m_userSettings.GetSetting("proxy_use_user");

	sSocksServer = m_userSettings.GetSetting("socks_address");
	sSocksPort = m_userSettings.GetSetting("socks_port");
	sSocksUser = m_userSettings.GetSetting("socks_username");
	//sSocksPassword = m_userSettings.GetPassSetting("socks_pass");
	sSocksPassword = m_userSettings.GetSetting("socks_pass");
	sSocksMode = m_userSettings.GetSetting("socks_mode");
	sSocksUseUser = m_userSettings.GetSetting("socks_use_user");
	sSocksVer = m_userSettings.GetSetting("socks_ver");
	
	sHttpsServer = m_userSettings.GetSetting("https_address");
	sHttpsPort = m_userSettings.GetSetting("https_port");
	sHttpsUser = m_userSettings.GetSetting("https_username");
	//sHttpsPassword = m_userSettings.GetPassSetting("https_pass");
	sHttpsPassword = m_userSettings.GetSetting("https_pass");
	sHttpsMode = m_userSettings.GetSetting("https_mode");
	sHttpsUseUser = m_userSettings.GetSetting("https_use_user");

	lsServer = m_userSettings.GetSetting("ls_addres");
	lsPort = m_userSettings.GetSetting("ls_port");
	lsUser = m_userSettings.GetSetting("ls_username");
	//lsPassword = m_userSettings.GetPassSetting("https_pass");
	lsPassword = m_userSettings.GetSetting("ls_pass");	
	lsType = rtl_atoi(m_userSettings.GetSetting("ls_type").c_str());
	std::string slsUseUser = m_userSettings.GetSetting("ls_use_user");
	if( slsUseUser == "1")
		lsUseUser = 1;
	else
		lsUseUser = 0;

	if( !sProxyServer.empty())
	{
		mEditProxyAddress.SetWindowText(sProxyServer.c_str());
		mEditProxyPort.SetWindowText(sProxyPort.c_str());
		mEditProxyUser.SetWindowText(sProxyUser.c_str());
		mEditProxyPass.SetWindowText(sProxyPassword.c_str());
	}
	else
	{
		mEditProxyAddress.SetWindowText("0.0.0.0");
		mEditProxyPort.SetWindowText("0");
	}

	if( !sSocksServer.empty())
	{
		mEditSocksAddress.SetWindowText(sSocksServer.c_str());
		mEditSocksPort.SetWindowText(sSocksPort.c_str());
		mEditSocksUser.SetWindowText(sSocksUser.c_str());
		mEditSocksPass.SetWindowText(sSocksPassword.c_str());
	}
	else
	{
		mEditSocksAddress.SetWindowText("0.0.0.0");
		mEditSocksPort.SetWindowText("0");
	}

	if( !sHttpsServer.empty())
	{
		mEditHttpsAddress.SetWindowText(sHttpsServer.c_str());
		mEditHttpsPort.SetWindowText(sHttpsPort.c_str());
		mEditHttpsUser.SetWindowText(sHttpsUser.c_str());
		mEditHttpsPass.SetWindowText(sHttpsPassword.c_str());
	}
	else
	{
		mEditHttpsAddress.SetWindowText("0.0.0.0");
		mEditHttpsPort.SetWindowText("0");
	}

	if( !sProxyMode.empty())
	{
		proxyMode = sProxyMode;
		if (proxyMode == "no")
			proxyMode = "system";

		if( proxyMode == "none")
			mRadioProxyNo.SetCheck(1);
		else if( proxyMode == "system")
			mRadioProxySystem.SetCheck(1);
		else if( proxyMode == "manual")
		{
			mRadioProxyManual.SetCheck(1);
			EnabledProxyItem(true);
		}
	}
	else
	{
		proxyMode = "system";
		mRadioProxySystem.SetCheck(1);
	}

	if( !sSocksMode.empty())
	{
		socksMode = sSocksMode;
		if (socksMode == "no")
			socksMode = "system";

		if( socksMode == "none")
			mRadioSocksNo.SetCheck(1);
		else if( socksMode == "system")
			mRadioSocksSystem.SetCheck(1);
		else if( socksMode == "manual")
		{
			mRadioSocksManual.SetCheck(1);
			EnabledSocksItem(true);
		}
	}
	else
	{
		socksMode = "system";
		mRadioSocksSystem.SetCheck(1);
	}

	if( !sHttpsMode.empty())
	{
		httpsMode = sHttpsMode;
		if (httpsMode == "no")
			httpsMode = "system";

		if( httpsMode == "none")
			mRadioHttpsNo.SetCheck(1);
		else if( httpsMode == "system")
			mRadioHttpsSystem.SetCheck(1);
		else if( httpsMode == "manual")
		{
			mRadioHttpsManual.SetCheck(1);
			EnabledHttpsItem(true);
		}
	}
	else
	{
		httpsMode = "system";
		mRadioHttpsSystem.SetCheck(1);
	}

	if( !sProxyUseUser.empty())
	{
		if( sProxyUseUser == "yes")
		{
			mCheckProxyUser.SetCheck(1);
			DoDataExchange(TRUE);
			DoDataExchange(FALSE);
			if( proxyMode == "manual")
				EnabledProxyUserItem(true);
		}
	}

	if( !sSocksUseUser.empty())
	{
		if( sSocksUseUser == "yes")
		{
			mCheckSocksUser.SetCheck(1);
			DoDataExchange(TRUE);
			DoDataExchange(FALSE);
			if( socksMode == "manual")
				EnabledSocksUserItem(true);
		}
	}

	if (sSocksVer != "5")
		EnabledSocksAuthentificate(false);

	if( !sHttpsUseUser.empty())
	{
		if( sHttpsUseUser == "yes")
		{
			mCheckHttpsUser.SetCheck(1);
			DoDataExchange(TRUE);
			DoDataExchange(FALSE);
			if( httpsMode == "manual")
				EnabledHttpsUserItem(true);
		}
	}

	if( !sSocksVer.empty())
	{
		if( sSocksVer == "4")
		{
			mRadioSocks5.SetCheck(0);
			mRadioSocks4.SetCheck(1);
			DoDataExchange(TRUE);
			DoDataExchange(FALSE);
		}
		else
		{
			mRadioSocks4.SetCheck(0);
			mRadioSocks5.SetCheck(1);
			DoDataExchange(TRUE);
			DoDataExchange(FALSE);
		}

	}
	else
	{
		mRadioSocks4.SetCheck(0);
		mRadioSocks5.SetCheck(1);
	}


}

void CBasePopupDialog::InitializeControls()
{
	SetIcon(LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)), TRUE);

	SetupIWS3DParams();
	DoDataExchange(TRUE);
}

void CBasePopupDialog::Destroy()
{
	PostQuitMessage( 0 );
}

LRESULT CBasePopupDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DoDataExchange(TRUE);
	Destroy();
	return 0;
}

//закрыть
LRESULT CBasePopupDialog::OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(TRUE);
	Destroy();
	return 0;
}

bool CBasePopupDialog::isPortValid(std::string asPort)
{
	int port = 0;
	sscanf_s(asPort.c_str(), "%d", &port);
	return (port > 0 && port < 65536);
}


bool CBasePopupDialog::isIPValid(std::string asIP)
{
	if (asIP.length() == 0)
		return false;

/*
	if (asIP.find('.') < 0)
		return false;

	asIP += ".";

	int i = 0;
	int octet = 0;
	int pos = 0;
	while ( (pos = asIP.find('.')) > -1)
	{
		std::string octetStr = asIP.substr(0, pos);
		asIP = asIP.substr(pos + 1, asIP.length() - pos - 1);
		if (sscanf(octetStr.c_str(), "%d", &octet) != 1)
			return false;
		if (octet < 0 || octet > 255)
			return false;
		i++;
	}

	if (i != 4)
		return false;
*/
	return true;
}

bool CBasePopupDialog::IsLSProxySetting( std::string &asProxyIP, std::string &asProxyPort, int aProxyUseUser, std::string &asProxyUser, std::string &asProxyPassword)
{
	if( lsServer == asProxyIP && lsPort == asProxyPort)
	{
		if( aProxyUseUser == lsUseUser)
		{
			if( lsUseUser == 0 || lsUseUser == 1 && lsUser == asProxyUser && lsPassword == asProxyPassword)
				return true;
		}		
	}
	return false;
}

int CBasePopupDialog::SaveProxyParams()
{
	std::string asProxyIP = msEditProxyAddress;
	std::string asProxyPort = msEditProxyPort;
	std::string sProxyUser = msEditProxyUser;
	std::string sProxyPassword = msEditProxyPass;

	std::string asSocksIP = msEditSocksAddress;
	std::string asSocksPort = msEditSocksPort;
	std::string sSocksUser = msEditSocksUser;
	std::string sSocksPassword = msEditSocksPass;

	std::string asHttpsIP = msEditHttpsAddress;
	std::string asHttpsPort = msEditHttpsPort;
	std::string sHttpsUser = msEditHttpsUser;
	std::string sHttpsPassword = msEditHttpsPass;

	bool bLSActual = false;

	// Сохраняем настройки HTTP Proxy
	if( !m_userSettings.SaveSetting( "proxy_mode", proxyMode))
		return 0;

	if(proxyMode == "manual")
	{
		if (!isIPValid(asProxyIP))
		{
			::MessageBoxW(m_hWnd, GetMessageString( MSG_PROXY_IP_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
			return 2;
		}

		if (!isPortValid(asProxyPort))
		{
			::MessageBoxW(m_hWnd, GetMessageString( MSG_PROXY_PORT_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
			return 2;
		}

		if (miCheckProxyUser == 1)
		{
			if (sProxyUser == "")
			{
				::MessageBoxW(m_hWnd, GetMessageString( MSG_PROXY_USER_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
				return 2;
			}
			if (sProxyPassword == "")
			{
				::MessageBoxW(m_hWnd, GetMessageString( MSG_PROXY_PASS_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
				return 2;
			}
		}

		if( !m_userSettings.SaveSetting( "proxy_address", asProxyIP))
			return 0;

		if( !m_userSettings.SaveSetting( "proxy_port", asProxyPort))
			return 0;

		if(miCheckProxyUser == 1)
		{
			if( !m_userSettings.SaveSetting( "proxy_use_user", "yes"))
				return 0;

			if( !m_userSettings.SaveSetting( "proxy_username", sProxyUser))
				return 0;
			//if( !m_userSettings.SavePassSetting( "proxy_pass", sProxyPassword, false))
			if( !m_userSettings.SaveSetting( "proxy_pass", sProxyPassword))
				return 0;
		}
		else
		{
			if( !m_userSettings.SaveSetting( "proxy_use_user", "none"))
				return 0;
		}
		if( lsType == PT_HTTP)
			bLSActual = IsLSProxySetting( asProxyIP, asProxyPort, miCheckProxyUser, sProxyUser, sProxyPassword);
	}

	// Сохраняем настройки SOCKS
	if( !m_userSettings.SaveSetting( "socks_mode", socksMode))
		return 0;

	if(socksMode == "manual")
	{
		if (!isIPValid(asSocksIP))
		{
			::MessageBoxW(m_hWnd, GetMessageString( MSG_SOCKS_PORT_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
			return 2;
		}

		if (!isPortValid(asSocksPort))
		{
			::MessageBoxW(m_hWnd, GetMessageString( MSG_SOCKS_IP_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
			return 2;
		}

		if (miCheckSocksUser == 1 && miRadioSocks4 == 0)
		{
			if (sSocksUser == "")
			{
				::MessageBoxW(m_hWnd, GetMessageString( MSG_SOCKS_USER_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
				return 2;
			}
			if (sSocksPassword == "")
			{
				::MessageBoxW(m_hWnd, GetMessageString( MSG_SOCKS_PASS_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
				return 2;
			}
		}

		if( !m_userSettings.SaveSetting( "socks_address", asSocksIP))
			return 0;

		if( !m_userSettings.SaveSetting( "socks_port", asSocksPort))
			return 0;

		if(miRadioSocks4 == 1)
		{
			if( !m_userSettings.SaveSetting( "socks_ver", "4"))
				return 0;
		}
		else
		{
			if( !m_userSettings.SaveSetting( "socks_ver", "5"))
				return 0;

			if(miCheckSocksUser == 1)
			{
				if( !m_userSettings.SaveSetting( "socks_use_user", "yes"))
					return 0;

				if( !m_userSettings.SaveSetting( "socks_username", sSocksUser))
					return 0;
				//if( !m_userSettings.SavePassSetting( "socks_pass", sSocksPassword, false))
				if( !m_userSettings.SaveSetting( "socks_pass", sSocksPassword))
					return 0;
			}
			else
			{
				if( !m_userSettings.SaveSetting( "socks_use_user", "none"))
					return 0;
			}
		}
		if(!bLSActual && (lsType == PT_SOCKS4 || lsType == PT_SOCKS5))
			bLSActual = IsLSProxySetting( asSocksIP, asSocksPort, miCheckSocksUser, sSocksUser, sSocksPassword);
	}

	// Сохраняем настройки HTTPS Proxy
	if( !m_userSettings.SaveSetting( "https_mode", httpsMode))
		return 0;

	if(httpsMode == "manual")
	{
		if (!isIPValid(asHttpsIP))
		{
			::MessageBoxW(m_hWnd, GetMessageString( MSG_HTTPS_IP_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
			return 2;
		}

		if (!isPortValid(asHttpsPort))
		{
			::MessageBoxW(m_hWnd, GetMessageString( MSG_HTTPS_PORT_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
			return 2;
		}

		if (miCheckHttpsUser == 1)
		{
			if (sHttpsUser == "")
			{
				::MessageBoxW(m_hWnd, GetMessageString( MSG_HTTPS_USER_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
				return 2;
			}
			if (sHttpsPassword == "")
			{
				::MessageBoxW(m_hWnd, GetMessageString( MSG_HTTPS_PASS_NOT_VALID), GetMessageString( MSGTITLE_ERROR), MB_OK | MB_ICONERROR);
				return 2;
			}
		}

		if( !m_userSettings.SaveSetting( "https_address", asHttpsIP))
			return 0;

		if( !m_userSettings.SaveSetting( "https_port", asHttpsPort))
			return 0;

		if(miCheckHttpsUser == 1)
		{
			if( !m_userSettings.SaveSetting( "https_use_user", "yes"))
				return 0;

			if( !m_userSettings.SaveSetting( "https_username", sHttpsUser))
				return 0;
			//if( !m_userSettings.SavePassSetting( "https_pass", sHttpsPassword, true))
			if( !m_userSettings.SaveSetting( "https_pass", sHttpsPassword))
				return 0;
		}
		else
		{
			if( !m_userSettings.SaveSetting( "https_use_user", "none"))
				return 0;
		}
		if(!bLSActual && lsType == PT_HTTPS)
			bLSActual = IsLSProxySetting( asHttpsIP, asHttpsPort, miCheckHttpsUser, sHttpsUser, sHttpsPassword);
	}
	
	if(!bLSActual)
	{		
 		if( !m_userSettings.SaveSetting( "ls_addres", ""))
			return 0;
		if( !m_userSettings.SaveSetting( "ls_port", ""))
			return 0;		
		if( !m_userSettings.SaveSetting( "ls_username", ""))
			return 0;
		if( !m_userSettings.SaveSetting( "ls_pass", ""))
			return 0;				
		if( !m_userSettings.SaveSetting( "ls_use_user", ""))
			return 0;
		if( !m_userSettings.SaveSetting( "ls_tunnel", ""))
			return 0;
		if( !m_userSettings.SaveSetting( "ls_enabled", ""))
			return 0;
		if( !m_userSettings.SaveSetting( "ls_type", ""))
			return 0;
		if( !m_userSettings.SaveSetting( "ls_name", ""))
			return 0;
	}
	return m_userSettings.SaveGlobalSettings();
}

//проверка правильности введённого IP адреса
bool CBasePopupDialog::IsIPAddress( std::string& asProxy)
{
	unsigned long input_adr = INADDR_NONE;
	input_adr = inet_addr(asProxy.c_str());//конвертирует строку в адрес

	if ( input_adr != INADDR_NONE )//если удачно то проверяем сколько точек в адресе
	{
		CComString ProxyName = asProxy.c_str();
		int index = -1;
		index = ProxyName.Find(".");
		if ( index != -1 )
		{
			index = ProxyName.Find(".",index+1);//нашли первую точку ищем вторую начиная со следующего символа
			if ( index != -1 )
			{
				index = ProxyName.Find(".",index+1);
				if ( index != -1 )
				{
					//если 3 точки,т.е адрес состоит из 4 частей то кул!
					return true;
				}
			}
		}
	}
	::MessageBox(m_hWnd, "Некорректный IP адрес", "Ошибка", MB_OK | MB_ICONERROR);
	return false;
}

//////////////////////////////////////////////////////////////////////////
//сохранить
LRESULT CBasePopupDialog::OnBnClickedButton2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	DoDataExchange(TRUE);
	char text[MAX_PATH] = _T("");//строка введённого адреса

	int saveResult = SaveProxyParams();
	
	if( saveResult == 0)
	{
		::MessageBoxW( 0, GetMessageString(MSG_SAVE_FAILED), GetMessageString(MSGTITLE_ERROR), MB_OK);
	}
	else if( saveResult == 1)
	{
		if( m_notifiedPlayersCount > 0)
			::MessageBoxW( 0, GetMessageString(MSG_SAVE_OK_ACTIVE_RUN_FOUND), GetMessageString(MSG_ATTENTION), MB_OK);
		else
			::MessageBoxW( 0, GetMessageString(MSG_SAVE_OK), GetMessageString(MSG_ATTENTION), MB_OK);

		InformExternalPlayer();

		Destroy();
	}
	return 0;
}
LRESULT CBasePopupDialog::OnBnClickedAutoProxy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioSocks4.GetCheck()==BST_CHECKED?true:false;
	if(val)
	{
		EnabledProxyItem(false);
		mCheckProxyUser.SetCheck(0);
		EnabledProxyUserItem(false);
	}
	return true;
}

LRESULT CBasePopupDialog::OnBnClickedUseProxyUser(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{
	EnabledProxyUserItem(mCheckProxyUser.GetCheck()==BST_CHECKED?true:false);
	return 0;
}

LRESULT CBasePopupDialog::OnSocks4Clicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioSocks4.GetCheck()==BST_CHECKED?true:false;
	EnabledSocksAuthentificate(!val);	
	return 0;
}

LRESULT CBasePopupDialog::OnSocks5Clicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioSocks5.GetCheck()==BST_CHECKED?true:false;
	EnabledSocksAuthentificate(val);
	return 0;
}

LRESULT CBasePopupDialog::OnProxyManualClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioProxyManual.GetCheck()==BST_CHECKED?true:false;
	EnabledProxyItem(val);

	if (val)
		proxyMode = "manual";

	return 0;
}

LRESULT CBasePopupDialog::OnProxyNoClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioProxyNo.GetCheck()==BST_CHECKED?true:false;
	if (val)
		EnabledProxyItem(false);

	if (val)
		proxyMode = "none";

	return 0;
}

LRESULT CBasePopupDialog::OnProxySystemClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioProxySystem.GetCheck()==BST_CHECKED?true:false;
	if (val)
		EnabledProxyItem(false);

	if (val)
		proxyMode = "system";

	return 0;
}

LRESULT CBasePopupDialog::OnSocksManualClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioSocksManual.GetCheck()==BST_CHECKED?true:false;
	EnabledSocksItem(val);

	if (val)
		socksMode = "manual";

	return 0;
}

LRESULT CBasePopupDialog::OnSocksNoClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioSocksNo.GetCheck()==BST_CHECKED?true:false;
	if (val)
		EnabledSocksItem(false);

	if (val)
		socksMode = "none";

	return 0;
}

LRESULT CBasePopupDialog::OnSocksSystemClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioSocksSystem.GetCheck()==BST_CHECKED?true:false;
	if (val)
		EnabledSocksItem(false);

	if (val)
		socksMode = "system";

	return 0;
}

LRESULT CBasePopupDialog::OnHttpsManualClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioHttpsManual.GetCheck()==BST_CHECKED?true:false;
	EnabledHttpsItem(val);

	if (val)
		httpsMode = "manual";

	return 0;
}

LRESULT CBasePopupDialog::OnHttpsNoClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioHttpsNo.GetCheck()==BST_CHECKED?true:false;
	if (val)
		EnabledHttpsItem(false);

	if (val)
		httpsMode = "none";

	return 0;
}

LRESULT CBasePopupDialog::OnHttpsSystemClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool val = mRadioHttpsSystem.GetCheck()==BST_CHECKED?true:false;
	if (val)
		EnabledHttpsItem(false);

	if (val)
		httpsMode = "system";

	return 0;
}


LRESULT CBasePopupDialog::OnBnClickedUseSocksUser(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{
	EnabledSocksUserItem(mCheckSocksUser.GetCheck()==BST_CHECKED?true:false);
	return 0;
}

LRESULT CBasePopupDialog::OnBnClickedUseHttpsUser(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{
	EnabledHttpsUserItem(mCheckHttpsUser.GetCheck()==BST_CHECKED?true:false);
	return 0;
}

void CBasePopupDialog::EnabledProxyUserItem(bool val)
{
	mEditProxyUser.EnableWindow(val);
	mEditProxyPass.EnableWindow(val);
	mStaticProxyPass.EnableWindow(val);
	mStaticProxyUser.EnableWindow(val);
}

void CBasePopupDialog::EnabledSocksUserItem(bool val)
{
	mEditSocksUser.EnableWindow(val);
	mEditSocksPass.EnableWindow(val);
	mStaticSocksPass.EnableWindow(val);
	mStaticSocksUser.EnableWindow(val);
}

void CBasePopupDialog::EnabledHttpsUserItem(bool val)
{
	mEditHttpsUser.EnableWindow(val);
	mEditHttpsPass.EnableWindow(val);
	mStaticHttpsPass.EnableWindow(val);
	mStaticHttpsUser.EnableWindow(val);
}

void CBasePopupDialog::EnabledSocksAuthentificate(bool val)
{
	mCheckSocksUser.EnableWindow(val);
	if (val && mCheckSocksUser.GetCheck() != BST_CHECKED)
	{
		return;
	}

	EnabledSocksUserItem( val);
}

void CBasePopupDialog::EnabledProxyItem(bool val)
{
	//mStaticProxy.EnableWindow(val);
	mStaticProxyAddress.EnableWindow(val);
	mEditProxyAddress.EnableWindow(val);
	mStaticProxyPort.EnableWindow(val);
	mEditProxyPort.EnableWindow(val);
	mCheckProxyUser.EnableWindow(val);
	if(!val)
	{
		EnabledProxyUserItem(false);
	}
	else
	{
		if (mCheckProxyUser.GetCheck() == 1)
			EnabledProxyUserItem(true);
	}
}

void CBasePopupDialog::EnabledSocksItem(bool val)
{
	//mStaticSocks.EnableWindow(val);
	mStaticSocksAddress.EnableWindow(val);
	mEditSocksAddress.EnableWindow(val);
	mStaticSocksPort.EnableWindow(val);
	mEditSocksPort.EnableWindow(val);
	mCheckSocksUser.EnableWindow(val);
	mRadioSocks4.EnableWindow(val);
	mRadioSocks5.EnableWindow(val);

	if (mRadioSocks4.GetCheck() == 1)
	{
		mCheckSocksUser.EnableWindow(false);
		EnabledSocksUserItem(false);
	}
	else
	{
		if(!val)
		{
			EnabledSocksUserItem(false);
		}
		else
		{
			if (mCheckSocksUser.GetCheck() == 1)
				EnabledSocksUserItem(true);
		}
	}
}

void CBasePopupDialog::EnabledHttpsItem(bool val)
{
	//mStaticHttps.EnableWindow(val);
	mStaticHttpsAddress.EnableWindow(val);
	mEditHttpsAddress.EnableWindow(val);
	mStaticHttpsPort.EnableWindow(val);
	mEditHttpsPort.EnableWindow(val);
	mCheckHttpsUser.EnableWindow(val);

	if(!val)
	{
		EnabledHttpsUserItem(false);
	}
	else
	{
		if (mCheckHttpsUser.GetCheck() == 1)
			EnabledHttpsUserItem(true);
	}
}

void PlayerEnums(HANDLE ahProcess, HWND ahWnd, void* apUserData)
{
	if( apUserData)
	{
		((CBasePopupDialog*)apUserData)->NotifyPlayer( ahProcess, ahWnd);
	}
}

void CBasePopupDialog::NotifyPlayer(HANDLE ahProcess, HWND ahWnd)
{
	m_notifiedPlayersCount++;
	::PostMessage( ahWnd, WM_RELOAD_SETTINGS, 0, 0);	
}

void CBasePopupDialog::InformExternalPlayer()
{
	// get folder of the current process
	/*wchar_t szPlayerDir[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szPlayerDir, MAX_PATH);
	PathRemoveFileSpecW(szPlayerDir);*/

	CProcessWindowFinder	finder;
	m_notifiedPlayersCount = 0;
	finder.EnumPlayersWindow("player.exe", &PlayerEnums, this);
}
