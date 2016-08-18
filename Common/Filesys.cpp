//
// Stingray Software Extension Classes
// Copyright (C) 1995 Stingray Software Inc.
// All Rights Reserved
//
// This source code is only intended as a supplement to the
// Stingray Extension Class product.
// See the SEC help files for detailed information
// regarding using SEC classes.
//
//  Author:             SPL
//  Description:        Implementation of SECFileSystem
//  Created:            Oct 30, 1995
//

//***************************************************************************
//  Include Files
//***************************************************************************

#include "stdafx.h"
#include <atlbase.h>
#include "filesys.h"
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
//static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//#define new DEBUG_NEW


//***************************************************************************
//
// Name:    SECFileSystem
//
// Purpose: Constructor.
//
// Notes:   None.
//
//***************************************************************************
SECFileSystem::SECFileSystem()
{
  // Initialize instance variables
  m_nMaxFileNameLength = 1024;
} // SECFileSystem


//***************************************************************************
//
// Name:    ~SECFileSystem
//
// Purpose: Destructor.
//
// Notes:   None.
//
//***************************************************************************
SECFileSystem::~SECFileSystem()
{
  // No code.
} // ~SECFileSystem


//***************************************************************************
//
// Name:    GetFileStatus
//
// Purpose: To return the status information about the specified file.
//          See the MFC help on the CFileStatus struct for more information.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          CFileStatus Status;
//          BOOL bRetVal = fs.GetFileStatus("c:\\test.txt", Status);
//
// Notes:    None.
//
//***************************************************************************
/*BOOL
SECFileSystem::GetFileStatus(const CComString& FileName, CFileStatus& FileStatus)
{
  return CFile::GetStatus(FileName, FileStatus);
} // GetFileStatus


//***************************************************************************
//
// Name:    GetFileCreateTime
//
// Purpose: To return the creation time of the specified file.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          CTime CreateTime;
//          BOOL bRetVal = fs.GetFileCreateTime("c:\\test.txt", CreateTime);
//
// Notes:   None.
//
//***************************************************************************
BOOL
SECFileSystem::GetFileCreateTime(const CComString& FileName, CTime& time)
{
  CFileStatus FileStatus;
  if (CFile::GetStatus(FileName, FileStatus))
  {
    time = FileStatus.m_ctime;
    return TRUE;
  } // if

  return FALSE;
} // GetFileCreateTime


//***************************************************************************
//
// Name:    GetFileModifyTime
//
// Purpose:  To return the last modification time of the specified file.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          CTime ModifyTime;
//          BOOL bRetVal = fs.GetFileModifyTime("c:\\test.txt", ModifyTime);
//
// Notes:   None.
//
//***************************************************************************
BOOL
SECFileSystem::GetFileModifyTime(const CComString& FileName, CTime& time)
{
  CFileStatus FileStatus;
  if (CFile::GetStatus(FileName, FileStatus))
  {
    time = FileStatus.m_mtime;
    return TRUE;
  } // if

  return FALSE;
} // GetFileModifyTime


//***************************************************************************
//
// Name:    GetFileAccessTime
//
// Purpose: To return the time of last access for the specified file.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          CTime AccessTime;
//          BOOL bRetVal = fs.GetFileAccessTime("c:\\test.txt", AccessTime);
//
// Notes:   None.
//
//***************************************************************************
BOOL
SECFileSystem::GetFileAccessTime(const CComString& FileName, CTime& time)
{
  CFileStatus FileStatus;
  if (CFile::GetStatus(FileName, FileStatus))
  {
    time = FileStatus.m_atime;
    return TRUE;
  } // if

  return FALSE;
} // GetFileAccessTime


//***************************************************************************
//
// Name:    GetFileSize
//
// Purpose: To return the size of the specified file in bytes.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          unsigned long lSize = 0;
//          BOOL bRetVal = fs.GetFileSize("c:\\test.txt", lSize);
//
// Notes:   None.
//
//***************************************************************************
BOOL
SECFileSystem::GetFileSize(const CComString& FileName, unsigned long& lSize)
{
  CFileStatus FileStatus;
  if (CFile::GetStatus(FileName, FileStatus))
  {
    lSize = FileStatus.m_size;
    return TRUE;
  } // if

  return FALSE;
} // GetFileSize


//***************************************************************************
//
// Name:    GetFileAttribute
//
// Purpose: To return the attribute bits associated with the file.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          BOOL bRetVal = fs.GetFileAttribute("c:\\test.txt", bAttr);
//
// Notes:   See the Attribute enum in filesys.h for specific attributes.
//
//***************************************************************************
BOOL
SECFileSystem::GetFileAttribute(const CComString& FileName, BYTE& nAttribute)
{
  CFileStatus FileStatus;
  if (CFile::GetStatus(FileName, FileStatus))
  {
    nAttribute = FileStatus.m_attribute;
    return TRUE;
  } // if

  return FALSE;
} // GetFileAttribute
*/

#ifdef WIN32
//***************************************************************************
//
// Name:    GetADirectoryEntry
//
// Purpose: Function used in Win32 to get get directory entries with the
//          correct attributes set.
//
// Ret Val: TRUE: If no error.
//          FALSE: Error occurred.
//
// Example:
//
// Notes:   None
//
//***************************************************************************
BOOL
SECFileSystem::GetADirectoryEntry(CComString &fileName, const CComString& Wildcard /* = "" */, const unsigned long eFileAttrib /* = normal */)
{
  BOOL      bRetVal = TRUE;
  DWORD		tmp;
  unsigned long mask = eFileAttrib;

  // If they passed in a Wildcard, we want to lookup the first entry.
  if (Wildcard != "")
  {
    m_hFind = FindFirstFile((const TCHAR *)Wildcard, &m_FileInfo);
    if (m_hFind == INVALID_HANDLE_VALUE)
    {
      bRetVal = FALSE;
    } // if
  } // if
  else
  {
    bRetVal = FindNextFile(m_hFind, &m_FileInfo);
  } // else


  if (bRetVal == FALSE)
  {
    // Error occurred.
    fileName = "";
    return FALSE;
  } // if

  tmp = m_FileInfo.dwFileAttributes;
  if (mask==_A_NORMAL || mask==FILE_ATTRIBUTE_NORMAL)
  {  
    if (!(tmp & 0x5e)) tmp = 0x80, mask = 0x80;
  }
  if (mask==allfiles || (tmp & mask))
  {
	fileName=m_FileInfo.cFileName;
  } // if
  else
  {
    fileName = "";
  } // else

  return TRUE;
} // GetADirectoryEntry
#endif


