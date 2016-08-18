#include "StdAfx.h"
#include "BigFileManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ERROR_FILE_NAME_LENGTH			-1
#define ERROR_DIR_NAME_LENGTH			-2
#define ERROR_FILE_OPEN_FOR_READ		-3
#define ERROR_FILE_READING				-4
#define ERROR_FILE_OPEN_FOR_WRITE		-5
#define ERROR_FILE_WRITING				-6

#define ERROR_BIGFILE_OPEN				-7
#define ERROR_COUNT_FILE_INTO_BIGFILE	-8
#define ERROR_INFO_BIGFILE_READING		-9
#define ERROR_CONTENT_BIGFILE_READING	-10
#define ERROR_BIGFILE_CREATE			-11
#define ERROR_COUNT_FILE_WRITING		-12
#define ERROR_INFO_BIGFILE_WRITING		-13
#define ERROR_CONTENT_BIGFILE_WRITING	-14
#define ERROR_FILE_READING_FROM_BIGFILE -15

#define ERROR_ADDING_FILE_OPEN			1
#define ERROR_ADDING_FILE_READING		2
#define ERROR_FIND_DIR_INTO_BIG_FILE	3
#define ERROR_FIND_FILE_INTO_BIG_FILE	4


CFileMemoryInfo::CFileMemoryInfo()
{	
	memset( this, 0, sizeof(CFileMemoryInfo));
	countChild = 0;
	offset = 0;		
	time = (__int64)0;	
};


CTreeNodeInfo::CTreeNodeInfo():
	MP_VECTOR_INIT(dirMemoryMap), 
	MP_VECTOR_INIT(fileMemoryMap)
{
}

CTreeNodeInfo::CTreeNodeInfo(const char *pName):
	MP_VECTOR_INIT(dirMemoryMap), 
	MP_VECTOR_INIT(fileMemoryMap)
{
	sName = pName;
}

CBigFileManager::CBigFileManager()/*:
		MP_VECTOR_INIT(pFileMemoryInfosTemp);*/
{
	countFile = 0;
	iContentSize = 0;
	countAddedElement = 0;
	posContentBegin = 0;
	hFileBigFile = INVALID_HANDLE_VALUE;
	bFileInfoReadOnly = false;
	mpBigFileBuffer = MP_NEW(CDataBuffer2);
	MP_NEW_VECTOR(tmp, vecMPCFileMemoryInfo);
	pFileMemoryInfos = tmp;
}

CBigFileManager::~CBigFileManager()
{
	Release();
	mpBigFileBuffer->deleteData();
	MP_DELETE( mpBigFileBuffer);
	MP_DELETE( pFileMemoryInfos);
}

void CBigFileManager::Destroy()
{
	MP_DELETE_THIS;
}

bool CBigFileManager::CheckOperatedPath( CWComString &asDirPath, std::vector<const wchar_t*> *av_pDirPath)
{
	if( av_pDirPath == NULL)
		return false;
	//std::vector<const wchar_t*>::iterator iter = av_pDirPath->begin();
	int len = av_pDirPath->size();
	for(int i=0;i<len;i++)
	{
		const wchar_t *p = (wchar_t*)(*av_pDirPath)[i];
		int pos = asDirPath.Find( p, 0);
		if( pos>-1 && pos<2)
			return true;
	}
	return false;
}

int CBigFileManager::ReadDirAndAddToBigFile( const wchar_t *fname, const wchar_t *ext, CWComString &sPath, int deep, CTreeNodeInfo *apRoot)
{
	wchar_t path_buffer[MAX_PATH2];
	//_wmakepath( path_buffer, NULL, NULL, fname, ext);
	_wmakepath_s( path_buffer, NULL, NULL, fname, ext);

	//CFileMemoryInfo dirInfo;
	if(wcslen(path_buffer)>=50)
		return ERROR_FILE_NAME_LENGTH;

	CComString ansiDirName = path_buffer;
	//rtl_strcpy( dirInfo.name, ansiDirName.GetLength(), ansiDirName.GetBuffer());

	//int pos = (*pFileMemoryInfos).size();
	//(*pFileMemoryInfos).push_back( dirInfo);
	CTreeNodeInfo *pNode = MP_NEW( CTreeNodeInfo);
	pNode->sName = ansiDirName.GetBuffer();
	int countSubObject = 0;
	int code = CreateBigMemoryFileFromDirRec( (sPath + path_buffer), NULL, deep+1, countSubObject, pNode);
	if( code<0)
	{
		MP_DELETE( pNode);
		return code;
	}
	else if( countSubObject==0)
	{
		MP_DELETE( pNode);
		//(*pFileMemoryInfos).erase( (*pFileMemoryInfos).begin() + pos);
	}
	else
	{
		AddDirNodeTree( pNode, apRoot);
		//(*pFileMemoryInfos)[ pos].countChild = countSubObject;
	}
	return countSubObject > 0 ? 1 : 0;
}

