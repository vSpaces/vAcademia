// updaterDlg.h : header file
//
#pragma once

//#include "StdAfx.h"
#include <vector>
//#include "ComString.h"
#include "oms_context.h"
#include "res.h"
#include "crmMemResourceFile.h"
#include "ComMan.h"
#include "BigFileman.h"

using namespace std;
//using namespace oms;
using namespace res;


// CUpdaterDlg dialog
class CupdaterFiles 
{
	friend class CUpdaterDlg;
// Construction
public:
	CupdaterFiles( oms::omsContext *apOMSContext, CUpdaterDlg *apDlg);
	CupdaterFiles();
	void UpdateClient_CopyFile(DWORD aDataSize,BYTE *aData,DWORD dwDowloadUpdateSize);
	bool LoadingFiles();
	void setEvent();
	void wait();
	void releaseMutex();

protected:

	bool CreateEmptyFile(CComString fullFilePath);
	bool delDir( CComString &sDir);
	void ReleaseThread();
	void CreateBatFile( CComString &moduleName, CComString &myUpdateStrFrom, CComString &myUpdateStrTo);

	void LoadBigFileDll();
	int CreateBigFileManagerImpl( bigfile::IBigFileManager **pBigFileMan, LPCTSTR aModuleFile);
	void DestroyBigFileDll();
	void DestroyBigFileManagerImpl( bigfile::IBigFileManager *apBigFileMan, LPCTSTR aModuleFile);

protected:
	cm::cmICommunicationManager		*sp_ComMan;
	bool bNext;
	HANDLE hLoadingFilesThread;
	int iStep;
	IResMan		*sp_ResourceManager;
	IResource		*sp_Resource;
	IResource	*sp_LocalResource;
	// получить адрес функции в dll
	//bool SetUpParametr();
	CComString ClientPath;
	std::vector<CComString> BatFiles; // пути батников для запуска
	HANDLE MutexBatExecuting;
	HANDLE eventDownloadBegin;

	oms::omsContext * pOMSContext;
	HANDLE m_hCheckLogin;
	CUpdaterDlg *pDlg;
	std::vector<CComString> v_fileSkip;
	HANDLE hBigFileModule;
	bigfile::IBigFileManager *pBigFileManUI;
	bigfile::IBigFileManager *pBigFileManCommon;
};