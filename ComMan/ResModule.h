// ResModule.h: interface for the CResModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESMODULE_H__75BF4B13_857C_418D_AEE9_A0419467DF3F__INCLUDED_)
#define AFX_RESMODULE_H__75BF4B13_857C_418D_AEE9_A0419467DF3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResManCreation.h"
#include <libxml/xpath.h>
#include "comman.h"
#include "BigFileman.h"

using namespace res;

class CResModule : 
	//public CComObjectRootEx<CComMultiThreadModel>,
	public IResModule,
	public IResModuleMetadata,
	public CResManCreation
{
public:
	CResModule();
	virtual ~CResModule();

	/*DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CResModule)
		COM_INTERFACE_ENTRY(IResModule)
		COM_INTERFACE_ENTRY(IResModuleMetadata)
	END_COM_MAP()*/

public:
	virtual void open( const wchar_t* aModuleID, const wchar_t* aModulePath, const wchar_t* aModuleFile, DWORD aFlags);

	void setStatus( DWORD aStatus){ dwStatus = aStatus;};
public:
// IResModule
	// Закрыть модуль и удалить объект из памяти
	void destroy();
	// Закрыть модуль
	long close();
	// Открыть ресурс
	long openResource(/*[in]*/ BSTR aResPath, /*[in]*/ DWORD aFlags, /*[out]*/ IResource** aRes);
	// Открыть ресурс асинхронно
	long openAsynchResource( BSTR aResPath,  DWORD aFlags, IResourceNotify* aNotify, IResource** aRes);
	// Проверить существует ли ресурс в модуле
	long resourceExists(/*[in]*/ BSTR aResPath, /*[in]*/ DWORD aFlags, /*[out]*/ DWORD* aStatus, bool abCheckOnlyExistInList = false);
	// Вернуть статус модуля
	long getStatus(/*[out]*/ DWORD* aStatus);
	// Вернуть размер модуля
	long getSize( /*[out]*/ DWORD* aSize);
	// Вернуть резмер загруженной части модуля
	long getDownloadSize( /*[out]*/ DWORD* aSize);
	// Загрузить модуль
	long download( /*[in]*/ BOOL aWait);
	// Вернуть процент загрузки модуля
	long getDownloadProcess( /*[out]*/ BYTE* aPercent);
	// Распаковать модуль
	long unpack( /*[in]*/ BOOL aWait);
	// Вернуть процент распаковки модуля
	//long getUnpackProgress( /*[out]*/ BYTE* aPercent, DWORD *pwUnpackTotalSize = NULL);
	// Запаковать модуль
	//long pack( /*[in]*/ BOOL aWait);
	// Вернуть процент запаковки модуля
	//long getPackProgress( /*[out]*/ BYTE* aPercent);
	long getMetadata( /*[out]*/ IResModuleMetadata** aMetadata);
	long copyResource(/*[in]*/ BSTR aSrcPath, /*[in]*/ IResModule* aDestModule, /*[in]*/ BSTR aDestPath);
	long copyFile(/*[in]*/ BSTR aSrcPath, /*[in]*/ BSTR aDestPath);
	long moveFile(/*[in]*/ BSTR aSrcPath, /*[in]*/ BSTR aDestPath, /*[out]*/ BOOL* aSuccess);
	long getFullPath(/*[in]*/ BSTR aResPath, /*[in]*/ DWORD aFlags, CWComString &sFullPath);
	long removeResource(/*[in]*/ BSTR aPath, /*[out]*/ BOOL* aSuccess);
	long downloadMetadata(BOOL aWait);
	long getRelation(/*[in]*/ int aRelN, CWComString &aModuleID);
	long extract(/*[in]*/ BSTR aSrcPath, /*[in]*/ BSTR aDestSubPath, CWComString &aFullPath);
	long resumeDownload();
	long pauseDownload();
	long getManifestDownloadedSize(/*[out]*/ DWORD* aSize);
	long getManifestSize(/*[out]*/ DWORD* aSize);

public:
	long removeFolder(/*[in]*/ BSTR aPath, /*[in]*/ BOOL aYesToAll, /*[out]*/ BOOL* aSuccess);
// IResModuleMetadata
	// Вернуть идентификатор модуля
	long getIdentifier(/*[out]*/ BSTR* aID);
	// Вернуть название модуля
	long getTitle(/*[out]*/ BSTR* aTitle, /*[in, out]*/ BSTR* aLanguage);
	// Вернуть язык модуля
	long getLanguage(/*[out]*/ BSTR* aLanguage);
	// Вернуть описание модуля
	long getDescription(/*[out]*/ BSTR* aDescription, /*[in, out]*/ BSTR* aLanguage);
	// Вернуть строку из метаданных
	long getMetadataString(/*[in]*/ BSTR aXPath, /*[out]*/ BSTR*aStr);
	long getTaxonomy( /*[out]*/ CWComString &aTaxonomy);

protected:
	bool loadMetadata();
	bool getMetadataStr( const wchar_t* aXPath,  wchar_t** aStr, xmlXPathObjectPtr* aResult = NULL, xmlNodePtr aRoot = NULL);
	bool getMetadataLangStr( const wchar_t* aXPath, wchar_t** aStr, BSTR* aLang);	
	bool downloadModule();
	bool downloadMetadata();
	static DWORD WINAPI downloadThread_(LPVOID param);
	virtual int downloadThread();
	static DWORD WINAPI downloadMetadataThread_(LPVOID param);
	virtual int downloadMetadataThread();
	bool openModule();
	long openFileInPackFile( BSTR aResPath, DWORD aFlags, IResourceNotify* aNotify, IResource** aRes);
	bool isFileInPack( DWORD aFlags);

protected:
	// Статус модуль
	DWORD dwStatus;
	// Идентификатор модуля
	CWComString sModuleID;
	// Базовый путь для ресурсов модуля
	CWComString sModulePath;
	// Путь к ресурсному файлу модуля
	CWComString sModuleFile;
	// Размер модуля
	DWORD dwSize;
	// Размер загруженной части модуля
	DWORD dwDownloadedSize;
	// Дескриптор потока асинхронной загрузки
	HANDLE hDownloadThread;
	// Разрешает работу потока асинхронной загрузки
	bool bEnableDownloadThread;
	// Дескриптор потока асинхронной загрузки
	HANDLE hDownloadMetadataThread;
	// Разрешает работу потока асинхронной загрузки
	bool bEnableDownloadMetadataThread;
	// DOM манифеста
	xmlDocPtr pXmlManifest;
	// Размер манифеста
	DWORD dwManifestSize;
	// Размер загруженной части манифеста
	DWORD dwManifestDownloadedSize;
	// Событие для продолжения загрузки
	HANDLE hDownloadEvent;

	bigfile::IBigFileManagerReader *pBigFileManagerReader;

private:
	static int openThread_( CResModule* aThis);
	bool bUserModule;
};

#endif // !defined(AFX_RESMODULE_H__75BF4B13_857C_418D_AEE9_A0419467DF3F__INCLUDED_)
