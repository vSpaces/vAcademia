#pragma once

#include <vector>

#include "oms_context.h"
#include "res.h"
//#include "crmMemResourceFile.h"
#include "ComMan.h"
//#include "BigFileman.h"

using namespace std;
using namespace res;

class CClientDowloader
{
	friend class CUpdaterDll;
public:
	CClientDowloader( oms::omsContext *apOMSContext, CUpdaterDll *apDll);
	~CClientDowloader();

	bool loadingFile();
	void setDll(  CUpdaterDll *apDll);

protected:

	bool ErrorMessageBox( const char *apInfo, int aCode);
	void ReleaseThread();

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
	bool memparse_string_unicode(BYTE* buf, int& pos, CComString& val, unsigned int iDataSize);


protected:
	cm::cmICommunicationManager		*sp_ComMan;
	bool bNext;
	HANDLE hLoadingFilesThread;
	IResMan		*sp_ResourceManager;
	IResource		*sp_Resource;
	IResource	*sp_LocalResource;

	CComString ClientPath;
	std::vector<CComString> BatFiles; // пути батников для запуска

	oms::omsContext * pOMSContext;
	HANDLE m_hCheckLogin;
	CUpdaterDll *pDll;
	std::vector<CComString> v_fileSkip;
	//HANDLE hBigFileModule;
	//bigfile::IBigFileManager *pBigFileManUI;
	//bigfile::IBigFileManager *pBigFileManCommon;

	CComString sUIPath;
	CComString sCommonPath;
	CComString sArchivePathBase;
	CWComString fullPathFile;
};