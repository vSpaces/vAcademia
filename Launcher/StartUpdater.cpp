#ifdef RMML_BUILD
#include "mlRMML.h"
#endif

#include "StartUpdater.h"
#include "launchererrors.h"

#include <windows.h>
#include <mmsystem.h>
#include <atlconv.h>

#ifdef LAUNCHER_SLN
#define MP_NEW_ARR(a, b) new a[b]
#define MP_DELETE_ARR(a) delete[] a
#endif

CStartUpdater::CStartUpdater()
{
}

CStartUpdater::~CStartUpdater(void)
{
}

std::wstring CStartUpdater::prepareUpdate(const std::wstring& sRootDir, const std::wstring& sAppDir, unsigned int& auErrorCode, unsigned int& auSystemErrorCode)
{
	// Копирование нужных файлов во временную директорию и запуск апдейтера из нее. Это позволит корректно заменять рабочие файлы в папке с установленной академией.
	// Нужные файлы:
	// - Comman.dll 
	// - libcurl.dll
	// - zlib32.dll
	// - LogWriter.dll
	// - mdump.dll
	// - rtl.dll 
	// - ServiceMan.dll 
	// - player.ini.
	// - updater.exe
	// - iconv.dll
	// - libxml2.dll
	// - BigFileMan.dll

	std::wstring sUpdaterDir = createDirectories( sAppDir, auErrorCode, auSystemErrorCode);
	if( auErrorCode != LAUNCH_ERROR_NO_ERROR)
		return L"";

	const wchar_t*	lpFiles[] = {L"Comman.dll"
						, L"libcurl.dll"
						, L"LogWriter.dll"
						, L"mdump.dll"
						, L"rtl.dll"
						//, L"ServiceMan.dll"
						, L"zlib32.dll"
						, L"player.ini"
						, L"libxml2.dll"
						, L"iconv.dll"
						, L"BigFileMan.dll"
						, L"msvcp100.dll"
						, L"msvcr100.dll"						
						, L"updater.exe"
	};

	auErrorCode = copyFiles( sRootDir, sUpdaterDir, lpFiles, sizeof(lpFiles)/sizeof(lpFiles[0]), auSystemErrorCode);

	std::wstring path = sUpdaterDir;
	path += L"vacademia_path.txt";
	FILE*  fl = NULL;
	int res =_wfopen_s(&fl, path.c_str(), L"w");
	if (fl && res == 0)
	{
		fwrite( sRootDir.c_str(),sRootDir.size()*sizeof(wchar_t),1, fl);
		fclose(fl);
	}

	if( auErrorCode != LAUNCH_ERROR_NO_ERROR)
		return sUpdaterDir;

	std::wstring pathInfoUpdater = sAppDir;
	pathInfoUpdater += L"\\updater_path.txt";
	FILE*  fl1 = NULL;
	res = _wfopen_s(&fl1, pathInfoUpdater.c_str(), L"w");
	if (fl1 && res == 0)
	{
		fwrite( sUpdaterDir.c_str(),sUpdaterDir.size()*sizeof(wchar_t), 1, fl1);
		fclose(fl1);
	}

	return sUpdaterDir;
}

std::wstring CStartUpdater::createDirectories(const std::wstring& asBaseDir, unsigned int& auErrorCode, unsigned int& auSystemErrorCode)
{
	auErrorCode = LAUNCH_ERROR_NO_ERROR;

	std::wstring sUpdaterDir = asBaseDir + L"\\Vacademia";
//MessageBoxW(0, sUpdaterDir.c_str(), L"1", MB_OK);
	USES_CONVERSION;
	if( !CreateDirectoryW(sUpdaterDir.c_str(),NULL))
	{
		unsigned int errorCode = GetLastError();
		auSystemErrorCode = errorCode;
		if( errorCode == ERROR_ACCESS_DENIED ||  errorCode == ERROR_INVALID_ACCESS)
		{			
			auErrorCode = LAUNCH_ERROR_PATH_BY_ACCESS;			
		}
		else if( errorCode == ERROR_PATH_NOT_FOUND)
		{
			auErrorCode = LAUNCH_ERROR_PATH_NOT_FOUND;
		}
		else if( errorCode != ERROR_ALREADY_EXISTS)
		{
			auErrorCode = LAUNCH_ERROR_CREATE_DIR_FAILED;			
		}		
	}
//MessageBoxW(0, L"1", L"", MB_OK);
	if( auErrorCode != LAUNCH_ERROR_NO_ERROR)
		return L"";
//MessageBoxW(0, L"2", L"", MB_OK);
	wchar_t buf[100];
	sUpdaterDir += L"\\temp_updater_";
	_itow_s(timeGetTime(), (wchar_t*)buf, 100, 10);
	sUpdaterDir += buf;
	sUpdaterDir += L"\\";

//MessageBoxW(0, sUpdaterDir.c_str(), L"2", MB_OK);
	if( !CreateDirectoryW(sUpdaterDir.c_str(), NULL))
	{
		unsigned int errorCode = GetLastError();
		auSystemErrorCode = errorCode;
		if( errorCode == ERROR_ACCESS_DENIED ||  errorCode == ERROR_INVALID_ACCESS)
		{
			auErrorCode = LAUNCH_ERROR_PATH_BY_ACCESS;			
			return sUpdaterDir;
		}
		else if( errorCode == ERROR_PATH_NOT_FOUND)
		{
			auErrorCode = LAUNCH_ERROR_PATH_NOT_FOUND;
		}
		else if( errorCode != ERROR_ALREADY_EXISTS)
		{
			auErrorCode = LAUNCH_ERROR_CREATE_DIR_FAILED;			
		}		
	}

	return sUpdaterDir;
}

unsigned int  CStartUpdater::copyFiles(const std::wstring& asFromDir, const std::wstring& asToDir, const wchar_t** alpcFilesNames, unsigned int auCount, unsigned int& auSystemErrorCode)
{
	for(unsigned int i=0; i < auCount; i++)
	{
		if( !alpcFilesNames[i])
			continue;

		std::wstring sFromPath = asFromDir + alpcFilesNames[i];
		std::wstring sToPath = asToDir + alpcFilesNames[i];

		if (!CopyFileW(sFromPath.c_str(),sToPath.c_str(), FALSE))
		{
			int iErrorCode = GetLastError();
			auSystemErrorCode = iErrorCode;
			USES_CONVERSION;
			if( iErrorCode == ERROR_ACCESS_DENIED || iErrorCode == ERROR_INVALID_ACCESS)
			{					
				return LAUNCH_ERROR_COPY_FILE_FAILED_BY_ACCESS;
			}
			return LAUNCH_ERROR_COPY_FILE_FAILED;
		}
	}

	return LAUNCH_ERROR_NO_ERROR;
}