//***************************************************************************
//
// Name:     GetDirectoryEntry
//
// Purpose:  To return the next directory entry based on a wildcard, etc...
//
// Example:  CComString *pFileName = fs.GetDirectoryEntry("*.txt", hidden);
//
// Notes:    None.
//
//***************************************************************************
CComString *
SECFileSystem::GetDirectoryEntry(const CComString& Wildcard /* = "" */, const unsigned long eFileAttrib /* = normal */)
{
#ifdef WIN32

  // Turn off critical error handler.
  UINT nPrevErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);

  BOOL      bRetVal = TRUE;

  CComString fileName;
  CComString l_Wildcard = Wildcard;
  while (bRetVal  &&  fileName == "")
  {
    bRetVal = GetADirectoryEntry(fileName, l_Wildcard, eFileAttrib);
    l_Wildcard = "";
  } // while

  // Turn on critical error handler.
  ::SetErrorMode(nPrevErrorMode);

  if (bRetVal == FALSE  &&  fileName == "")
  {
    return NULL;
  } // if

  // Create a string and copy the name to it.
  CComString *pString;
  MP_NEW_V(pString, CComString, fileName);
  return pString;

#else

	int nRetVal;

	// Turn off DOS critical error handler.
	::SetErrorMode(1);

	// If they passed in a Wildcard, we want to lookup the first entry.
	if (Wildcard != "")
	{
		nRetVal = _dos_findfirst((const char *) Wildcard, (int) eFileAttrib, &m_FileInfo);
	} // if
	else
	{
		nRetVal = _dos_findnext(&m_FileInfo);
	} // else

	if (nRetVal != 0)
	{
		// Turn on DOS critical error handler.
		::SetErrorMode(0);

		// Error occurred, return NULL.
		return NULL;
	} // if

	// Create a string and copy the name to it.
	CComString *pString = MP_NEW(CComString);
	memcpy(pString->GetBufferSetLength(m_nMaxFileNameLength), m_FileInfo.name, 13);
	//pString->ReleaseBuffer(-1);

	// Make the filename lower case.
	pString->MakeLower();

	// Turn on DOS critical error handler.
	::SetErrorMode(0);

	return pString;

#endif
} // GetDirectoryEntry


//***************************************************************************
//
// Name:    GetCurrentPath
//
// Purpose:  To return the directory from a path.
//
// Example:  CComString Path = fs.GetCurrentPath("c:\\foo\\bar\\what.txt");
//          ATLASSERT(Path == "c:\\foo\\bar\\");
//
// Notes:    None
//
//***************************************************************************
CWComString SECFileSystem::GetCurrentPath(const CWComString& PathAndFileName, const BOOL bIncludeFileSystem /* = TRUE */)
{
	CWComString FileName = PathAndFileName; // Copy to modify;
	CWComString  Path = L"";

	// Find the last "\" in the string and return everything up to and including it.
	char	delimChar = L'\\';
	int nIndex = FileName.Find(delimChar);
	if( nIndex == -1)
	{
		delimChar = L'/';
		nIndex = FileName.Find(delimChar);
	}
	while(nIndex != -1)
	{
		Path = Path + FileName.Left(nIndex + 1);
		FileName = FileName.Right(FileName.GetLength() - nIndex - 1);

		nIndex = FileName.Find(delimChar);
	} // while


	if (!bIncludeFileSystem)
	{
		// Remove the filesystem name from the front of the path.
		int nIndex = Path.Find(L':');
		if (nIndex != -1)
		{
			Path = Path.Right(Path.GetLength() - nIndex - 1);
		} // if
	} // if

	return Path;
} // GetCurrentPath

//***************************************************************************
//
// Name:     GetCurrentDirectory
//
// Purpose:  To get the current working directory.
//
// Example:  CComString CurrentDir = fs.GetCurrentDirecory();
//
// Notes:    None
//
//***************************************************************************
CWComString
SECFileSystem::GetCurrentDirectory(const CWComString& FileSystem /* = "" */)
{
  // Turn off DOS critical error handler.
  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  CComString CurrentFileSystem = GetCurrentFileSystem();
  if (FileSystem != L"")
  {
    if (!ChangeFileSystem(FileSystem))
    {
      return L"";
    } // if
  } // if

  CWComString String;

#ifdef WIN32

  DWORD dwRetVal = ::GetCurrentDirectoryW(m_nMaxFileNameLength, String.GetBufferSetLength(m_nMaxFileNameLength));
  //String.ReleaseBuffer(-1);

  if (dwRetVal == 0)
  {
    String = L"";
  } // if

#else
	wchar_t *pRetVal = _wgetcwd(String.GetBufferSetLength(m_nMaxFileNameLength), m_nMaxFileNameLength);
	//String.ReleaseBuffer(-1);

	// Make the string lower case.
	String.MakeLower();

	// If an error occured, clean up and return NULL.
	if (pRetVal == 0)
	{
		String = L"";
	} // if

#endif

  if (FileSystem != L"")
  {
	  USES_CONVERSION;
	  ATLASSERT(ChangeFileSystem( A2W(CurrentFileSystem.GetBuffer())));
  } // if

  // Turn on DOS critical error handler.
  ::SetErrorMode(0);

  return String;
} // GetCurrentDirectory


//***************************************************************************
//
// Name:    ChangeDirectory
//
// Purpose: To change the current working directory.
//
// Example: BOOL bRetVal = fs.ChangeDirectory("c:\\foo\\bar");
//
// Notes:   None.
//
//***************************************************************************
BOOL
SECFileSystem::ChangeDirectory(const CWComString& NewDirectory)
{
  CWComString DirName = NewDirectory;

  // Remove any trailing slashes.
  int nLength = DirName.GetLength();
  if (nLength  &&
      DirName.GetAt(nLength - 1) == L'\\')
  {
    DirName = DirName.Left(nLength - 1);
  } // if

#ifdef WIN32

  return SetCurrentDirectoryW((const WCHAR *) DirName);

#else

	int nRetVal = _wchdir((const WCHAR *) DirName);
	if (nRetVal == -1)
	{
		return FALSE;
	} // if

	return TRUE;

#endif
} // ChangeDirectory


//***************************************************************************
//
// Name:    RenameDirectory
//
// Purpose: To rename a directory.
//
// Ret Val: TRUE : No error, or OldName == NewName.
//          FALSE : Error.
//
// Example: BOOL bRetVal = fs.RenameDirectory("c:\\foo", "c:\\bar");
//
// Notes:   Using WIN16, if the new directory name is longer than 8.3,
//          the parts that are too long will be truncated.
//
//***************************************************************************
BOOL
SECFileSystem::RenameDirectory(const CWComString& OldName, const CWComString& NewName)
{
  /*if (OldName == NewName)
  {
    return TRUE;
  } // if

  if (!DirectoryExists(OldName)  ||  DirectoryExists(NewName))
  {
    return FALSE;
  } // if

  TRY
  {
    CFile::Rename((const TCHAR *) OldName, (const TCHAR *) NewName);
  } // TRY
  CATCH(CFileException, Exception)
  {
    return FALSE;
  } // CATCH
  END_CATCH
*/
  return TRUE;
} // RenameDirectory


