
#include <wtypes.h>
#include <stdio.h>
#include "VacademiaReportLib.h"

extern VACADEMIAREPORT_API void getApplicationDataDirectory( wchar_t *lpFilePath);
extern VACADEMIAREPORT_API BOOL getFileNameByProjName(  const wchar_t *aFileName, const wchar_t *ext, wchar_t *lpFileName);
extern VACADEMIAREPORT_API TCHAR * lstrrchr(LPCTSTR string, int ch);
extern VACADEMIAREPORT_API WCHAR * lstrrchrW(LPCWSTR string, wchar_t ch);
extern VACADEMIAREPORT_API void FormatTime(LPTSTR output, FILETIME TimeToPrint);
extern VACADEMIAREPORT_API TCHAR * GetFilePart(LPCTSTR source);
extern VACADEMIAREPORT_API WCHAR * GetFilePart(LPCWSTR source);
//////////////////////////////////////////////////////////////////////////
extern VACADEMIAREPORT_API BOOL OpenLog__( WCHAR *sFileName, BOOL bCreateBakup, BOOL bAppend);
extern VACADEMIAREPORT_API BOOL WriteLog__( TCHAR *sInfo);
extern VACADEMIAREPORT_API BOOL WriteLog__( WCHAR *sInfo);
extern VACADEMIAREPORT_API BOOL WriteOnceLog__( FILE *fp__, TCHAR *sKey, TCHAR *sInfo);
extern VACADEMIAREPORT_API void CloseLog__();
extern VACADEMIAREPORT_API BOOL RestoreLog();