int CBigFileManager::ReadFileAndAddToBigFile( const wchar_t *apFileName, CWComString &sPath, CTreeNodeInfo *apRoot)
{
	CComString sNewPath = apFileName;
	sNewPath.Replace("\\", "");
	if(sNewPath.GetLength() >= 50)
	{
		return ERROR_FILE_NAME_LENGTH;
	}
	//CFileMemoryInfo fileInfo;
	CFileMemoryInfo *pFileInfo = MP_NEW( CFileMemoryInfo);
	sNewPath.MakeLower();
	rtl_strcpy( pFileInfo->name, sizeof( pFileInfo->name), sNewPath);
	sNewPath.Insert(0, sPath.GetBuffer());
	HANDLE hFile = CreateFile( sNewPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE)
		return ERROR_FILE_OPEN_FOR_READ;
	DWORD dwBufferSize = GetFileSize(hFile, NULL);
	DWORD dwReadSize = 0;
	BYTE *pBuffer = NULL;
	if( dwBufferSize > 0)
	{
		pBuffer = MP_NEW_ARR( BYTE, dwBufferSize);
		if( !::ReadFile( hFile, pBuffer, dwBufferSize, &dwReadSize, NULL))
		{			
			CloseHandle( hFile);
			return ERROR_FILE_READING;
		}
	}
	FILETIME tmp;
	GetFileTime( hFile, NULL, NULL, &tmp);
	__int64 highTime = tmp.dwHighDateTime;
	pFileInfo->time = (__int64)((highTime << 32) + tmp.dwLowDateTime);
	CloseHandle( hFile);
	if( dwReadSize > 0)
	{
		pFileInfo->offset = mpBigFileBuffer->getSize();
		mpBigFileBuffer->addData( pBuffer, dwReadSize);
	}
	pFileInfo->size = dwReadSize;
	//fileInfo.offset = iContentSize;
	AddFileNodeTree( pFileInfo, apRoot);
	//(*pFileMemoryInfos).push_back( fileInfo);
	iContentSize+=dwReadSize;
	return 1;
}

int CBigFileManager::CreateBigMemoryFileFromDirRec( const wchar_t *apDirPath, std::vector<const wchar_t*> *av_pDirPath, int deep, int &countSubObject, CTreeNodeInfo *apRoot)
{	
	CFileFind find;
	CWComString sFile;
	CWComString sPath = apDirPath;
	if( sPath[ sPath.GetLength()-1] == L'\\' || sPath[ sPath.GetLength()-1] == L'/'){
	}
	else{
		sPath  +=  L'\\';
	}
	sFile = sPath + L"*.*";

	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];

	if( av_pDirPath == NULL || av_pDirPath->size()==0)
		deep = 1;

	int iRet = 0;
	BOOL bFind = find.FindFile( sFile.GetBuffer());
	vector<std::wstring> v_files;
	while( bFind )
	{
		bFind = find.FindNextFile();
		if( find.IsDirectory() )
		{
			if( !find.IsDots() )
			{
				CWComString sNewPath = find.GetFilePath().c_str();
				sNewPath.MakeLower();
				_wsplitpath_s( sNewPath, NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT);
				if( sNewPath.Find(L".svn") == -1 && sNewPath.Find(L"cvs") == -1 && (deep > 0 || CheckOperatedPath(sNewPath, av_pDirPath)))
				{
					int code = ReadDirAndAddToBigFile( fname, ext, sPath, deep, apRoot);
					if(code < 0)
						return code;
					if(code > 0)
						iRet++;
				}
			}
		}
		else 
		{
			if( !find.IsHidden() && deep>0)
			{
				std::wstring sFile = find.GetFilePath().c_str();
				v_files.push_back( sFile);				
			}  
		}
	}
	vector<std::wstring>::iterator v_files_iter = v_files.begin();
	for( ;v_files_iter != v_files.end(); ++v_files_iter)
	{
		std::wstring sFile = v_files_iter->c_str();
		int code = ReadFileAndAddToBigFile( sFile.c_str(), sPath, apRoot);
		if(code < 0)
			return code;
		iRet++;
	}
	countSubObject = iRet;
	return 0;
}