//***************************************************************************
//
// Name:    MakeDirectory
//
// Purpose: To make a directory.
//
// Example: BOOL bRetVal = fs.MakeDirectory("c:\\foo\\bar");
//
// Notes:   None
//
//***************************************************************************
BOOL
SECFileSystem::MakeDirectory(const CWComString& NewDirectory)
{
#ifdef WIN32

  SECURITY_ATTRIBUTES security_attrib;
  security_attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
  security_attrib.lpSecurityDescriptor = NULL;
  security_attrib.bInheritHandle = TRUE;

  BOOL bRetVal = CreateDirectoryW((const WCHAR *) NewDirectory, &security_attrib);
  return bRetVal;

#else

	int nRetVal = _wmkdir((const wchar_t *) NewDirectory);
	if (nRetVal == -1)
	{
		return FALSE;
	} // if

	return TRUE;

#endif
} // MakeDirectory


//***************************************************************************
//
// Name:    MakePath
//
// Purpose: To make all the directories in a given path.  If any of the
//          directories exist, the creation continues with lower level
//          directories.
//
// Ret Val: TRUE : No error.
//          FALSE : Unable to create path.
//                  Path already exists.
//                  No path given.
//
// Example: BOOL bRetVal = fs.MakePath("c:\\foo\\bar\\what");
//
// Notes:    None
//
//***************************************************************************
BOOL
SECFileSystem::MakePath(const CWComString& NewDirectory)
{
  CWComString NewDir = NewDirectory;  // Copy string for manipulation
  CWComString  DirName;
  BOOL    bRetVal = TRUE;

  // Error if no directory specified.
  if (NewDir.GetLength() == 0)
  {
    return FALSE;
  } // if

  // Make sure the directory name ends in a slash
  if (NewDir[NewDir.GetLength() - 1] != L'\\')
  {
    NewDir = NewDir + L'\\';
  } // if

  // Create each directory in the path
  UINT  nIndex = 0;
  BOOL  bDone = FALSE;
  while (!bDone)
  {
    // Extract one directory
    nIndex = NewDir.Find(L'\\');
    if (nIndex != -1)
    {
      DirName = DirName + NewDir.Left(nIndex);
      NewDir = NewDir.Right(NewDir.GetLength() - nIndex - 1);

      // The first time through, we might have a drive name
      if (DirName.GetLength() >= 1  &&  DirName[DirName.GetLength() - 1] != L':')
      {
        bRetVal = MakeDirectory(DirName);
      } // if
      DirName = DirName + L'\\';
    } // if
    else
    {
      // We're finished
      bDone = TRUE;
    } // else
  } // while

  // Return the last MakeDirectory() return value.
  return bRetVal;
} // MakePath


//***************************************************************************
//
// Name:    DeleteDirectory
//
// Purpose: To delete a directory.  Optionally, all lower level files
//          and directories can be deleted.
//
// Ret Val: TRUE : Successfully deleted directory.
//          FALSE : Failed to delete directory.
//
// Example: BOOL bRetVal = fs.DeleteDirectory("c:\\foo\\bar", TRUE);
//
// Notes:   If bDeleteFilesAndDirs == FALSE, directory must be empty or
//          an error will occur and FALSE will be returned.
//
//***************************************************************************
BOOL
SECFileSystem::DeleteDirectory(const CWComString& Directory, const BOOL bDeleteFilesAndDirs /* = FALSE */)
{
  /*if (bDeleteFilesAndDirs)
  {
    CStringList *pSubDirs = GetSubdirList(Directory);
    for (POSITION pos = pSubDirs->GetHeadPosition(); pos != NULL; )
    {
      CComString DirName = pSubDirs->GetNext(pos);
      DeleteDirectory(DirName, bDeleteFilesAndDirs);
    } // for

    pSubDirs = NULL;

    DeleteFiles(AppendWildcard(Directory, "*.*"), (const unsigned long)allfiles);
  } // if

#ifdef WIN32

  BOOL bRetVal = RemoveDirectory((const TCHAR *) Directory);
  return bRetVal;

#else

	int nRetVal = _trmdir((const char *) Directory);
	if (nRetVal == -1)
	{
		return FALSE;
	} // if

	return TRUE;
#endif*/
	return TRUE;
} // DeleteDirectory


//***************************************************************************
//
// Name:    GetDirectorySize
//
// Purpose: To return the size (in bytes) of the files in the specified
//          directory.
//
// Example: LONG lSize = fs.GetDirectorySize("c:\\foo\\bar", "*.*", TRUE);
//
// Notes:   If an error occurs in reading the status of a file, that
//          file is skipped and not counted as part of the size.
//          There is currently not way to find out if an error of this
//          kind occured.
//
//***************************************************************************
LONG
SECFileSystem::GetDirectorySize(const CComString& Directory /* = "" */, const CComString& WildCard /* = "*.*" */, const BOOL bRecurseSubdirs /* = FALSE */)
{
  /*LONG lSize = 0;

  // Do all the subdirectories first...
  if (bRecurseSubdirs)
  {
    CStringList *pSubDirs = GetSubdirList(Directory);
    for (POSITION pos = pSubDirs->GetHeadPosition(); pos != NULL; )
    {
      CComString DirName = pSubDirs->GetNext(pos);
      lSize += GetDirectorySize(DirName, WildCard, bRecurseSubdirs);
    } // for    
    pSubDirs = NULL;
  } // if

  // Find the sizes of all the files in the specified directory.
  CStringList *pFileList = GetFileList(AppendWildcard(Directory, WildCard), 
  									  (const unsigned long)allfiles);
  for (POSITION pos = pFileList->GetHeadPosition(); pos != NULL; )
  {
    CFileStatus status;
    CComString FileName = pFileList->GetNext(pos);
    if (CFile::GetStatus(FileName, status))
    {
      lSize += status.m_size;
    } // if
  } // for  
  pFileList = NULL;
  
  return lSize;*/
	return TRUE;
} // GetDirectorySize



//***************************************************************************
//
// Name:    GetCurrentFileSystem
//
// Purpose: To return a string containing the current file system name.
//
// Example: CComString FileSystem = fs.GetCurrentFileSystem();
//
// Notes:   None
//
//***************************************************************************
CWComString
SECFileSystem::GetCurrentFileSystem()
{
  unsigned int nDrive = 0;
  char cDrive = 'a';

#ifdef WIN32
  nDrive = _getdrive();
#else
	_dos_getdrive(&nDrive);
#endif
  cDrive = (char) ('a' + nDrive - 1);
  CComString String( _T(cDrive), 1);
  String += ":\\";

  USES_CONVERSION;
  return A2W(String.GetBuffer());
} // GetCurrentFileSystem


