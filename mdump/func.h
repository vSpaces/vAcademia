
#include <wtypes.h>
#include <stdio.h>

extern BOOL getUUID( char **psUUID);
extern BOOL GetValueStrFromIni( WCHAR *fileName, TCHAR *lpSection, TCHAR *lpKey, TCHAR **sValue, FILE *fp, int count = MAX_PATH);
extern BOOL GetValueStrFromIni( WCHAR *fileName, TCHAR *lpSection, TCHAR *lpKey, WCHAR **sValue, FILE *fp, int count = MAX_PATH);
extern void getApplicationDataDirectory( wchar_t *lpFilePath);
extern BOOL getFileNameByProjName( const char *aFileName, const wchar_t *ext, wchar_t *lpFileName);
extern TCHAR * lstrrchr(LPCTSTR string, int ch);
extern WCHAR * lstrrchrW(LPCWSTR string, WCHAR ch);
extern void FormatTime(LPTSTR output, FILETIME TimeToPrint);
extern TCHAR * GetFilePart(LPCTSTR source);
extern WCHAR * GetFilePart(LPCWSTR source);
extern int lstrpos(LPCTSTR string, LPCTSTR strfind);
extern TCHAR * substr(LPCTSTR string, int pos, int len);

extern void DeleteLog__( TCHAR *sFileName, TCHAR *m_szAppName);
extern BOOL OpenLog__( FILE **fp__, WCHAR *sFileName, TCHAR *m_szAppName);
extern BOOL WriteLog__( FILE *fp__, TCHAR *sInfo);
extern BOOL WriteLog__( FILE *fp__, WCHAR *sInfo);
extern BOOL WriteLog__( TCHAR *sInfo);
extern BOOL WriteLog__( WCHAR *sInfo);
extern BOOL WriteOnceLog__( FILE *fp__, TCHAR *sInfo);
extern void CloseLog__( FILE **fp__);

extern int ZipFiles( int iCrashType, TCHAR *szProjectName, WCHAR *szDumpPath, FILE *fp__, TCHAR *sMumpLog);

#define MAX_PATH_4  4 * MAX_PATH