int CBigFileManager::CreateBigMemoryFileFromDir( const wchar_t *apDirPath, std::vector<const wchar_t*> *av_pDirPath, int deep)
{
	countFile = 0;
	countAddedElement = 0;
	iContentSize = 0;
	int countSubObject = 0;
	int code = CreateBigMemoryFileFromDirRec( apDirPath, av_pDirPath, deep, countSubObject, &root);
	if( code < 0)
		return code;
	//countFile = (*pFileMemoryInfos).size();
	return countSubObject;
}

int CBigFileManager::ReadBigFile( const wchar_t *apFilePath, int right, bool abFileInfoReadOnly)
{
	bFileInfoReadOnly = abFileInfoReadOnly;
	sBigFilePath = apFilePath;

	DWORD rightFlags = right == 0 ? GENERIC_READ : (right == 1 ? GENERIC_WRITE : GENERIC_READ | GENERIC_WRITE);
	hFileBigFile = CreateFileW( apFilePath, rightFlags, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFileBigFile == INVALID_HANDLE_VALUE || hFileBigFile == NULL)
		return ERROR_BIGFILE_OPEN;
	DWORD dwBufferSize = GetFileSize(hFileBigFile, NULL);
	DWORD dwReadSize = 0;
	DWORD dwReadSizeAll = 0;
	posContentBegin = 0;
	countFile = 0;
	countAddedElement = 0;
	if(!::ReadFile( hFileBigFile, &countFile, sizeof(unsigned int), &dwReadSize, NULL))
		return ERROR_COUNT_FILE_INTO_BIGFILE;

	dwReadSizeAll += dwReadSize;
	(*pFileMemoryInfos).resize( countFile);
	
	unsigned int size = countFile*sizeof(CFileMemoryInfo);
	if(!::ReadFile( hFileBigFile, (LPVOID)&(*pFileMemoryInfos)[0], size, &dwReadSize, NULL))
		return ERROR_INFO_BIGFILE_READING;
	dwReadSizeAll += dwReadSize;
	posContentBegin = dwReadSizeAll;
	iContentSize = dwBufferSize - dwReadSizeAll;

	if(!abFileInfoReadOnly)
	{
		mpBigFileBuffer->createBuffer( iContentSize + (int)(iContentSize/4));
		if(!::ReadFile( hFileBigFile, mpBigFileBuffer->getBytes(), iContentSize, &dwReadSize, NULL))
			return ERROR_CONTENT_BIGFILE_READING;
		mpBigFileBuffer->setDataLength( dwReadSize);	
		CloseHandle( hFileBigFile);
		hFileBigFile = NULL;
	}
	dwReadSizeAll += dwReadSize;
	mpBigFileBuffer->reset();
	return dwReadSizeAll;
}

int CBigFileManager::SaveBigFile( const wchar_t *apFilePath)
{
	CWComString sFileOld;
	hFileBigFile = CreateFileW( apFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFileBigFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( hFileBigFile);
		hFileBigFile = NULL;
		sFileOld = apFilePath;
		sFileOld += L".bak";
		::MoveFileW( apFilePath, sFileOld);
	}
	hFileBigFile = CreateFileW( apFilePath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFileBigFile == INVALID_HANDLE_VALUE)
		return ERROR_BIGFILE_CREATE;
	//CDataBuffer dataInfo;// = new CDataBuffer();
	
	DWORD dwWriteBytesAll = 0;
	DWORD dwWriteBytes = 0;
	if(!::WriteFile( hFileBigFile, &countFile, sizeof(unsigned int), &dwWriteBytes, NULL))
	{
		return ERROR_COUNT_FILE_WRITING;
	}
	dwWriteBytesAll += dwWriteBytes;
	unsigned int size = countFile*sizeof(CFileMemoryInfo);
	if(!::WriteFile( hFileBigFile, (LPCVOID)&(*pFileMemoryInfos)[0], size, &dwWriteBytes, NULL))
	{
		return ERROR_INFO_BIGFILE_WRITING;
	}
	dwWriteBytesAll += dwWriteBytes;
	mpBigFileBuffer->reset();
	if(!::WriteFile( hFileBigFile, mpBigFileBuffer->getBytes(), mpBigFileBuffer->getSize(), &dwWriteBytes, NULL))
	{
		return ERROR_CONTENT_BIGFILE_WRITING;
	}
	CloseHandle( hFileBigFile);
	dwWriteBytesAll += dwWriteBytes;
	if(sFileOld.GetLength()>1)
	{
		::DeleteFileW(sFileOld);
	}
	return dwWriteBytesAll;
}

void makeDir(const wchar_t* aDirPath)
{
	wchar_t temp[MAX_PATH2];
	memset(&temp, 0, MAX_PATH2);
	wcscat_s(temp, aDirPath);

	wchar_t* start;
	wchar_t* point;
	for(start = point = temp; *point; point++)
	{
		if(*point == L'/')
		{
			*point = L'\0'; 
			::CreateDirectoryW(start, NULL);
			*point = L'/';
		}
	}
}

