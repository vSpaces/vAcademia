/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef __decs_h   /* prevent multiple inclusions */
#define __decs_h

#include <windows.h>
#ifndef __unzip_h
//#  include "../unzip.h"
	typedef void zvoid;
	typedef unsigned char   uch;    /* code assumes unsigned bytes; these type-  */
	typedef unsigned short  ush;    /*  defs replace byte/UWORD/ULONG (which are */
	typedef unsigned long   ulg;    /*  predefined on some systems) & match zip  */
	typedef struct _UzpBuffer {    /* rxstr */
		ulg   strlength;           /* length of string */
		char  *strptr;             /* pointer to string */
	} UzpBuffer;
#endif
#ifndef __structs_h
  #ifndef Far
  #  define Far far
  #endif

  /* Porting definitions between Win 3.1x and Win32 */
  #ifdef WIN32
  #  define far
  #  define _far
  #  define __far
  #  define near
  #  define _near
  #  define __near
  #  ifndef FAR
  #    define FAR
  #  endif
  #endif

  #ifndef DEFINED_ONCE
  #define DEFINED_ONCE

  typedef int (WINAPI DLLPRNT) (LPSTR, unsigned long);
  typedef int (WINAPI DLLPASSWORD) (LPSTR, int, LPCSTR, LPCSTR);
  typedef int (WINAPI DLLSERVICE) (LPCSTR, unsigned long);
  #endif
  typedef void (WINAPI DLLSND) (void);
  typedef int (WINAPI DLLREPLACE)(LPSTR);
  typedef void (WINAPI DLLMESSAGE)(unsigned long, unsigned long, unsigned,
     unsigned, unsigned, unsigned, unsigned, unsigned,
     char, LPSTR, LPSTR, unsigned long, char, void*); /*jedi - последний параметр введен для передачи буфера для его заполнения списком файлов*/

  typedef struct {
    DLLPRNT *print;
    DLLSND *sound;
    DLLREPLACE *replace;
    DLLPASSWORD *password;
    DLLMESSAGE *SendApplicationMessage;
    DLLSERVICE *ServCallBk;
    unsigned long TotalSizeComp;
    unsigned long TotalSize;
    unsigned long CompFactor;       /* "long" applied for proper alignment, only */
    unsigned long NumMembers;
    WORD cchComment;
	void far* pUserData;
  } USERFUNCTIONS, far * LPUSERFUNCTIONS;

  typedef struct {
    int ExtractOnlyNewer;
    int SpaceToUnderscore;
    int PromptToOverwrite;
    int fQuiet;
    int ncflag;
    int ntflag;
    int nvflag;
    int nfflag;
    int nzflag;
    int ndflag;
    int noflag;
    int naflag;
    int nZIflag;
    int C_flag;
    int fPrivilege;
    LPSTR lpszZipFN;
    LPSTR lpszExtractDir;
  } DCL, far * LPDCL;

#endif

#define Wiz_Match match

#ifdef __cplusplus
extern "C" {
#endif

void    WINAPI Wiz_NoPrinting(int f);
int     WINAPI Wiz_Validate(LPSTR archive, int AllCodes);
BOOL    WINAPI Wiz_Init(zvoid *, LPUSERFUNCTIONS);
BOOL    WINAPI Wiz_SetOpts(zvoid *, LPDCL);
int     WINAPI Wiz_Unzip(zvoid *, int, char **, int, char **);
int     WINAPI Wiz_SingleEntryUnzip(int, char **, int, char **,
                                    LPDCL, LPUSERFUNCTIONS);

int     WINAPI Wiz_UnzipToMemory(LPSTR zip, LPSTR file,
                                 LPUSERFUNCTIONS lpUserFunctions,
                                 UzpBuffer *retstr);
int     WINAPI Wiz_Grep(LPSTR archive, LPSTR file, LPSTR pattern,
                        int cmd, int SkipBin,
                        LPUSERFUNCTIONS lpUserFunctions);
LPUSERFUNCTIONS WINAPI Wiz_GetUserFunctions();

#ifdef __cplusplus
}
#endif

#endif /* __decs_h */
