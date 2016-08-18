
#pragma once

#if defined (BROWSER_EXPORTS)
#  define BROWSER_DECL  __declspec(dllexport)
#else
# define BROWSER_DECL   __declspec(dllimport)
#endif

BROWSER_DECL IBrowser* CreateBrowserObject(IBrowserCallBack* apCallBack, HWND ahwndParentWindow);
BROWSER_DECL void DeleteBrowserObject(IBrowser* apBrowser);
