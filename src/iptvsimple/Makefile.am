LIBS   =
LIBDIR = ../../addons/pvr.iptvsimple
LIB    = ../../addons/pvr.iptvsimple/$(ADDONNAME)

lib_LTLIBRARIES = libiptvsimpleaddon.la

libiptvsimpleaddon_la_SOURCES = \
	client.cpp \
	client.h \
	PVRIptvData.cpp \
	PVRIptvData.h
	
libiptvsimpleaddon_la_LDFLAGS = \
	-avoid-version

libiptvsimpleaddon_la_LIBADD = \
	$(ZLIB_LIBS)

AM_CPPFLAGS = \
	$(ARCH_DEFINES)

INCLUDES = \
	-I. \
	-I$(top_srcdir)/include/xbmc -I$(top_srcdir)/src \
	-DUSE_DEMUX

EXTRA_DIST = \
	include

$(ADDONNAME): libiptvsimpleaddon.la
	cp -f .libs/libiptvsimpleaddon.$(SOEXT) $(ADDONNAME)

$(LIB): $(ADDONNAME)
	cp -f $(ADDONNAME) $(LIB)

all: $(LIB)

clean-local:
	-rm -f $(LIB) $(ADDONNAME)

install:
