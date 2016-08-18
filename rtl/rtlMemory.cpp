#include "StdAfx.h"
#include ".\rtlMemory.h"

namespace rtl
{
	namespace memory
	{
		// Wrapper for strcpy
		VA_RTL_API RtlError rtl_memcpy( void * dst, size_t numberOfElements, const void * src, size_t count)
		{
			ATLASSERT( dst);
			ATLASSERT( src);
			//ATLASSERT( count >= 0 && count<10*1024*1024 /*10 ьс*/);

#if _MSC_VER >= 1500 //vs2008

			errno_t errorCode = memcpy_s( dst,numberOfElements, src, count);
			if( errorCode == 0)
				return RTLERROR_NOERROR;

			ATLASSERT( FALSE);
			return RTLERROR_CONVERTERROR;

#else
			ATLASSERT( numberOfElements >= count);
			memcpy(dst, src, count);
			return RTLERROR_NOERROR;

#endif
		}
	}
}