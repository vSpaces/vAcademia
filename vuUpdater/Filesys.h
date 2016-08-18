// Stringray Software Extension Classes
// Copyright (C) 1995 Stingray Software Inc,
// All rights reserved.
//
// SEC FileSystem file format header
//
// This source code is only intended as a supplement to the
// Stingray Extension Classes
// See the Stingray Extension Classes help files for
// detailed information regarding using SEC classes.
//
// CHANGELOG:
//
//    SPL       11/30/95 Started
//
//
//

#ifndef __SECFILESYSTEM_H__
#define __SECFILESYSTEM_H__


//
// SEC Extension DLL
// Initialize declaration context
//

#ifdef WIN32

#include <direct.h>
#include "Comstring.h"
// winbase.h defines this to be (0x100000L) for some reason.
#ifdef GetFreeSpace
#undef GetFreeSpace
#endif

#else

#include "dos.h"
#include "direct.h"

#endif

#define SYSVER_UNKNOWN	0
#define SYSVER_NT_2000	0x10
#define SYSVER_9598ME	0x20
#define SYSVER_NT		(1|SYSVER_NT_2000)
#define SYSVER_2000		(2|SYSVER_NT_2000)
#define SYSVER_95		(1|SYSVER_9598ME)
#define SYSVER_98		(2|SYSVER_9598ME)
#define SYSVER_ME		(3|SYSVER_9598ME)

//***************************************************************************
// Definitions
//***************************************************************************
#define DRIVE_UNDETERMINED 0


class SECFileSystem
{

// Constructors
public:
  SECFileSystem();        // Constructor

// Attributes
private:

#ifdef WIN32

  WIN32_FIND_DATA  m_FileInfo;
  HANDLE           m_hFind;

#else

	struct _find_t	m_FileInfo;

#endif

  int              m_nMaxFileNameLength;


// Implementation
public:
	CComString AppendPaths(CComString Path1, CComString Path2);
  virtual ~SECFileSystem();        // Destructor

  // File/Directory Attributes

#ifndef WIN32
#define FILE_ATTRIBUTE_NORMAL CFile::normal
#endif

  enum Attribute {
    normal    = FILE_ATTRIBUTE_NORMAL,  // Normal Files (Win32 API version 0x80)
    readOnly  = FILE_ATTRIBUTE_READONLY,        // Read-Only Files
    hidden    = FILE_ATTRIBUTE_HIDDEN,          // Hidden Files
    system    = FILE_ATTRIBUTE_SYSTEM,          // System Files
    volume    = 0x08,          // Volume Label
    directory  = 0x10,      // Subdirectory Entry
    archive    = FILE_ATTRIBUTE_ARCHIVE,        // Archive File

    allfiles  = 0xFFFFFFFF,
  }; // Attribute


// Operations

  // Operations - Directory
  CComString       GetCurrentDirectory(const CComString& FileSystem = "");
  BOOL          ChangeDirectory(const CComString& NewDirectory);
  BOOL          RenameDirectory(const CComString& OldName, const CComString& NewName);
  BOOL          MakeDirectory(const CComString& NewDirectory);
  BOOL          MakePath(const CComString& NewDirectory);
  BOOL          DeleteDirectory(const CComString& Directory, const BOOL bDeleteFilesAndDirs = FALSE);

  LONG          GetDirectorySize(const CComString& Directory = "", const CComString& WildCard = "*.*", const BOOL bRecurseSubdirs = FALSE);

  // Operations - FileSystem

  CComString       GetCurrentFileSystem();
  BOOL          ChangeFileSystem(const CComString& FileSystem);
  //CStringList * GetFileSystemList();
  CComString       GetVolumeLabel(const CComString& FileSystem);
  LONG          GetFileSystemType(const CComString& FileSystem);
  BOOL          IsReadOnlyFileSystem(const CComString& FileSystem);
  LONG          GetFreeSpace(const CComString& FileSystem);
  LONG          GetTotalSpace(const CComString& FileSystem);

  // Operations - File

  /*BOOL          GetFileStatus(const CComString& FileName, CFileStatus& FileStatus);
  BOOL          GetFileCreateTime(const CComString& FileName, CTime& time);
  BOOL          GetFileModifyTime(const CComString& FileName, CTime& time);
  BOOL          GetFileAccessTime(const CComString& FileName, CTime& time);
  BOOL          GetFileSize(const CComString& FileName, unsigned long& lSize);
  BOOL          GetFileAttribute(const CComString& FileName, BYTE& Attribute);*/

  BOOL          RenameFile(const CComString& OldFileName, const CComString& NewFileName);
  BOOL          DeleteFile(const CComString& FileName);
  BOOL          DeleteFiles(const CComString& FileSpec, const unsigned long eFileAttrib = normal);

  BOOL          CopyFile(const CComString& SourceFileName, const CComString& DestFileName, const unsigned long lBuffSize = 10240);
  BOOL          CopyFiles(const CComString& FileSpec, const CComString& DestPath, const BOOL bIgnoreErrors = FALSE, const unsigned long eFileAttrib = normal);

  // Operations - Comparison

  BOOL          CompareFiles(const CComString& FileName1, const CComString& FileName2, const unsigned long lBuffSize = 10240);
  BOOL          CompareDirectories(const CComString& PathName1, const CComString& PathName2, const BOOL bRecurseSubdirs = FALSE, const BOOL bCompareFileContents = FALSE, const unsigned long eFileAttrib = normal);

  // Operations - Existance
  
  BOOL          FileExists(const CComString& PathAndFileName);
  BOOL          FileSystemExists(const CComString& FileSystemName);
  BOOL          DirectoryExists(const CComString& Path);

  // Operations - File/Path Name

  CComString       GetFullPathName(const CComString& PathAndFileName);
  CComString       GetFileName(const CComString& PathAndFileName);
  CComString       GetPath(const CComString& PathAndFileName, const BOOL bIncludeFileSystem = TRUE);
  CComString       GetExtension(const CComString& PathAndFileName);
  CComString       GetFileSystem(const CComString& PathAndFileName);
  CComString       GetBaseFileName(const CComString& PathAndFileName);

  CComString       AppendWildcard(const CComString& Path, const CComString& Wildcard);

  // Operations - Directory Listing

  /*CStringList * GetDirectory(const CComString& SearchString, const unsigned long eFileAttrib, const BOOL bRecurseSubDirs=FALSE, CStringList *pStringList = NULL);

  CStringList * GetSubdirList(const CComString& SearchDir, const BOOL bPathInName = TRUE);
  CStringList * GetFileList(const CComString& SearchString, const unsigned long eFileAttrib);*/

  CComString *     GetDirectoryEntry(const CComString& Wildcard = "", const unsigned long eFileAttrib = normal);


  // Operations - MFC CStringList

  //static void   Sort(CStringList *pStringList, BOOL bCase = TRUE);

  // Operations - MFC GUI

  //static void   LoadListBox(CListBox *pListBox, const CStringList * pStringList);
  //static void   LoadComboBox(CComboBox *pComboBox, const CStringList * pStringList);

	static int GetSystemVersion();

// Overridables


// Implementation

protected:
  BOOL          ChangeFileSystem(const char cFileSystem);
  //BOOL          CloseFile(CFile *pFile) const;
  BOOL          GetADirectoryEntry(CComString &fileName, const CComString& Wildcard = "", const unsigned long eFileAttrib = normal);

}; // SECFileSystem


#endif // _SECFILESYSTEM_H_

