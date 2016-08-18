// ExportConfig.cpp: implementation of the CExportConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "cal3d_max_exporter.h"
#include "ExportConfig.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExportConfig::CExportConfig()
{
	miStartFrame=0;
	miEndFrame=0;
	miDispl=0;
	miFrameRate=0;
	mbMakeRel=false;
	mbSaveFirst=false;
	mbSaveLast=false;
}

CExportConfig::~CExportConfig()
{

}

bool CExportConfig::SetECFName(LPCTSTR alpcECFName){
	bool bRet=false;
	msECFName=alpcECFName;
	msConfigName=_T("");
	// if file exists
	// read configs
	mslConfigs.RemoveAll();
	mslSections.RemoveAll();
	TCHAR lpcBuf[MAX_PATH+10]=_T("");
	int iSec=1;
	CString sSection; sSection.Format("Config%d",iSec);
	while(GetPrivateProfileString( sSection, _T("Name"), _T(""), lpcBuf, MAX_PATH, msECFName)) {
		mslConfigs.AddTail(lpcBuf);
		mslSections.AddTail(sSection);
		TCHAR lpcBuf2[MAX_PATH+10]=_T("");
		if(GetPrivateProfileString( sSection, _T("CAF"), _T(""), lpcBuf2, MAX_PATH, msECFName)){
			if(msCAFName==lpcBuf2)
				bRet=SetConfigName(lpcBuf);
		}
		sSection.Format("Config%d",++iSec);
	}
	if(msConfigName.IsEmpty()){
		msConfigName=msCAFName;
		int iIdx=msConfigName.ReverseFind(_T('\\'));
		if(iIdx>0)
			msConfigName=msConfigName.Right(msConfigName.GetLength()-iIdx-1);
	}
	return bRet;
}

bool CExportConfig::SetConfigName(LPCSTR alpcConfName){
	msConfigName=alpcConfName;
	int iCNum=GetConfigNum(alpcConfName);
	CString sSection; sSection.Format("Config%d",iCNum);
	TCHAR lpcBuf[MAX_PATH+10]=_T("");
	if(!GetPrivateProfileString( sSection, _T("Name"), _T(""), lpcBuf, MAX_PATH, msECFName))
		return false;
	if(msConfigName!=lpcBuf) return false;
	if(GetPrivateProfileString( sSection, _T("CSF"), _T(""), lpcBuf, MAX_PATH, msECFName))
		msCSF=lpcBuf;
	if(GetPrivateProfileString( sSection, _T("Face"), _T(""), lpcBuf, MAX_PATH, msECFName))
		msFace=lpcBuf;
	if(GetPrivateProfileString( sSection, _T("Bones"), _T(""), lpcBuf, MAX_PATH, msECFName))
		msBones=lpcBuf;
	if(GetPrivateProfileString( sSection, _T("StartFrame"), _T(""), lpcBuf, MAX_PATH, msECFName))
		miStartFrame=atoi(lpcBuf);
	if(GetPrivateProfileString( sSection, _T("EndFrame"), _T(""), lpcBuf, MAX_PATH, msECFName))
		miEndFrame=atoi(lpcBuf);
	if(GetPrivateProfileString( sSection, _T("Displ"), _T(""), lpcBuf, MAX_PATH, msECFName))
		miDispl=atoi(lpcBuf);
	if(GetPrivateProfileString( sSection, _T("FrameRate"), _T(""), lpcBuf, MAX_PATH, msECFName))
		miFrameRate=atoi(lpcBuf);
	if(GetPrivateProfileString( sSection, _T("MakeRel"), _T(""), lpcBuf, MAX_PATH, msECFName))
		mbMakeRel=(lpcBuf[0]==_T('1'));
	if(GetPrivateProfileString( sSection, _T("SaveFirst"), _T(""), lpcBuf, MAX_PATH, msECFName))
		mbSaveFirst=(lpcBuf[0]==_T('1'));
	if(GetPrivateProfileString( sSection, _T("SaveLast"), _T(""), lpcBuf, MAX_PATH, msECFName))
		mbSaveLast=(lpcBuf[0]==_T('1'));
	return true;
}

CString CExportConfig::GetConfigName(int aiConfIdx){
	if(aiConfIdx<0) return msConfigName;
	POSITION pos=mslConfigs.GetHeadPosition();
	int iIdx=0;
	while(pos){
		CString sConfigName=mslConfigs.GetNext(pos);
		if(iIdx==aiConfIdx) return sConfigName;
		iIdx++;
	}
	CString sEmpty;
	return sEmpty;
}

int CExportConfig::GetConfigNum(LPCTSTR alpcConfName){
	CString sConfName(alpcConfName);
	int iSec=1;
	TCHAR lpcBuf[300]=_T("");
	CString sSection; sSection.Format("Config%d",iSec);
	while(GetPrivateProfileString( sSection, _T("Name"), _T(""), lpcBuf, MAX_PATH, msECFName)) {
		if(sConfName==lpcBuf) return iSec;
		sSection.Format("Config%d",++iSec);
	}
	return iSec;
}

