#
# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is this file as it was released upon August 6, 1998.
#
# The Initial Developer of the Original Code is
# Christopher Seawood.
# Portions created by the Initial Developer are Copyright (C) 1998
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#   Benjamin Smedberg <benjamin@smedbergs.us>
#
# Alternatively, the contents of this file may be used under the terms of
# either of the GNU General Public License Version 2 or later (the "GPL"),
# or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****

# A netscape style .mk file for autoconf builds

INCLUDED_AUTOCONF_MK = 1
USE_AUTOCONF 	= 1
MOZILLA_CLIENT	= 1
target          = i686-pc-mingw32
ac_configure_args = 
BUILD_MODULES	= @BUILD_MODULES@
MOZILLA_VERSION = 

MOZ_BUILD_APP = @MOZ_BUILD_APP@
MOZ_APP_NAME	= 
MOZ_APP_DISPLAYNAME = 
MOZ_APP_VERSION = 

MOZ_PKG_SPECIAL = 

prefix		= /usr/local
exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
includedir	= ${prefix}/include
libdir		= ${exec_prefix}/lib
datadir		= ${prefix}/share
mandir		= ${prefix}/man

installdir	= $(libdir)/$(MOZ_APP_NAME)-$(MOZ_APP_VERSION)
sdkdir		= $(libdir)/$(MOZ_APP_NAME)-devel-$(MOZ_APP_VERSION)

TOP_DIST	= dist
ifneq (,$(filter /%,$(TOP_DIST)))
DIST		= $(TOP_DIST)
else
DIST		= $(DEPTH)/$(TOP_DIST)
endif

MOZ_JS_LIBS		   = $(libdir)/js$(MOZ_BITS)$(VERSION_NUMBER).lib

MOZ_SYNC_BUILD_FILES = 

MOZ_DEBUG	= 
MOZ_DEBUG_MODULES = 
MOZ_PROFILE_MODULES = 
MOZ_DEBUG_ENABLE_DEFS		= -DDEBUG -D_DEBUG -DDEBUG_tandy -DTRACING
MOZ_DEBUG_DISABLE_DEFS	= -DNDEBUG -DTRIMMED
MOZ_DEBUG_FLAGS	= -Zi
MOZ_DEBUG_LDFLAGS=-DEBUG -DEBUGTYPE:CV
MOZ_DBGRINFO_MODULES	= 
MOZ_EXTENSIONS  = @MOZ_EXTENSIONS@
MOZ_IMG_DECODERS= @MOZ_IMG_DECODERS@
MOZ_IMG_ENCODERS= @MOZ_IMG_ENCODERS@
MOZ_JSDEBUGGER  = @MOZ_JSDEBUGGER@
MOZ_PERF_METRICS = 
MOZ_LEAKY	= 
MOZ_MEMORY      = 
MOZ_JPROF       = 
MOZ_SHARK       = 
MOZ_CALLGRIND   = 
MOZ_VTUNE       = 
DEHYDRA_PATH    = 

ENABLE_EAZEL_PROFILER=
EAZEL_PROFILER_CFLAGS=
EAZEL_PROFILER_LIBS=
GC_LEAK_DETECTOR = 
NS_TRACE_MALLOC = 
INCREMENTAL_LINKER = 1
MACOSX_DEPLOYMENT_TARGET = 
BUILD_STATIC_LIBS = 
ENABLE_TESTS	= 1
JS_ULTRASPARC_OPTS = @JS_ULTRASPARC_OPTS@

TAR=@TAR@

RM = rm -f

# The MOZ_UI_LOCALE var is used to build a particular locale. Do *not*
# use the var to change any binary files. Do *not* use this var unless you
# write rules for the "clean-locale" and "locale" targets.
MOZ_UI_LOCALE = 

MOZ_COMPONENTS_VERSION_SCRIPT_LDFLAGS = 
MOZ_COMPONENT_NSPR_LIBS=$(NSPR_LIBS)

MOZ_FIX_LINK_PATHS=

XPCOM_FROZEN_LDOPTS=@XPCOM_FROZEN_LDOPTS@
XPCOM_LIBS=@XPCOM_LIBS@
MOZ_TIMELINE=

ENABLE_STRIP	= 
PKG_SKIP_STRIP	= 

MOZ_POST_DSO_LIB_COMMAND = 
MOZ_POST_PROGRAM_COMMAND = 

MOZ_BUILD_ROOT             = c:/Users/Tandy/Programming/mozilla-1.9.2/js/src

MOZ_INSURE = 
MOZ_INSURIFYING = 
MOZ_INSURE_DIRS = 
MOZ_INSURE_EXCLUDE_DIRS = 

MOZ_NATIVE_NSPR = 

CROSS_COMPILE   = 

