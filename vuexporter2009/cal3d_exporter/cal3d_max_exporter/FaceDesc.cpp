// FaceDesc.cpp: implementation of the CFaceDesc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cal3d_max_exporter.h"
#include "FaceDesc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFaceDesc::CFaceDesc()
{
	wNum = 0;
}

CFaceDesc::~CFaceDesc()
{

}

bool CFaceDesc::Create(LPSTR fileName)
{

	CFile* pFile = NULL;
	DWORD fileLen;
	char* pBuf;

	try{
		pFile = new CFile(fileName, CFile::modeRead);
		fileLen = pFile->GetLength();
	}
	catch(CFileException pEx)	{
		return false;
	};

	pBuf = (char*)malloc(fileLen);

	char curCh;
	CString st = "";
	DWORD counter = 0;
	WORD wBuffer[6];
	float fl = 0.0;
	int	  i1 = 0;
	curCh = ' ';
	int vNum = 0;
	int i = 0;
	while(i < 6)
	{
		curCh = pBuf[counter];
		if((curCh != ',')&(curCh != ';')&(curCh != ' ')&(curCh != (char)10)&(curCh != (char)13)&(counter < fileLen))
		{
			st += curCh;
		} else
		{	
			if(st != "")
			{
				i1 = atoi(st);
				wBuffer[i] = (WORD)i1;
			  i++;
			  st = "";
			}
		};
		counter++;
	};

	wNum = wBuffer[0];
	vNum = wBuffer[1];
	w1 = wBuffer[2];
	w2 = wBuffer[3];
	w3 = wBuffer[4];
	w4 = wBuffer[5];

	i = 0;
	while(i < vNum)
	{
		curCh = pBuf[counter];
		if((curCh != ',')&(curCh != ';')&(curCh != ' ')&(curCh != (char)10)&(curCh != (char)13)&(counter < fileLen))
		{
			st += curCh;
		} else
		{	
			if(st != "")
			{
			  i1 = atoi(st);
			  wIndexes.push_back((WORD)i1);
			  i++;
			  st = "";
			}
		};
		counter++;
	};

	D3DVECTOR	vector;
	WORD vcnt = 0;

	st = "";
	while (counter < fileLen)
	{
		curCh = pBuf[counter];
		if((curCh != ',')&(curCh != ';')&(curCh != (char)10)&(curCh != (char)13))
		{
			st += curCh;
		} else
		{	
			if(st != "")
			{
			  fl = atof(st);
			  memcpy((void*)((DWORD)&vector+vcnt*sizeof(float)), (void*)&fl, sizeof(float));
			  vcnt++;
			  if(vcnt == 3){
				  vcnt = 0;
				  wDeltas.push_back(vector);
			  }
			  st = "";
			}
		};
		counter++;
	};
	return true;
}
