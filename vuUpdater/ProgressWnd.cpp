// ProgressWnd.cpp: implementation of the CProgressWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProgressWnd.h"
#include <xstring>
#include <commctrl.h>
#include "ComMan.h"

//extern CComModule _Module;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define NOTIFY(x)	if( notify)	notify->Notify(x);
#define BUFSIZE	65535


inline void _tcrep(LPTSTR str, TCHAR tf, TCHAR rep)
{
	while ( *str)
	{
		if (*str == tf)
			*str = rep;
		str = _tcsinc(str);
	}
}

CProgressWnd::CProgressWnd()
{
	notify = NULL;
	sp_LocalResource = NULL;
	sp_Resource = NULL;
}

CProgressWnd::~CProgressWnd()
{
	if(sp_Resource!=NULL)
	{
		sp_Resource->close();
		delete sp_Resource;
		sp_Resource = NULL;
	}
	if(sp_LocalResource!=NULL)
	{
		sp_LocalResource->close();
		delete sp_LocalResource;
		sp_LocalResource = NULL;
	}
}

LRESULT CProgressWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CoInitialize(NULL);
	DefWindowProc(uMsg, wParam, lParam);
	RECT dlgrect;
	dialog.Create(m_hWnd);
	dialog.GetWindowRect(&dlgrect);
	int nTitleHeight  = GetSystemMetrics(SM_CYCAPTION);
    int nBorderWidth  = GetSystemMetrics(SM_CXBORDER);
    int nBorderHeight = GetSystemMetrics(SM_CYBORDER);
    // Account for size of title bar and borders for exact match
    // of window client area to default video size
    SetWindowPos(NULL, 0, 0, (dlgrect.right-dlgrect.left) + 2*nBorderWidth,
            (dlgrect.bottom-dlgrect.top) + nTitleHeight + 2*nBorderHeight,
            SWP_NOMOVE | SWP_NOOWNERZORDER);
	dialog.ShowWindow(SW_SHOW);
	return 0;
}

BOOL _BuildStorage(TCHAR* localBase, TCHAR*	dirTree)
{
	TCHAR directory[MAX_PATH];
	memset(&directory, 0, MAX_PATH);
	GetCurrentDirectory(MAX_PATH, directory);

	TCHAR temp[MAX_PATH];
	memset(&temp, 0, MAX_PATH);
	_tcscat(temp, dirTree);

	if(SetCurrentDirectory(localBase) == FALSE)
		return FALSE;

	LPTSTR start;
	LPTSTR point;
	for(start = point = temp; *point; point++)
	{
		if(*point == _T('\\'))
		{
			*point = _T('\0'); 
			CreateDirectory(start, NULL);
			SetCurrentDirectory(start);
			start = point + 1;
		}
	}

	SetCurrentDirectory(directory);
	return TRUE;
}

void CProgressWnd::StartDownload(BSTR sURL, BSTR sLocalPath)
{
	dwArchiveSize = 0;
	dwLoadedSize = 0;
	memset(&szFullLocal, 0, MAX_PATH);

	if( !sp_ResourceManager)
	{
		NOTIFY(NI_NORESMAN);
		return;
	}
	if( GetEnvironmentVariable(_T("TEMP"), (LPTSTR)&szTempDir, MAX_PATH-1) == 0)
	{
		NOTIFY(NI_SAVEFAILED);
		return;
	}
	TCHAR	szURL[MAX_PATH];
	memset(&szURL, 0, MAX_PATH);
	TCHAR	szLocalPath[MAX_PATH];
	memset(&szLocalPath, 0, MAX_PATH);
	BSTR2TSTR(szLocalPath, sLocalPath, countof(szLocalPath));
	BSTR2TSTR(szURL, sURL, countof(szURL));
	_tcrep((LPTSTR)&szURL, _T('/'), _T('\\'));

	memset(&szTempBase, 0, MAX_PATH);
	_tcscat(szTempBase, szTempDir);

	if( !_BuildStorage((TCHAR*)&szTempDir, (TCHAR*)&szLocalPath))
	{
		NOTIFY(NI_SAVEFAILED);
		HandleError();
		return;
	}
	_tcscat(szTempDir, szLocalPath);

	LPTSTR	point;
	for(point=&szURL[lstrlen(szURL)-1]; point>(LPTSTR)&szURL; point--)
		if(*point == _T('\\'))
			break;
	
	_tcscat(szFullLocal, szTempDir);
	_tcscat(szFullLocal, point);
	
	WCHAR wcFullLocal[MAX_PATH];
	TSTR2BSTR(wcFullLocal, (TCHAR*)&szFullLocal, countof(szFullLocal));

	HandleError();
	
	if( SUCCEEDED(sp_ResourceManager->openResource(sURL, RES_LOCAL|RES_REMOTE|RES_FILE_CACHE|RES_ASYNCH, &sp_Resource)))
	{
		DWORD dwStatus;
		sp_Resource->getStatus( &dwStatus);
		if ((dwStatus & RES_EXISTS) == 0){
			sp_Resource->close();
			delete sp_Resource;
			sp_Resource = NULL;
			NOTIFY(NI_NORESOURCE);
			return;
		}
		if( FAILED(sp_Resource->getSize(&dwArchiveSize)))
		{
			sp_Resource->close();
			delete sp_Resource;
			sp_Resource = NULL;
			NOTIFY(NI_LOADFAILED);
			return;
		}
		int ic = dwArchiveSize / BUFSIZE;
		dialog.SendDlgItemMessage(PBM_SETRANGE, 0, MAKELPARAM(0, 100)); 
	}
	else
	{
		NOTIFY(NI_NORESOURCE);
		return;
	}
	
	SetTimer(0, 5);
}