int CBigFileManager::ExtractingOneElem( std::vector<CFileMemoryInfo>::iterator &iter, const wchar_t *apExtractDirPath)
{
	int iRet = 1;
	CWComString s = apExtractDirPath;
	USES_CONVERSION;
	CWComString iterName = A2W(iter->name);
	s += iterName;
	if( iter->countChild>0)
	{
		s += L"/";
		makeDir( s);
		iRet += UnpackRec( iter, s.GetBuffer(), iter->countChild);
	}
	else
	{
		HANDLE hFile = CreateFileW( s, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE)
			return ERROR_FILE_OPEN_FOR_WRITE;
		DWORD dwWriteBytes = 0;
		if(  iter->size > 0)
		{
			if(!bFileInfoReadOnly)
			{
				mpBigFileBuffer->seek( iter->offset);
				if(!::WriteFile(hFile, mpBigFileBuffer->getBytes(), iter->size, &dwWriteBytes, NULL))
				{
					return ERROR_FILE_WRITING;
				}				
			}
			else
			{
				BYTE *pBuffer = MP_NEW_ARR( BYTE, iter->size);
				int errorCode = 0;
				if(!ReadFile( pBuffer, iter->size, iter->offset, errorCode))
					return ERROR_FILE_READING_FROM_BIGFILE;
				if( !::WriteFile(hFile, pBuffer, iter->size, &dwWriteBytes, NULL))
				{
					return ERROR_FILE_WRITING;
				}
			}
			FILETIME fTime;
			fTime.dwHighDateTime = (iter->time >> 32);
			fTime.dwLowDateTime = (iter->time & 0xFFFFFFFF);
			SetFileTime( hFile, (LPFILETIME)NULL, (LPFILETIME)NULL, &fTime);
		}
		CloseHandle( hFile);
		iter++;
	}
	return iRet;
}

int CBigFileManager::UnpackRec( std::vector<CFileMemoryInfo>::iterator &iter, const wchar_t *apExtractDirPath, int count)
{
	int iRet = 0;
	iter++;
	for(int i=0;i<count && iter!=(*pFileMemoryInfos).end();i++)
	{
		int iCode = ExtractingOneElem( iter, apExtractDirPath);
		if(iCode<1)
			return iCode;
		iRet += iCode;
	}
	return iRet;
}

int CBigFileManager::Unpack( const wchar_t *apExtractDirPath)
{
	mutex.lock();
	std::vector<CFileMemoryInfo>::iterator iter = (*pFileMemoryInfos).begin();
	int iRet = 0;
	CWComString s = apExtractDirPath;
	s += L"/";
	s.Replace(L"\\", L"/");
	s.Replace(L"//", L"/");
	while(iter!=(*pFileMemoryInfos).end())
	{
		int iCode = ExtractingOneElem( iter, s.GetBuffer());
		if(iCode<1)
		{
			mutex.unlock();
			return iCode;
		}
		iRet += iCode;
	}
	mutex.unlock();
	return iRet;
}

void CBigFileManager::Release()
{
	countFile = 0;
	mpBigFileBuffer->reset();
	mpBigFileBuffer->deleteData();
	mpBigFileBuffer->init();
	if(bFileInfoReadOnly && hFileBigFile != NULL)
	{
		CloseHandle( hFileBigFile);
		hFileBigFile = NULL;
	}
	pFileMemoryInfos->clear();
}

unsigned int CBigFileManager::GetPosForFileInfo( CFileMemoryInfo *pFileInfo, CTreeNodeInfo *apRoot)
{
	CFileMemoryInfoVectorIter iter = apRoot->fileMemoryMap.begin();
	unsigned int i = 0;
	for(;iter!=apRoot->fileMemoryMap.end();++iter,++i)
	{
		CFileMemoryInfo *pFileInfo2 = (*iter);
		if( _stricmp( pFileInfo2->name, pFileInfo->name)>0)
		{			
			break;
		}
	}
	return i;
}

int CBigFileManager::AddFileNodeTree( CFileMemoryInfo *pFileInfo, CTreeNodeInfo *apRoot)
{
	unsigned int pos = GetPosForFileInfo( pFileInfo, apRoot);
	if( pos >= apRoot->fileMemoryMap.size())
		apRoot->fileMemoryMap.push_back( pFileInfo);
	else
		apRoot->fileMemoryMap.insert( apRoot->fileMemoryMap.begin() + pos, pFileInfo);
	return 1;
}

