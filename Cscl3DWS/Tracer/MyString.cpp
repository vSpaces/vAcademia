
#include "stdafx.h"
#include "MyString.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* MYS_WC2MB(const wchar_t* apwc){
	int iSLen=wcslen(apwc);
	char* pch=new char[iSLen+1];
	char* pch2=pch;
	unsigned short* pwc=(unsigned short*)apwc;
	for(int ii=0; ii<iSLen; ii++){
		*pch2++=(char)*pwc++; // ??
	}
	pch[iSLen]=0;
	return pch;
}

CMyString::CMyString(){
	mpszString=NULL;
	AllocString(0);
}

CMyWString::CMyWString(){
	mpwcString=NULL;
	AllocString(0);
}

CMyString::CMyString(char* auStr){
	mpszString=NULL;
	int nLen = strlen(auStr);
	AllocString(nLen);
	strcpy(mpszString,auStr);
}

CMyString::CMyString(wchar_t* auStr){
	mpszString=NULL;
	int nLen = wcslen(auStr);
	AllocString(nLen);
	char* psz=MYS_WC2MB(auStr);
	strcpy(mpszString,psz);
	delete psz;
}

CMyWString::CMyWString(wchar_t* auStr){
	mpwcString=NULL;
	int nLen = wcslen(auStr);
	AllocString(nLen);
	wcscpy(mpwcString,auStr);
}

CMyString::CMyString(CMyString& strSrc){
	mpszString=NULL;
	int nLen = strSrc.GetLength();
	AllocString(nLen);
	strncpy(mpszString,strSrc.mpszString,nLen);
}

CMyString::CMyString(CMyWString& strSrc){
	mpszString=NULL;
	int nLen = strSrc.GetLength();
	AllocString(nLen);
	char* psz=MYS_WC2MB((MYS_LPWCSTR)strSrc);
	strcpy(mpszString,psz);
	delete psz;
}

CMyString::~CMyString(){
	if(mpszString!=NULL)
		free(mpszString);
}

CMyWString::~CMyWString(){
	if(mpwcString!=NULL)
		free(mpwcString);
}

void CMyString::StringCopy(CMyString& str, int nLen, int nIndex, int nExtra) const{
	int nNewLen = nLen + nExtra;
	if (nNewLen != 0)
	{
		str.AllocString(nNewLen);
		memcpy(str.GetString(), mpszString+nIndex, nLen * sizeof(char));
	}
}

void CMyString::StringCopy(int nSrcLen, const char* lpszSrcData){
	AllocString(nSrcLen);
	memcpy(mpszString, lpszSrcData, nSrcLen * sizeof(char));
	mpszString[nSrcLen] = '\0';
}

void CMyString::ConcatCopy(const char* lpszData){
	// Save the existing string
	int nLen = GetLength();
	char* pszTemp = (char*) malloc((nLen+1) * sizeof(char));
	memcpy(pszTemp, mpszString, nLen * sizeof(char));
	pszTemp[nLen] = '\0';

	// Calculate the new string length and realloc memory
	int nDataLen = strlen(lpszData);
	int nNewLen = nLen + nDataLen;
	ReAllocString(nNewLen);

	// Copy the strings into the new buffer
	memcpy(mpszString, pszTemp, nLen * sizeof(char));
	memcpy(mpszString+nLen, lpszData, nDataLen * sizeof(char));

	// Cleanup
	free(pszTemp);
}

void CMyString::AllocString(int nLen)
{
	if(nLen<0) return;
	if (mpszString != NULL)
		free(mpszString);
	mpszString = (char*) malloc((nLen+1) * sizeof(char));
	mpszString[nLen] = 0;
}

void CMyString::ReAllocString(int nLen)
{
	mpszString = (char*) realloc(mpszString, (nLen+1) * sizeof(char));
	mpszString[nLen] = '\0';
}

void CMyWString::AllocString(int nLen)
{
	if(nLen<0) return;
	if (mpwcString != NULL)
		free(mpwcString);
	mpwcString = (wchar_t*) malloc((nLen+1) * sizeof(unsigned short));
	mpwcString[nLen] = 0;
}

const CMyString& CMyString::operator=(CMyString& strSrc){
	if (mpszString != strSrc.GetString())
		StringCopy(strSrc.GetLength(), strSrc.GetString());
	return *this;
}

