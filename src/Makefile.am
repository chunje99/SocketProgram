bin_PROGRAMS=vimtest main server client ftpServer muxServer muxer
lib_LIBRARIES=libcommon.a
lib_LTLIBRARIES=libcommon.la

AM_CPPFLAGS=-I$(top_builddir)/libs/Inet \
			-I$(top_builddir)/libs/Protocols \
			-I$(top_builddir)/libs/Ffmpeg

#AM_CFLAGS=-I$(top_builddir)/libs/Inet \
#			-I$(top_builddir)/libs/Protocols \
#			-I$(top_builddir)/libs/Ffmpeg

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

muxServer_SOURCES=muxServer.cpp
muxServer_LDADD = libcommon.a \
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

muxer_SOURCES=muxer.cpp
muxer_LDADD = libcommon.a \
			   $(top_builddir)/libs/Inet/libinet.a \
			   $(top_builddir)/libs/Protocols/libprotocol.a \
			   $(top_builddir)/libs/Ffmpeg/libFfmpeg.a \
			   -lssl \
			   -lcrypto \
			   -lglog \
			   $(FFMPEG_AVFORMAT_LIBS) \
			   $(FFMPEG_AVCODEC_LIBS) \
			   $(FFMPEG_AVUTIL_LIBS) \
			   $(FFMPEG_SWSCALE_LIBS) \
			   $(FFMPEG_SWRESAMPLE_LIBS)

muxer_CXXFLAGS = "-D__STDC_CONSTANT_MACROS"
muxer_CXXFLAGS += $(FFMPEG_AVCODEC_CFLAGS) 
muxer_CXXFLAGS += $(FFMPEG_AVFORMAT_CFLAGS) 
muxer_CXXFLAGS += $(FFMPEG_AVUTIL_CFLAGS)
muxer_CXXFLAGS += $(FFMPEG_SWSCALE_CFLAGS)
muxer_CXXFLAGS += $(FFMPEG_SWRESAMPLE_CFLAGS)
muxer_CXXFLAGS += $(AM_CPPFLAGS)

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