int CBigFileManager::GetPosForDirInfo( CTreeNodeInfo *pDirInfo, CTreeNodeInfo *apRoot)
{
	MP_VECTOR<CTreeNodeInfo *>::iterator iter = apRoot->dirMemoryMap.begin();
	int i = 0;
	for(;iter!=apRoot->dirMemoryMap.end();++iter,++i)
	{
		CTreeNodeInfo *pDirInfo2 = (*iter);
		if( _stricmp( pDirInfo2->sName.GetBuffer(), pDirInfo->sName.GetBuffer())>0)
		{			
			break;
		}
	}
	return i;
}

int CBigFileManager::AddDirNodeTree( CTreeNodeInfo *pDirInfo, CTreeNodeInfo *apRoot)
{
	unsigned int pos = GetPosForDirInfo( pDirInfo, apRoot);
	if( pos >= apRoot->dirMemoryMap.size())
		apRoot->dirMemoryMap.push_back( pDirInfo);
	else
		apRoot->dirMemoryMap.insert( apRoot->dirMemoryMap.begin() + pos, pDirInfo);
	return 1;
}

int CBigFileManager::CreateNodeTree( std::vector<CFileMemoryInfo>::iterator &iter, CTreeNodeInfo *apRoot)
{
	int iRet = 1;
	if(strlen(iter->name) == 0)
	{
		iter++;
		return iRet;
	}
	if( iter->countChild>0)
	{
		CTreeNodeInfo *pNode = NULL;
		MP_NEW_V( pNode, CTreeNodeInfo, iter->name);
		iRet += CreateTreeInfoRec( iter, pNode, iter->countChild);
		AddDirNodeTree( pNode, apRoot);
		//apRoot->dirMemoryMap.push_back( pNode);
	}
	else
	{
#if _MSC_VER >= 1600
		AddFileNodeTree( iter._Ptr, apRoot);	
#else
		AddFileNodeTree(  iter._Myptr, apRoot);		
#endif
		iter++;
	}
	return iRet;
}

int CBigFileManager::CreateTreeInfoRec( std::vector<CFileMemoryInfo>::iterator &iter, CTreeNodeInfo *apRoot, int count)
{
	int iRet = 0;
	iter++;
	for(int i=0;i<count && iter!=(*pFileMemoryInfos).end();i++)
	{
		int iCode = CreateNodeTree( iter, apRoot);
		iRet += iCode;	
	}
	return iRet;
}

int CBigFileManager::CreateTreeInfo()
{
	std::vector<CFileMemoryInfo>::iterator iter = (*pFileMemoryInfos).begin();
	int iRet = 0;
	while(iter!=(*pFileMemoryInfos).end())
	{
		int iCode = CreateNodeTree( iter, &root);
		if(iCode<1)
			return iCode;
		iRet += iCode;
	}
	return iRet;
}

bool CBigFileManager::ReadBigFileAndCreateTreeInfo( const wchar_t *apFilePath, int right, bool abFileInfoReadOnly)
{
	mutex.lock();
	bool bRes = ReadBigFile( apFilePath, right, abFileInfoReadOnly)>0?true:false;
	if(bRes)
		CreateTreeInfo();
	mutex.unlock();
	return bRes;
}

/*int CBigFileManager::CreateBigFileFromDirAndCreateTreeInfo( const char *apDirPath, const char *aNamePackFile, std::vector<const char*> *av_pDirPath, int deep)
{
	mutex.lock();
	CreateBigMemoryFileFromDir( apDirPath, av_pDirPath, deep);
	CreateTreeInfo();
	mutex.unlock();
	return true;
}*/

void CBigFileManager::CreateFindArray( std::vector<char *> &pp, const char *apFilePath)
{
	int nLen = strlen( apFilePath);
	int pos = 0;
	for(int i=0;i<nLen;i++)
	{
		if( apFilePath[i]=='/')
		{
			char *p = MP_NEW_ARR( TCHAR, i - pos + 1);
			strncpy_s(p, i - pos + 1, &apFilePath[pos], i - pos);
			p[i - pos] = '\0';
			pp.push_back( p);
			pos = i+1;
		}
	}
	char *p = MP_NEW_ARR( TCHAR, nLen - pos + 1);
	strncpy_s(p, nLen - pos + 1, &apFilePath[pos], nLen - pos);
	p[nLen - pos] = '\0';
	pp.push_back( p);
}

void CBigFileManager::DeleteFindArray( std::vector<char *> &pp)
{
	std::vector<char *>::iterator iterc = pp.begin();
	for(;iterc!=pp.end();++iterc)
	{
		char *p = (*iterc);
		//free(p);
		MP_DELETE_ARR( p);
		//p = NULL;
	}
	pp.clear();
}

