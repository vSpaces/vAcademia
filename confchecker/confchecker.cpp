
#include "stdafx.h"
#include <string>
#include "confchecker.h"
#include "ConfigurationChecker.h"
#include "Globals.h"
#include "Shellapi.h"
#include "HelperFunctions.h"
#include "shlobj.h"
#include <windows.h>
#include "VideoMemory.h"

#define ERROR_FLAG_REQUIRED_EXTENSIONS_NOT_EXISTS	1
#define ERROR_FLAG_DRIVERS_NOT_INSTALLED			2
#define ERROR_FLAG_VENDOR_UNSUPPORTED				4
#define ERROR_FLAG_DRIVER_UNSUPPORTED				8
#define ERROR_FLAG_DRIVER_BETA_VERSION				16
#define ERROR_FLAG_16_BIT_PER_PIXEL					32
#define ERROR_FLAG_DRIVER_INVALID					64
#define ERROR_FLAG_NOT_ADMIN						128

CGLApp glApp("configuration checker", 160, 100);

CComputerInfo compInfo;
#define URL_BUFFER_LENGTH 2048
#define URL_STRING_LENGTH (URL_BUFFER_LENGTH-1)
static	char urlBuffer[URL_BUFFER_LENGTH];
static	char errorBuffer[URL_BUFFER_LENGTH];
static bool bUrl = false;
static bool bVac4Page = false;
static bool bDebug = false;

#define LANG_RU		0
#define LANG_EN		1

#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))

static const std::string base64_chars = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

int UrlEncode(const char *source, char *dest, unsigned max)  
{
	static const char *digits = "0123456789ABCDEF";
	unsigned char ch;
	unsigned len = 0;
	char *start = dest;

	while (len < max - 4 && *source)
	{
		ch = (unsigned char)*source;
		if (*source == ' ') {
			*dest++ = '+';
		}
		else if (isalnum(ch) || strchr("-_.!~*'()", ch)) {
			*dest++ = *source;
		}
		else {
			*dest++ = '%';
			*dest++ = digits[(ch >> 4) & 0x0F];
			*dest++ = digits[       ch & 0x0F];
		}  
		source++;
	}
	*dest = 0;
	return start - dest;
}

std::string UrlEncodeString(const char *sz_decoded) {
	//size_t needed_length = strlen(decoded.length() * 3 + 3;
	char * buf = (char *)malloc(1024);
	UrlEncode(sz_decoded, buf, 1024);
	std::string result(buf);
	free(buf);
	return result;
}

void AddParam( char *paramName)
{
	if( strchr( urlBuffer, '?') != NULL)
		strcat( urlBuffer, "&");
	else
		strcat( urlBuffer, "?");
	strcat( urlBuffer, paramName);
}

