// goworldReg.cpp : Defines the entry point for the console application.
//

//
//#using <mscorlib.dll>
//using namespace System;
//using namespace System::Collections;

#include "stdafx.h"
#include "protocol.h"
#include "registry.h"
#include "ProcessFinder.h"

#define	REG_KEY_PROTOCOLSBASE	"Software\\VSpaces\\Protocol\\"
#define WM_SET_URL	(WM_USER + 112)


int main(int argc, _TCHAR* argv[])
{
//	testProtocol();
	if(argc<2)
	{
		printf("Необходимо указать параметр вида <vacademia://location[/#place][?параметры]>");
		return 0;
	}
//
	CProtocol	protocol;
	if( !protocol.parse( argv[1]))
	{
		printf("Неверный параметр <vacademia://island1/#location_name[?параметры]>");
		return 0;
	}

	CRegistry	registry( HKEY_LOCAL_MACHINE);
	CComString	regPath = REG_KEY_PROTOCOLSBASE + CComString("vacademia");
	if( !registry.VerifyKey( regPath))
	{
		printf("Протокол не найден");
		return 0;
	}

	if( !registry.Open( HKEY_LOCAL_MACHINE, regPath))
	{
		printf("Протокол не найден");
		return 0;
	}

	CComString sExecPath;
	if( !registry.VerifyKey( "") || !registry.Read("", sExecPath) || sExecPath.IsEmpty())
	{
		printf("Протокол не заполнен");
		return 0;
	}

	CProcessWindowFinder	finder;
	HWND hWnd = 0;
	sExecPath.TrimLeft();
	sExecPath.TrimRight();
	if( (hWnd = finder.get_process_hwnd(sExecPath)) != 0)
	{
		CComString sUrl(argv[1]);
		COPYDATASTRUCT ds;
		ds.dwData=2;
		ds.cbData=sUrl.GetLength();
		ds.lpData=sUrl.GetBuffer();
		SendMessage( hWnd, WM_COPYDATA, 2, (LPARAM)(LPVOID) &ds);
	}
	else
	{
		CComString sParams( "-url ");
		sParams += CComString(argv[1]);
		CComString sPath(sExecPath);
		sPath.Replace("vu.exe","");
		SetCurrentDirectory(sPath);
		ShellExecute(NULL, "open", sExecPath, sParams, NULL, SW_SHOW);
	}
	return 0;
}