//***************************************************************************
//
// Name:    ChangeFileSystem
//
// Purpose: To change the current file system.
//
// Example: BOOL bRetVal = fs.ChangeFileSystem('c');
//
// Notes:   Obsolete.  The version that takes a CComString parameter
//          should be used instead.  This change was made in preparation
//          for the addition of Windows NT support.
//
//***************************************************************************
BOOL
SECFileSystem::ChangeFileSystem(const char cFileSystem)
{
  unsigned int nNewDrive;
  BOOL bRetVal = FALSE;

  // Turn off DOS critical error handler.
  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  nNewDrive = _totupper(cFileSystem) - 'A' + 1;

  if (nNewDrive >= 0  &&  nNewDrive <= 26)
  {
#ifdef WIN32

    if (_chdrive(nNewDrive) == 0)
    {
      bRetVal = TRUE;
    } // if

#else
    
		// Attempt change filesystems.
    unsigned int nNumDrives;		// ignored return value
		_dos_setdrive(nNewDrive, &nNumDrives);

		// Make sure the change actually happened (it's the only way).
		unsigned int nCurrentDrive;
		_dos_getdrive(&nCurrentDrive);
		if (nCurrentDrive == nNewDrive)
		{
			bRetVal = TRUE;
		} // if

#endif
  } // if

  // Turn on DOS critical error handler.
  ::SetErrorMode(0);

  return bRetVal;
} // ChangeFileSystem


//***************************************************************************
//
// Name:    ChangeFileSystem
//
// Purpose: To change the current file system.
//
// Example: BOOL bRetVal = fs.ChangeFileSystem("c:\\");
//
// Notes:   Expects a filesystem name of the form "X:\".
//
//***************************************************************************
BOOL
SECFileSystem::ChangeFileSystem(const CWComString& FileSystem)
{
#ifdef WIN32

  return SetCurrentDirectoryW((const WCHAR *) FileSystem);

#else

	BOOL bRetVal = FALSE;

	if (FileSystem.GetLength() > 0)
	{
		USES_CONVERSION;
		wchar_t cFileSystem = W2A(FileSystem.GetBuffer())[0];
		bRetVal = ChangeFileSystem(cFileSystem);
	} // if

	return bRetVal;

#endif
} // ChangeFileSystem


//***************************************************************************
//
// Name:    GetVolumeLabel
//
// Purpose: To get the volume label for a filesystem.
//
// Example: CComString VolumeLabel = fs.GetVolumneLabel("c:\\");
//
// Notes:   None
//
//***************************************************************************
CComString
SECFileSystem::GetVolumeLabel(const CComString& FileSystem)
{
  ::SetErrorMode(SEM_FAILCRITICALERRORS);

#ifdef WIN32

  CComString  VolumeLabel;
  DWORD    dwVolumeSerialNumber;
  DWORD    dwMaximumComponentLength;
  DWORD    dwFileSystemFlags;
  TCHAR    FileSystemName[255];

  if (!GetVolumeInformation((const TCHAR *) FileSystem,
                        VolumeLabel.GetBufferSetLength(255), 255,
                        &dwVolumeSerialNumber,
                        &dwMaximumComponentLength,
                        &dwFileSystemFlags,
                        FileSystemName, 255  ))
  {
    VolumeLabel = _T("");
  } // if

#else

	CComString VolumeLabel = "";
	CComString *pVolLabel = GetDirectoryEntry(AppendWildcard(FileSystem, "*.*"), volume);
	if (pVolLabel != NULL)
	{
		VolumeLabel = *pVolLabel;

		MP_DELETE(pVolLabel);
		pVolLabel = NULL;
	} // if

#endif

  ::SetErrorMode(0);

  return VolumeLabel;
} // GetVolumeLabel


//***************************************************************************
//
// Name:    GetFileSystemType
//
// Purpose: To get the type (removable, network, etc...) of the specified
//          filesystem.
//
// Ret Val: 0 = Error.
//          DRIVE_REMOVABLE : Removable Disk (ie. floppy)
//          DRIVE_FIXED : Non-Removable Disk (ie. hard drive)
//          DRIVE_REMOTE : Network Disk (ie. NFS mounted, Netware volume)
//
// Example: LONG lType = fs.GetFileSystemType("c:\\");
//          assert(lType == DRIVE_FIXED);
//
// Notes:   None
//
//***************************************************************************
LONG
SECFileSystem::GetFileSystemType(const CComString& FileSystem)
{
  LONG lType = DRIVE_UNDETERMINED;

#ifdef WIN32

  lType = GetDriveType((const TCHAR *) FileSystem);

#else

	if (FileSystem.GetLength() > 0)
	{
		char cFileSystem = (char) _totupper(FileSystem[0]);

		int nDriveNum = _totupper(cFileSystem) - 'A';
	
		if (nDriveNum >= 0  &&  nDriveNum <= 26)
		{
			::SetErrorMode(1);
			lType = GetDriveType(nDriveNum);
			::SetErrorMode(0);
		} // if
	} // if

#endif

  return lType;
} // GetFileSystemType


//***************************************************************************
//
// Name:  GetFreeSpace
//
//***************************************************************************
LONG
SECFileSystem::GetFreeSpace(const CComString& FileSystem)
{
#ifdef WIN32

  LONG  lRetVal;
  DWORD dwSectorsPerCluster;
  DWORD dwBytesPerSector;
  DWORD dwFreeClusters;
  DWORD dwClusters;

  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  if (GetDiskFreeSpace((const TCHAR *) FileSystem,
      &dwSectorsPerCluster,
      &dwBytesPerSector,
      &dwFreeClusters,
      &dwClusters))
  {
    lRetVal = dwSectorsPerCluster * dwBytesPerSector * dwFreeClusters;
  } // if
  else
  {
    lRetVal = -1;
  } // else

  ::SetErrorMode(0);

  return lRetVal;

#else

	unsigned long lFreeSpace = -1;
	struct _diskfree_t diskspace;

	if (FileSystem.GetLength() > 0)
	{
		char cFileSystem = (char) _totupper(FileSystem[0]);

		int nDriveNum = _totupper(cFileSystem) - 'A';
	
		if (nDriveNum >= 0  &&  nDriveNum <= 26)
		{
			::SetErrorMode(1);
			if	(_dos_getdiskfree(	nDriveNum + 1,	&diskspace	) == 0)
			{
					lFreeSpace =(unsigned long) diskspace.avail_clusters *
											(unsigned long) diskspace.sectors_per_cluster	*
											(unsigned long) diskspace.bytes_per_sector;
			} // if
			::SetErrorMode(0);
		} // if
	} // if

	return lFreeSpace;

#endif
} // GetFreeSpace


