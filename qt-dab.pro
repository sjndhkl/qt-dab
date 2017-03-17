######################################################################
# Automatically generated by qmake (2.01a) Tue Oct 6 19:48:14 2009
# but modified by me to accomodate for the includes for qwt, hamlib and
# portaudio
######################################################################

TEMPLATE	= app
TARGET		= qt-dab-0.998
QT		+= widgets network 
CONFIG		+= console
QMAKE_CFLAGS	+=  -flto -ffast-math
QMAKE_CXXFLAGS	+=  -flto -ffast-math
QMAKE_LFLAGS	+=  -flto
#QMAKE_CFLAGS	+=  -g
#QMAKE_CXXFLAGS	+=  -g
#QMAKE_LFLAGS	+=  -g
DEPENDPATH += . \
	      ./src \
	      ./includes \
	      ./src/ofdm \
	      ./src/backend \
	      ./src/backend/audio \
	      ./src/backend/data \
	      ./src/backend/data/journaline \
	      ./src/output \
	      ./src/various \
	      ./src/input \
	      ./src/input/rawfiles \
	      ./src/input/wavfiles \
	      ./includes/ofdm \
	      ./includes/backend \
	      ./includes/backend/audio \
	      ./includes/backend/data \
	      ./includes/backend/data/journaline \
	      ./includes/output \
	      ./includes/various 

INCLUDEPATH += . \
	      ./ \
	      ./src \
	      ./includes \
	      ./includes/ofdm \
	      ./includes/backend \
	      ./includes/backend/audio \
	      ./includes/backend/data \
	      ./includes/backend/data/journaline \
	      ./includes/output \
	      ./includes/various \
	      ./src/input \
	      ./src/input/rawfiles \
	      ./src/input/wavfiles 

# Input
HEADERS += ./radio.h \
	   ./includes/dab-constants.h \
	   ./includes/ofdm/ofdm-processor.h \
	   ./includes/ofdm/ofdm-decoder.h \
	   ./includes/ofdm/phasereference.h \
	   ./includes/ofdm/phasetable.h \
	   ./includes/ofdm/freq-interleaver.h \
	   ./includes/backend/viterbi.h \
	   ./includes/backend/fic-handler.h \
	   ./includes/backend/msc-handler.h \
	   ./includes/backend/fib-processor.h  \
	   ./includes/backend/galois.h \
	   ./includes/backend/reed-solomon.h \
	   ./includes/backend/charsets.h \
	   ./includes/backend/firecode-checker.h \
	   ./includes/backend/dab-processor.h \
	   ./includes/backend/dab-virtual.h \
	   ./includes/backend/audio/dab-audio.h \
	   ./includes/backend/audio/mp2processor.h \
	   ./includes/backend/audio/mp4processor.h \
	   ./includes/backend/audio/faad-decoder.h \
	   ./includes/backend/data/dab-data.h \
	   ./includes/backend/data/data-processor.h \
	   ./includes/backend/data/pad-handler.h \
	   ./includes/backend/data/virtual-datahandler.h \
	   ./includes/backend/data/ip-datahandler.h \
	   ./includes/backend/data/mot-databuilder.h \
	   ./includes/backend/data/mot-data.h \
	   ./includes/backend/data/journaline-datahandler.h \
	   ./includes/backend/data/journaline/dabdatagroupdecoder.h \
	   ./includes/backend/data/journaline/crc_8_16.h \
	   ./includes/backend/data/journaline/log.h \
	   ./includes/backend/data/journaline/newssvcdec_impl.h \
	   ./includes/backend/data/journaline/Splitter.h \
	   ./includes/backend/data/journaline/dabdgdec_impl.h \
	   ./includes/backend/data/journaline/newsobject.h \
	   ./includes/backend/data/journaline/NML.h \
	   ./includes/backend/protection.h \
	   ./includes/backend/eep-protection.h \
	   ./includes/backend/uep-protection.h \
	   ./includes/output/audio-base.h \
	   ./includes/output/newconverter.h \
	   ./includes/output/audiosink.h \
	   ./includes/output/fir-filters.h \
           ./includes/various/fft.h \
	   ./includes/various/ringbuffer.h \
	   ./includes/various/Xtan2.h \
	   ./src/input/virtual-input.h \
	   ./src/input/rawfiles/rawfiles.h \
           ./src/input/wavfiles/wavfiles.h

FORMS +=	./src/input/filereader-widget.ui 

