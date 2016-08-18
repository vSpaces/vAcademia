#pragma once

namespace rtl
{
	namespace memory
	{
		VA_RTL_API RtlError rtl_memcpy( void * dst, size_t numberOfElements, const void * src, size_t count);
	}
}