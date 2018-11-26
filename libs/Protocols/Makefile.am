lib_LIBRARIES=libprotocol.a
lib_LTLIBRARIES=libprotocol.la

AM_CPPFLAGS=-I$(top_builddir)/libs/Inet

libSources = \
	Protocol.cpp \
	Protocol.h \
	ProtocolSimple.cpp \
	ProtocolSimple.h \
	ProtocolFtp.cpp \
	ProtocolFtp.h \
	ProtocolHttp.cpp \
	ProtocolHttp.h

libprotocol_a_SOURCES= \
   			$(libSources)
libprotocol_a_CFLAGS= $(AM_CFLAGS)
libprotocol_a_CXXFLAGS= $(AM_CXXFLAGS)

libprotocol_la_SOURCES= \
   			$(libSources)
libprotocol_la_CFLAGS= $(AM_CFLAGS)
libprotocol_la_CXXFLAGS= $(AM_CXXFLAGS)