SOURCES += ./main.cpp \
	   ./radio.cpp \
	   ./src/ofdm/ofdm-processor.cpp \
	   ./src/ofdm/ofdm-decoder.cpp \
	   ./src/ofdm/phasereference.cpp \
	   ./src/ofdm/phasetable.cpp \
	   ./src/ofdm/freq-interleaver.cpp \
	   ./src/backend/viterbi.cpp \
	   ./src/backend/fic-handler.cpp \
	   ./src/backend/msc-handler.cpp \
	   ./src/backend/protection.cpp \
	   ./src/backend/eep-protection.cpp \
	   ./src/backend/uep-protection.cpp \
	   ./src/backend/fib-processor.cpp  \
	   ./src/backend/galois.cpp \
	   ./src/backend/reed-solomon.cpp \
	   ./src/backend/charsets.cpp \
	   ./src/backend/firecode-checker.cpp \
	   ./src/backend/dab-virtual.cpp \
	   ./src/backend/dab-processor.cpp \
	   ./src/backend/protTables.cpp \
	   ./src/backend/audio/dab-audio.cpp \
	   ./src/backend/audio/mp2processor.cpp \
	   ./src/backend/audio/mp4processor.cpp \
	   ./src/backend/data/pad-handler.cpp \
	   ./src/backend/data/dab-data.cpp \
	   ./src/backend/data/data-processor.cpp \
	   ./src/backend/data/virtual-datahandler.cpp \
	   ./src/backend/data/ip-datahandler.cpp \
	   ./src/backend/data/mot-databuilder.cpp \
	   ./src/backend/data/mot-data.cpp \
	   ./src/backend/data/journaline-datahandler.cpp \
	   ./src/backend/data/journaline/crc_8_16.c \
	   ./src/backend/data/journaline/log.c \
	   ./src/backend/data/journaline/newssvcdec_impl.cpp \
	   ./src/backend/data/journaline/Splitter.cpp \
	   ./src/backend/data/journaline/dabdgdec_impl.c \
	   ./src/backend/data/journaline/newsobject.cpp \
	   ./src/backend/data/journaline/NML.cpp \
	   ./src/output/audio-base.cpp \
	   ./src/output/newconverter.cpp \
	   ./src/output/audiosink.cpp \
	   ./src/output/fir-filters.cpp \
           ./src/various/fft.cpp \
	   ./src/various/Xtan2.cpp \
	   ./src/input/virtual-input.cpp \
	   ./src/input/rawfiles/rawfiles.cpp \
           ./src/input/wavfiles/wavfiles.cpp
#
#	for unix systems this is about it. Adapt when needed for naming
#	and locating libraries. If you do not need a device as
#	listed, just comment the line out.
#
unix {
DESTDIR		= ./linux-bin
FORMS 		+= ./forms/dabradio.ui 
FORMS		+= ./forms/technical_data.ui
INCLUDEPATH	+= /usr/local/include
LIBS		+= -lfftw3f  -lusb-1.0 -ldl  #
LIBS		+= -lportaudio
LIBS		+= -lz
LIBS		+= -lsndfile
LIBS		+= -lsamplerate
LIBS		+= -lfaad
CONFIG		+= dabstick
CONFIG		+= sdrplay
CONFIG		+= rtl_tcp
CONFIG		+= airspy
CONFIG		+= spectrum
#CONFIG		+= tcp-streamer		# use for remote listening
DEFINES		+= TECHNICAL_DATA
DEFINES		+= MSC_DATA__		# use at your own risk
#CONFIG		+= try-epg
#DEFINES	+= __QUALITY		# just a counter
}
#
# an attempt to have it run under W32
win32 {
DESTDIR	= ../../windows-bin
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
LIBS		+= -L/usr/i686-w64-mingw32/sys-root/mingw/lib
LIBS		+= -lfftw3f
LIBS		+= -lportaudio
LIBS		+= -lsndfile
LIBS		+= -lsamplerate
LIBS		+= -lole32
LIBS		+= -lwinpthread
LIBS		+= -lwinmm
LIBS 		+= -lstdc++
LIBS		+= -lws2_32
LIBS		+= -lfaad
LIBS		+= -lusb-1.0
LIBS		+= -lz
FORMS 		+= ./forms/dabradio.ui 
FORMS		+= ./forms/technical_data.ui
CONFIG		+= extio
CONFIG		+= airspy
CONFIG		+= rtl_tcp
CONFIG		+= dabstick
CONFIG		+= sdrplay
#CONFIG		+= tcp-streamer		# use for remote listening
CONFIG		+= spectrum
DEFINES		+= TECHNICAL_DATA
DEFINES		+= MSC_DATA__		# use at your own risk
}

#######################################
#

