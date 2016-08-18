# Microsoft Developer Studio Project File - Name="libiconv_vc6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libiconv_vc6 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libiconv_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libiconv_vc6.mak" CFG="libiconv_vc6 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libiconv_vc6 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libiconv_vc6 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libiconv_vc6 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBICONV_VC6_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\include" /I "..\lib" /I "..\libcharset" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "libiconv_vc6 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBICONV_VC6_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\lib" /I "..\libcharset" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILDING_LIBICONV" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "libiconv_vc6 - Win32 Release"
# Name "libiconv_vc6 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\lib\iconv.c
# End Source File
# Begin Source File

SOURCE=..\windows\iconv.rc
# End Source File
# Begin Source File

SOURCE="..\lib\libiconv-2.rc"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="..\lib\libiconv-3.rc"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\libcharset\lib\localcharset.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\lib\aliases.h
# End Source File
# Begin Source File

SOURCE=..\lib\aliases_aix.h
# End Source File
# Begin Source File

SOURCE=..\lib\aliases_dos.h
# End Source File
# Begin Source File

SOURCE=..\lib\aliases_extra.h
# End Source File
# Begin Source File

SOURCE=..\lib\aliases_osf1.h
# End Source File
# Begin Source File

SOURCE=..\lib\armscii_8.h
# End Source File
# Begin Source File

SOURCE=..\lib\ascii.h
# End Source File
# Begin Source File

SOURCE=..\lib\big5.h
# End Source File
# Begin Source File

SOURCE=..\lib\big5hkscs.h
# End Source File
# Begin Source File

SOURCE=..\lib\c99.h
# End Source File
# Begin Source File

SOURCE=..\lib\ces_big5.h
# End Source File
# Begin Source File

SOURCE=..\lib\ces_gbk.h
# End Source File
# Begin Source File

SOURCE=..\lib\cjk_variants.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_1.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_15.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_2.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_3.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_4.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_4a.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_4b.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_5.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_6.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_7.h
# End Source File
# Begin Source File

SOURCE=..\lib\cns11643_inv.h
# End Source File
# Begin Source File

SOURCE=..\lib\config.h
# End Source File
# Begin Source File

SOURCE=..\lib\converters.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1046.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1124.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1125.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1129.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1133.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1161.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1162.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1163.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1250.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1251.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1252.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1253.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1254.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1255.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1256.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1257.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp1258.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp437.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp737.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp775.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp850.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp852.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp853.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp855.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp856.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp857.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp858.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp860.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp861.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp862.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp863.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp864.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp865.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp866.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp869.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp874.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp922.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp932.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp932ext.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp936ext.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp943.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp949.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp950.h
# End Source File
# Begin Source File

SOURCE=..\lib\cp950ext.h
# End Source File
# Begin Source File

SOURCE=..\lib\dec_hanyu.h
# End Source File
# Begin Source File

SOURCE=..\lib\dec_kanji.h
# End Source File
# Begin Source File

SOURCE=.\dllexport.h
# End Source File
# Begin Source File

SOURCE=..\lib\euc_cn.h
# End Source File
# Begin Source File

SOURCE=..\lib\euc_jisx0213.h
# End Source File
# Begin Source File

SOURCE=..\lib\euc_jp.h
# End Source File
# Begin Source File

SOURCE=..\lib\euc_kr.h
# End Source File
# Begin Source File

SOURCE=..\lib\euc_tw.h
# End Source File
# Begin Source File

SOURCE=..\lib\flags.h
# End Source File
# Begin Source File

SOURCE=..\lib\flushwc.h
# End Source File
# Begin Source File

SOURCE=..\lib\gb12345.h
# End Source File
# Begin Source File

SOURCE=..\lib\gb12345ext.h
# End Source File
# Begin Source File

SOURCE=..\lib\gb18030.h
# End Source File
# Begin Source File

SOURCE=..\lib\gb18030ext.h
# End Source File
# Begin Source File

SOURCE=..\lib\gb18030uni.h
# End Source File
# Begin Source File

SOURCE=..\lib\gb2312.h
# End Source File
# Begin Source File

SOURCE=..\lib\gbk.h
# End Source File
# Begin Source File

SOURCE=..\lib\gbkext1.h
# End Source File
# Begin Source File

SOURCE=..\lib\gbkext2.h
# End Source File
# Begin Source File

SOURCE=..\lib\gbkext_inv.h
# End Source File
# Begin Source File

SOURCE=..\lib\georgian_academy.h
# End Source File
# Begin Source File

SOURCE=..\lib\georgian_ps.h
# End Source File
# Begin Source File

SOURCE=..\src\gettext.h
# End Source File
# Begin Source File

SOURCE=..\lib\hkscs.h
# End Source File
# Begin Source File

SOURCE=..\lib\hp_roman8.h
# End Source File
# Begin Source File

