
#include "StdAfx.h"
#include <assert.h>
#include <windows.h>
#include <winbase.h>
#include "comutil.h"
#include "ComputerInfo.h"
#include "HelperFunctions.h"

CComputerInfo::CComputerInfo()
{
}

void mycpuid(int* cpuInfo, unsigned int code)
{
	int a, b, c, d;
	__asm
	{
		mov eax, code
		cpuid
		mov a, eax
		mov b, ebx
		mov c, ecx
		mov d, edx
	}	

	cpuInfo[0] = a;
	cpuInfo[1] = b;
	cpuInfo[2] = c;
	cpuInfo[3] = d;
}

bool CComputerInfo::IsLaptop()
{
	SYSTEM_POWER_STATUS sps;
	if (GetSystemPowerStatus(&sps))
	{
		return ((sps.BatteryFlag != 128) && (sps.BatteryFlag != 255));
	}
	else
	{
		return false;
	}
}

std::string CComputerInfo::GetProcessorName()
{
	int CPUInfo[4] = {-1};
    mycpuid(&CPUInfo[0], 0x80000000);
    unsigned int nExIds = CPUInfo[0];

    // Get the information associated with each extended ID.
    char CPUBrandString[0x40] = { 0 };
    for( unsigned int i=0x80000000; i<=nExIds; ++i)
    {
        mycpuid(&CPUInfo[0], i);

        // Interpret CPU brand string and cache information.
        if  (i == 0x80000002)
        {
            memcpy( CPUBrandString,
            CPUInfo,
            sizeof(CPUInfo));
        }
        else if( i == 0x80000003 )
        {
            memcpy( CPUBrandString + 16,
            CPUInfo,
            sizeof(CPUInfo));
        }
        else if( i == 0x80000004 )
        {
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }
	}

	return CPUBrandString;
}

std::string CComputerInfo::GetCompName()
{
	char computerName[500];
	unsigned long size = 250;
	GetComputerName((LPTSTR)&computerName, &size);
	
	return computerName;
}

unsigned int CComputerInfo::GetCPUFrequency()
{
	HKEY rKey;
	unsigned char freq[512];	
	DWORD freqSize = sizeof(freq);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &rKey) != ERROR_SUCCESS)
	{
		assert(false);
		return 0;
	}
	if (RegQueryValueEx(rKey, "~MHz", NULL, NULL, &freq[0], &freqSize) != ERROR_SUCCESS)
	{
		assert(false);
		return 0;
	}
	if (RegCloseKey(rKey) != ERROR_SUCCESS)
	{
		assert(false);
		return 0;
	}

	return (freq[0] + (freq[1] << 8));
}

UINT64 CComputerInfo::GetPhysicalMemorySize()
{
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof (stat);
	GlobalMemoryStatusEx(&stat);
	return stat.ullTotalPhys;
}

UINT64 CComputerInfo::GetAvailablePhysicalMemorySize()
{
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof (stat);
	GlobalMemoryStatusEx(&stat);
	return stat.ullAvailPhys;
}

UINT64 CComputerInfo::GetVirtualMemorySize()
{
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof (stat);
	GlobalMemoryStatusEx(&stat);
	return stat.ullTotalVirtual;
}

UINT64 CComputerInfo::GetAvailableVirtualMemorySize()
{
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof (stat);
	GlobalMemoryStatusEx(&stat);
	return stat.ullAvailVirtual;
}

unsigned int CComputerInfo::GetProcessorCount()
{
	_SYSTEM_INFO nfo;
	GetSystemInfo(&nfo);
	return nfo.dwNumberOfProcessors;
}

unsigned int CComputerInfo::GetOSBits()
{
	typedef BOOL (WINAPI *IW64PFP)(HANDLE, BOOL *);

	BOOL res = FALSE;
	IW64PFP IW64P = (IW64PFP)GetProcAddress(GetModuleHandle("kernel32"), "IsWow64Process");

	if (IW64P != NULL)
	{
		IW64P(GetCurrentProcess(), &res);
	}

	return (res == FALSE) ? 32 : 64;
}

std::string CComputerInfo::GetOSVersion()
{
	unsigned int dwVersion = 0;
    unsigned int dwMajorVersion = 0;
    unsigned int dwMinorVersion = 0;
    unsigned int dwBuild = 0;

    dwVersion = GetVersion();
    dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

    if (dwVersion < 0x80000000)              
	{
        dwBuild = (DWORD)(HIWORD(dwVersion));
	}
    
	std::string versionStr = IntToStr(dwMajorVersion);
	versionStr += ".";
	versionStr += IntToStr(dwMinorVersion);
	versionStr += " build ";
	versionStr += IntToStr(dwBuild);

	return versionStr;
}


