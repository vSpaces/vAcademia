#include "stdafx.h"
#include "Refiner.h"



//void RefineFrame(CComString &sPath, int posResh)
void RefineFrame(CWComString &sPath, __RefinerInfo *refinerInfo, int posResh)
{
	int posExt = sPath.ReverseFind('.');
	CWComString sExt = sPath.Mid(posExt, posResh-posExt);
	if(sExt==L".ppt" || sExt==L".pdf" || sExt==L".pptx")
	{
		sExt = L".jpg";
	}

	int pos = sPath.Find(L"frame(", posResh);
	int posEndFrame = -1;
	int posParam=-1;
	if(pos>-1)
	{
		posEndFrame = sPath.Find(L")", pos);
		posParam=pos+6;
	}
	else 
	{
		int posNextCommand = sPath.Find(L"&", posResh);
		if(posNextCommand>-1)
		{
			posEndFrame = posNextCommand;
			pos = posParam = posResh+1;
		}
		else 
		{
			posEndFrame = sPath.GetLength();
			pos = posParam = posResh+1;
		}
	}

	CWComString sNumber = sPath.Mid(posParam, posEndFrame-posParam);
	if(_wtoi(sNumber.GetBuffer())==0 && (sNumber.GetLength()>1 || sNumber.GetAt(0)!=L'0'))
	{
		if(posExt>-1 && posExt<posResh && posExt>posResh - sExt.GetLength() - 1)
		{
			refinerInfo->sPathFileForActions = sPath.Left(posResh);
			refinerInfo->sPathFull = sPath.Left(posExt) + sPath.Right(sPath.GetLength()-posResh) + sExt.GetBuffer();
		}
		else
		{
			if(posExt>posResh)
				refinerInfo->sPathFileForActions = sPath.Left(posResh)+sExt.GetBuffer();
			else
			refinerInfo->sPathFileForActions = sPath.Left(posResh);
			refinerInfo->sPathFull = sPath;
		}
		return;
	}
	
	CWComString sPathName = sPath.Left(posResh)+L"_images/";
	int posT = sPathName.ReverseFind(L'.');
	if(posT>-1)
		sPathName.SetAt(posT, L'_');

	CWComString sFileName = L"0_0000";
	sPathName += sFileName.Left(sFileName.GetLength() - sNumber.GetLength()) + sNumber;
	if(pos == posResh+1)
	{
		if(posEndFrame == sPath.GetLength())
			sPath = sPathName;
		else
			sPath = sPathName + L"#" + sPath.Right(sPath.GetLength()-posEndFrame-1);
	}
	else if(sPath[pos-1]==L'&')
	{
		sPath = sPathName + sPath.Mid(posResh, pos-posResh-1)+sPath.Right(sPath.GetLength()-posEndFrame-1);
	}
	sPathName+=sExt.GetBuffer();
	sPath+=sExt.GetBuffer();
	refinerInfo->sPathFileForActions = sPathName;
	refinerInfo->sPathFull = sPath;
}

void AddCommand(CWComString &sActions, __RefinerInfo *refinerInfo, int posResh, int &index)
{
	int pos = sActions.Find(L"(", 0);
	if(pos==-1)
		return;
	refinerInfo->actions[index] = MP_NEW(__Action);
	int posEnd = sActions.Find(L")", pos);
	refinerInfo->actions[index]->name = sActions.Mid(posResh+1, pos-posResh-1);
	refinerInfo->actions[index]->paramList = sActions.Mid(pos + 1, posEnd - pos - 1);
	sActions = sActions.Right(sActions.GetLength()-posEnd - 1);
}

PATHREFINE_API __RefinerInfo *RefineResourcePath(LPWSTR aPath)
//PATHREFINE_API void RefineResourcePath(LPWSTR aPath, __RefinerInfo *refinerInfo)
{
	CWComString sPath = aPath;
	int posResh = sPath.Find(L"#", 0);
	if(posResh >-1)
	{
		__RefinerInfo *refinerInfo = new __RefinerInfo();
		RefineFrame(sPath, refinerInfo, posResh);
		posResh = sPath.Find(L"#", 0);
		int i=0;
		while(posResh>-1 && i<5)
		{
			AddCommand(sPath, refinerInfo, posResh, i);
			posResh = sPath.Find(L"&", 0);
			i++;
		}
		return refinerInfo;
	}
	return NULL;
}

PATHREFINE_API __RefinerInfo *RefineResourcePathOnlyFrame(LPWSTR aPath)
//PATHREFINE_API void RefineResourcePath(LPWSTR aPath, __RefinerInfo *refinerInfo)
{
	CWComString sPath = aPath;
	int posResh = sPath.Find(L"#", 0);
	if(posResh >-1)
	{
		__RefinerInfo *refinerInfo = MP_NEW( __RefinerInfo);
		RefineFrame(sPath, refinerInfo, posResh);
		return refinerInfo;
	}
	return NULL;
}

PATHREFINE_API LPWSTR GetRefineResourcePath(LPWSTR aPath)
//PATHREFINE_API void RefineResourcePath(LPWSTR aPath, __RefinerInfo *refinerInfo)
{
	__RefinerInfo *refinerInfo = RefineResourcePathOnlyFrame(aPath);
	if(refinerInfo!=NULL)
	{
		CWComString sPath = refinerInfo->sPathFull;
		DeleteRefineResourcePathInfo(refinerInfo);
		return sPath.AllocSysString();
	}
	return aPath;
}

PATHREFINE_API void DeleteRefineResourcePathInfo(__RefinerInfo *pData)
{
	if(pData!=NULL)
	{
		__RefinerInfo *pRefinerInfo = (__RefinerInfo *)pData;
		MP_DELETE( pRefinerInfo);
	}
}