void AddParam( char *paramName, char *paramValue)
{
	if( strchr( urlBuffer, '?') != NULL)
		strcat( urlBuffer, "&");
	else
		strcat( urlBuffer, "?");
	std::string sParamValue = UrlEncodeString(paramValue);	
	strcat( urlBuffer, paramName);
	strcat( urlBuffer, "=");
	strcat( urlBuffer, sParamValue.c_str());
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	std::string sLang = "lang";
	std::string sLangValue = "rus";
	if (strstr(lpCmdLine, "-language eng") != NULL)
	{
		glApp.SetLangID(LANG_EN);
		sLangValue = "eng";
	}

	LPSTR lpHttpBegin = strstr(lpCmdLine, "http:");
	ZeroMemory(urlBuffer, URL_BUFFER_LENGTH);
	if( lpHttpBegin != NULL)
	{
		LPSTR lpHttpEnd = lpHttpBegin;
		while( *lpHttpEnd != ' ' && *lpHttpEnd != 0)
			lpHttpEnd++;

		unsigned int urlLength = lpHttpEnd - lpHttpBegin;
		if( urlLength > URL_BUFFER_LENGTH)
			urlLength = URL_BUFFER_LENGTH;
		lstrcpyn( (LPSTR)&urlBuffer[0], lpHttpBegin, urlLength+1);
		urlBuffer[urlLength] = 0;
		AddParam( (char*)sLang.c_str(), (char*)sLangValue.c_str());
		bUrl = true;
	}

	if( strstr(lpCmdLine, "-vac4") != NULL)
	{
		bVac4Page = true;
	}

	if( strstr(lpCmdLine, "-debug") != NULL)
	{
		bDebug = true;
	}

	LPSTR lpErrorBegin = strstr(lpCmdLine, "errorCode");
	ZeroMemory(errorBuffer, URL_BUFFER_LENGTH);
	if( lpErrorBegin != NULL)
	{
		LPSTR lpErrorEnd = lpErrorBegin;
		while( *lpErrorEnd != ' ' && *lpErrorEnd != 0)
			lpErrorEnd++;

		unsigned int urlLength = lpErrorEnd - lpErrorBegin;
		if( urlLength > URL_BUFFER_LENGTH)
			urlLength = URL_BUFFER_LENGTH;
		lstrcpyn( (LPSTR)&errorBuffer[0], lpErrorBegin+9, urlLength+1-9);
		errorBuffer[urlLength] = 0;
	}

	glApp.WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

CGLApp::CGLApp(std::string name, unsigned int width, unsigned int height):
	CGLApplication(name, width, height, true)
{
	m_langID = 0;
	m_strings[0][LANG_RU] = "Проверка конфигурации";
	m_strings[0][LANG_EN] = "Checking configuration";
	m_strings[1][LANG_RU] = "Не хватает необходимых OpenGL-расширений";
	m_strings[1][LANG_EN] = "Some of required OpenGL extenstions are not found";
	m_strings[2][LANG_RU] = "Не установлены драйверы для видеокарты!";
	m_strings[2][LANG_EN] = "Drivers for your videocard are not installed!";
	m_strings[3][LANG_RU] = "Установлены устаревшие драйверы для видеокарты!";
	m_strings[3][LANG_EN] = "Installed drivers for your videocard are outdated!";
	m_strings[4][LANG_RU] = "В браузере будет открыта страничка загрузки драйверов для вашей видеокарты! Пожалуйста, установите свежие драйверы до запуска Виртуальной Академии, в противном случае запуск может окончиться неудачей или программа может работать с ошибками...";
	m_strings[4][LANG_EN] = "Now driver download page will be opened in browser! Please, update videodrivers to get good and stable work of program!";
	m_strings[5][LANG_RU] = "Для справки: ваша видеокарта называется ";
	m_strings[5][LANG_EN] = "Your videocard model is ";
	m_strings[6][LANG_RU] = "Программа не проверялась на совместимость работы с видеокартами производителя вашей видеокарты. Возможно программа будет работать некорректно или будет работать только в низком режиме качества!";
	m_strings[6][LANG_EN] = "Program may be not compatable with your videocard because of unknown videocard vendor. Maybe, program will be working uncorrect or slow or only in low quality mode!";
}

void CGLApp::SetLangID(int langID)
{
	m_langID = langID;
}

std::string GetBrowserString(std::string inp)
{
	std::string res;
	char tmp[2];
	tmp[1] = 0;

	for (int i = 0; i < inp.size(); i++)
	if (inp[i] != ' ')
	{
		tmp[0] = inp[i];
		res += tmp;
	}

	return res;
}

std::string GetApplicationDataDirectory()
{
	CHAR tmp[MAX_PATH];
	SHGetSpecialFolderPath(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}

void OpenUrl( LPSTR urlBuffer)
{
	HINSTANCE hinst = ShellExecute( 0, "open", (LPSTR)urlBuffer, "", "", SW_SHOWNORMAL);
	if((int)hinst < 33)
	{
		hinst = ShellExecute( 0, "open", "iexplore" , (LPSTR)urlBuffer, "", SW_SHOWNORMAL);

		if((int)hinst < 33)
		{
			HKEY key;
			std::string sPath;
			long resKey = RegOpenKeyEx(HKEY_CLASSES_ROOT, TEXT("http\\shell\\open\\command"), 0, KEY_QUERY_VALUE, &key);
			bool bSuccess = resKey == ERROR_SUCCESS ? true : false;
			if ( bSuccess)
			{
				int i = 0;
				DWORD dwName =  256;
				char name[256];
				DWORD dwData = 3124;
				char data[3124];
				DWORD type = REG_SZ;			
				while( RegEnumValue( key, i, &name[0], &dwName, NULL, &type, (BYTE*)&data[0], &dwData) == ERROR_SUCCESS)
				{
					sPath = data;
					int pos = sPath.find(".exe");
					if(pos > -1)
					{
						sPath = sPath.substr( 0, pos + 4);
						if( sPath[0] == '\"')
							sPath = sPath.substr(1, sPath.size()-1);				
						break;
					}
					else
						sPath = "";
					i++;
				}
				RegCloseKey(key);
				if(sPath.size())
				{
					STARTUPINFO si;
					ZeroMemory(&si,sizeof(si));
					si.cb = sizeof(si);			
					PROCESS_INFORMATION pi;
					ZeroMemory(&pi,sizeof(pi));		
					std::string sCmd = "-url ";
					sCmd += urlBuffer;
					BOOL result = CreateProcess((LPSTR)sPath.c_str(), (LPSTR)sCmd.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
					if( result)
					{
						CloseHandle(pi.hProcess);
						CloseHandle(pi.hThread);				
					}
					else
					{
						//int err = GetLastError();
						//err+=0;
						bSuccess = false;					
					}	
				}
				else
					bSuccess = false;
			}
			if(!bSuccess )
			{
				STARTUPINFO si;
				ZeroMemory(&si,sizeof(si));
				si.cb = sizeof(si);			
				PROCESS_INFORMATION pi;
				ZeroMemory(&pi,sizeof(pi));	
				char data[3124];
				std::string sCmd = "-url ";
				sCmd += urlBuffer;
				SHGetSpecialFolderPath(NULL, data, CSIDL_PROGRAM_FILES, FALSE);
				sPath = data;
				sPath += "\\Internet Explorer\\iexplore.exe";
				BOOL result = CreateProcess((LPSTR)sPath.c_str(), (LPSTR)sCmd.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
				if( result)
				{
					CloseHandle(pi.hProcess);
					CloseHandle(pi.hThread);				
				}
			}
		}
	}
}

bool CGLApp::DrawScene()
{
	bool isNewDriverWanted = false;

	CVideoMemory videoMemory;

	int vendorCheckAnswer = confChecker.IsVendorSupported();

	std::string browserString = "gpu_vendor=";
	browserString += UrlEncodeString(gpuInfo.GetVendorName().c_str());
	browserString += "&gpu_model=";
	if (vendorCheckAnswer == DRIVERS_NOT_INSTALLED)
		browserString += UrlEncodeString(gpuInfo.GetDeviceName().c_str());
	else
		browserString += UrlEncodeString(gpuInfo.GetRendererGLName().c_str());	
	browserString += "&gpu_memory=";
	browserString += UrlEncodeString(IntToStr(videoMemory.GetTotalSize(gpuInfo.GetVendorID())).c_str());
	int lead, major, minor;
	gpuInfo.GetDriverVersion(lead, major, minor);
	browserString += "&video_driver=";
	browserString += UrlEncodeString(IntToStr(major).c_str());
	browserString += ".";
	browserString += UrlEncodeString(IntToStr(minor).c_str());
	browserString += "&os=";
	browserString += UrlEncodeString(compInfo.GetOSVersion().c_str());	
	browserString += "&sys_mem=";
	browserString += UrlEncodeString(ULongToStr(compInfo.GetPhysicalMemorySize()).c_str());	
	browserString += "&core_count=";
	browserString += UrlEncodeString(IntToStr(compInfo.GetProcessorCount()).c_str());	
	browserString += "&bits=";
	browserString += UrlEncodeString(IntToStr(compInfo.GetOSBits()).c_str());	
	browserString += "&processor=";
	browserString += UrlEncodeString(compInfo.GetProcessorName().c_str());	
	browserString += "&gpu_generation=";
	browserString += UrlEncodeString(IntToStr(gpuInfo.GetVideocardGeneration()).c_str());
	browserString += "&cpu_freq=";
	browserString += UrlEncodeString(IntToStr(compInfo.GetCPUFrequency()).c_str());

	char pVersion[256];
	char pModuleDirectory[3124];
	GetCurrentDirectory( 3124, pModuleDirectory);
	std::string sModuleDirectory = pModuleDirectory;
	sModuleDirectory += "\\player.ini";
	if (GetPrivateProfileString( _T("settings"), _T("version"), _T(""), pVersion, MAX_PATH, sModuleDirectory.c_str())) 
	{
		browserString += "&version=";
		browserString += UrlEncodeString(pVersion);
	}

	char laptop = compInfo.IsLaptop()? '1' : '0'; 
	browserString += "&laptop=";
	browserString += laptop;

	char szUserName[2048];
	ZeroMemory(szUserName, sizeof(szUserName));
	DWORD UserNameSize = _countof(szUserName)-2;
	GetUserName(szUserName, &UserNameSize);
	if( szUserName[0] != 0)
	{
		browserString += "&user_name=";
		browserString += UrlEncodeString(szUserName);
	}

	char szComputerName[2048];
	ZeroMemory(szComputerName, sizeof(szComputerName));
	DWORD ComputerNameSize = _countof(szComputerName)-2;
	GetComputerName(szComputerName, &ComputerNameSize);
	if( szComputerName[0] != 0)
	{
		browserString += "&comp_name=";
		browserString += UrlEncodeString(szComputerName);
	}

	if( bUrl)
		AddParam((char*)browserString.c_str());

	int errorFlags = 0;

	if (!confChecker.AreRequiredExtensionsExists())
	{
		if( bUrl)
		{
			errorFlags += ERROR_FLAG_REQUIRED_EXTENSIONS_NOT_EXISTS;			
		}
		else
			MessageBox(NULL, m_strings[1][m_langID].c_str(), m_strings[0][m_langID].c_str(), MB_ICONERROR);
		isNewDriverWanted = true;
	}

	if (vendorCheckAnswer == DRIVERS_NOT_INSTALLED)
	{
		if( bUrl)
		{
			errorFlags += ERROR_FLAG_DRIVERS_NOT_INSTALLED;			
		}
		else
			MessageBox(NULL, m_strings[2][m_langID].c_str(), m_strings[0][m_langID].c_str(), MB_ICONERROR);
		isNewDriverWanted = true;
	}
	else if (vendorCheckAnswer == VENDOR_UNSUPPORTED)
	{
		if( bUrl)
		{
			errorFlags += ERROR_FLAG_VENDOR_UNSUPPORTED;			
		}
		else
			MessageBox(NULL, m_strings[6][m_langID].c_str(), m_strings[0][m_langID].c_str(), MB_ICONERROR);
		isNewDriverWanted = true;
	}
	else if (vendorCheckAnswer == DRIVER_UNSUPPORTED_VERSION)
	{		
		if( bUrl)
		{
			errorFlags += ERROR_FLAG_DRIVER_UNSUPPORTED;			
		}
		else
		{
			int lead, major, minor;
			gpuInfo.GetDriverVersion(lead, major, minor);
			MessageBox(NULL, m_strings[3][m_langID].c_str(), m_strings[0][m_langID].c_str(), MB_ICONERROR);
		}
		isNewDriverWanted = true;
	}
	else if (vendorCheckAnswer == DRIVER_INVALID)
	{		
		if( bUrl)
		{
			errorFlags += ERROR_FLAG_DRIVER_INVALID;			
		}
		else
		{
			int lead, major, minor;
			gpuInfo.GetDriverVersion(lead, major, minor);
			MessageBox(NULL, m_strings[3][m_langID].c_str(), m_strings[0][m_langID].c_str(), MB_ICONERROR);
		}
		isNewDriverWanted = true;
	}

	if (vendorCheckAnswer != DRIVER_UNSUPPORTED_VERSION)
	if (!gpuInfo.IsOfficialVersion())
	{
		if( bUrl)
		{
			errorFlags += ERROR_FLAG_DRIVER_BETA_VERSION;			
		}
		isNewDriverWanted = true;
	}

	HKEY rKey;	
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, "vacademia\\shell\\open", 0, KEY_QUERY_VALUE, &rKey) != ERROR_SUCCESS)
	{		
		errorFlags += ERROR_FLAG_NOT_ADMIN;
	}	

	/*
	закомментарил, т.к. клиент научился сам переводить десктоп в другое состояние (32 бит)

	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
	if (dm.dmBitsPerPel == 16)
	{
		if( bUrl)
		{
			errorFlags += ERROR_FLAG_16_BIT_PER_PIXEL;			
		}
	}*/	

	if ((bUrl) && (errorFlags != 0))
	{
		AddParam("error_flags", (char *)IntToStr(errorFlags).c_str());
		if( errorBuffer[0] != 0)
			AddParam("error_flags", (char *)errorBuffer);
	}

	if (isNewDriverWanted)
	{
		int vendorID = gpuInfo.GetVendorID();

		if(!bUrl)
		{			
			std::string mes = m_strings[4][m_langID].c_str();
			std::string deviceName = gpuInfo.GetDeviceName();
			mes += "\n";
			mes += "\n";
			if (deviceName != "Unknown")
			{
				mes += m_strings[5][m_langID].c_str() + deviceName;
			}
			else
			{
				if (vendorCheckAnswer == DRIVERS_NOT_INSTALLED)
				{
					mes += m_strings[5][m_langID].c_str() + deviceName + "(" + gpuInfo.GetDeviceStringID() +")";
				}
				else
				{
					mes += m_strings[5][m_langID].c_str() + gpuInfo.GetRendererGLName();
				}
			}

			MessageBox(NULL, mes.c_str(), m_strings[0][m_langID].c_str(), MB_ICONINFORMATION);			
			
			switch (vendorID)
			{
			case VENDOR_NVIDIA:
				{
					HINSTANCE  res = ShellExecute(NULL, "open", "http://www.nvidia.ru/Download/index.aspx", "", "", SW_SHOW);

					if( (int)res <= 32)
					{
						res = ShellExecute( 0, "open", "iexplore" , "http://www.nvidia.ru/Download/index.aspx", "", SW_SHOW);
					}
				}
					break;

			case VENDOR_ATI:
				{
					HINSTANCE  res = ShellExecute(NULL, "open", "http://support.amd.com/us/gpudownload/Pages/index.aspx", "", "", SW_SHOW);
					if( (int)res <= 32)
					{
						res = ShellExecute( 0, "open", "iexplore" , "http://support.amd.com/us/gpudownload/Pages/index.aspx", "", SW_SHOW);
					}
				}
				break;

			case VENDOR_INTEL:
				{
					HINSTANCE  res = ShellExecute(NULL, "open", "http://downloadcenter.intel.com/Default.aspx", "", "", SW_SHOW);
					if( (int)res <= 32)
					{
						res = ShellExecute( 0, "open", "iexplore" , "http://downloadcenter.intel.com/Default.aspx", "", SW_SHOW);
					}
				}
				break;
			}
		}		
	}

	if( bVac4Page)
	{
		OpenUrl( "http://www.ntonyx.com/vac.htm");
	}

	if(bUrl)
	{
		OpenUrl( (LPSTR)urlBuffer);

		if (bDebug)
		{
			FILE* fl = fopen("debug.txt", "wb");
			fprintf(fl, urlBuffer);
			fprintf(fl, "\n");
			fprintf(fl, "total: %d\n", videoMemory.GetDirectDrawTotal());
			fprintf(fl, "totalFree: %d\n", videoMemory.GetDirectDrawFree());
			fprintf(fl, "local: %d\n", videoMemory.GetDirectDrawLocal());
			fprintf(fl, "localFree: %d\n", videoMemory.GetDirectDrawLocalFree());
			fprintf(fl, "agp: %d\n", videoMemory.GetDirectDrawLocalAGP());
			fprintf(fl, "vendorID: %d\n", gpuInfo.GetVendorID());
			fprintf(fl, "vendorName: %d\n", gpuInfo.GetVendorName());
			fprintf(fl, "vendorString: %d\n", gpuInfo.GetVendorStringID());
			fclose(fl);
		}
	}

	return false;
}

void CGLApp::OnLoading()
{
	gpuInfo.Analyze();
	confChecker.InitFromXML("");
}