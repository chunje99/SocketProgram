bin_PROGRAMS=vimtest main server client ftpServer
lib_LIBRARIES=libcommon.a
lib_LTLIBRARIES=libcommon.la

AM_CPPFLAGS=-I$(top_builddir)/libs/Inet \
			-I$(top_builddir)/libs/Protocols

vimtest_SOURCES=vimtest.cpp
vimtest_LDADD = libcommon.a \
				-lssl \
				-lcrypto

main_SOURCES=main.cpp
main_LDADD = libcommon.a \
			 -lpthread \
			 -lssl \
			 -lcrypto

server_SOURCES=server.cpp
server_LDADD = libcommon.a \
			   $(top_builddir)/libs/Inet/libinet.a \
			   $(top_builddir)/libs/Protocols/libprotocol.a \
			   -lpthread \
			   -lssl \
			   -lcrypto \
			   -lglog

ftpServer_SOURCES=ftpServer.cpp
ftpServer_LDADD = libcommon.a \
			   $(top_builddir)/libs/Inet/libinet.a \
			   $(top_builddir)/libs/Protocols/libprotocol.a \
			   -lpthread \
			   -lssl \
			   -lcrypto \
			   -lglog

client_SOURCES=client.cpp
client_LDADD = libcommon.a \
			   $(top_builddir)/libs/Inet/libinet.a \
			   $(top_builddir)/libs/Protocols/libprotocol.a \
			   -lssl \
			   -lcrypto \
			   -lglog

libSources = Elevator.cpp \
	Elevator.h \
	Person.cpp \
	Person.h

libcommon_a_SOURCES= \
					$(libSources)
libcommon_a_CFLAGS= $(AM_CFLAGS)
libcommon_a_CXXFLAGS= $(AM_CXXFLAGS)

libcommon_la_SOURCES= \
					$(libSources)
libcommon_la_CFLAGS= $(AM_CFLAGS)
libcommon_la_CXXFLAGS= $(AM_CXXFLAGS)