CTreeNodeInfo *CBigFileManager::FindTreeNode(CTreeNodeInfo *apNode, const char *p)
{
	std::vector<CTreeNodeInfo *>::iterator iter = apNode->dirMemoryMap.begin();
	for(;iter!=apNode->dirMemoryMap.end();++iter)
	{
		CTreeNodeInfo *pNodeInfo = (*iter);
		if(!strcmp( p, pNodeInfo->sName))
		//if( pNodeInfo->sName.Find(p, 0) == 0)
		{
			return pNodeInfo;
		}
	}
	return NULL;
}

CTreeNodeInfo *CBigFileManager::FindTreeDir( const char *apDirPath, bool bCreateIfNoExist)
{
	CTreeNodeInfo *pNode = &root;
	if( apDirPath == NULL || strlen(apDirPath) == 0)
		return pNode;
	std::vector<char *> pp;
	CreateFindArray( pp, apDirPath);
	std::vector<char *>::iterator iterc = pp.begin();	
	while( pNode != NULL && iterc != pp.end())
	{
		char *p = (*iterc);
		++iterc;
		CTreeNodeInfo *pNodeFound = FindTreeNode( pNode, p);
		if( bCreateIfNoExist && pNodeFound == NULL)
		{
			CTreeNodeInfo *pNodeNewDir = NULL;
			MP_NEW_V(pNodeNewDir, CTreeNodeInfo, p);
			//iRet += CreateTreeInfoRec( iter, pNode, iter->countChild);
			AddDirNodeTree( pNodeNewDir, pNode);
			//pNode->dirMemoryMap.push_back( pNodeNewDir);
			pNode = pNodeNewDir;
			countAddedElement++;
		}
		else
			pNode = pNodeFound;
		if(iterc == pp.end())
		{
			DeleteFindArray( pp);
			return pNode;
		}
	}
	DeleteFindArray( pp);
	return NULL;
}

CFileMemoryInfo *CBigFileManager::FindTreeFile( const char *apFilePath)
{
	std::vector<char *> pp;
	CreateFindArray( pp, apFilePath);
	std::vector<char *>::iterator iterc = pp.begin();
	CTreeNodeInfo *pNode = &root;
	while( pNode != NULL && iterc != pp.end())
	{
		char *p = (*iterc);
		++iterc;
		if(iterc == pp.end())
		{
			CFileMemoryInfoVector::iterator iter = pNode->fileMemoryMap.begin();
			for(int i=0;iter!=pNode->fileMemoryMap.end();++iter,++i)
			{
				CFileMemoryInfo *pFileInfo = (*iter);
				if( !strcmp( p, pFileInfo->name))
				{
					DeleteFindArray( pp);
					return pFileInfo;
				}
			}
			pNode = NULL;
		}
		else
			pNode = FindTreeNode( pNode, p);
	}
	DeleteFindArray( pp);
	return NULL;
}

bool CBigFileManager::OpenFile( const char *apFilePath, unsigned int &aFileSize, unsigned int &aRealOffset, __int64 &aTime)
{
	mutex.lock();
	aFileSize = 0;
	aRealOffset = 0;
	bool bFind = false;
	CFileMemoryInfo *pFileInfo = FindTreeFile( apFilePath);
	if(pFileInfo != NULL)
	{
		aFileSize = pFileInfo->size;
		aRealOffset = pFileInfo->offset;
		aTime = pFileInfo->time;
		bFind = true;
	}
	mutex.unlock();
	return bFind;
}

unsigned int CBigFileManager::ReadFile( BYTE *pData, unsigned int aFileSize, unsigned int realOffset, int &errorCode)
{
	if( aFileSize == 0)
		return 0;
	mutex.lock();
	if(bFileInfoReadOnly)
	{
		DWORD dwPointer = SetFilePointer( hFileBigFile, posContentBegin + realOffset, NULL, FILE_BEGIN);
		if( dwPointer == INVALID_SET_FILE_POINTER)
		{
			errorCode = GetLastError();
			mutex.unlock();
			return 0;
		}
		DWORD iReaded = 0;
		if(!::ReadFile( hFileBigFile, pData, aFileSize, &iReaded, NULL))
		{
			errorCode = GetLastError();
			iReaded = 0;
		}
		mutex.unlock();
		return iReaded;
	}
	else
	{
		mpBigFileBuffer->seek( realOffset);
		if(mpBigFileBuffer->readData( &pData, aFileSize))
		{
			mutex.unlock();
			return aFileSize;
		}
	}
	mutex.unlock();
	return 0;
}

