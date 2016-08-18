#ifndef __BIGFILEMAN_H_
#define __BIGFILEMAN_H_

#ifdef BIGFILEMAN_EXPORTS
#define BIGFILEMAN_API __declspec(dllexport)
#else
#define BIGFILEMAN_API __declspec(dllimport)
#endif

#ifdef WIN32
#define BIGFILEMAN_NO_VTABLE __declspec(novtable)
#else
#define BIGFILEMAN_NO_VTABLE
#endif

//#include "oms_context.h"
#include <vector>


namespace bigfile
{
	struct BIGFILEMAN_NO_VTABLE IBigFileManagerBase
	{
		virtual void Release() = 0;
		virtual void Destroy() = 0;
		virtual int CreateTreeInfo() = 0;
	};

	struct BIGFILEMAN_NO_VTABLE IBigFileManagerReader : IBigFileManagerBase
	{
		virtual bool OpenFile( const char *apFilePath, unsigned int &aFileSize, unsigned int &aRealOffset, __int64 &aTime) = 0;
		virtual unsigned int ReadFile( BYTE *pData, unsigned int aFileSize, unsigned int realOffset, int &errorCode) = 0;
		virtual int Unpack( const wchar_t *apExtractDirPath) = 0;
	};
		
	struct BIGFILEMAN_NO_VTABLE IBigFileManagerWriter : IBigFileManagerBase
	{
		virtual int CreateBigMemoryFileFromDir( const wchar_t *apDirPath, std::vector<const wchar_t*> *av_pDirPath, int deep = 0) = 0;
		virtual int SaveBigFile( const wchar_t *apFilePath) = 0;
		virtual int ReSaveBigFile( const wchar_t *apFilePath) = 0;
	};

	struct BIGFILEMAN_NO_VTABLE IBigFileManager : IBigFileManagerReader
	{
		virtual unsigned int AddFile( const char *apFilePath, BYTE *pData, unsigned int aByteWrite, __int64 atime) = 0;
		virtual unsigned int AddFile( const char *apFilePath, const char *apPathBase, const wchar_t *apFullFilePath) = 0;
		virtual unsigned int DeleteFile( const char *apFilePath) = 0;
		virtual int ReSaveBigFile( const wchar_t *apFilePath) = 0;
	};

	BIGFILEMAN_API unsigned int CreateBigFileManagerReader( IBigFileManagerReader **ppBigFileManagerReader, const wchar_t* aModuleFile);
	BIGFILEMAN_API unsigned int CreateBigFileManagerWriter( IBigFileManagerWriter **ppBigFileManagerWriter, const wchar_t* aModuleFile);
	BIGFILEMAN_API unsigned int CreateBigFileManager( IBigFileManager **ppBigFileManagerReader, const wchar_t* aModuleFile);
	BIGFILEMAN_API void DestroyBigFileManager(  IBigFileManagerBase *pBigFileManager, const wchar_t* aModuleFile);
}
#endif
