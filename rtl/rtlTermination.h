#pragma once

namespace rtl
{
	namespace termination
	{
		VA_RTL_API int rtl_TerminateMyProcess(UINT uExitCode);
		VA_RTL_API int rtl_TerminateOtherProcess(HANDLE hProcess, UINT uExitCode);
	}
}