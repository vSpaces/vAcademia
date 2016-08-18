#ifndef __COMMANRES_H_
#define __COMMANRES_H_

#ifdef WIN32
#define COMMAN_NO_VTABLE __declspec(novtable)
#else
#define COMMAN_NO_VTABLE
#endif

namespace res
{
	struct IResource;

	struct COMMAN_NO_VTABLE IResourceNotify
	{
		virtual long onResourceNotify( IResource* apIResource, DWORD aNotifyCode, DWORD aNotifyServerCode, wchar_t *aErrorText) = 0;
		virtual void destroy() = 0;
		virtual void setAsynchResource(void* aAsynch) = 0;
		virtual bool isNeedDestroyByComman() = 0;
	};

	struct COMMAN_NO_VTABLE IResource
	{
		virtual void destroy()=0;
		virtual long close()=0;
		virtual long read(BYTE* aBuffer, DWORD aBufferSize, DWORD* aReadSize, DWORD *aError=NULL)=0;
		virtual long write(const BYTE* aBuffer, DWORD aBufferSize, DWORD * aWroteSize, DWORD aTotalSize=0)=0;
		virtual long seek(DWORD aPos, BYTE aFrom)=0;
		virtual long getPos(DWORD* aPos)=0;
		//virtual long getLastModifyDateTime(DWORD* aDateTime)=0;
		virtual long getSize(DWORD* aSize)=0;
		virtual long getCachedSize(DWORD* aSize)=0;
		virtual long getCacheProgress( BYTE* aPercent)=0;
		virtual long getStatus(DWORD* aStatus)=0;
		virtual long setSize(DWORD aSize)=0;
		virtual void getTime(FILETIME& aTime)=0;
		virtual long unpack( const wchar_t* aPath, BOOL aWait, __int64 aFreeBytesAvailable)=0;
		virtual long getUnpackProgress( BYTE* aPercent, DWORD *pwUnpackTotalSize = NULL)=0;
		virtual long getURL(wchar_t ** a_ppURL)=0;
		virtual bool getRefineLoadedResourcePath(wchar_t* pBuffer, unsigned int auBufferSize)=0;
		virtual long unpackToMemory( wchar_t * aFileName, BOOL* aSuccessful, DWORD* aDataSize, BYTE** aData)=0;
		// Вернуть резмер загруженной части модуля
		virtual long getDownloadedSize( DWORD* aSize)=0;
		// Загрузить модуль
		virtual long download( IResourceNotify* aNotify, const wchar_t* apwcLocalPath=NULL)=0;
		// Вернуть процент загрузки модуля
		virtual long getDownloadProcess( BYTE* aPercent)=0;
		virtual long unpack2( const wchar_t * aPath,  IResourceNotify* aNotify)=0;
		// Загрузить модуль
		virtual long downloadToCache( IResourceNotify* aNotify);
		virtual long unpackToCache( wchar_t * aPath,  IResourceNotify* aNotify)=0;
		virtual long saveToCache(void* pCacheData, DWORD dwCacheDataSize)=0;
		virtual long upload( IResource* aSrcRes,  IResourceNotify* aNotify)=0;
		virtual long getUploadedSize( DWORD *aSize)=0;
		virtual long registerAtResServer( IResourceNotify* aNotify)=0;
	};

	struct COMMAN_NO_VTABLE IResModuleMetadata
	{
		virtual long getIdentifier( wchar_t * *aID) = 0;
		virtual long getTitle(wchar_t * *aTitle, wchar_t * *aLanguage) = 0;
		virtual long getLanguage( wchar_t * *aLanguage) = 0;
		virtual long getDescription( wchar_t * *aDescription, wchar_t * *aLanguage) = 0;
		virtual long getMetadataString( wchar_t * aXPath, wchar_t * *aStr) = 0;
		virtual long getTaxonomy( CWComString &aTaxonomy) = 0;
		virtual long getRelation( int aRelN, CWComString &aModuleID) = 0;
	};