//***************************************************************************
//
// Name:  GetTotalSpace
//
//***************************************************************************
LONG
SECFileSystem::GetTotalSpace(const CComString& FileSystem)
{
#ifdef WIN32

  LONG  lRetVal;
  DWORD dwSectorsPerCluster;
  DWORD dwBytesPerSector;
  DWORD dwFreeClusters;
  DWORD dwClusters;

  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  if (GetDiskFreeSpace((const TCHAR *) FileSystem,
      &dwSectorsPerCluster,
      &dwBytesPerSector,
      &dwFreeClusters,
      &dwClusters))
  {
    lRetVal = dwSectorsPerCluster * dwBytesPerSector * dwClusters;
  } // if
  else
  {
    lRetVal = -1L;
  } // else

  ::SetErrorMode(0);

  return lRetVal;

#else

	unsigned long lTotalSpace = -1;
	struct _diskfree_t diskspace;

	if (FileSystem.GetLength() > 0)
	{
		char cFileSystem = (char) _totupper(FileSystem[0]);

		int nDriveNum = _totupper(cFileSystem) - 'A';
	
		if (nDriveNum >= 0  &&  nDriveNum <= 26)
		{
			::SetErrorMode(1);
			if	(_dos_getdiskfree(	nDriveNum + 1,	&diskspace	) == 0)
			{
					lTotalSpace =(unsigned long) diskspace.total_clusters *
											(unsigned long) diskspace.sectors_per_cluster	*
											(unsigned long) diskspace.bytes_per_sector;
			} // if
			::SetErrorMode(0);
		} // if
	} // if

	return lTotalSpace;

#endif
} // GetTotalSpace


//***************************************************************************
//
// Name:  IsReadOnlyFileSystem
//
//***************************************************************************
BOOL
SECFileSystem::IsReadOnlyFileSystem(const CComString& FileSystem)
{
/*  BOOL            bRetVal = TRUE;
  CFile           TestFile;
  CFileException  fileException;
  CComString         TempFileName;

  // Turn off DOS critical error handler.
  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  // Get the temp file name
  TempFileName = "xxxxtest.999";

  // Try to create the temporary file.
  if (TestFile.Open(AppendWildcard(FileSystem, TempFileName), CFile::modeCreate, &fileException))
  {
    TestFile.Close();
    if (DeleteFile(AppendWildcard(FileSystem, TempFileName)))
    {
      bRetVal = FALSE;
    } // else
  } // if

  // Turn on DOS critical error handler.
  ::SetErrorMode(0);

  return bRetVal;*/
	return TRUE;
} // IsReadOnlyFileSystem


//***************************************************************************
//
// Name:    RenameFile
//
// Purpose: To rename a file.
//
// Ret Val: TRUE : No error, or OldFileName == NewFileName.
//          FALSE : Error.
//
// Example: BOOL bRetVal = fs.RenameFile("c:\\foo.txt", "c:\\bar.doc");
//          BOOL bRetVal = fs.RenameFile("c:\\foo.txt", "c:\\what\\bar.doc");
//
// Notes:   A file can also be moved using this function by specifying a
//          different directory in the NewFileName.
//
//***************************************************************************
BOOL
SECFileSystem::RenameFile(const CComString& OldFileName, const CComString& NewFileName)
{
/*  if (OldFileName == NewFileName)
  {
    return TRUE;
  } // if

  if (!FileExists(OldFileName)  ||  FileExists(NewFileName))
  {
    return FALSE;
  } // if

  TRY
  {
    CFile::Rename((const TCHAR *) OldFileName, (const TCHAR *) NewFileName);
  } // TRY
  CATCH(CFileException, Exception)
  {
    return FALSE;
  } // CATCH
  END_CATCH
*/
  return TRUE;
} // RenameFile


//***************************************************************************
//
// Name:    DeleteFiles
//
// Purpose: To delete a set of files based on a wildcard file specification.
//
// Example: BOOL bRetVal = fs.DeleteFiles("c:\\foo\\bar\\*.txt");
//
//***************************************************************************
BOOL
SECFileSystem::DeleteFiles(const CComString& FileSpec, const unsigned long eFileAttrib /* = normal */)
{
/*  BOOL bRetVal = TRUE;
  CStringList *pDir = GetDirectory(FileSpec, eFileAttrib);

  for (POSITION pos = pDir->GetHeadPosition(); pos != NULL; )
  {
    CComString FileName = pDir->GetNext(pos);
    if (DeleteFile(FileName) == FALSE)
    {
      bRetVal = FALSE;
    } // if
  } // for

  // Clean up.
  pDir = NULL;

  return bRetVal;*/
	return TRUE;
} // DeleteFiles


//***************************************************************************
//
// Name:    DeleteFile
//
// Purpose: To delete a file.
//
// Example:
//          BOOL bRetVal = fs.DeleteFile("c:\foo.txt");
//
// Notes:   None
//
//***************************************************************************
BOOL
SECFileSystem::DeleteFile(const CComString& FileName)
{
/*  TRY
  {
    CFile::Remove((const TCHAR *) FileName);
  } // TRY
  CATCH(CFileException, Exception)
  {
    return FALSE;
  } // CATCH
  END_CATCH
*/
  return TRUE;
} // DeleteFile


//***************************************************************************
//
// Name:    CloseFile
//
// Purpose: To close a file.
//
// Ret Val: TRUE : File was closed successfully.
//          FALSE : Error occured.
//
// Notes:   None
//
//***************************************************************************
/*BOOL
SECFileSystem::CloseFile(CFile *pFile) const
{
  BOOL bRetVal = TRUE;

  TRY
  {
    pFile->Close();
  } // TRY
  CATCH(CFileException, e)
  {
    bRetVal = FALSE;
  } // CATCH
  END_CATCH

  return bRetVal;
} // CloseFile
*/

//***************************************************************************
//
// Name:    CopyFile
//
// Purpose: To copy a file.
//
// Ret Val: TRUE : File was copied successfully.
//          FALSE : Error occured.
//
// Example: BOOL bRetVal = fs.CopyFile("foo.txt", "a:\\bar.txt", 20480);
//
// Notes:   If the destination file exists, an error occurs.
//
//***************************************************************************
BOOL
SECFileSystem::CopyFile(const CComString& SourceFileName, const CComString& DestFileName, const unsigned long lBuffSize /* = 10240 */)
{
  BOOL    bRetVal  = TRUE;

  return bRetVal;
} // CopyFile


//***************************************************************************
//
// Name:    CopyFiles
//
// Purpose: To copy a set of files based on a wildcard file specification.
//
// Example: BOOL bRetVal = fs.CopyFiles("c:\\foo\\bar\\*.txt", "c:\\foo2");
//
// Notes:   If an error occurs, the copy is aborted.  There is no way to
//          which if any of the files were copied.
//
//          Even if you are ignoring errors, the return value indicates
//          whether an error occured.
//
//***************************************************************************
BOOL
SECFileSystem::CopyFiles(const CComString& FileSpec, const CComString& DestPath, const BOOL bIgnoreErrors /* = FALSE */, const unsigned long eFileAttrib /* = normal */)
{
  BOOL bRetVal = TRUE;
/*  CStringList *pDir = GetDirectory(FileSpec, eFileAttrib);

  for (POSITION pos = pDir->GetHeadPosition(); pos != NULL; )
  {
    CComString FileName = pDir->GetNext(pos);
    CComString DestFileName = AppendWildcard(DestPath, GetFileName(FileName));

    if (CopyFile(FileName, DestFileName) == FALSE)
    {
      bRetVal = FALSE;

      if (bIgnoreErrors == FALSE)
      {
        break;
      } // if
    } // if
  } // for

  // Clean up.
  pDir = NULL;
*/
  return bRetVal;
} // CopyFiles


