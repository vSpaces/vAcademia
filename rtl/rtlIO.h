#pragma once

namespace rtl
{
	namespace IO
	{
		VA_RTL_API RtlError rtl_deleteFile(const wchar_t *path);
		VA_RTL_API RtlError rtl_deleteFolder(const wchar_t *path);
	}
}