//		TCHAR	szURL[MAX_PATH];
//		memset(&szURL, 0, MAX_PATH);
//		TCHAR	szLocalPath[MAX_PATH];
//		memset(&szLocalPath, 0, MAX_PATH);
//		BSTR2TSTR(szLocalPath, sLocalPath, countof(szLocalPath));
//		BSTR2TSTR(szURL, sURL, countof(szURL));
//		_tcrep((LPTSTR)&szURL, _T('/'), _T('\\'));
//		_tcrep((LPTSTR)&szLocalPath, _T('/'), _T('\\'));
//		if( GetEnvironmentVariable(_T("TEMP"), (LPTSTR)&szTempDir, MAX_PATH-1) == 0)
//		{
//			NOTIFY(NI_SAVEFAILED);
//			return;
//		}
//		memset(&szTempBase, 0, MAX_PATH);
//		_tcscat(szTempBase, szTempDir);
//
//		DWORD	status;
//		if( !_BuildStorage((TCHAR*)&szTempDir, (TCHAR*)&szLocalPath))
//		{
//			NOTIFY(NI_SAVEFAILED);
//			HandleError();
//			return;
//		}
//		_tcscat(szTempDir, szLocalPath);
//
//		LPTSTR	point;
//		for(point=&szURL[lstrlen(szURL)-1]; point>(LPTSTR)&szURL; point--)
//			if(*point == _T('\\'))
//			{
//				break;
//			}
//		
//		_tcscat(szFullLocal, szTempDir);
//		_tcscat(szFullLocal, point);
//		
//		WCHAR wcFullLocal[MAX_PATH];
//		TSTR2BSTR(wcFullLocal, (TCHAR*)&szFullLocal, countof(szFullLocal));
//		
//		if( SUCCEEDED(sp_ResourceManager->resourceExists(wcFullLocal, RES_LOCAL, &status)) && (status&RES_EXISTS))
//		{
//			// Check file properties
//			CComQIPtr<IResource>	sp_CheckResource;
//			DWORD	dwCheckSize;
//			if( FAILED(sp_ResourceManager->openResource(wcFullLocal, RES_LOCAL|RES_REMOTE, &sp_CheckResource))
//			|| FAILED(sp_CheckResource->getStatus( &dwStatus)) || ((dwStatus & RES_EXISTS) == 0) 
//			|| FAILED(sp_CheckResource->getSize(&dwCheckSize)))
//			{
//				NOTIFY(NI_LOADFAILED);
//				sp_CheckResource->close();
//				sp_CheckResource.Release();
//				HandleError();
//				return;
//			}
//			sp_CheckResource->close();
//			sp_CheckResource.Release();
//			if( dwCheckSize == dwArchiveSize)
//			{
//				SetResManLocalBase();
//				NOTIFY(NI_EXISTALREADY);
//				HandleError();
//				return;
//			}
//		}
//		if( FAILED(sp_ResourceManager->openResource(wcFullLocal, RES_TO_WRITE|RES_LOCAL|RES_CREATE, &sp_LocalResource))
//			|| FAILED(sp_LocalResource->getStatus( &dwStatus)) || ((dwStatus & RES_EXISTS) == 0))
//		{
//			sp_Resource.Release();
//			NOTIFY(NI_SAVEFAILED);
//			HandleError();
//			return;
//		}

