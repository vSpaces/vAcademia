// packModule.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../BigFileMan/BigFileMan.h"

bigfile::IBigFileManagerReader *mpBigFileManReader = NULL;
bigfile::IBigFileManagerWriter *mpBigFileManWriter = NULL;
bigfile::IBigFileManagerBase *mpBigFileMan = NULL;
HMODULE	hBigFileModule = NULL;

HMODULE static LoadDLL( LPCSTR alpcRelPath, LPCSTR alpcName)
{
	CComString sModuleDirectory;
	GetModuleFileName( NULL, sModuleDirectory.GetBufferSetLength(MAX_PATH2), MAX_PATH2);

	CComString sCurrentDirectory;
	GetCurrentDirectory( MAX_PATH2, sCurrentDirectory.GetBufferSetLength(MAX_PATH2));

	sModuleDirectory.Replace("\\", "/");
	sModuleDirectory.Replace("//", "/");
	int pos = sModuleDirectory.ReverseFind('//');
	if(pos>-1)
	{
		sModuleDirectory = sModuleDirectory.Left(pos);
	}
	CComString sPath = sModuleDirectory + "/" + alpcName;
	sPath.Replace("\\", "/");
	sPath.Replace("//", "/");
	SetCurrentDirectory( sModuleDirectory);
	printf( "path bigmanfile.dll: %s\n", sModuleDirectory.GetBuffer());
	//HMODULE handle = ::LoadLibrary( sPath);
	HMODULE handle = ::LoadLibrary( alpcName);	
	SetCurrentDirectory( sCurrentDirectory);
	return handle;
}

bool static CreateBigFileManagerReaderImpl( const char *aModulePath)
{
	if ( hBigFileModule)
	{		
		unsigned int (*lpCreateBigFileManager)( bigfile::IBigFileManagerReader **ampBigFileMan, const wchar_t *asModulePath);
		(FARPROC&)lpCreateBigFileManager = (FARPROC) GetProcAddress( (HMODULE) hBigFileModule, "CreateBigFileManagerReader");
		USES_CONVERSION;
		if ( lpCreateBigFileManager)
			lpCreateBigFileManager( &mpBigFileManReader, A2W(aModulePath));
		if ( !mpBigFileManReader)
		{
			printf( "Not getting adress of function CreateBigFileManagerReader of library bigmanfile.dll, code %d\n", GetLastError());
			FreeLibrary( (HMODULE) hBigFileModule);
			hBigFileModule = 0;
		return false;
	}
	}
	mpBigFileMan = mpBigFileManReader;
	return true;
}

bool static CreateBigFileManagerWriterImpl( const char *aModulePath)
{
	if ( hBigFileModule)
	{
		unsigned int (*lpCreateBigFileManager)( bigfile::IBigFileManagerWriter **ampBigFileMan, const char *asModulePath);
		(FARPROC&)lpCreateBigFileManager = (FARPROC) GetProcAddress( (HMODULE) hBigFileModule, "CreateBigFileManagerWriter");
		if ( lpCreateBigFileManager)
			lpCreateBigFileManager( &mpBigFileManWriter, aModulePath);
		if ( !mpBigFileManWriter)
		{
			printf( "Not getting address of function CreateBigFileManagerWriter of library bigmanfile.dll, code %d\n", GetLastError());
			FreeLibrary( (HMODULE) hBigFileModule);
			hBigFileModule = 0;
			return false;
		}
	}
	mpBigFileMan = mpBigFileManWriter;
	return true;
}

void static DestroyBigFileManagerImpl( const char *aModulePath)
{
	if( !hBigFileModule)
		return;

	void (*lpDestroyBigFileManager)( bigfile::IBigFileManagerBase *ampBigFileMan, const char *asModulePath);
	(FARPROC&)lpDestroyBigFileManager = (FARPROC)GetProcAddress( (HMODULE)hBigFileModule, "DestroyBigFileManager");
	if( lpDestroyBigFileManager)
		lpDestroyBigFileManager( mpBigFileMan, aModulePath);
	FreeLibrary( (HMODULE)hBigFileModule);
	hBigFileModule = NULL;
}

