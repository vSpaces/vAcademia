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


// CUpdaterDll dialog
class CupdaterFiles 
{
	friend class CUpdaterDll;
// Construction
public:
	CupdaterFiles( oms::omsContext *apOMSContext, CUpdaterDll *apDll);
	~CupdaterFiles();

	bool loadingFiles();
	void setEvent();
	//void wait();
	void releaseMutex();
	void setDll(  CUpdaterDll *apDll);
	void delCache( int aProgress = -1);
	bool testCreateFile();
	void GetStringGUID( wchar_t* apwcGUID, unsigned short aSizeGUID);
protected:

	bool ErrorMessageBox( const char *apInfo, int aCode);
	bool CopyFileToClient( CWComString &sSrcFilePath, CWComString &sDestFilePath);
	bool UpdateClient_CopyFileRec( CWComString &sFullPathDir, CWComString &sDestFilePath, int &aProgress);
	void UpdateClient_CopyFile( CWComString &sFullPathDir, DWORD dwDowloadUpdateSize);
	bool CreateEmptyFile(CWComString fullFilePath);
	void ReleaseThread();
	//void CreateBatFile( CWComString &moduleName, CWComString &myUpdateStrFrom, CWComString &myUpdateStrTo);

	void Clear();
	void LoadBigFileDll();
	int CreateBigFileManagerImpl( bigfile::IBigFileManager **pBigFileMan, const wchar_t* aModuleFile);
	void DestroyBigFileDll();
	void DestroyBigFileManagerImpl( bigfile::IBigFileManager *apBigFileMan, const wchar_t* aModuleFile);
	template<class T>
		__forceinline bool memparse_object(BYTE* buf, int& pos, T& val, unsigned int iDataSize)
	{
		int sizeofVal = sizeof( T);
		if ((unsigned int)(pos + sizeofVal) > iDataSize)
			return false;
		memcpy( &val, &buf[pos], sizeofVal);	
		pos += sizeofVal;
		return true;
	}
	
	template<class T>
		__forceinline bool memparse_length( BYTE* buf, int& pos, T& aLength, unsigned int iDataSize, int anItemSize = 1)
	{
		if (!memparse_object( buf, pos, aLength, iDataSize))
			return false;

		if ((unsigned int)(pos + aLength * anItemSize) > iDataSize)
			return false;
		return true;
	}
	bool memparse_string_unicode(BYTE* buf, int& pos, CWComString& val, unsigned int iDataSize);


protected:
	cm::cmICommunicationManager		*sp_ComMan;
	bool bNext;
	HANDLE hLoadingFilesThread;
	IResMan		*sp_ResourceManager;
	IResource		*sp_Resource;
	IResource	*sp_LocalResource;
	// получить адрес функции в dll
	//bool SetUpParametr();
	CWComString ClientPath;
	//std::vector<CWComString> BatFiles; // пути батников для запуска
	//HANDLE MutexBatExecuting;
	HANDLE eventDownloadBegin;

	oms::omsContext * pOMSContext;
	HANDLE m_hCheckLogin;
	CUpdaterDll *pDll;
	std::vector<CComString> v_fileSkip;
	HANDLE hBigFileModule;
	bigfile::IBigFileManager *pBigFileManUI;
	bigfile::IBigFileManager *pBigFileManCommon;
	bigfile::IBigFileManager *pBigFileManGeometry;	
	bool bUIChange;
	bool bCommonChange;
	bool bGeometryChange;

	CWComString sUIPath;
	CWComString sCommonPath;
	CWComString sGeometryPath;
	CWComString sArchivePathBase;
	CWComString fullPathFile;
};