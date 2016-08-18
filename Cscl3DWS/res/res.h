// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: res.h,v $
// $Revision: 1.63 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2010/04/09 12:03:34 $ $Author: jedi $
//
//////////////////////////////////////////////////////////////////////

#ifndef __res_h___
#define __res_h___

#include <istream>
#include "res_consts.h"

// Для импорта/экспорта классов и процедур
#if defined (OMS_RESM_BUILD)
#  define RESM_DECL  __declspec(dllexport)
#else                 // defined (OMS_RESM_BUILD)
# define RESM_DECL   __declspec(dllimport)
#endif                // defined (OMS_RESM_BUILD)

#ifndef WIN32
#define RESM_NO_VTABLE
#else
#define RESM_NO_VTABLE __declspec(novtable)
#endif

namespace oms{
	class Stream;
}

#ifdef RES_TO_WRITE
	#define RES_CONST_WAS_DEFINED
	#undef RES_TO_WRITE
	#undef RES_CREATE
	#undef RES_MEM_CACHE
#endif

class ILogWriter;
class CAsyncModuleImpl;

namespace res
{
using oms::omsresult;
using std::istream;
using std::iostream;

struct IResourceNotify;

struct RESM_NO_VTABLE resIAsynchResource
{
	virtual omsresult OnLoaded()=0;
	virtual omsresult OnDownloaded()=0;
	virtual omsresult OnCached()=0;
	virtual omsresult OnUnpacked()=0;
	virtual omsresult OnError(unsigned int auiCode, unsigned int auiServerCode, wchar_t* aNotifyServerText)=0;
	virtual omsresult OnUploaded(wchar_t* aNotifyServerText)=0;
	virtual omsresult OnExisted( bool auiCode)=0;
	virtual omsresult OnRegistered(wchar_t* aNotifyServerText)=0;	
};

struct RESM_NO_VTABLE resIResource
{
	virtual omsresult GetIStream(istream* &apistrm)=0;
	virtual omsresult GetStream(oms::Stream* &apOMSStream)=0;
	virtual unsigned int	Read( unsigned char* aBuffer, unsigned int aBufferSize, unsigned int *aError)=0;
	virtual unsigned int	Write( unsigned char* aBuffer, unsigned int aBufferSize)=0;
	virtual bool	Seek( long aPos, unsigned char aFrom)=0;
	virtual unsigned long	GetPos()=0;
	virtual unsigned long	GetSize()=0;
	virtual unsigned long	GetCachedSize()=0;
	virtual __int64	GetTime()=0;
	virtual void	Close()=0;
	virtual const wchar_t* GetURL()=0;
	virtual bool GetFileNameRefineLoaded(wchar_t **wsName, const wchar_t *pCurName)=0;
	//
	virtual void	Download( resIAsynchResource* aAsynch=NULL, const wchar_t* apwcLocalPath=NULL)=0;
	virtual void	DownloadToCache( resIAsynchResource* aAsynch=NULL)=0;
	virtual bool	Unpack(resIAsynchResource* aAsynch=NULL)=0;
	virtual bool	UnpackToCache(resIAsynchResource* aAsynch=NULL)=0;
	virtual unsigned long	GetDownloadedSize()=0;
	virtual unsigned int GetState()=0;
	virtual unsigned int GetUnpackingProgress()=0;
	virtual bool	Upload(resIResource* apSrcRes, resIAsynchResource* aAsynch=NULL)=0;
	virtual unsigned long	GetUploadedSize()=0;
	virtual bool RegisterAtResServer( resIAsynchResource* aAsynch)=0;
};

struct RESM_NO_VTABLE resIWrite: public resIResource
{
	virtual omsresult GetIOStream(iostream* &apiostrm)=0;
};

struct IAsyncModuleImpl;

struct RESM_NO_VTABLE resIModuleNotify
{
	virtual void OnModuleOpened( const wchar_t* lpcModuleName) = 0;
	virtual void OnModuleCreated( const wchar_t* lpcModuleName, bool* abIsRemote) = 0;
	virtual void OnModuleError( const wchar_t* lpcModuleName, unsigned int aNotifyCode) = 0;
	virtual void SetAsyncModule( IAsyncModuleImpl* aThreadModule) = 0;
};

struct RESM_NO_VTABLE IAsyncModuleImpl
{
	virtual void Destroy() = 0;
	virtual void SetResIModuleNotify( resIModuleNotify* apResIModuleNotify) = 0;
};

struct RESM_NO_VTABLE IAsyncCheckExist
{
	virtual void setHandler( resIAsynchResource* apHandler) = 0;
	virtual void destroy() = 0;
};

/**
 * Интерфейс, возвращаемый при создании менеджера ресурсов
 */

struct RESM_NO_VTABLE resIResourceManager
{
	virtual	void ClearInfoList();
	virtual void PackResource(wchar_t* path, wchar_t* newPath) = 0;
	// Данная функция сейчас является частично синхронной, т.к. в ней есть вызов RefineURL, который является СИНХРОННЫМ.
	// Это приводит к тому, что вызов OpenAsynchResource из главного потока может привести к повисанию процесса.
	virtual omsresult OpenAsynchResource(const wchar_t* apwcURL, resIResource* &ares, IResourceNotify* apNotifier, unsigned int dwFlags = 0)=0;
	virtual omsresult OpenResource(const wchar_t* apwcURL, resIResource* &ares, unsigned int dwFlags=0)=0;	
	virtual omsresult OpenToWrite(const wchar_t* apwcURL, resIWrite* &ares, unsigned int adwFlags=0)=0;
	virtual bool ResourceExists(const wchar_t* apwcURL, unsigned int dwFlags, bool abCheckOnlyExistInList, unsigned int &codeError)=0;
	virtual bool ResourceExists(const wchar_t* apwcURL, unsigned int &codeError)=0;
	virtual bool ResourceExists(const wchar_t* apwcURL, unsigned int dwFlags, unsigned int &codeError)=0;
	virtual IAsyncCheckExist *ResourceExistsAsync( resIAsynchResource* aAsynch, const wchar_t* apwcURL, unsigned int dwFlags=0, bool abCheckOnlyExistInList = false)=0;
	virtual bool GetModuleExists(const wchar_t* apwcModuleID, unsigned int dwFlags, void **ppModule, unsigned int *adwCodeError)=0;
	virtual void DestroyModule(void **ppModule)=0;
	virtual wchar_t* GetFullPath(const wchar_t* apwcURL, DWORD aFlags = RES_LOCAL)=0;
	virtual wchar_t* GetLastError()=0;
	virtual bool SetModuleBase(const wchar_t* apwcModuleBase)=0;
	virtual bool GetModuleBase(const wchar_t** apwcModuleBase)=0;
	virtual int SetModuleID(const wchar_t* apwcModuleID, bool aSync = false, wchar_t *asScene = NULL, unsigned int *apErrorCode = NULL)=0;
	virtual bool GetModuleIDAndReady(wchar_t* apwcBuffer, unsigned int auBufferSize, unsigned int* auOutSize, bool* abReady)=0;
	virtual void OnLoadedModuleMeta(unsigned int aID, int iError);
	virtual bool SetUIBase(const wchar_t* apwcUIBase)=0;
	virtual omsresult AddResourceAliasAndPath(const wchar_t* apwcAlias, const wchar_t* apwcOwnerModuleID, 
											  const wchar_t* apwcResModuleID, unsigned int dwFlags = 0)=0;
	virtual omsresult CreateModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias, bool aRemote=false, bool aAsynchMode=false, resIModuleNotify* apResIModuleNotify=NULL)=0;
	virtual omsresult OpenModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias, bool aAsynchMode=false, resIModuleNotify* apResIModuleNotify=NULL)=0;
	virtual omsresult CloseModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias)=0;
	virtual omsresult CopyResource(const wchar_t* apwcSrcURL, const wchar_t* apwcDestURL)=0;
	virtual omsresult DownloadModule(const wchar_t* apwcModuleID)=0;
	virtual const wchar_t* Extract(const wchar_t* apwcSrc, const wchar_t* apwcSubPath)=0;
	virtual const wchar_t* GetTaxonomy (const wchar_t* apwcModuleID)=0;
	//virtual const wchar_t** GetRBRList(const wchar_t* apwcModuleID)=0;
	virtual const wchar_t* GetCacheBase()=0;
	virtual void SetUserBase(const wchar_t* sDir)=0;
	virtual void GetUserBase(const wchar_t** psDir)=0;
	virtual void GetUIBase(const wchar_t** psDir)=0;
	virtual void GetUISkinBase(const wchar_t** psDir)=0;
	virtual void SetUISkinBase(const wchar_t* psDir)=0;
	virtual void GetIResourceAndClose(resIResource* ares, LPVOID *appRes)=0;
	virtual void InitResourceManager()=0;
	virtual void DestroyResourceManager()=0;
	virtual void SetILogWriter(ILogWriter* lw)=0;
	virtual BOOL RemoveResource(const wchar_t* aPath) = 0;
	virtual bool IsConnectionResServer()=0;
	virtual bool GetModuleIDByAlias(const wchar_t* apwcAlias, const wchar_t* apwcOwnerModuleID, wchar_t **apModuleID, DWORD aSize)=0;
	virtual int PackUpdateClientFilesAndGetInfo( const wchar_t* aVersion, wchar_t *apBufferForFileName, unsigned int auBufferWideCharsCount, unsigned int &aFileSize)=0;
	virtual int PackClientSetup(wchar_t *apBufferForFileName, unsigned int auBufferWideCharsCount, unsigned int &aFileSize)=0;
	virtual void WriteUpdaterLog(const char *aLogValue)=0;
};

}
#ifdef RES_CONST_WAS_DEFINED
#define RES_TO_WRITE	0x00000100
#define RES_CREATE		0x00000200
#define RES_MEM_CACHE	0x00000400
#undef RES_CONST_WAS_DEFINED
#endif

//RESM_DECL omsresult CreateResourceManager(oms::omsContext* amcx);
//RESM_DECL void DestroyResourceManager(oms::omsContext* amcx);

#endif // __res_h___