//***************************************************************************
//
// Name:    CompareFiles
//
// Purpose: To compare the contents of two files to see if they are the
//          same.
//
// Ret Val: TRUE, if the files are the same.
//          FALSE, if the files are different, or an error occurs.
//
// Example: BOOL bRetVal = fs.CompareFiles("foo.txt", "bar.txt", 20480);
//
// Notes:   None
//
//***************************************************************************
BOOL
SECFileSystem::CompareFiles(const CComString& FileName1, const CComString& FileName2, const unsigned long lBuffSize /* = 10240 */)
{
  BOOL    bRetVal  = TRUE;

  return bRetVal;
}


//***************************************************************************
//
// Name:    CompareDirectories
//
//***************************************************************************
BOOL
SECFileSystem::CompareDirectories(const CComString& PathName1, const CComString& PathName2, const BOOL bRecurseSubdirs /* = FALSE */, const BOOL bCompareFileContents /* = FALSE */, const unsigned long eFileAttrib /* = normal */)
{
  BOOL bRetVal = TRUE;
/*

  // See if the two pathnames are the same.

  if ( PathName1 == PathName2 )
  {
    return TRUE;
  } // if


  // Get the first directory listing.

  CComString CurrentDirectory = GetCurrentDirectory();
  if (ChangeDirectory(GetPath(PathName1)) == 0)
  {
    return FALSE;
  } // if
  CStringList *pDir1 = GetDirectory(GetFileName(PathName1), eFileAttrib, bRecurseSubdirs);
  if (ChangeDirectory(CurrentDirectory) == 0)
  {
      pDir1 = NULL;
    return FALSE;
  } // if


  // Get the second directory listing.

  if (ChangeDirectory(GetPath(PathName2)) == 0)
  {
      
      pDir1 = NULL;
    return FALSE;
  } // if
  CStringList *pDir2 = GetDirectory(GetFileName(PathName2), eFileAttrib, bRecurseSubdirs);
  if (ChangeDirectory(CurrentDirectory) == 0)
  {
      
      pDir1 = NULL;
      
      
      pDir2 = NULL;
     
    return FALSE;
  } // if


  // Compare the directory listings.

  if (pDir1->GetCount() != pDir2->GetCount())
  {
    bRetVal = FALSE;
  } // if
  else
  {
    Sort(pDir1);
    Sort(pDir2);

    POSITION pos2 = pDir2->GetHeadPosition();
    for (POSITION pos1 = pDir1->GetHeadPosition(); pos1 != NULL; )
    {
      CComString String1 = pDir1->GetNext(pos1);
      CComString String2 = pDir2->GetNext(pos2);
      TRACE(_T("SECFileSystem::CompareDirectories %s %s\n"), (const TCHAR *) String1, (const TCHAR *)String2);

      if (String1 != String2)
      {
        bRetVal = FALSE;
        break;
      } // if
      else
      {
        if (bCompareFileContents)
        {
          if (!CompareFiles(AppendWildcard(GetPath(PathName1), String1), AppendWildcard(GetPath(PathName2), String2)))
          {
            bRetVal = FALSE;
            break;
          } // if
        } // if
      } // else
    } // for
  } // else


  // Clean up.
  
  pDir1 = NULL;

  
  pDir2 = NULL;
*/
  return bRetVal;
} // CompareDirectories


//***************************************************************************
//
// Name:    GetFullPathName
//
// Example:  fs.GetFullPathName("c:\\foo\\bar\\what.txt");
//           fs.GetFullPathName("c:\\foo\\..\\bar\\what.txt");
//           fs.GetFullPathName("\\bar\\..\\what.txt");
//           fs.GetFullPathName("..\\bar\\.\\what.txt");
//           fs.GetFullPathName("c:..\\foo\\bar\\what.txt");
//
//***************************************************************************
CWComString
SECFileSystem::GetFullPathName(const CWComString& PathAndFileName)
{
#ifdef WIN32

  LPWSTR pTemp;
  CWComString FullPathName;

  ::GetFullPathNameW((const WCHAR *)PathAndFileName,
                    m_nMaxFileNameLength,
                    FullPathName.GetBufferSetLength(m_nMaxFileNameLength),
                    &pTemp);

  //FullPathName.ReleaseBuffer(-1);

  return FullPathName;

#else

	CWComString FullPathName;

	// Determine the drive.
	CWComString FileSystem = GetFileSystem(PathAndFileName);
	if (FileSystem == L"")
	{
		FileSystem = GetCurrentFileSystem();
	} // if

	// Determine if the path is relative to the root of the filesystem
	// or relative to the current directory of the filesystem.
	int nIndex = PathAndFileName.Find(L":\\");
	if (nIndex != -1)
	{
		FullPathName = FileSystem;
	} // if
	else if (PathAndFileName.GetLength()  &&  PathAndFileName[0] == L'\\')
	{
		FullPathName = FileSystem;
	} // else if
	else
	{
		FullPathName = GetCurrentDirectory(FileSystem) + L"\\";
	} // else

	CWComString PartialPath = GetPath(PathAndFileName + L"\\", FALSE);
	if (PartialPath[0] == L'\\')
	{
		PartialPath = PartialPath.Right(PartialPath.GetLength() - 1);
	} // if
	if (PartialPath.GetLength())
	{
		PartialPath = PartialPath.Left(PartialPath.GetLength() - 1);
	} // if

	FullPathName = FullPathName + PartialPath;


	// Change all the slash-dot-slash sequences to slashes.

	BOOL bDone = FALSE;
	while (!bDone)
	{
		int nIndex = FullPathName.Find(L"\\.\\");
		if (nIndex == -1)
		{
			bDone = TRUE;
		} // if
		else
		{
			FullPathName = FullPathName.Left(nIndex) + FullPathName.Right(FullPathName.GetLength() - nIndex - 2);
		} // else
	} // while


	// Handle all the slash-dot-dot-slash sequences.

	bDone = FALSE;
	while (!bDone)
	{
		int nIndex = FullPathName.Find(L"\\..\\");
		if (nIndex == -1)
		{
			bDone = TRUE;
		} // if
		else
		{
#if 0
			FullPathName = L"";
#else
			PartialPath = FullPathName.Left(nIndex);
			int nIndex2 = PartialPath.ReverseFind(L'\\');
			if (nIndex2 == -1)
			{
				FullPathName = "";
			} // if
			else
			{
				FullPathName = PartialPath.Left(nIndex2) + FullPathName.Right(FullPathName.GetLength() - nIndex - 3);
			} // else
#endif
		} // else
	} // while

	return FullPathName;

#endif
} // GetFullPathName