OS_CPPFLAGS	= 
OS_CFLAGS	= $(OS_CPPFLAGS) -TC -nologo -W3 -Gy -Fd$(COMPILE_PDBFILE)
OS_CXXFLAGS	= $(OS_CPPFLAGS) -GR- -TP -nologo -Zc:wchar_t- -W3 -Gy -Fd$(COMPILE_PDBFILE)
OS_LDFLAGS	=  -MANIFESTUAC:NO -NXCOMPAT -SAFESEH -DYNAMICBASE

OS_COMPILE_CFLAGS = $(OS_CPPFLAGS) $(ACDEFINES) -D_JS_CONFDEFS_H_ -DMOZILLA_CLIENT
OS_COMPILE_CXXFLAGS = $(OS_CPPFLAGS) $(ACDEFINES) -D_JS_CONFDEFS_H_ -DMOZILLA_CLIENT

OS_INCLUDES	= $(NSPR_CFLAGS)
OS_LIBS		=  kernel32.lib user32.lib gdi32.lib winmm.lib wsock32.lib advapi32.lib
ACDEFINES	= -D_CRT_SECURE_NO_WARNINGS=1 -D_CRT_NONSTDC_NO_WARNINGS=1 -DWINVER=0x500 -D_WIN32_WINNT=0x500 -D_WIN32_IE=0x0500 -DJS_HAVE___INTN=1 -DHAVE_SYSTEMTIMETOFILETIME=1 -DHAVE_GETSYSTEMTIMEASFILETIME=1 -DJS_STDDEF_H_HAS_INTPTR_T=1 -DX_DISPLAY_MISSING=1 -DHAVE_SNPRINTF=1 -D_WINDOWS=1 -DWIN32=1 -DXP_WIN=1 -DXP_WIN32=1 -DHW_THREADS=1 -DSTDC_HEADERS=1 -DNEW_H=\<new\> -DWIN32_LEAN_AND_MEAN=1 -DNO_X11=1 -DHAVE_MMINTRIN_H=1 -DHAVE_OLEACC_IDL=1 -DHAVE_WPCAPI_H=1 -D_X86_=1 -DD_INO=d_ino -DFEATURE_NANOJIT=1 -DJS_TRACER=1 -DAVMPLUS_IA32=1 -DAVMPLUS_WIN32=1 -DCPP_THROW_NEW=throw\(\) -DMOZ_DLL_SUFFIX=\".dll\" 

WARNINGS_AS_ERRORS = -WX

MOZ_OPTIMIZE	= 1
MOZ_OPTIMIZE_FLAGS = -O1
MOZ_OPTIMIZE_LDFLAGS = 
MOZ_OPTIMIZE_SIZE_TWEAK = 

MOZ_RTTI_FLAGS_ON = -GR
MOZ_EXCEPTIONS_FLAGS_ON = -EHsc

MOZ_PROFILE_GUIDED_OPTIMIZE_DISABLE = 
PROFILE_GEN_CFLAGS = -GL
PROFILE_GEN_LDFLAGS = -LTCG:PGINSTRUMENT
PROFILE_USE_CFLAGS = -GL -wd4624 -wd4952
PROFILE_USE_LDFLAGS = -LTCG:PGUPDATE

WIN_TOP_SRC	= c:/Users/Tandy/Programming/mozilla-1.9.2/js/src
CYGWIN_WRAPPER	= 
AS_PERL         = 
CYGDRIVE_MOUNT	= 
AR		= lib -NOLOGO -OUT:"$@"
AR_FLAGS	= 
AR_EXTRACT	= $(AR) x
AR_LIST		= $(AR) t
AR_DELETE	= $(AR) d
AS		= $(CC)
ASFLAGS		= 
AS_DASH_C_FLAG	= -c
LD		= link
RC		= rc.exe
RCFLAGS		= 
WINDRES		= :
USE_SHORT_LIBNAME = 1
IMPLIB		= 
FILTER		= 
BIN_FLAGS	= 
_MSC_VER	= 1500

DLL_PREFIX	= 
LIB_PREFIX	= 
OBJ_SUFFIX	= obj
LIB_SUFFIX	= lib
DLL_SUFFIX	= .dll
BIN_SUFFIX	= .exe
ASM_SUFFIX	= s
IMPORT_LIB_SUFFIX = lib
USE_N32		= 
HAVE_64BIT_OS	= 

# Temp hack.  It is not my intention to leave this crap in here for ever.
# Im talking to fur right now to solve the problem without introducing 
# NS_USE_NATIVE to the build system -ramiro.
NS_USE_NATIVE = 

CC		    = cl
CXX		    = cl

CC_VERSION	= 15.00.21022.08
CXX_VERSION	= 15.00.21022.08

GNU_AS		= 
GNU_LD		= 
GNU_CC		= 
GNU_CXX		= 
HAVE_GCC3_ABI	= 
INTEL_CC	= 
INTEL_CXX	= 

