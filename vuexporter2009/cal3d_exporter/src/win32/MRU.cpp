// MRU.cpp: implementation of the CMRU class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "cal3d_max_exporter.h"
#include "MRU.h"
#include "..\filesys.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMRU::CMRU()
{

}

CMRU::~CMRU()
{

}

void CMRU::Load(LPCSTR alpcName){
	RemoveAll();
	SECFileSystem fsys;
	TCHAR lpcStrBuffer[500];
	GetModuleFileName( AfxGetInstanceHandle(), lpcStrBuffer, 256);
	m_sFullFileName = fsys.GetFullPathName(lpcStrBuffer);
	CString oExePath = fsys.GetPath(m_sFullFileName);
	m_sFullFileName = oExePath+alpcName+".mru";
	CStdioFile file;
	if(file.Open(m_sFullFileName,CFile::modeRead | CFile::typeText)){
		CString sItem(_T("--"));
		while(file.ReadString(sItem)){
			if(sItem.IsEmpty()) continue;
			AddTail(sItem);
		}
		file.Close();
	}
}

void CMRU::UpdateAndSave(LPCSTR alpcLastUsedItem){
	if(m_sFullFileName.IsEmpty()) return;
	CString sItemWP(alpcLastUsedItem);
	// update
	if(!sItemWP.IsEmpty()){
		int iIdx=sItemWP.Find('|');
		if(iIdx>0)
			sItemWP=sItemWP.Left(iIdx);
		POSITION pos=GetHeadPosition();
		while(pos){
			POSITION posp=pos;
			CString sItem=GetNext(pos);
			if(sItem.Find(sItemWP)>=0){
				RemoveAt(posp);
				break;
			}
		}
		AddHead(alpcLastUsedItem);
	}
	// save
	CStdioFile file;
	if(file.Open(m_sFullFileName,CFile::modeCreate | CFile::modeWrite | CFile::typeText)){
		POSITION pos=GetHeadPosition();
		while(pos){
			CString sItem=GetNext(pos);
			file.WriteString(sItem);
			file.WriteString(_T("\n"));
		}
		file.Close();
	}
}