//***************************************************************************
//
// Name:    GetFileName
//
// Purpose:  Extract a file name from a path.
//
// Example:  CComString FileName = fs.GetFileName("c:\\foo\\bar\\what.txt");
//          assert(FileName == "what.txt");
//
// Notes:    None
//
//***************************************************************************
CWComString
SECFileSystem::GetFileName(const CWComString& PathAndFileName)
{
  CWComString FileName = PathAndFileName;  // Copy to make modifications.

  // Find the last "\" in the string and return everything after it.
  /*int nIndex = FileName.Find('\\');
  while(nIndex != -1)
  {
    FileName = FileName.Right(FileName.GetLength() - nIndex - 1);

    nIndex = FileName.Find('\\');
  } // while*/
  int nIndex = FileName.ReverseFind(L'\\');
  if( nIndex != -1)
	  FileName = FileName.Right(FileName.GetLength() - nIndex - 1);

  return FileName;
} // GetFileName


//***************************************************************************
//
// Name:    GetPath
//
// Purpose:  To return the directory from a path.
//
// Example:  CWComString Path = fs.GetPath(L"c:\\foo\\bar\\what.txt");
//          assert(Path == L"c:\\foo\\bar\\");
//
// Notes:    None
//
//***************************************************************************

CWComString
SECFileSystem::GetPath(const CWComString& PathAndFileName, const BOOL bIncludeFileSystem /* = TRUE */)
{
	CWComString FileName = PathAndFileName; // Copy to modify;
	CWComString  Path = L"";

	// Find the last "\" in the string and return everything up to and including it.
	wchar_t	delimChar = L'\\';
	int nIndex = FileName.Find(delimChar);
	if( nIndex == -1)
	{
		delimChar = L'/';
		nIndex = FileName.Find(delimChar);
	}
	while(nIndex != -1)
	{
		Path = Path + FileName.Left(nIndex + 1);
		FileName = FileName.Right(FileName.GetLength() - nIndex - 1);

		nIndex = FileName.Find(delimChar);
	} // while


	if (!bIncludeFileSystem)
	{
		// Remove the filesystem name from the front of the path.
		int nIndex = Path.Find(L':');
		if (nIndex != -1)
		{
			Path = Path.Right(Path.GetLength() - nIndex - 1);
		} // if
	} // if

	return Path;
} // GetPath


//***************************************************************************
//
// Name:    GetExtension
//
// Purpose:  To return the extension from a path.
//
// Example:  CComString Path = fs.GetExtension("c:\\foo\\bar\\what.txt");
//          assert(Path == "txt");
//          CComString Path = fs.GetExtension("c:\\foo\\bar\\what.txt.john");
//          assert(Path == "john");
//
// Notes:    None
//
//***************************************************************************
CWComString
SECFileSystem::GetExtension(const CWComString& PathAndFileName)
{
  // Find the last "." in the string and return everything after it.
  int nIndex = PathAndFileName.ReverseFind(L'.');
  CWComString strExt;

  if (nIndex > 0 && nIndex < (PathAndFileName.GetLength() - 1))
	  strExt = PathAndFileName.Right(PathAndFileName.GetLength() - nIndex - 1);
  else
	  strExt.Empty();

  return strExt;
} // GetExtension


//***************************************************************************
//
// Name:    GetFileSystem
//
// Purpose:  To return the filesystem name from the given path.
//
// Example:  CComString Path = fs.GetFileSystem("c:\\foo\\bar\\what.txt");
//          assert(Path == "c:\\");
//          CComString Path = fs.GetFileSystem("c:");
//          assert(Path == "c:");
//          CComString Path = fs.GetFileSystem("\\foo\\bar\\what.txt");
//          assert(Path == "");
//
// Notes:    None
//
//***************************************************************************
CWComString
SECFileSystem::GetFileSystem(const CWComString& PathAndFileName)
{
  CWComString FileSystem;

  // Find the last "." in the string and return everything after it.
  CWComString	_PathAndFileName = PathAndFileName;
  int nIndex = _PathAndFileName.Find(L":\\");
  if (nIndex != -1)
  {
    FileSystem = PathAndFileName.Left(nIndex+2);
  } // if
  else
  {
    nIndex = PathAndFileName.Find(L':');
    if (nIndex != -1)
    {
      FileSystem = PathAndFileName.Left(nIndex+1);
    } // if
  } // else

  return FileSystem;
} // GetFileSystem


//***************************************************************************
//
// Name:    GetBaseFileName
//
// Purpose:  To return the filename (without extension) given a path.
//
// Example:  CComString Path = fs.GetBaseFileName("c:\\foo\\bar\\what.txt");
//          assert(Path == "what");
//          CComString Path = fs.GetBaseFileName("what.txt");
//          assert(Path == "what");
//
// Notes:    None
//
//***************************************************************************
CWComString
SECFileSystem::GetBaseFileName(const CWComString& PathAndFileName)
{
  CWComString FileName = GetFileName(PathAndFileName);
  CWComString Ext = GetExtension(FileName);

  if (FileName.Find(L'.') != -1)
	return FileName.Left(FileName.GetLength() - Ext.GetLength() - 1);
  else
	return FileName;
} // GetBaseFileName


//***************************************************************************
//
// Name:    FileExists
//
// Purpose:  To determine if a file exists.
//
// Example:
//          BOOL bExists = fs.FileExists("c:\\foo\\bar\\what.txt");
//
// Notes:    A directory is considered a file for purposes of existance.
//
//***************************************************************************
BOOL
SECFileSystem::FileExists(const CComString& PathAndFileName)
{
/*  CFileStatus FileStatus;

  if (CFile::GetStatus(PathAndFileName, FileStatus) == TRUE)
  {
    return TRUE;
  } // if
  else
  {
    return FALSE;
  } // if*/
	return FALSE;
} // FileExists


//***************************************************************************
//
// Name:    FileSystemExists
//
// Purpose:  To determine if a filesystem exists.
//
// Example:
//          BOOL bExists = fs.FileSystemExists("c:\\");
//
// Notes:    None.
//
//***************************************************************************
BOOL
SECFileSystem::FileSystemExists(const CComString& FileSystemName)
{
  CComString TempFSName = FileSystemName;  // cast away the const.
  TempFSName.MakeLower();

  BOOL bRetVal = FALSE;
 /* CStringList *pFSList = GetFileSystemList();
  for (POSITION pos = pFSList->GetHeadPosition(); pos != NULL; )
  {
    CComString FSName = pFSList->GetNext(pos);
    if (FSName == TempFSName)
    {
      bRetVal = TRUE;
      break;
    } // if
  } // for

  
  pFSList = NULL;
/*/
  return bRetVal;
} // FileSystemExists