void CExportConfig::Save(){
	if(msECFName.IsEmpty()) return;
	if(msConfigName.IsEmpty()){
		msConfigName.Format("Untitled%d",1);
	}
	CString sSection;
	sSection.Format("Config%d",GetConfigNum(msConfigName));
	WritePrivateProfileString(sSection, _T("Name"), msConfigName, msECFName);
	WritePrivateProfileString(sSection, _T("CAF"), msCAFName, msECFName);
	WritePrivateProfileString(sSection, _T("CSF"), msCSF, msECFName);
	WritePrivateProfileString(sSection, _T("Face"), msFace, msECFName);
	WritePrivateProfileString(sSection, _T("Bones"), msBones, msECFName);
	CString sNum;
	sNum.Format("%d",miStartFrame);
	WritePrivateProfileString(sSection, _T("StartFrame"), sNum, msECFName);
	sNum.Format("%d",miEndFrame);
	WritePrivateProfileString(sSection, _T("EndFrame"), sNum, msECFName);
	sNum.Format("%d",miDispl);
	WritePrivateProfileString(sSection, _T("Displ"), sNum, msECFName);
	sNum.Format("%d",miFrameRate);
	WritePrivateProfileString(sSection, _T("FrameRate"), sNum, msECFName);
	sNum.Format("%d",mbMakeRel?1:0);
	WritePrivateProfileString(sSection, _T("MakeRel"), sNum, msECFName);
	sNum.Format("%d",mbSaveFirst?1:0);
	WritePrivateProfileString(sSection, _T("SaveFirst"), sNum, msECFName);
	sNum.Format("%d",mbSaveLast?1:0);
	WritePrivateProfileString(sSection, _T("SaveLast"), sNum, msECFName);
}

bool CExportConfig::ThereIsCAFInECF(LPCSTR alpcECFName){
	TCHAR lpcBuf[MAX_PATH+10]=_T("");
	int iSec=1;
	CString sSection; sSection.Format("Config%d",iSec);
	while(GetPrivateProfileString( sSection, _T("Name"), _T(""), lpcBuf, MAX_PATH, alpcECFName)) {
		mslConfigs.AddTail(lpcBuf);
		mslSections.AddTail(sSection);
		TCHAR lpcBuf2[MAX_PATH+10]=_T("");
		if(GetPrivateProfileString( sSection, _T("CAF"), _T(""), lpcBuf2, MAX_PATH, alpcECFName)){
			if(msCAFName==lpcBuf2) return true;
		}
		sSection.Format("Config%d",++iSec);
	}
	return false;
}

bool CExportConfig::SetSectionActive(LPCTSTR alpcSection)
{
	//msConfigName=alpcSection;
	
	CString sSection = alpcSection;
	TCHAR lpcBuf[MAX_PATH+10]=_T("");
	if(!GetPrivateProfileString( sSection, _T("Name"), _T(""), lpcBuf, MAX_PATH, msECFName))
		return false;
	if(GetPrivateProfileString( sSection, _T("CAF"), _T(""), lpcBuf, MAX_PATH, msECFName))
		msCAFName=lpcBuf;
	if(GetPrivateProfileString( sSection, _T("CSF"), _T(""), lpcBuf, MAX_PATH, msECFName))
		msCSF=lpcBuf;
	if(GetPrivateProfileString( sSection, _T("Face"), _T(""), lpcBuf, MAX_PATH, msECFName))
		msFace=lpcBuf;
	if(GetPrivateProfileString( sSection, _T("Bones"), _T(""), lpcBuf, MAX_PATH, msECFName))
		msBones=lpcBuf;
	if(GetPrivateProfileString( sSection, _T("StartFrame"), _T(""), lpcBuf, MAX_PATH, msECFName))
		miStartFrame=atoi(lpcBuf);
	if(GetPrivateProfileString( sSection, _T("EndFrame"), _T(""), lpcBuf, MAX_PATH, msECFName))
		miEndFrame=atoi(lpcBuf);
	if(GetPrivateProfileString( sSection, _T("Displ"), _T(""), lpcBuf, MAX_PATH, msECFName))
		miDispl=atoi(lpcBuf);
	if(GetPrivateProfileString( sSection, _T("FrameRate"), _T(""), lpcBuf, MAX_PATH, msECFName))
		miFrameRate=atoi(lpcBuf);
	if(GetPrivateProfileString( sSection, _T("MakeRel"), _T(""), lpcBuf, MAX_PATH, msECFName))
		mbMakeRel=(lpcBuf[0]==_T('1'));
	if(GetPrivateProfileString( sSection, _T("SaveFirst"), _T(""), lpcBuf, MAX_PATH, msECFName))
		mbSaveFirst=(lpcBuf[0]==_T('1'));
	if(GetPrivateProfileString( sSection, _T("SaveLast"), _T(""), lpcBuf, MAX_PATH, msECFName))
		mbSaveLast=(lpcBuf[0]==_T('1'));
	return true;
}

