
#include "StdAfx.h"
#include "Browser.h"
#include "BrowserPlugin.h"

IBrowser* CreateBrowserObject(IBrowserCallBack* apCallBack, HWND ahwndParentWindow)
{
	CBrowser* pBrowser = new CBrowser(apCallBack, ahwndParentWindow);

	return pBrowser;
}

void DeleteBrowserObject(IBrowser* apBrowser)
{
	if (apBrowser != NULL){
		CBrowser* pBrowser = (CBrowser*)apBrowser;
		delete pBrowser;
	}
}