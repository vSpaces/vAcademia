// TestUnzip32lib.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "..\lib\unzip32.h"


int WINAPI unzPrint(LPSTR apsz, unsigned long aul){
	// ??
	return 0;
}

int WINAPI unzReplace(LPSTR apsz){
	// ??
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

//int ExtractOnlyNewer;   = true for "update" without interaction
//                          (extract only newer/new files, without queries)
//int SpaceToUnderscore;  = true if convert space to underscore
//int PromptToOverwrite;  = true if prompt to overwrite is wanted
//int fQuiet;             = quiet flag. 1 = few messages, 2 = no messages, 0 = all messages
//int ncflag              = write to stdout if true
//int ntflag              = test zip file
//int nvflag              = verbose listing
//int nfflag              = "freshen" (replace existing files by newer versions)
//int nzflag              = display zip file comment
//int ndflag              = all args are files/dir to be extracted
//int noflag              = true if you are to always over-write files, false if not
//int naflag              = do end-of-line translation
//int nZIflag;            = get zip info if true
//int C_flag;             = be case insensitive if TRUE
//int fPrivilege          = 1 => restore Acl's, 2 => Use privileges
//LPSTR lpszZipFN         = zip file name
//LPSTR lpszExtractDir    = Directory to extract to. This should be NULL if you
//                          are extracting to the current directory.
	DCL dcl;
	memset(&dcl,0,sizeof(DCL));
	dcl.noflag=true; // всегда перезаписывать файлы
	dcl.ndflag=true; // создавать поддиректории
	dcl.lpszZipFN = "D:\\User\\BDima\\Programming\\cscl\\ComMan\\cache\\_projects_cscllan_projects_hotel_hotel.zip";//lpCmdLine;
	dcl.lpszExtractDir = "D:\\User\\BDima\\Programming\\cscl\\ComMan\\cache\\hotel";

//typedef struct {
//DLLPRNT *print;         = a pointer to the application's print routine.
//DLLSND *sound;          = a pointer to the application's sound routine. This
//                          can be NULL if your application doesn't use
//                          sound.
//DLLREPLACE *replace     = a pointer to the application's replace routine.
//DLLPASSWORD *password   = a pointer to the application's password routine.
//DLLMESSAGE *SendApplicationMessage = a pointer to the application's routine
//                          for displaying information about specific files
//                          in the archive. Used for listing the contents of
//                          an archive.
//DLLSERVICE *ServCallBk  = Callback function designed to be used for
//                          allowing the application to process Windows messages,
//                          or canceling the operation, as well as giving the
//                          option of a progress indicator. If this function
//                          returns a non-zero value, then it will terminate
//                          what it is doing. It provides the application with
//                          the name of the name of the archive member it has
//                          just processed, as well as it's original size.
//NOTE: The values below are filled in only when listing the contents of an
//      archive.
//
//unsigned long TotalSizeComp = value to be filled in by the dll for the
//                          compressed total size of the archive. Note this
//                          value does not include the size of the archive
//                          header and central directory list.
//unsigned long TotalSize = value to be filled in by the dll for the total
//                          size of all files in the archive.
//unsigned long CompFactor = value to be filled in by the dll for the overall
//                          compression factor. This could actually be computed
//                          from the other values, but it is available.
//unsigned long NumMembers = total number of files in the archive.
//WORD cchComment;        = flag to be set if archive has a comment
//} USERFUNCTIONS, far * LPUSERFUNCTIONS;

	USERFUNCTIONS ufns;
	memset(&ufns,0,sizeof(USERFUNCTIONS));
	ufns.print=unzPrint;
	ufns.replace=unzReplace;

//Wiz_SingleEntryUnzip(int ifnc, char **ifnv, int xfnc, char **xfnv,
//                     LPDCL lpDCL, LPUSERFUNCTIONS lpUserFunc)
//
//ifnc       = number of file names being passed. If all files are to be
//             extracted, then this can be zero.
//ifnv       = file names to be unarchived. Wildcard patterns are recognized
//             and expanded. If all files are to be extracted, then this can
//             be NULL.
//xfnc       = number of "file names to be excluded from processing" being
//             passed. If all files are to be extracted, set this to zero.
//xfnv       = file names to be excluded from the unarchiving process. Wildcard
//             characters are allowed and expanded. If all files are to be
//             extracted, set this argument to NULL.
//lpDCL      = pointer to a structure with the flags for setting the
//             various options, as well as the zip file name.
//lpUserFunc = pointer to a structure that contains pointers to functions
//             in the calling application, as well as sizes passed back to
//             the calling application etc. See below for a detailed description
//             of all the parameters
	int iRet=Wiz_SingleEntryUnzip(0, NULL, 0, NULL, &dcl, &ufns);
//                                    LPDCL, LPUSERFUNCTIONS);

	return 0;
}



