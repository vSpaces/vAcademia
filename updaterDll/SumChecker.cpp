#include "stdafx.h"
#include "SumChecker.h"

char SumChecker::sumStatus[4096] = "\0";
SumChecker::SumChecker(void)
{
}

SumChecker::~SumChecker(void)
{
}
DWORD SumChecker::CheckAllFiles(std::wstring path)
{
	char tmpString[255]; int res = 0;
	char decryptString[255];
	char fileName[100], fileCS[50], bufString[50];
	int j = 0, k=0; int key = 5;
	FILE *csFile;
	std::wstring tempPath;
	std::wstring csPath;
	csPath = path;// + "\\csMaster";
	csPath += L"csMaster";
	csFile = _wfopen(csPath.c_str(),L"r");
	if (csFile != NULL)
	{
	memset(sumStatus,0,4096);
	USES_CONVERSION;
	while(!feof(csFile))
	{
		k=0;j=0; tempPath = path;
		memset(tmpString,0,255);
		memset(fileName,0,100);
		memset(fileCS,0,50); 
		memset(bufString,0,50);
		memset(decryptString,0,255);
		fgets(decryptString,255,csFile);
		if(decryptString[0] == 0) break;
		for(int i = 0; i < strlen(decryptString); i++) 
		{
			tmpString[i] = decryptString[i] ^ key;
		}
		if (tmpString[0]!='\n' && tmpString[0]!='\0' && tmpString[0]!=' ')
		{
			while(tmpString[k]!=' ')
			{
				fileName[k] = tmpString[k]; 
				k++;
			}k++;
			while(tmpString[k]!='\0')
			{
				fileCS[j]=tmpString[k];
				k++;
				j++;
			}
			strncat(bufString,fileCS,32);
			tempPath += A2W(fileName);
			DWORD dwStatus = CheckSum::md5Counter(tempPath.c_str());

			if(fileName[0] != 0 && bufString[0] != 0)
			{
				if (strcmp(CheckSum::md5string,bufString)!=0)			
				{
					strcat(sumStatus, fileName);
					char ff[10];
					_itoa(dwStatus, ff, 10);
					strcat(sumStatus, " checksums do not match, code error: ");
					strcat(sumStatus, ff);
					strcat(sumStatus, " \n");
				}
			}else break;
		}else break;
	}
	fclose(csFile);
	}
	return 0;
}