int CComputerInfo::GetDisplayCount()
{
	return GetSystemMetrics(SM_CMONITORS);
}

int CComputerInfo::GetMouseButtonCount()
{
	return GetSystemMetrics(SM_CMOUSEBUTTONS);
}

bool CComputerInfo::IsMousePresent()
{
	return (GetSystemMetrics(SM_MOUSEPRESENT) != 0);
}

bool CComputerInfo::IsNetworkPresent()
{
	return (GetSystemMetrics(SM_NETWORK) != 0);
}

CComputerInfo::~CComputerInfo()
{
}

/*function GetNumColors: LongInt;
var
  BPP: Integer;
  DC: HDC;
begin
  DC := CreateDC('DISPLAY', nil, nil, nil);
  if DC <> 0 then
    begin
      try
        BPP := GetDeviceCaps(DC, BITPIXEL) * GetDeviceCaps(DC, PLANES);
      finally
        DeleteDC(DC);
      end;
      case BPP of
        1: Result := 2;
        4: Result := 16;
        8: Result := 256;
        15: Result := 32768;
        16: Result := 65536;
        24: Result := 16777216;
      end;
    end
  else
    Result := 0;
end; 

procedure TForm1.button1click(Sender: TObject); 
var 
  lpDisplayDevice: TDisplayDevice; 
  dwFlags: DWORD; 
  cc: DWORD; 
begin 
	form2.memo1.Clear;   
	lpDisplayDevice.cb := sizeof(lpDisplayDevice);   
	dwFlags := 0;   
	cc:= 0;   
	while EnumDisplayDevices(nil, cc, lpDisplayDevice , dwFlags) do   
	  begin   
	    Inc(cc);   
	    form2.memo1.lines.add(lpDisplayDevice.DeviceString);  
	        {Так же мы увидим дополнительную информацию в lpDisplayDevice}   
	    form2.show;   
	  end;   
	
end; 


WinV := GetVersion AND $0000FFFF;   
	Edit6.Text := IntToStr(Lo(WinV))+'.'+IntToStr(Hi(WinV));   


type
  PTOKEN_GROUPS = TOKEN_GROUPS^;

function RunningAsAdministrator(): Boolean;
var
  SystemSidAuthority: SID_IDENTIFIER_AUTHORITY = SECURITY_NT_AUTHORITY;
  psidAdmin: PSID;
  ptg: PTOKEN_GROUPS = nil;
  htkThread: Integer; { HANDLE }
  cbTokenGroups: Longint; { DWORD }
  iGroup: Longint; { DWORD }
  bAdmin: Boolean;
begin
  Result := false;
  if not OpenThreadToken(GetCurrentThread(), // get security token
    TOKEN_QUERY, FALSE, htkThread) then
    if GetLastError() = ERROR_NO_TOKEN then
    begin
      if not OpenProcessToken(GetCurrentProcess(),
        TOKEN_QUERY, htkThread) then
        Exit;
    end
    else
      Exit;

  if GetTokenInformation(htkThread, // get #of groups
    TokenGroups, nil, 0, cbTokenGroups) then
    Exit;

  if GetLastError() <> ERROR_INSUFFICIENT_BUFFER then
    Exit;

  ptg := PTOKEN_GROUPS(getmem(cbTokenGroups));
  if not Assigned(ptg) then
    Exit;

  if not GetTokenInformation(htkThread, // get groups
    TokenGroups, ptg, cbTokenGroups, cbTokenGroups) then
    Exit;

  if not AllocateAndInitializeSid(SystemSidAuthority,
    2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0, psidAdmin) then
    Exit;

  iGroup := 0;
  while iGroup < ptg^.GroupCount do // check administrator group
  begin
    if EqualSid(ptg^.Groups[iGroup].Sid, psidAdmin) then
    begin
      Result := TRUE;
      break;
    end;
    Inc(iGroup);
  end;
  FreeSid(psidAdmin);
end; */

