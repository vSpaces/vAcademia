// ExportConfig.h: interface for the CExportConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTCONFIG_H__6642C5A8_6EF0_4F49_AB28_8C824D88B821__INCLUDED_)
#define AFX_EXPORTCONFIG_H__6642C5A8_6EF0_4F49_AB28_8C824D88B821__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExportConfig  
{
	//
	CString msECFName;
	CString msConfigName;
public:
	CString msCAFName; // CAF-file name
	CStringList mslConfigs;
	CStringList mslSections;
	//
public:
	CString msCSF; // skeleton file name
	CString msFace; 
	CString msBones; // list of bones (if empty then selected all)
	int miStartFrame;
	int miEndFrame;
	int miDispl;
	int miFrameRate; // fps
	bool mbMakeRel;
	bool mbSaveFirst;
	bool mbSaveLast;

protected:


public:
	CExportConfig();
	virtual ~CExportConfig();
	void SetCAFName(LPCTSTR alpcCAFName){ msCAFName=alpcCAFName; }
	bool SetECFName(LPCSTR alpcECFName);
	bool SetConfigName(LPCSTR alpcConfName);
	void Save();
	CString GetConfigName(int aiConfIdx=-1);
	bool ThereIsCAFInECF(LPCSTR alpcECFName);
	int GetConfigNum(LPCTSTR alpcConfName);
	bool SetSectionActive(LPCTSTR alpcSection);
	//
};

#endif // !defined(AFX_EXPORTCONFIG_H__6642C5A8_6EF0_4F49_AB28_8C824D88B821__INCLUDED_)