HOST_CC		= $(CC)
HOST_CXX	= $(CXX)
HOST_CFLAGS	=  -TC -nologo -Fd$(HOST_PDBFILE) -DXP_WIN32 -DXP_WIN -DWIN32 -D_WIN32 -DNO_X11
HOST_CXXFLAGS	= 
HOST_OPTIMIZE_FLAGS = -O2
HOST_NSPR_MDCPUCFG = \"md/_winnt.cfg\"
HOST_AR		= lib
HOST_AR_FLAGS	= -NOLOGO -OUT:"$@"
HOST_LD		= $(LD)
HOST_RANLIB	= echo ranlib
HOST_BIN_SUFFIX	= .exe

HOST_OS_ARCH	= WINNT
host_cpu	= i686
host_vendor	= pc
host_os		= mingw32

TARGET_NSPR_MDCPUCFG = \"md/_win95.cfg\"
TARGET_CPU	= i686
TARGET_VENDOR	= pc
TARGET_OS	= mingw32
TARGET_MD_ARCH	= win32
TARGET_XPCOM_ABI = x86-msvc

AUTOCONF	= C:/mozilla-build/msys/bin/autoconf
PERL		= C:/mozilla-build/msys/bin/perl.exe
PYTHON		= c:/mozilla-build/python25/python.exe
RANLIB		= echo not_ranlib
WHOAMI		= :
UNZIP		= unzip
ZIP		= zip
XARGS		= xargs
STRIP		= echo not_strip
DOXYGEN		= :
PBBUILD_BIN	= 
SDP		= 
NSINSTALL_BIN	= 

NSPR_CONFIG	= 
NSPR_CFLAGS	= 
NSPR_LIBS	= 

USE_DEPENDENT_LIBS = 1

JS_NATIVE_EDITLINE = 
JS_DISABLE_SHELL   = 
EDITLINE_LIBS      = 

# MKSHLIB_FORCE_ALL is used to force the linker to include all object
# files present in an archive. MKSHLIB_UNFORCE_ALL reverts the linker
# to normal behavior. Makefile's that create shared libraries out of
# archives use these flags to force in all of the .o files in the
# archives into the shared library.
WRAP_MALLOC_LIB         = 
WRAP_MALLOC_CFLAGS      = 
DSO_CFLAGS              = 
DSO_PIC_CFLAGS          = 
MKSHLIB                 = $(LD) -NOLOGO -DLL -OUT:$@ -PDB:$(LINK_PDBFILE) $(DSO_LDOPTS)
MKCSHLIB                = $(LD) -NOLOGO -DLL -OUT:$@ -PDB:$(LINK_PDBFILE) $(DSO_LDOPTS)
MKSHLIB_FORCE_ALL       = 
MKSHLIB_UNFORCE_ALL     = 
DSO_LDOPTS              = -SUBSYSTEM:WINDOWS
DLL_SUFFIX              = .dll

NO_LD_ARCHIVE_FLAGS     = 

MOZ_TOOLKIT_REGISTRY_CFLAGS = \
	$(TK_CFLAGS)

MOZ_NATIVE_MAKEDEPEND	= 

MOZ_AUTO_DEPS	= 1
COMPILER_DEPEND = 
MDDEPDIR        := .deps

MOZ_DEMANGLE_SYMBOLS = 

# XXX - these need to be cleaned up and have real checks added -cls
CM_BLDTYPE=dbg
AWT_11=1
MOZ_BITS=32
OS_TARGET=WINNT
OS_ARCH=WINNT
OS_RELEASE=5.1
OS_TEST=i686

# For Solaris build
SOLARIS_SUNPRO_CC = 
SOLARIS_SUNPRO_CXX = 

# For AIX build
AIX_OBJMODEL = 

# For OS/2 build
MOZ_OS2_TOOLS = 
MOZ_OS2_USE_DECLSPEC = 
MOZ_OS2_HIGH_MEMORY = 1

MOZILLA_OFFICIAL = 

# Win32 options
MOZ_BROWSE_INFO	= 
MOZ_TOOLS_DIR	= /c/mozilla-build/moztools
MOZ_DEBUG_SYMBOLS = 
MOZ_QUANTIFY	= 
MSMANIFEST_TOOL = 1
WIN32_REDIST_DIR = 
MOZ_MEMORY_LDFLAGS = 

# Codesighs tools option, enables win32 mapfiles.
MOZ_MAPINFO	= 

WINCE		= 
WINCE_WINDOWS_MOBILE = 

MACOS_SDK_DIR	= 
NEXT_ROOT	= 
GCC_VERSION	= 
XCODEBUILD_VERSION= 
HAS_XCODE_2_1	= 
UNIVERSAL_BINARY= 
HAVE_DTRACE= 

VISIBILITY_FLAGS = 
WRAP_SYSTEM_INCLUDES = 

ENABLE_JIT = 1
NANOJIT_ARCH = i386
HAVE_ARM_SIMD= @HAVE_ARM_SIMD@