LRESULT CProgressWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == 0)
	{
		if( sp_Resource == NULL)
		{
			NOTIFY(NI_LOADFAILED);
			HandleError();
			KillTimer(0);
			return 0;
		}
		
		sp_Resource->getCachedSize( &dwLoadedSize);
		dialog.SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)((float)dwLoadedSize/dwArchiveSize*100), 0);
		char	text[MAX_PATH];
		memset(&text, 0, MAX_PATH);
		sprintf((char*)&text, "Загружено: %d из %d", dwLoadedSize, dwArchiveSize);

		dialog.SendDlgItemMessage(IDC_STATIC, WM_SETTEXT, 0, (LPARAM)&text);
		//dialog.SendDlgItemMessage(IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text);
		dialog.Invalidate();
		DWORD dwStatus;
		sp_Resource->getStatus( &dwStatus);
		if (RES_IS_ERROR( dwStatus))
		{
			KillTimer(0);
			NOTIFY(NI_LOADFAILED);
		}
		else if (dwLoadedSize == dwArchiveSize && (dwStatus & RES_ASYNCH) == 0)
		{
			KillTimer(0);
			text[ 0] = 0;
			//dialog.SendDlgItemMessage(IDC_LABEL, WM_SETTEXT, 0, (LPARAM)&text);
			dialog.SendDlgItemMessage(IDC_STATIC, WM_SETTEXT, 0, (LPARAM)&text);
			SetWindowText(_T("Распаковка..."));
			dialog.SendDlgItemMessage(PBM_SETRANGE, 0, MAKELPARAM(0, 100)); 
			//dialog.SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, 0, 0);
			dialog.Invalidate();
			if (FAILED( sp_Resource->unpack( CComBSTR( szTempDir), FALSE)))
			{
				NOTIFY(NI_SAVEFAILED);
			}
			else
				SetTimer(2, 100);
		}
	}
	else if(wParam == 1)
	{
		dialog.SendDlgItemMessage(IDC_PROGRESS1, PBM_STEPIT, 0, 0);
	}
	else if(wParam == 2)
	{
		DWORD dwStatus;
		sp_Resource->getStatus( &dwStatus);
		ATLTRACE( "Asynch: %d\n", (dwStatus & RES_ASYNCH) != 0);
		if (RES_IS_ERROR( dwStatus))
		{
			KillTimer(1);
			KillTimer(2);
			SetResManLocalBase();
			NOTIFY(NI_SAVEFAILED);
		}
		else if ((dwStatus & RES_ASYNCH) != 0)
		{
			BYTE btPercent;
			sp_Resource->getUnpackProgress( &btPercent);
			dialog.SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, btPercent, 0);
			dialog.Invalidate();
		}
		else
		{
			KillTimer(1);
			KillTimer(2);
			SetResManLocalBase();
			NOTIFY(NI_COMPLETE)
		}
	}
	return 0;
}

void CProgressWnd::HandleError()
{
	if(sp_Resource != NULL)
	{
		sp_Resource->close();
		delete sp_Resource;
		sp_Resource = NULL;
	}
	if(sp_LocalResource != NULL)
	{
		sp_LocalResource->close();
		delete sp_LocalResource;
		sp_LocalResource = NULL;
	}
}

LRESULT CProgressWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CoUninitialize();
	return 0;
}