//***************************************************************************
//
// Name:    DirectoryExists
//
// Purpose: To determine if a directory exists.
//
// Example:
//          BOOL bExists = fs.DirectoryExists("c:\\temp");
//
// Notes:   None.
//
//***************************************************************************
BOOL
SECFileSystem::DirectoryExists(const CComString& Path)
{
#ifdef WIN32
  DWORD dwRetVal = GetFileAttributes( Path );
  if ( dwRetVal == 0xFFFFFFFF )
    return FALSE;
  else
    return TRUE;
#else
  CFileStatus FileStatus;

  if (CFile::GetStatus(Path, FileStatus) == TRUE  &&
      FileStatus.m_attribute & directory)
  {
    return TRUE;
  } // if
  else if (FileSystemExists(GetFullPathName(Path)))
  {
    return TRUE;
  } // else if

  return FALSE;
#endif
} // DirectoryExists


//***************************************************************************
//
// Name:    Sort
//
// Purpose:  To sort the give string list.
//
// Exmaple:
//          fs.Sort(pDirList);
//
// Notes:    None
//
//***************************************************************************
/*void
SECFileSystem::Sort(CStringList *pStringList,BOOL bCase)
{
  int nListSize = pStringList->GetCount();
  for (int i = 0; i < nListSize-1; i++)
  {
    CComString String1 = pStringList->GetAt(pStringList->FindIndex(i));
    for (int j = i+1; j < nListSize; j++)
    {
      CComString String2 = pStringList->GetAt(pStringList->FindIndex(j));
      
	  if( bCase )
	  {
		
		  if (String1 > String2)
		  {
			  // Swap.
			  pStringList->SetAt(pStringList->FindIndex(i), String2);
			  pStringList->SetAt(pStringList->FindIndex(j), String1);
			  String1 = String2;
		  }
      }
	  else
	  {
		  if( String1.CompareNoCase(String2) == 1 )
		  {
  			  pStringList->SetAt(pStringList->FindIndex(i), String2);
			  pStringList->SetAt(pStringList->FindIndex(j), String1);
			  String1 = String2;
		  }
	  }//else
    } // for
  } // for
} // Sort
*/

//***************************************************************************
//
// Name:    LoadListBox
//
// Purpose:  To load a list box with the given string list.
//
// Exmaple:
//          fs.LoadListBox(pListBox, pDirList);
//
// Notes:    None
//
//***************************************************************************
/*void
SECFileSystem::LoadListBox(CListBox *pListBox, const CStringList * pStringList)
{
  CComString String;

  for (POSITION pos=pStringList->GetHeadPosition(); pos != 0; )
  {
    String = pStringList->GetNext(pos);
    pListBox->AddString(String);
  } // for
} // LoadListBox


//***************************************************************************
//
// Name:    LoadComboBox
//
// Purpose:  To load a combo box with the given string list.
//
// Example:
//          fs.LoadListBox(pComboBox1, pDirList);
//
// Notes:    None
//
//***************************************************************************
void
SECFileSystem::LoadComboBox(CComboBox *pComboBox, const CStringList * pStringList)
{
  CComString String;

  for (POSITION pos=pStringList->GetHeadPosition(); pos != 0; )
  {
    String = pStringList->GetNext(pos);
    pComboBox->AddString(String);
  } // for
} // LoadComboBox
*/

//***************************************************************************
//
// Name:    AppendWildcard
//
// Purpose:  To append a wildcard to a path.  It takes into account
//          whether the path has a backslash at the end.
//
// Example:
//          CComString foo = fs.AppendWildcard("c:\\foo\\bar", "*.txt");
//          assert(foo == "c:\\foo\\bar\\*.txt");
//          CComString foo = fs.AppendWildcard("c:\\foo\\bar\\", "*.txt");
//          assert(foo == "c:\\foo\\bar\\*.txt");
//
// Notes:    If the path is the empty string, the Wildcard is returned.
//
//***************************************************************************

CWComString
SECFileSystem::AppendWildcard(const CWComString& Path, const CWComString& Wildcard)
{
	int nLength = Path.GetLength();
	CWComString RetVal;

	if (nLength == 0  ||  ((CWComString)Path).GetAt(nLength - 1) == L'\\')
	{
		RetVal = Path + Wildcard;
	} // if
	else
	{
		RetVal = Path + L"\\" + Wildcard;
	} // else

	return RetVal;
} // AppendWildcard

int SECFileSystem::GetSystemVersion()
{
   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   //
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) ){
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.

      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         return SYSVER_UNKNOWN;
   }

   switch (osvi.dwPlatformId){
      case VER_PLATFORM_WIN32_NT:
         if ( osvi.dwMajorVersion <= 4 )
			 return SYSVER_NT;
         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			return SYSVER_2000;
         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			return SYSVER_2000;		// "Whistler"
         break;
      case VER_PLATFORM_WIN32_WINDOWS:
         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
			 return SYSVER_95;
         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
			 return SYSVER_98;
         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			 return SYSVER_ME;
         break;
      case VER_PLATFORM_WIN32s:
//         printf ("Microsoft Win32s ");
         break;
   }
   return SYSVER_UNKNOWN;
}

CComString	CorrectFilename(LPCSTR fileName)
{
	if(!fileName)	return "";
	CComString ret( fileName);
	ret.Replace("//", "/");
	ret.Replace("\\\\", "\\");
	ret.MakeLower();
	return ret;
}

CWComString	CorrectFilename(LPCWSTR fileName)
{
	if(!fileName)	return L"";
	CWComString ret( fileName);
	ret.Replace(L"//", L"/");
	ret.Replace(L"\\\\", L"\\");
	ret.MakeLower();
	return ret;
}

CWComString SECFileSystem::AppendPaths(CWComString Path1, CWComString Path2)
{
	Path1 = CorrectFilename(Path1);
	Path2 = CorrectFilename(Path2);

	int nLength = Path1.GetLength();
	if (nLength != 0  &&  Path1.GetBuffer()[nLength - 1] == L'\\' || Path1.GetBuffer()[nLength - 1] == L'/')
		Path1 = Path1.Left(Path1.GetLength()-1);

	while( Path1 != L"")
	{
		int dotsIndex = Path2.Find(L"..\\");
		if( dotsIndex == -1)
			dotsIndex = Path2.Find(L"../");
		if( dotsIndex != -1 && !Path1.IsEmpty())
		{
			int i = Path1.ReverseFind(L'\\');
			if( i == -1)	i = Path1.ReverseFind(L'/');
			if( i == -1)
			{
				Path1 = L"";
			}
			else
			{
				Path1 = Path1.Left(i);
			}
			Path2 = Path2.Right(Path2.GetLength()-(dotsIndex+2)-1);
		}else
			break;
	}

	Path1 = AppendWildcard( Path1, Path2);
	return Path1;
}