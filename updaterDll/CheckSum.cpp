#include "stdafx.h"
#include "CheckSum.h"
CheckSum::CheckSum(void)
{
}

CheckSum::~CheckSum(void)
{
}

#define BUFSIZE 1024*512
#define MD5LEN  16
CHAR CheckSum::md5string[34] = "0123456789abcdef0123456789abcdef";

DWORD CheckSum::md5Counter(LPCWSTR filename)
{
	DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = NULL;
    BYTE* rgbFile = new BYTE[BUFSIZE];
    DWORD cbRead = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";
	CHAR md5summ[] = "0123456789abcdef0123456789abcdef";
	hFile = CreateFileW(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	while(true)
	{
		if (INVALID_HANDLE_VALUE == hFile)
		{
			dwStatus = GetLastError();
			break;
		}
		if (!CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT))
		{
			dwStatus = GetLastError();
			break;
		}
		if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
		{
			dwStatus = GetLastError();
			break;
		}
		while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, &cbRead, NULL))
		{
			if (0 == cbRead)
			{
				break;
			}
			if (!CryptHashData(hHash, rgbFile, cbRead, 0))
			{
				dwStatus = GetLastError();
				break;
			}
		}
		if (!bResult)
		{
			dwStatus = GetLastError();
			break;
		}
		cbHash = MD5LEN;
		if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
		{
			for (DWORD i = 0; i < cbHash; i++)
			{
				md5summ[2*i]=rgbDigits[rgbHash[i] >> 4];
				md5summ[2*i+1]=rgbDigits[rgbHash[i] & 0xf];
			}
			md5summ[cbHash * 2] = 0; 
			memset(CheckSum::md5string,0,34);
			strcpy(CheckSum::md5string,md5summ);
		}
		else
		{
			dwStatus = GetLastError();
			break;
		}
		break;
	}
	delete[] rgbFile;
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	CloseHandle(hFile);
	return dwStatus;
}
void CheckSum::enumerateFilesHashes(wchar_t path[255])
{
	wchar_t fpath[255];
	int i=0,k=0;
	wchar_t filenamesWithPath[100][100];
	char filenames[100][100];
	WIN32_FIND_DATAW ffd; 
	HANDLE hFind;

	USES_CONVERSION;
	wcscpy(fpath,path);
	wcsncat(path, L"*", 1); 
	hFind = FindFirstFileW(path, &ffd);
	while (FindNextFileW(hFind, &ffd) != 0)
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			FindNextFileW(hFind, &ffd);
		}else
		{
			if (wcsstr(ffd.cFileName,L"dll")>0 || wcsstr(ffd.cFileName,L"exe")>0 || wcsstr(ffd.cFileName,L"pack")>0)
			{
				wcscpy(filenamesWithPath[i],fpath);
				wcscat(filenamesWithPath[i],ffd.cFileName);
				strcpy(filenames[i], W2A(ffd.cFileName));
				i++;
			}
		}
	}
	FindClose(hFind);
	FILE *csFile;
	csFile = _wfopen(L"csMaster", L"w+"); char xorString[500]; int key = 5;
	while(k!=i)
	{
		if(md5Counter(filenamesWithPath[k])==0)
		{
			strcat(filenames[k], " ");
			strcat(filenames[k],CheckSum::md5string);
			memset(xorString,0,500);
			for(int j = 0; j < strlen(filenames[k]); j++) 
			{
				xorString[j] = filenames[k][j] ^ key;
			}
			fprintf(csFile,"%s\n",xorString);
			k++;
		}
		else
		{
			printf("Check summ error: %d",md5Counter(filenamesWithPath[k]));
			k++;
		}
	}
	/*char uiPath[255], vuCommonPath[255];
	strcat(uiPath,fpath); strcat(uiPath,"ui\\ui.pack"); 
	md5Counter(uiPath);
	memset(uiPath,0,255);
	strcat(uiPath,"ui\\ui.pack"); strcat(uiPath," "); strcat(uiPath,CheckSum::md5string);
	memset(xorString,0,500);
	for(int j = 0; j < strlen(uiPath); j++) 
	{
		xorString[j] = uiPath[j] ^ key;
	}
	fprintf(csFile,"%s\n",xorString);
	strcat(vuCommonPath,fpath); strcat(vuCommonPath,"root\\files\\vu.common\\vu.common.pack"); 
	md5Counter(vuCommonPath);
	memset(vuCommonPath,0,255);
	strcat(vuCommonPath,"root\\files\\vu.common\\vu.common.pack"); strcat(vuCommonPath," "); strcat(vuCommonPath,CheckSum::md5string);
	memset(xorString,0,500);
	for(int j = 0; j < strlen(vuCommonPath); j++) 
	{
		xorString[j] = vuCommonPath[j] ^ key;
	}
	fprintf(csFile,"%s",xorString);*/
	fclose(csFile);
}