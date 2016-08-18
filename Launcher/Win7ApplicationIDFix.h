#pragma once

typedef HRESULT (*SETCURRENTPROCESSEXPLICITAPPUSERMODELIDPROC)(PCWSTR AppID);

// Gives this process an explicit App User Model ID, so that it can be treated as one item (with
//   the main application window and the shortcut) by Windows 7.
void SetAppUserModelId()
{
	return;
	// try to load Shell32.dll
	HMODULE hmodShell32 = LoadLibrary("shell32.dll");
	if (hmodShell32 != NULL)
	{
		// see if the function is exposed by the current OS
		SETCURRENTPROCESSEXPLICITAPPUSERMODELIDPROC pfnSetCurrentProcessExplicitAppUserModelID =
			reinterpret_cast<SETCURRENTPROCESSEXPLICITAPPUSERMODELIDPROC>(GetProcAddress(hmodShell32,
			"SetCurrentProcessExplicitAppUserModelID"));
		if (pfnSetCurrentProcessExplicitAppUserModelID != NULL)
		{
			pfnSetCurrentProcessExplicitAppUserModelID(L"VirtualSpaces.VirtualAcademy.1");
		}
		FreeLibrary(hmodShell32);
	}
}
