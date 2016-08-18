// ProgressWnd.h: interface for the CProgressWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRESSWND_H__6714A746_2529_40A8_B3F4_8CD2D336854E__INCLUDED_)
#define AFX_PROGRESSWND_H__6714A746_2529_40A8_B3F4_8CD2D336854E__INCLUDED_

#include "ProgressDlg.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/*
#ifndef PROGRESSWND_H
#define PROGRESSWND_H 
*/
#include "ComManRes.h"
#include "oms_context.h"
using namespace res;

enum	LOADER_NOTIFIES	{ NI_NORESMAN, NI_NORESOURCE, NI_LOADFAILED, NI_SAVEFAILED, NI_EXISTALREADY, NI_COMPLETE, NI_CLOSE, NI_CANCELLED};
enum	WINDOW_MODE	{ WMD_DOWNLOAD, WMD_INIT, WMD_UNINIT};

struct ILoaderNotifyTarget
{
	virtual	void	Notify(DWORD	msg) = 0;
};

class CProgressWnd  : public CWindowImpl<CProgressWnd>
{
public:
	CProgressWnd();
	virtual ~CProgressWnd();

	BEGIN_MSG_MAP(CProgressWnd)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
	END_MSG_MAP()

public:
	void	SetMode(WINDOW_MODE mode);
	void	SetResManLocalBase();
	BOOL	Unzip(HINSTANCE ahInst);
	void	HandleError();
	void	StartDownload(BSTR sURL, BSTR sLocalPath);
	void	SetNotify(ILoaderNotifyTarget*	anotify){ notify = anotify;}
	void	SetResManager(IResMan* val){ sp_ResourceManager = val;}
	void	SetResManager(oms::omsContext aContext){ 
				sp_ResourceManager = aContext.mpIResM; 
				sp_Comman = aContext.mpComMan;
			}

protected:
	CProgressDlg			dialog;
	IResMan		*sp_ResourceManager;
	cm::cmICommunicationManager	*sp_Comman;
	IResource	*sp_Resource;
	IResource	*sp_LocalResource;
	ILoaderNotifyTarget*	notify;
	DWORD			dwArchiveSize;
	DWORD			dwLoadedSize;
	TCHAR			szFullLocal[MAX_PATH];
	TCHAR			szTempDir[MAX_PATH];
	TCHAR			szTempBase[MAX_PATH];
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

#endif // !defined(AFX_PROGRESSWND_H__6714A746_2529_40A8_B3F4_8CD2D336854E__INCLUDED_)