/*
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#define BUFSIZE 256

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

BOOL GetOSDisplayString( LPTSTR pszOS)
{
   OSVERSIONINFOEX osvi;
   SYSTEM_INFO si;
   PGNSI pGNSI;
   PGPI pGPI;
   BOOL bOsVersionInfoEx;
   DWORD dwType;

   ZeroMemory(&si, sizeof(SYSTEM_INFO));
   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
      return 1;

   // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

   pGNSI = (PGNSI) GetProcAddress(
      GetModuleHandle(TEXT("kernel32.dll")), 
      "GetNativeSystemInfo");
   if(NULL != pGNSI)
      pGNSI(&si);
   else GetSystemInfo(&si);

   if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && 
        osvi.dwMajorVersion > 4 )
   {
      StringCchCopy(pszOS, BUFSIZE, TEXT("Microsoft "));

      // Test for the specific product.

      if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
      {
         if( osvi.wProductType == VER_NT_WORKSTATION )
             StringCchCat(pszOS, BUFSIZE, TEXT("Windows Vista "));
         else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 " ));

         pGPI = (PGPI) GetProcAddress(
            GetModuleHandle(TEXT("kernel32.dll")), 
            "GetProductInfo");

         pGPI( 6, 0, 0, 0, &dwType);

         switch( dwType )
         {
            case PRODUCT_ULTIMATE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Ultimate Edition" ));
               break;
            case PRODUCT_HOME_PREMIUM:
               StringCchCat(pszOS, BUFSIZE, TEXT("Home Premium Edition" ));
               break;
            case PRODUCT_HOME_BASIC:
               StringCchCat(pszOS, BUFSIZE, TEXT("Home Basic Edition" ));
               break;
            case PRODUCT_ENTERPRISE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition" ));
               break;
            case PRODUCT_BUSINESS:
               StringCchCat(pszOS, BUFSIZE, TEXT("Business Edition" ));
               break;
            case PRODUCT_STARTER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Starter Edition" ));
               break;
            case PRODUCT_CLUSTER_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Cluster Server Edition" ));
               break;
            case PRODUCT_DATACENTER_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition" ));
               break;
            case PRODUCT_DATACENTER_SERVER_CORE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition (core installation)" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER_CORE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition (core installation)" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER_IA64:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition for Itanium-based Systems" ));
               break;
            case PRODUCT_SMALLBUSINESS_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server" ));
               break;
            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
               StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server Premium Edition" ));
               break;
            case PRODUCT_STANDARD_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition" ));
               break;
            case PRODUCT_STANDARD_SERVER_CORE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition (core installation)" ));
               break;
            case PRODUCT_WEB_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Web Server Edition" ));
               break;
         }
         if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
            StringCchCat(pszOS, BUFSIZE, TEXT( ", 64-bit" ));
         else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
            StringCchCat(pszOS, BUFSIZE, TEXT(", 32-bit"));
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
      {
         if( GetSystemMetrics(SM_SERVERR2) )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Server 2003 R2, "));
         else if ( osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Storage Server 2003"));
         else if( osvi.wProductType == VER_NT_WORKSTATION &&
                  si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
         {
            StringCchCat(pszOS, BUFSIZE, TEXT( "Windows XP Professional x64 Edition"));
         }
         else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2003, "));

         // Test for the server type.
         if ( osvi.wProductType != VER_NT_WORKSTATION )
         {
            if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
            {
                if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition for Itanium-based Systems" ));
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition for Itanium-based Systems" ));
            }

            else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
            {
                if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter x64 Edition" ));
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise x64 Edition" ));
                else StringCchCat(pszOS, BUFSIZE, TEXT( "Standard x64 Edition" ));
            }

            else
            {
                if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Compute Cluster Edition" ));
                else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition" ));
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition" ));
                else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Web Edition" ));
                else StringCchCat(pszOS, BUFSIZE, TEXT( "Standard Edition" ));
            }
         }
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
      {
         StringCchCat(pszOS, BUFSIZE, TEXT("Windows XP "));
         if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Home Edition" ));
         else StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" ));
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
      {
         StringCchCat(pszOS, BUFSIZE, TEXT("Windows 2000 "));

         if ( osvi.wProductType == VER_NT_WORKSTATION )
         {
            StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" ));
         }
         else 
         {
            if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
               StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Server" ));
            else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
               StringCchCat(pszOS, BUFSIZE, TEXT( "Advanced Server" ));
            else StringCchCat(pszOS, BUFSIZE, TEXT( "Server" ));
         }
      }

       // Include service pack (if any) and build number.

      if( _tcslen(osvi.szCSDVersion) > 0 )
      {
          StringCchCat(pszOS, BUFSIZE, TEXT(" ") );
          StringCchCat(pszOS, BUFSIZE, osvi.szCSDVersion);
      }

      TCHAR buf[80];

      StringCchPrintf( buf, 80, TEXT(" (build %d)"), osvi.dwBuildNumber);
      StringCchCat(pszOS, BUFSIZE, buf);

      return TRUE; 
   }

   else
   {  
      printf( "This sample does not support this version of Windows.\n");
      return FALSE;
   }
}

int __cdecl _tmain()
{
    TCHAR szOS[BUFSIZE];

    if( GetOSDisplayString( szOS ) )
        _tprintf( TEXT("\n%s\n"), szOS );
}
*/