SOURCE=..\lib\hz.h
# End Source File
# Begin Source File

SOURCE=..\include\iconv.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso2022_cn.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso2022_cnext.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso2022_jp.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso2022_jp1.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso2022_jp2.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso2022_jp3.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso2022_kr.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso646_cn.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso646_jp.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_1.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_10.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_13.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_14.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_15.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_16.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_2.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_3.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_4.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_5.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_6.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_7.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_8.h
# End Source File
# Begin Source File

SOURCE=..\lib\iso8859_9.h
# End Source File
# Begin Source File

SOURCE=..\lib\isoir165.h
# End Source File
# Begin Source File

SOURCE=..\lib\isoir165ext.h
# End Source File
# Begin Source File

SOURCE=..\lib\java.h
# End Source File
# Begin Source File

SOURCE=..\lib\jisx0201.h
# End Source File
# Begin Source File

SOURCE=..\lib\jisx0208.h
# End Source File
# Begin Source File

SOURCE=..\lib\jisx0212.h
# End Source File
# Begin Source File

SOURCE=..\lib\jisx0213.h
# End Source File
# Begin Source File

SOURCE=..\lib\johab.h
# End Source File
# Begin Source File

SOURCE=..\lib\johab_hangul.h
# End Source File
# Begin Source File

SOURCE=..\lib\koi8_r.h
# End Source File
# Begin Source File

SOURCE=..\lib\koi8_ru.h
# End Source File
# Begin Source File

SOURCE=..\lib\koi8_t.h
# End Source File
# Begin Source File

SOURCE=..\lib\koi8_u.h
# End Source File
# Begin Source File

SOURCE=..\lib\ksc5601.h
# End Source File
# Begin Source File

SOURCE=..\lib\libcharset.h
# End Source File
# Begin Source File

SOURCE=..\lib\loop_unicode.h
# End Source File
# Begin Source File

SOURCE=..\lib\loop_wchar.h
# End Source File
# Begin Source File

SOURCE=..\lib\loops.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_arabic.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_centraleurope.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_croatian.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_cyrillic.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_greek.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_hebrew.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_iceland.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_roman.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_romania.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_thai.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_turkish.h
# End Source File
# Begin Source File

SOURCE=..\lib\mac_ukraine.h
# End Source File
# Begin Source File

SOURCE=..\lib\mulelao.h
# End Source File
# Begin Source File

SOURCE=..\lib\nextstep.h
# End Source File
# Begin Source File

SOURCE=..\lib\riscos1.h
# End Source File
# Begin Source File

SOURCE=..\lib\shift_jisx0213.h
# End Source File
# Begin Source File

SOURCE=..\lib\sjis.h
# End Source File
# Begin Source File

SOURCE=..\lib\tcvn.h
# End Source File
# Begin Source File

SOURCE=..\lib\tds565.h
# End Source File
# Begin Source File

SOURCE=..\lib\tis620.h
# End Source File
# Begin Source File

SOURCE=..\lib\translit.h
# End Source File
# Begin Source File

SOURCE=..\lib\ucs2.h
# End Source File
# Begin Source File

SOURCE=..\lib\ucs2be.h
# End Source File
# Begin Source File

SOURCE=..\lib\ucs2internal.h
# End Source File
# Begin Source File

SOURCE=..\lib\ucs2le.h
# End Source File
# Begin Source File

SOURCE=..\lib\ucs2swapped.h
# End Source File
# Begin Source File

SOURCE=..\lib\ucs4.h
# End Source File
# Begin Source File

SOURCE=..\lib\ucs4be.h
# End Source File
# Begin Source File

SOURCE=..\lib\ucs4internal.h
# End Source File
# Begin Source File

SOURCE=..\lib\ucs4le.h
# End Source File
# Begin Source File

SOURCE=..\lib\ucs4swapped.h
# End Source File
# Begin Source File

SOURCE=..\lib\uhc_1.h
# End Source File
# Begin Source File

SOURCE=..\lib\uhc_2.h
# End Source File
# Begin Source File

SOURCE=..\lib\utf16.h
# End Source File
# Begin Source File

SOURCE=..\lib\utf16be.h
# End Source File
# Begin Source File

SOURCE=..\lib\utf16le.h
# End Source File
# Begin Source File

SOURCE=..\lib\utf32.h
# End Source File
# Begin Source File

SOURCE=..\lib\utf32be.h
# End Source File
# Begin Source File

SOURCE=..\lib\utf32le.h
# End Source File
# Begin Source File

SOURCE=..\lib\utf7.h
# End Source File
# Begin Source File

SOURCE=..\lib\utf8.h
# End Source File
# Begin Source File

SOURCE=..\lib\vietcomb.h
# End Source File
# Begin Source File

SOURCE=..\lib\viscii.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\lib\Makefile.in
# End Source File
# End Target
# End Project
