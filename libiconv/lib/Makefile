# Makefile for libiconv/lib

#### Start of system configuration section. ####

# Directories used by "make":
srcdir = .

# Directories used by "make install":
prefix = c:/progra~1/LibIconv
local_prefix = /usr/local
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib

# Programs used by "make":
CC = gcc
CFLAGS =   -mms-bitfields -fms-extensions -fno-rtti -fno-exceptions -mcpu=i386 -march=i386 
CPPFLAGS =     -O2 -DBUILDING_LIBICONV -DBUILDING_LIBCHARSET -Ic:/progra~1/LibIconv/include
LDFLAGS =   -Wl,-s -Wl,--enable-auto-import 
INCLUDES = -I. -I$(srcdir) -I../include -I$(srcdir)/../include
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LIBTOOL_COMPILE = $(LIBTOOL) --mode=compile
LIBTOOL_LINK = $(LIBTOOL) --mode=link
LIBTOOL_INSTALL = $(LIBTOOL) --mode=install
LIBTOOL_UNINSTALL = $(LIBTOOL) --mode=uninstall
CP = cp
MV = mv
LN = cp -fp
LN_S =  cp -fp
RM = rm -f

# Programs used by "make install":
INSTALL = /bin/install.exe -cpD
INSTALL_PROGRAM = /bin/install.exe -cpD
INSTALL_DATA = /bin/install.exe -cp

#### End of system configuration section. ####

SHELL = /bin/sh

# Before making a release, change this according to the libtool documentation,
# section "Library interface versions".
LIBICONV_VERSION_INFO = 3:0:1

# Needed by $(LIBTOOL).
top_builddir = ..

SOURCES = $(srcdir)/iconv.c

OBJECTS = iconv.lo

LIBCHARSET_OBJECTS = ../libcharset/lib/localcharset.lo

all :  libiconv.la

libiconv.la : $(OBJECTS)
	$(LIBTOOL_LINK) $(CC) $(LDFLAGS) -o libiconv.la -rpath $(libdir) -version-info $(LIBICONV_VERSION_INFO) -no-undefined $(OBJECTS) $(LIBCHARSET_OBJECTS)

libiconv_plug.so : libiconv_plug_.so
	$(RM) libiconv_plug.so
	$(LN_S) libiconv_plug_.so libiconv_plug.so

libiconv_plug_linux.so : $(SOURCES)
	if test -n "yes"; then \
	  $(LIBTOOL_LINK) $(CC) $(LDFLAGS) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) -fPIC -DPIC -DLIBICONV_PLUG $(SOURCES) -x none $(LIBCHARSET_OBJECTS) -shared -o libiconv_plug_linux.so; \
	else \
	  $(LIBTOOL_LINK) $(CC) $(LDFLAGS) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) -KPIC -DPIC -DLIBICONV_PLUG $(SOURCES) $(LIBCHARSET_OBJECTS) -shared -o libiconv_plug_linux.so; \
	fi

libiconv_plug_solaris.so : $(SOURCES)
	if test -n "yes"; then \
	  $(LIBTOOL_LINK) $(CC) $(LDFLAGS) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) -fPIC -DPIC -DLIBICONV_PLUG $(SOURCES) -x none $(LIBCHARSET_OBJECTS) -G -o libiconv_plug_solaris.so; \
	else \
	  $(LIBTOOL_LINK) $(CC) $(LDFLAGS) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) -KPIC -DPIC -DLIBICONV_PLUG $(SOURCES) $(LIBCHARSET_OBJECTS) -G -o libiconv_plug_solaris.so; \
	fi

libiconv_plug_osf.so : $(SOURCES)
	if test -n "yes"; then \
	  $(LIBTOOL_LINK) $(CC) $(LDFLAGS) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) -fPIC -DPIC -DLIBICONV_PLUG $(SOURCES) -x none $(LIBCHARSET_OBJECTS) -shared -o libiconv_plug_osf.so; \
	else \
	  $(LIBTOOL_LINK) $(CC) $(LDFLAGS) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) -DPIC -DLIBICONV_PLUG $(SOURCES) $(LIBCHARSET_OBJECTS) -c -o iconv_plug.o; \
	  /bin/ld -shared -expect_unresolved \* -o libiconv_plug_osf.so iconv_plug.o; \
	fi

iconv.lo : $(srcdir)/iconv.c $(srcdir)/converters.h $(srcdir)/encodings.def $(srcdir)/encodings_aix.def $(srcdir)/encodings_osf1.def $(srcdir)/encodings_dos.def $(srcdir)/encodings_local.def $(srcdir)/aliases.h $(srcdir)/aliases_aix.h $(srcdir)/aliases_osf1.h $(srcdir)/aliases_dos.h $(srcdir)/flags.h
	$(LIBTOOL_COMPILE) $(CC) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) -c $(srcdir)/iconv.c

# Installs the library and include files only. Typically called with only
# $(libdir) and $(includedir) - don't use $(prefix) and $(exec_prefix) here.
install-lib : all force
	if [ ! -d $(libdir) ] ; then mkdir $(libdir) ; fi
	$(LIBTOOL_INSTALL) $(INSTALL_DATA) libiconv.la $(libdir)/libiconv.la

install : all force
	if [ ! -d $(DESTDIR)$(prefix) ] ; then mkdir $(DESTDIR)$(prefix) ; fi
	if [ ! -d $(DESTDIR)$(exec_prefix) ] ; then mkdir $(DESTDIR)$(exec_prefix) ; fi
	if [ ! -d $(DESTDIR)$(libdir) ] ; then mkdir $(DESTDIR)$(libdir) ; fi
	$(LIBTOOL_INSTALL) $(INSTALL_DATA) libiconv.la $(DESTDIR)$(libdir)/libiconv.la
	if test -n ""; then $(INSTALL_DATA)  $(DESTDIR)$(libdir)/.new && mv $(DESTDIR)$(libdir)/.new $(DESTDIR)$(libdir)/ ; fi

installdirs : force
	if [ ! -d $(DESTDIR)$(prefix) ] ; then mkdir $(DESTDIR)$(prefix) ; fi
	if [ ! -d $(DESTDIR)$(exec_prefix) ] ; then mkdir $(DESTDIR)$(exec_prefix) ; fi
	if [ ! -d $(DESTDIR)$(libdir) ] ; then mkdir $(DESTDIR)$(libdir) ; fi

uninstall : force
	$(LIBTOOL_UNINSTALL) $(RM) $(DESTDIR)$(libdir)/libiconv.la
	if test -n ""; then $(RM) $(DESTDIR)$(libdir)/ ; fi

check : all

mostlyclean : clean

clean : force
	$(RM) *.o *.lo *.a *.la libiconv_plug* core so_locations
	$(RM) -r .libs _libs

distclean : clean
	$(RM) Makefile config.h libtool

maintainer-clean : distclean

force :

