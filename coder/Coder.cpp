// Coder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ComString.h"
#include "cd.h"

#define	CODE_VALUE		0xEAD5

void print_help()
{

}

void	replaceExt(byte* src, int count)
{
	for( int i=0; i<(count-3); i++)
	{
		if( src[i]=='.' && src[i+1]=='m' && src[i+2]=='j' && src[i+3]=='s')
		{
			src[i+3]='z';
		}
	}
}

bool handle_file(CComString& inFileName, WIN32_FIND_DATA& FileData, CComString& outFileName, bool isDelete)
{
	printf("%s\n", inFileName.GetBuffer(0));
	HANDLE	hFile = CreateFile(inFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if( hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	DWORD	fileLen = GetFileSize(hFile, NULL);
	CloseHandle(hFile);

	FILE*	file = fopen(FileData.cFileName, "rb");
	if( !file)
	{
		return false;
	}

	if( fileLen > 0)
	{
		byte*	fileContent = (byte*)malloc(fileLen);
		fread(fileContent, fileLen, 1, file);

		byte*	fileContentOut = (byte*)malloc(fileLen);
		CVipCoder::decompMem(fileContent, fileContentOut, fileLen);

		FILE*	fOut = fopen(outFileName, "wb");
		if( fOut)
		{
			fwrite(fileContentOut, fileLen, 1, fOut);
			fclose(fOut);
		}
		free(fileContent);
		free(fileContentOut);
		fclose(file);
	}

	if (isDelete)
	{
		remove(inFileName);
	}

	return true;
}

void handle_dir(LPCTSTR dirName)
{
	CComString	curDir = dirName;
	if( !SetCurrentDirectory(dirName))	return;

	WIN32_FIND_DATA FileData; 
	HANDLE hSearch; 

	hSearch = FindFirstFile("*.xml", &FileData); 
	if (hSearch != INVALID_HANDLE_VALUE) 
	{
		do
		{
			CComString	inFileName = CComString(dirName) + FileData.cFileName;
			CComString	outFileName = inFileName;
			
			handle_file(inFileName, FileData, outFileName, false);			
		} while (FindNextFile(hSearch, &FileData));

		hSearch = FindFirstFile("*.mjs", &FileData); 

		if (hSearch != INVALID_HANDLE_VALUE)
		{
			do
			{
				CComString	inFileName = CComString(dirName) + FileData.cFileName;
				CComString	outFileName = inFileName;
				outFileName.SetAt(outFileName.GetLength() - 1, 'z');

				handle_file(inFileName, FileData, outFileName, true);
			} while (FindNextFile(hSearch, &FileData));
		}
		
		// Locate directories
		hSearch = FindFirstFile("*", &FileData);
		if (hSearch != INVALID_HANDLE_VALUE) 
		{
			do 
			{
				if( (FileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				{
					CComString	fileName = FileData.cFileName;
					if( fileName!="." && fileName!="..")
					{
						handle_dir(curDir + fileName + "\\");
					}
				}
			} while (FindNextFile(hSearch, &FileData));
		}
	} 
}

int main(int argc, char* argv[])
{
	if( argc != 2)
	{
		printf("Must take 2 arguments!\n");
		print_help();
	}
	handle_dir(argv[1]);
	return 0;
}

