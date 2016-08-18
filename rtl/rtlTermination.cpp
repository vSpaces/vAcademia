#include "StdAfx.h"
#include ".\rtlTermination.h"

namespace rtl
{
	namespace termination
	{
		VA_RTL_API BOOL rtl_TerminateMyProcess(UINT uExitCode)
		{
			return ::TerminateProcess(GetCurrentProcess(), uExitCode);
		}

		VA_RTL_API BOOL rtl_TerminateOtherProcess(HANDLE hProcess, UINT uExitCode)
		{
			if (GetCurrentProcess() == hProcess)
			{
				ATLASSERT( FALSE);
				return FALSE;
			}
			return ::TerminateProcess(hProcess, uExitCode);
		}
	}
}