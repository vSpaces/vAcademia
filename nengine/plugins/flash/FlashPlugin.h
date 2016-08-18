
#pragma once

#if defined (FLASH_EXPORTS)
#  define FLASH_DECL  __declspec(dllexport)
#else
# define FLASH_DECL   __declspec(dllimport)
#endif

FLASH_DECL IFlash* CreateFlashObject(IFlashCallBack* apCallBack, HWND ahwndParentWindow, IResLibrary* apResLibrary);
FLASH_DECL void DeleteFlashObject(IFlash* apFlash);