	struct COMMAN_NO_VTABLE IResModule
	{
		virtual void destroy() = 0;
		virtual long close() = 0;
		virtual long openResource( wchar_t * aResPath, DWORD aFlags, IResource **aRes) = 0;
		virtual long openAsynchResource(BSTR aResPath, DWORD aFlags, IResourceNotify *aNotify, IResource **aRes) = 0;
		virtual long resourceExists(wchar_t * aResPath, DWORD aFlags, DWORD *aStatus, bool abCheckOnlyExistInList = false) = 0;
		virtual long getStatus(DWORD *aStatus) = 0;
		virtual long getSize( DWORD *aSize) = 0;
		virtual long getDownloadSize( DWORD *aSize) = 0;
		virtual long download(BOOL aWait) = 0;
		virtual long getDownloadProcess( BYTE *aPercent) = 0;
		virtual long unpack( BOOL aWait) = 0;
		//virtual long getUnpackProgress( BYTE *aPercent, DWORD *pwUnpackTotalSize = NULL) = 0;
		//virtual long pack(BOOL aWait) = 0;
		//virtual long getPackProgress( BYTE *aPercent) = 0;
		virtual long getMetadata( IResModuleMetadata **aMetadata) = 0;
		virtual long copyResource( wchar_t * aSrcPath, IResModule *aDestModule, wchar_t * aDestPath) = 0;
		virtual long getFullPath( wchar_t * aResPath, DWORD aFlags, CWComString &sFullPath) = 0;
		virtual long copyFile( wchar_t * aSrcPath, wchar_t * aDestPath) = 0;
		virtual long extract( wchar_t * aSrcPath, wchar_t * aDestSubPath, CWComString &aFullPath) = 0;
		virtual long downloadMetadata( BOOL aWait) = 0;
		virtual long getManifestSize( DWORD *aSize) = 0;
		virtual long getManifestDownloadedSize( DWORD *aSize) = 0;
		virtual long pauseDownload() = 0;
		virtual long resumeDownload() = 0;
		virtual long removeResource( wchar_t * aPath, BOOL *aSuccess) = 0;
		virtual long moveFile( wchar_t * aSrcPath, wchar_t * aDestPath, BOOL *aSuccess) = 0;
		virtual long removeFolder( wchar_t * aPath, BOOL aYesToAll, BOOL *aSuccess) = 0;
	};

	struct COMMAN_NO_VTABLE IResMan
	{
		virtual long openResource( wchar_t * aResPath, DWORD aFlags, IResource **aRes) = 0;
		virtual long openModule( wchar_t * aModuleID, DWORD aFlags, IResModule **aRes) = 0;
		virtual long resourceExists( wchar_t * aResPath, DWORD aFlags, DWORD *aStatus, bool abCheckOnlyExistInList = false) = 0;
		virtual long getFullPath( wchar_t * aResPath, DWORD aFlags, CWComString &sFullPath) = 0;
		virtual long getRemoteBase( wchar_t * *pVal) = 0;
		virtual long getLocalBase( wchar_t * *pVal) = 0;
		virtual long getCacheBase( CWComString &pVal) = 0;
		virtual const wchar_t *getCacheBase() = 0;
		virtual long setCacheBase( wchar_t * newVal) = 0;
		virtual long getRepositoryBase( std::wstring &pVal) = 0;
		virtual long getUserBase( wchar_t * *pVal) = 0;
		virtual long setupResMan( wchar_t * aRegKey) = 0;
		virtual long getModuleExists( wchar_t * aModuleID, DWORD aFlags, DWORD *aStatus, IResModule **ppModule) = 0;
		virtual long writeStillOpened( ) = 0;
		virtual long openAsynchResource( wchar_t * aResPath, DWORD aFlags, IResourceNotify *aNotify, IResource **aRes) = 0;
		virtual long importModule( wchar_t * aFileName, DWORD *aErrors) = 0;
		virtual long openModuleByPath( wchar_t * aModulePath, IResModule **aRes) = 0;
	};
}
#endif