const CMyString& CMyString::operator=(const char* lpsz){
	StringCopy(strlen(lpsz), lpsz);
	return *this;
}

const CMyString& CMyString::operator=(wchar_t* bstrStr)
{
	int nLen = wcslen(bstrStr);
	char* psz=MYS_WC2MB(bstrStr);
	StringCopy(nLen, psz);
	delete psz;
	return *this;
}

const CMyString& CMyString::operator+=(CMyString& strSrc){
	ConcatCopy((MYS_LPCSTR) strSrc);
	return *this;
}

const CMyString& CMyString::operator+=(CMyWString& strSrc){
	char* lpsz=MYS_WC2MB((MYS_LPWCSTR)strSrc);
	ConcatCopy(lpsz);
	delete lpsz;
	return *this;
}

const CMyString& CMyString::operator+=(const char* lpsz){
	ConcatCopy(lpsz);
	return *this;
}

const CMyString& CMyString::operator+=(const wchar_t* bstrStr){
	char* lpsz=MYS_WC2MB(bstrStr);
	ConcatCopy(lpsz);
	delete lpsz;
	return *this;
}

const CMyString& CMyString::operator+=(int aiNum){
	bool bMinus=false;
	if(aiNum < 0){
		aiNum=-aiNum;
		bMinus=true;
	}
	char szNum[100];
	szNum[99]=0;
	char* pch=&szNum[99];
	if(aiNum==0) *(--pch)='0';
	while(aiNum>0){
		*(--pch)=(aiNum%10)+'0';
		aiNum/=10;
	}
	if(bMinus) *(--pch)='-';
	ConcatCopy(pch);
	return *this;
}

const CMyString& CMyString::operator+=(double adNum){
	char sz[100];
	sprintf(sz,"%g",adNum);
	ConcatCopy(sz);
	return *this;
}

int CMyString::GetLength() const
{ 
	return strlen(mpszString); 
}

int CMyWString::GetLength() const
{ 
	return wcslen(mpwcString); 
}

CMyString CMyString::Left(int nCount) const{
	if (nCount < 0)
		nCount = 0;

	CMyString newStr;
	StringCopy(newStr, nCount, 0, 0);
	return newStr;
}

int CMyString::Replace(char chOld, char chNew){
	int nCount = 0;
	if (chOld != chNew){
		char* psz = mpszString;
		char* pszEnd = psz + GetLength();
		while(psz < pszEnd){
			if (*psz == chOld){
				*psz = chNew;
				nCount++;
			}
			psz++;
		}
	}
	return nCount;
}

int CMyString::Replace(const char* lpszOld, const char* lpszNew){
	int nSourceLen = strlen(lpszOld);
	if (nSourceLen == 0) return 0;
	int nReplaceLen = strlen(lpszNew);
	int nCount = 0;
	char* lpszStart = mpszString;
	char* lpszEnd = lpszStart + GetLength();
	char* lpszTarget;

	// Check to see if any changes need to be made
	while (lpszStart < lpszEnd){
		while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL){
			lpszStart = lpszTarget + nSourceLen;
			nCount++;
		}
		lpszStart += strlen(lpszStart) + 1;
	}
	// Do the actual work
	if (nCount > 0){
		int nOldLen = GetLength();
		int nNewLen = nOldLen + (nReplaceLen - nSourceLen) * nCount;
		if (GetLength() < nNewLen){
			CMyString szTemp = mpszString;
			ReAllocString(nNewLen);
			memcpy(mpszString, (const char*) szTemp, nOldLen * sizeof(char));
		}

		lpszStart = mpszString;
		lpszEnd = lpszStart + GetLength();
		while (lpszStart < lpszEnd){
			while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL){
				int nBalance = nOldLen - (lpszTarget - mpszString + nSourceLen);
				memmove(lpszTarget + nReplaceLen, lpszTarget + nSourceLen, nBalance * sizeof(char));
				memcpy(lpszTarget, lpszNew, nReplaceLen * sizeof(char));
				lpszStart = lpszTarget + nReplaceLen;
				lpszStart[nBalance] = '\0';
				nOldLen += (nReplaceLen - nSourceLen);
			}
			lpszStart += strlen(lpszStart) + 1;
		}
	}
	return nCount;
}