bool static delDir( CComString &sDir,  CComString &sNotDelFile)
{
	WIN32_FIND_DATA fileData;
	HANDLE	hSearch =  FindFirstFile(sDir + "*.*", &fileData); 

	if (hSearch != INVALID_HANDLE_VALUE )
	{
		BOOL bContinueSearch = TRUE;
		while (bContinueSearch)
		{
			if (strcmp(fileData.cFileName, ".") == 0 || strcmp(fileData.cFileName, "..") == 0)
			{
				bContinueSearch = FindNextFile(hSearch, &fileData);
				continue;
			}

			if((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				CComString s = sDir + fileData.cFileName;
				s+="\\";
				CComString sDir;
				delDir( s, sDir);
				bContinueSearch = FindNextFile(hSearch, &fileData);
				if(!::RemoveDirectory( s.GetBuffer()))
				{
					int ff = GetLastError();
					ff+=0;
				}
				continue;
			}

			CComString s = sDir+fileData.cFileName;
			int ff = 0;
			if(s.GetLength()<21)
			{
				ff++;
			}
			bContinueSearch = FindNextFile(hSearch, &fileData);
			if(sNotDelFile.GetLength() == 0 || s.Find(sNotDelFile.GetBuffer(), 0) == -1)
			{
				if(!SetFileAttributes( s.GetBuffer(), FILE_ATTRIBUTE_NORMAL))
				{
					int ff = GetLastError();
					ff+=0;
				}
				if(!::DeleteFile( s.GetBuffer()))
				{
					int ff = GetLastError();
					ff+=0;
				}
			}
		}
	}
	FindClose(hSearch);
	return true;
}


int _tmain(int argc, _TCHAR* argv[])
{
	if( argc < 2)
	{
		printf( "Need to parameter: path to module for pack\n");
		return 1;
	}
	ATLASSERT( !mpBigFileMan);
	if ( mpBigFileMan)
		return false;

#ifdef DEBUG
#define	BIGFILEMAN_MANAGER_DLL "BigFileMan.dll"
#else
#define	BIGFILEMAN_MANAGER_DLL "BigFileMan.dll"
#endif

	if ( !hBigFileModule)
		hBigFileModule = LoadDLL( "", BIGFILEMAN_MANAGER_DLL);

	if(!hBigFileModule)
	{
		printf( "Cannot load library bigmanfile.dll, code %d\n", GetLastError());
		return 1;
	}

	CComString sOperation;
	CComString sUnpackDir;
	if( argc > 3)
	{
		sOperation = argv[2];
		sUnpackDir = argv[3];
		sUnpackDir.Replace("\"", "");
		sUnpackDir.Replace("\'", "");
	}
	CComString sModulePath = argv[1];
	sModulePath.Replace("\\", "/");
	sModulePath.Replace("\"", "");
	sModulePath.Replace("\'", "");
	if( sOperation == "-u")
	{
		if(!CreateBigFileManagerReaderImpl( sModulePath))
		{
			//printf( "Is not open bigfileman.dll\n");
			return 1;
		}
		if( sModulePath.Find(".pack")==-1)
		{
			printf( "It is not pack file...\n");
			return 1;
		}
		printf( "unpack module...\n");
		//mpBigFileManReader->ReadBigFile( sModulePath);
		USES_CONVERSION;
		mpBigFileManReader->Unpack( A2W(sUnpackDir.GetBuffer()));
		DestroyBigFileManagerImpl( sModulePath);
		return 0;
	}
	if(!CreateBigFileManagerWriterImpl( sModulePath))
	{
		//printf( "Is not open bigfileman.dll\n");
		return 1;
	}
	if( sModulePath.Find(".pack")>-1)
	{
		printf( "It is not path for pack...\n");
		return 1;
	}
	CComString sModuleDir = sModulePath;
	int pos = sModuleDir.ReverseFind('/');
	if( pos == sModuleDir.GetLength() - 1)
		sModuleDir = sModuleDir.Left( sModuleDir.GetLength() - 1);
	pos = sModuleDir.ReverseFind('/');
	CComString sModuleName = sModuleDir.Right( sModuleDir.GetLength() - pos - 1);
	//sModuleDir = sModuleDir.Left( sModuleDir.ReverseFind('/') + 1);
	if( sModulePath.ReverseFind('/') != sModulePath.GetLength() - 1)
		sModulePath += '/';
	printf( "Reading module...\n");
	CComString sModulePath2 = sModulePath + sModuleName + ".pack";	
	DeleteFile( sModulePath2.GetBuffer());
	USES_CONVERSION;
	int countFile = mpBigFileManWriter->CreateBigMemoryFileFromDir( A2W(sModulePath.GetBuffer()), NULL);
	if( countFile < 1)
	{
		printf( "Error reading files - %d\n", countFile);
		mpBigFileMan->Release();
		DestroyBigFileManagerImpl( sModulePath);
		return 1;
	}
	printf( "readed %d objects\n", countFile);
	printf( "Saving pack module file...\n");
	int countByteSave = mpBigFileManWriter->ReSaveBigFile( A2W(sModulePath2.GetBuffer()));
	if( countFile < 1)
	{
		printf( "Error saving pack file - %d\n", countByteSave);
		mpBigFileMan->Release();
		DestroyBigFileManagerImpl( sModulePath);
		return 1;
	}
	DestroyBigFileManagerImpl( sModulePath);
	// не надо, чтобы билд машина каждый раз не забирать, и удалением из блида будет заниматьс€ билд-машина
	//printf( "delete unpacked files...\n");
	//delDir(sModulePath,  sModuleName + ".pack");
	return 0;
}