#       If you want to see the spectrum, take "CONFIG += spectrum"
spectrum {
        DEFINES         += HAVE_SPECTRUM
#adapt to the correct path for your system
	INCLUDEPATH += /usr/local/include /usr/include/qt4/qwt /usr/include/qt5/qwt /usr/include/qt4/qwt /usr/include/qwt
	INCLUDEPATH	+= ./includes/scopes-qwt6
	HEADERS		+= ./includes/scopes-qwt6/spectrogramdata.h \
	                   ./includes/scopes-qwt6/iqdisplay.h 
	SOURCES		+= ./src/scopes-qwt6/iqdisplay.cpp
        DEPENDPATH	+= ./optional-scope
	INCLUDEPATH	+= ./includes/scopes-qwt6
        INCLUDEPATH	+= ./optional-scope
        FORMS           += ./optional-scope/scopewidget.ui
        HEADERS         += ./optional-scope/spectrum-handler.h
	HEADERS		+= ./includes/scopes-qwt6
        SOURCES         += ./optional-scope/spectrum-handler.cpp
#correct for the correct path to the qwt6 library on your system
#	LIBS		+= -lqwt
	LIBS		+= -lqwt-qt5
}

#	devices
#
#	dabstick
dabstick {
	DEFINES		+= HAVE_DABSTICK
	INCLUDEPATH	+= /home/jan/rtl-sdr/include
	INCLUDEPATH	+= ./src/input/dabstick
	HEADERS		+= ./src/input/dabstick/dabstick.h \
	                   ./src/input/dabstick/dongleselect.h
	SOURCES		+= ./src/input/dabstick/dabstick.cpp \
	                   ./src/input/dabstick/dongleselect.cpp
	FORMS		+= ./src/input/dabstick/dabstick-widget.ui
}
#
#	the SDRplay
#
sdrplay {
	DEFINES		+= HAVE_SDRPLAY
	INCLUDEPATH	+= ./src/input/sdrplay 
	HEADERS		+= ./src/input/sdrplay/sdrplay.h \
	                   ./src/input/sdrplay/sdrplayselect.h
	SOURCES		+= ./src/input/sdrplay/sdrplay.cpp \
	                   ./src/input/sdrplay/sdrplayselect.cpp
	FORMS		+= ./src/input/sdrplay/sdrplay-widget.ui
}
#
#
# airspy support
#
airspy {
	DEFINES		+= HAVE_AIRSPY
	INCLUDEPATH	+= ./src/input/airspy \
	                    /usr/local/include/libairspy
	HEADERS		+= ./src/input/airspy/airspy-handler.h \
	                    /usr/local/include/libairspy/airspy.h
	SOURCES		+= ./src/input/airspy/airspy-handler.cpp 
	FORMS		+= ./src/input/airspy/airspy-widget.ui
}

#	extio dependencies, windows only
#
extio {
	DEFINES		+= HAVE_EXTIO
	INCLUDEPATH	+= ./src/input/extio-handler
	HEADERS		+= ./src/input/extio-handler/extio-handler.h \
	                   ./src/input/extio-handler/common-readers.h \
	                   ./src/input/extio-handler/virtual-reader.h
	SOURCES		+= ./src/input/extio-handler/extio-handler.cpp \
	                   ./src/input/extio-handler/common-readers.cpp \
	                   ./src/input/extio-handler/virtual-reader.cpp
}

tcp-streamer	{
	DEFINES		+= TCP_STREAMER
	QT		+= network
	HEADERS		+= ./includes/output/tcp-streamer.h
	SOURCES		+= ./src/output/tcp-streamer.cpp
}

#
rtl_tcp {
	DEFINES		+= HAVE_RTL_TCP
	QT		+= network
	INCLUDEPATH	+= ./src/input/rtl_tcp
	HEADERS		+= ./src/input/rtl_tcp/rtl_tcp_client.h
	SOURCES		+= ./src/input/rtl_tcp/rtl_tcp_client.cpp
	FORMS		+= ./src/input/rtl_tcp/rtl_tcp-widget.ui
}

tcp-streamer    {
        DEFINES         += TCP_STREAMER
        QT              += network
        HEADERS         += ./includes/output/tcp-streamer.h
        SOURCES         += ./src/output/tcp-streamer.cpp
}

try-epg	{
	DEFINES		+= TRY_EPG
	Qt		+= xml
	DEPENDPATH	+= ./src/backend/data/epg \
	                   ./includes/backend/data/epg 
	INCLUDEPATH	+= ./includes/backend/data/epg 
	HEADERS		+= ./includes/backend/data/epg/epgdec.h 
	SOURCES		+= ./src/backend/data/epg/epgdec.cpp 
}

