#include "StdAfx.h"
#include ".\rtlIO.h"

namespace rtl
{
	namespace IO
	{
		VA_RTL_API RtlError rtl_deleteFile(const wchar_t *path)
		{
			if(path == NULL)
				return RTLERROR_NULL_PATH;
			
			errno_t errorCode = DeleteFileW(path);
			if( errorCode != 0)
				return RTLERROR_FILE_ACCESS_DENIDED;

			return RTLERROR_NOERROR;
		}

		VA_RTL_API RtlError rtl_deleteFolder(const wchar_t *path)
		{

			if(path == NULL) 
				return RTLERROR_NULL_PATH;

			errno_t errorCode = RemoveDirectoryW(path);
			if( errorCode != 0)
				return RTLERROR_FOLDER_ACCESS_DENIDED;
					
			return RTLERROR_NOERROR;		
		}
	}
}