BOOL CProgressWnd::Unzip(HINSTANCE ahInst)
{
	if( !sp_ResourceManager)	return FALSE;
	HGLOBAL hResBits;
    HMODULE hModule;
    HRSRC hResource;
    DWORD dwSize;

    hModule = ahInst;
    hResource = FindResource (hModule, TEXT("#4000"), RT_RCDATA);
	if( hResource == 0)
		hResource = FindResource (hModule, TEXT("#4000"), RT_BITMAP);
	if( hResource == 0)
		return FALSE;
    hResBits = LoadResource (hModule, hResource);
    dwSize = SizeofResource (hModule, hResource);

	TCHAR	szUnzipPath[MAX_PATH];
	memset(&szUnzipPath, 0, MAX_PATH);
	_tcscat(szUnzipPath, szTempDir);
	_tcscat(szUnzipPath, _T("unzip.bin"));
	DWORD dwStatus;
	IResource *sp_UnzipResource = NULL;
	WCHAR wcUnzipPath[MAX_PATH];
	TSTR2BSTR(wcUnzipPath, (TCHAR*)&szUnzipPath, countof(szUnzipPath));
	if( FAILED(sp_ResourceManager->openResource(wcUnzipPath, RES_TO_WRITE|RES_CREATE|RES_LOCAL, &sp_UnzipResource))
			|| sp_UnzipResource==NULL 
			|| FAILED(sp_UnzipResource->getStatus( &dwStatus)) || ((dwStatus & RES_EXISTS) == 0))
	return FALSE;
	DWORD wrote;
	if( FAILED(sp_UnzipResource->write((const byte*)hResBits, dwSize, &wrote) || wrote!=dwSize))
	{
		sp_UnzipResource->close();
		delete sp_UnzipResource;
		sp_UnzipResource = NULL;
		return FALSE;
	}
	sp_UnzipResource->close();
	delete sp_UnzipResource;
	sp_UnzipResource = NULL;
	// Create process
	TCHAR szName[MAX_PATH];
	TCHAR szCommand[MAX_PATH*4];
	memset(&szName, 0, MAX_PATH);
	memset(&szCommand, 0, 4*MAX_PATH);

	//unzip.exe "C:\Documents and Settings\Alex.MULTIMEDIA.000\Local Settings\Temp\CSCL\English\18.zip" -d "C:\Documents and Settings\Alex.MULTIMEDIA.000\Local Settings\Temp\CSCL\English"
	_tcscat(szCommand, _T("\""));
	_tcscat(szCommand, szTempDir);
	_tcscat(szCommand, _T("\\unzip.bin\" -o \""));
	_tcscat(szCommand, szFullLocal);
	_tcscat(szCommand, "\" -d \"");
	//_tcscat(szCommand, _T("  \""));
	if(lstrlen(szTempDir) > 0)
		szTempDir[lstrlen(szTempDir)-1] = (TCHAR)0;
	_tcscat(szCommand, szTempDir);
	_tcscat(szCommand, "\"");

	/*//pkzipc.exe "C:\Documents and Settings\Alex.MULTIMEDIA.000\Local Settings\Temp\CSCL\English\18.zip" -d "C:\Documents and Settings\Alex.MULTIMEDIA.000\Local Settings\Temp\CSCL\English"
	LPTSTR szLocal = &szFullLocal[countof(szFullLocal)-1];
	while( szLocal>(LPTSTR)&szFullLocal && *szLocal!=_T('\\'))
		szLocal--;
	szLocal = _tcsinc(szLocal);
	_tcscat(szCommand, _T("\""));
	_tcscat(szCommand, szTempDir);
	_tcscat(szCommand, _T("\pkzipc.bin\" -extract -directories "));
	_tcscat(szCommand, szLocal);
	//_tcscat(szCommand, "");
*/
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));

	if(CreateProcess(NULL, szCommand, NULL, NULL, FALSE, CREATE_NO_WINDOW | DETACHED_PROCESS, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return TRUE;
	}
	return FALSE;
}

void CProgressWnd::SetResManLocalBase()
{
	if(sp_Comman != NULL)
	{
		LPTSTR	chOld = (LPTSTR)&szTempBase;
		while ( *chOld)
		{
			if (*chOld == _T('\\'))
				*chOld = _T('/');
			chOld = _tcsinc(chOld);
		}

		WCHAR wcFullLocal[MAX_PATH];
		TSTR2BSTR(wcFullLocal, (TCHAR*)&szTempBase, countof(szTempBase));
		sp_Comman->SetLocalBase(wcFullLocal);
		//sp_ResourceManager->setLocalBase(wcFullLocal);
		//m_pComManager.Release();
	}
}

void CProgressWnd::SetMode(WINDOW_MODE mode)
{
	switch(mode)
	{
	case WMD_INIT:
		dialog.SendDlgItemMessage(PBM_SETRANGE, 0, MAKELPARAM(0, 20)); 
		dialog.SendDlgItemMessage(PBM_SETSTEP, 1, 0); 
		SetTimer(1, 10);
		break;
	case WMD_UNINIT:
		KillTimer(0);
		NOTIFY(NI_CLOSE);
		break;
	}
}

LRESULT CProgressWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE)
	{
		KillTimer(0);
		KillTimer(1);
		HandleError();
		NOTIFY(NI_CANCELLED);
	}
	bHandled = FALSE;
	return DefWindowProc(uMsg, wParam, lParam);
}