unsigned int CBigFileManager::AddFile( const char *apFilePath, BYTE *pData, unsigned int aByteWrite, __int64 atime)
{
	CComString sNewPath = apFilePath;
	sNewPath.Replace("\\", "/");
	if(sNewPath[0] == '/')
		sNewPath = sNewPath.Right( sNewPath.GetLength() - 1);
	sNewPath.MakeLower();
	//CFileMemoryInfo fileMemoryInfo;
	CFileMemoryInfo *pFileMemoryInfo = FindTreeFile( sNewPath);
	bool bFind = true;
	if( pFileMemoryInfo == NULL)
	{
		bFind = false;
		//pFileMemoryInfo = &fileMemoryInfo;
		pFileMemoryInfo = MP_NEW( CFileMemoryInfo);
		CComString sFileName = sNewPath.Right( sNewPath.GetLength() - sNewPath.ReverseFind('/') - 1);
		if( sFileName.GetLength() < 50)
			rtl_strcpy(pFileMemoryInfo->name, sizeof( pFileMemoryInfo->name), sFileName.GetBuffer());
	}
	pFileMemoryInfo->time = atime;
	pFileMemoryInfo->size = aByteWrite;
	pFileMemoryInfo->offset = iContentSize;	
	if( !bFind)
	{
		CComString sDir = sNewPath.Left( sNewPath.ReverseFind('/'));
		CTreeNodeInfo *pNode = FindTreeDir( sDir, true);
		//pFileMemoryInfosTemp.push_back( *pFileMemoryInfo);
		if(pNode == NULL)
		{	
			/*pFileMemoryInfosTemp.push_back( *pFileMemoryInfo);
			std::vector<CFileMemoryInfo>::iterator iter = pFileMemoryInfosTemp.begin() + (countFile - 1);
			CreateNodeTree( iter, &root);*/
			return ERROR_FIND_DIR_INTO_BIG_FILE;
		}
		countAddedElement++;
		countFile++;
		AddFileNodeTree( pFileMemoryInfo, pNode);
	}

	if( aByteWrite > 0)
	{
		mpBigFileBuffer->addData( pData, aByteWrite);
		iContentSize += aByteWrite;
	}
	else
	{
		iContentSize += aByteWrite;
	}
	return 0;
}

unsigned int CBigFileManager::AddFile( const char *apFilePath, const char *apPathBase, const wchar_t *apFullFilePath)
{
	USES_CONVERSION;
	CWComString sNewPath;
	if( apFullFilePath != NULL)
		sNewPath = apFullFilePath;
	else if( apPathBase != NULL)
	{
		sNewPath = A2W(apPathBase);
		sNewPath += A2W(apFilePath);
	}
	HANDLE hFile = CreateFileW( sNewPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		hFile = CreateFileW( sNewPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			return GetLastError();//ERROR_ADDING_FILE_OPEN;
		}
	}
	DWORD dwBufferSize = GetFileSize(hFile, NULL);
	DWORD dwReadSize = 0;
	BYTE *pBuffer = MP_NEW_ARR( BYTE, dwBufferSize);
	if( dwBufferSize > 0 && (!::ReadFile( hFile, pBuffer, dwBufferSize, &dwReadSize, NULL) || dwReadSize == 0))
	{
		MP_DELETE_ARR( pBuffer);
		return GetLastError();//ERROR_ADDING_FILE_READING;
	}
	FILETIME tmp;
	GetFileTime( hFile, NULL, NULL, &tmp);
	__int64 _time = tmp.dwHighDateTime;
	_time = (__int64)((_time << 32) + tmp.dwLowDateTime);
	CloseHandle( hFile);	
	unsigned int res = AddFile( apFilePath, pBuffer, dwReadSize, _time);
	MP_DELETE_ARR( pBuffer);
	return res;
}

void CBigFileManager::DeleteDir( const char *apDirPath)
{
	CComString sNewPath = apDirPath;
	CComString sDir = sNewPath.Left( sNewPath.ReverseFind('/'));
	sNewPath = sNewPath.Right( sNewPath.GetLength() - sNewPath.ReverseFind('/') - 1);
	CTreeNodeInfo *pNode = FindTreeDir( sDir, true);
	std::vector<CTreeNodeInfo *>::iterator iter = pNode->dirMemoryMap.begin();
	for(;iter!=pNode->dirMemoryMap.end();++iter)
	{
		CTreeNodeInfo *pDirInfo = (*iter);
		if( !strcmp( pDirInfo->sName, sNewPath.GetBuffer()))
		{
			pNode->dirMemoryMap.erase( iter);
			break;
		}
	}
	if( pNode->fileMemoryMap.size() == 0 && pNode->dirMemoryMap.size() == 0)
	{
		DeleteDir( sDir);
	}
}

