# -*- Mode: Makefile -*-

INCLUDED_AUTOCONF_MK = 1
USE_AUTOCONF	= 1

MOZILLA_CLIENT	= 

prefix		= /usr/local
exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
includedir	= ${prefix}/include/nspr
libdir		= ${exec_prefix}/lib
datadir		= ${prefix}/share

dist_prefix	= ${MOD_DEPTH}/dist
dist_bindir	= ${dist_prefix}/bin
dist_includedir = ${dist_prefix}/include/nspr
dist_libdir	= ${dist_prefix}/lib

DIST		= $(dist_prefix)

RELEASE_OBJDIR_NAME = WINNT5.1_DBG.OBJ
OBJDIR_NAME	= .
OBJDIR		= $(OBJDIR_NAME)
OBJ_SUFFIX	= obj
LIB_SUFFIX	= lib
DLL_SUFFIX	= dll
ASM_SUFFIX	= s
MOD_NAME	= nspr20

MOD_MAJOR_VERSION = 4
MOD_MINOR_VERSION = 8
MOD_PATCH_VERSION = 3

LIBNSPR		= $(dist_libdir)/libnspr$(MOD_MAJOR_VERSION).$(LIB_SUFFIX)
LIBPLC		= $(dist_libdir)/libplc$(MOD_MAJOR_VERSION).$(LIB_SUFFIX)

CROSS_COMPILE	= 
BUILD_OPT	= 

USE_CPLUS	= 
USE_IPV6	= 
USE_N32		= 
USE_64		= 
GC_LEAK_DETECTOR = 
ENABLE_STRIP	= 

USE_PTHREADS	= 
USE_BTHREADS	= 
PTHREADS_USER	= 
CLASSIC_NSPR	= 

AS		= $(CC)
ASFLAGS		= $(CFLAGS)
CC		= cl
CCC		= cl
NS_USE_GCC	= 
GCC_USE_GNU_LD	= 
MSC_VER		= 1500
AR		= lib -NOLOGO -OUT:"$@"
AR_FLAGS	= 
LD		= link
RANLIB		= echo not_ranlib
PERL		= perl
RC		= rc.exe
RCFLAGS		= 
STRIP		= echo not_strip
NSINSTALL	= nsinstall
FILTER		= 
IMPLIB		= 
CYGWIN_WRAPPER	= 
MT		= mt

OS_CPPFLAGS	= 
OS_CFLAGS	= $(OS_CPPFLAGS)  -W3 -nologo -GF -Gy -Od -MD -GT -Z7 $(DSO_CFLAGS)
OS_CXXFLAGS	= $(OS_CPPFLAGS)  -Z7 $(DSO_CFLAGS)
OS_LIBS         =  
OS_LDFLAGS	=  -DEBUG
OS_DLLFLAGS	= -nologo -DLL -SUBSYSTEM:WINDOWS
DLLFLAGS	=  -OUT:"$@" -DEBUG
EXEFLAGS  = 
OPTIMIZER	= 

MKSHLIB		= $(LD) $(DSO_LDOPTS) -o $@
DSO_CFLAGS	= 
DSO_LDOPTS	= 

RESOLVE_LINK_SYMBOLS = 1

HOST_CC		= 
HOST_CFLAGS	= 
HOST_LDFLAGS	= 

DEFINES		=  -UNDEBUG -DDEBUG_tandy  -DDEBUG=1 -DXP_PC=1 -DWIN32=1 -D_DEBUG=1 -DWINNT=1 -D_X86_=1 

MDCPUCFG_H	= _winnt.cfg
PR_MD_CSRCS	= 
PR_MD_ASFILES	= 
PR_MD_ARCH_DIR	= windows
CPU_ARCH	= x86

OS_TARGET	= WINNT
OS_ARCH		= WINNT
OS_RELEASE	= 5.1
OS_TEST		= i686

NOSUCHFILE	= /no-such-file
AIX_LINK_OPTS	= 
MOZ_OBJFORMAT	= 
ULTRASPARC_LIBRARY = 

OBJECT_MODE	= 
ifdef OBJECT_MODE
export OBJECT_MODE
endif

VISIBILITY_FLAGS = 
WRAP_SYSTEM_INCLUDES = 

MACOSX_DEPLOYMENT_TARGET = 
ifdef MACOSX_DEPLOYMENT_TARGET
export MACOSX_DEPLOYMENT_TARGET
endif

MACOS_SDK_DIR	= 

SYMBIAN_SDK_DIR = 

NEXT_ROOT	= 
ifdef NEXT_ROOT
export NEXT_ROOT
endif
