#pragma once
#include "BigFileman.h"
#include <map>
#include "DataBuffer.h"
#include "DataBuffer2.h"
#include "FileFind.h"
#include "Mutex.h"

//using namespace oms;
using namespace bigfile;

class CFileMemoryInfo
{
public:
	char name[50];
	unsigned short countChild;
	unsigned int offset;
	unsigned int size;
	__int64 time;
	CFileMemoryInfo();
};

typedef MP_VECTOR<CFileMemoryInfo*> CFileMemoryInfoVector;
typedef CFileMemoryInfoVector::iterator CFileMemoryInfoVectorIter;

class CTreeNodeInfo
{
public:
	CComString sName;
	CFileMemoryInfoVector fileMemoryMap;
	MP_VECTOR<CTreeNodeInfo*> dirMemoryMap;
	CTreeNodeInfo();
	CTreeNodeInfo(const char *pName);
};

typedef std::map<wchar_t[50], CTreeNodeInfo*> CTreeNodeInfoMap;
typedef CTreeNodeInfoMap::iterator CTreeNodeMapIter;


class CBigFileManager : public IBigFileManagerWriter, public IBigFileManager
{
public:
	typedef std::vector<CFileMemoryInfo> vecCFileMemoryInfo;
	typedef MP_VECTOR<CFileMemoryInfo> vecMPCFileMemoryInfo;
	CBigFileManager();
	virtual ~CBigFileManager();

	bool ReadBigFileAndCreateTreeInfo( const wchar_t *apFilePath, int right, bool abFileInfoReadOnly = true);
	bool OpenFile( const char *apFilePath, unsigned int &aFileSize, unsigned int &aRealOffset, __int64 &aTime);
	unsigned int ReadFile( BYTE *pData, unsigned int aFileSize, unsigned int realOffset, int &errorCode);
	int CreateTreeInfo();
	void Release();
	void Destroy();

	unsigned int AddFile( const char *apFilePath, BYTE *pData, unsigned int aByteWrite, __int64 atime);
	unsigned int AddFile( const char *apFilePath, const char *apPathBase, const wchar_t *apFullFilePath);
	unsigned int DeleteFile( const char *apFilePath);

	unsigned int FlushTreeFiles( vector<CFileMemoryInfo> *pFileMemoryInfosNew, CDataBuffer2 *pBigFileBufferNew, CTreeNodeInfo *aRoot);

	// Unicode functions
	int ReadDirAndAddToBigFile( const wchar_t *fname, const wchar_t *ext, CWComString &sPath, int deep, CTreeNodeInfo *apRoot);
	int ReadFileAndAddToBigFile( const wchar_t *apFileName, CWComString &sPath, CTreeNodeInfo *apRoot);
	int CreateBigMemoryFileFromDir( const wchar_t *apRootDirPath, std::vector<const wchar_t*> *av_pDirPath, int deep = 0);
	int ReadBigFile( const wchar_t *apFilePath, int right, bool bFileInfoReadOnly = true);
	int SaveBigFile( const wchar_t *apFilePath);
	int ReSaveBigFile( const wchar_t *apFilePath);
	int Unpack( const wchar_t *apExtractDirPath);

protected:
	CDataBuffer2 *mpBigFileBuffer;
	unsigned int countFile;
	unsigned int countAddedElement;
	unsigned int posContentBegin;
	unsigned int iContentSize;
	MP_VECTOR<CFileMemoryInfo> *pFileMemoryInfos;
	//MP_VECTOR<CFileMemoryInfo> *pFileMemoryInfosNew;
	//MP_VECTOR<CFileMemoryInfo> pFileMemoryInfosTemp;
	CWComString sBigFilePath;

	CTreeNodeInfo root;
	CMutex mutex;
	bool bFileInfoReadOnly;
	HANDLE hFileBigFile;

	int CreateBigMemoryFileFromDirRec( const wchar_t *apDirPath, std::vector<const wchar_t*> *av_pDirPath, int deep, int &countSubObject, CTreeNodeInfo *apRoot);
	bool CheckOperatedPath( CWComString &asDirPath, std::vector<const wchar_t*> *av_pDirPath);
	int UnpackRec( std::vector<CFileMemoryInfo>::iterator &iter, const wchar_t *apExtractDirPath, int count);
	int ExtractingOneElem( std::vector<CFileMemoryInfo>::iterator &iter, const wchar_t *apExtractDirPath);

	int CreateTreeInfoRec( std::vector<CFileMemoryInfo>::iterator &iter, CTreeNodeInfo *apRoot, int count);
	int CreateNodeTree( std::vector<CFileMemoryInfo>::iterator &iter, CTreeNodeInfo *apRoot);

	unsigned int GetPosForFileInfo( CFileMemoryInfo *pFileInfo, CTreeNodeInfo *apRoot);
	int AddFileNodeTree( CFileMemoryInfo *pFileInfo, CTreeNodeInfo *apRoot);
	int GetPosForDirInfo( CTreeNodeInfo *pDirInfo, CTreeNodeInfo *apRoot);
	int AddDirNodeTree( CTreeNodeInfo *pDirInfo, CTreeNodeInfo *apRoot);

	void CreateFindArray( std::vector<char *> &pp, const char *apFilePath);
	CTreeNodeInfo *FindTreeNode(CTreeNodeInfo *apNode, const char *p);
	void DeleteFindArray( std::vector<char *> &pp);
	CFileMemoryInfo *FindTreeFile( const char *apFilePath);
	CTreeNodeInfo *FindTreeDir( const char *apDirPath, bool bCraeteIfNoExist = false);
	void DeleteDir( const char *apDirPath);
};