unsigned int CBigFileManager::DeleteFile( const char *apFilePath)
{	
	CComString sNewPath = apFilePath;
	sNewPath.Replace("\\", "/");
	if(sNewPath[0] == '/')
		sNewPath = sNewPath.Right( sNewPath.GetLength() - 1);
	sNewPath.MakeLower();
	CFileMemoryInfo *pFileMemoryInfo = FindTreeFile( sNewPath);
	if( pFileMemoryInfo == NULL)
	{
		return ERROR_FIND_FILE_INTO_BIG_FILE;		
	}	
	pFileMemoryInfo->size = 0xFFFFFFFF;
	pFileMemoryInfo->offset = 0xFFFFFFFF;
	CComString sDir = sNewPath.Left( sNewPath.ReverseFind('/'));
	CTreeNodeInfo *pNode = FindTreeDir( sDir, false);
	CFileMemoryInfoVector::iterator iter = pNode->fileMemoryMap.begin();
	for(;iter!=pNode->fileMemoryMap.end();++iter)
	{
		CFileMemoryInfo *pFileInfo = (*iter);
		if( !strcmp( pFileMemoryInfo->name, pFileInfo->name))
		{
			pNode->fileMemoryMap.erase( iter);
			break;
		}
	}
	if( pNode->fileMemoryMap.size() == 0 && pNode->dirMemoryMap.size() == 0)
	{
		DeleteDir( sDir);
	}
	return 1;
}

int CBigFileManager::ReSaveBigFile( const wchar_t *apFilePath)
{
	CDataBuffer2 *mpBigFileBufferNew = MP_NEW( CDataBuffer2);
	mpBigFileBufferNew->createBuffer( mpBigFileBuffer->getSize() + countAddedElement * sizeof(CFileMemoryInfo) + 1);
	//vecCFileMemoryInfo *pFileMemoryInfosNew = new vecCFileMemoryInfo;
	MP_NEW_VECTOR( pFileMemoryInfosNew, vecMPCFileMemoryInfo);
	int countFileNew = FlushTreeFiles( pFileMemoryInfosNew, mpBigFileBufferNew, &root);
	Release();
	countFile = countFileNew;
	MP_DELETE( mpBigFileBuffer);
	mpBigFileBuffer = mpBigFileBufferNew;
	MP_DELETE( pFileMemoryInfos);
	pFileMemoryInfos = pFileMemoryInfosNew;
	//pFileMemoryInfosTemp.clear();
	if( apFilePath == NULL)
		return SaveBigFile( sBigFilePath);
	return SaveBigFile( apFilePath);

}

unsigned int CBigFileManager::FlushTreeFiles( vecCFileMemoryInfo *pFileMemoryInfosNew, CDataBuffer2 *pBigFileBufferNew, CTreeNodeInfo *aRoot)
{
	unsigned int count = 0;
	CFileMemoryInfoVector::iterator iterFile = aRoot->fileMemoryMap.begin();
	std::vector<CTreeNodeInfo *>::iterator iter = aRoot->dirMemoryMap.begin();
	for(;iter!=aRoot->dirMemoryMap.end();++iter)
	{
		CTreeNodeInfo *pNodeInfo = (*iter);
		CFileMemoryInfo dirInfo;
		if( pNodeInfo->sName.GetLength() < 50)
			rtl_strcpy(dirInfo.name, sizeof( dirInfo.name), pNodeInfo->sName.GetBuffer());
		dirInfo.countChild = (unsigned short)(pNodeInfo->fileMemoryMap.size() + pNodeInfo->dirMemoryMap.size());
		pFileMemoryInfosNew->push_back( dirInfo);		
		++count;
		count += FlushTreeFiles( pFileMemoryInfosNew, pBigFileBufferNew, pNodeInfo);
	}
	for(;iterFile!=aRoot->fileMemoryMap.end();iterFile++)
	{
		CFileMemoryInfo *pFileInfo = (*iterFile);
		BYTE *pBuffer = MP_NEW_ARR( BYTE, pFileInfo->size + 1);
		mpBigFileBuffer->seek( pFileInfo->offset);

		CFileMemoryInfo fileInfo;
		rtl_strcpy(fileInfo.name, sizeof( fileInfo.name), pFileInfo->name);
		fileInfo.size = pFileInfo->size;
		fileInfo.time = pFileInfo->time;
		if( pFileInfo->size > 0 && mpBigFileBuffer->readData( &pBuffer, pFileInfo->size))
		{
			fileInfo.offset = pBigFileBufferNew->getSize();
			pBigFileBufferNew->addData( pBuffer, pFileInfo->size);
		}		

		pFileMemoryInfosNew->push_back( fileInfo);
		count++;
	}
	return count;
}