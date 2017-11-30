TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    fileinfolist.cpp \
    filetransfer.cpp \
    retransmissionthread.cpp \
    slidingwindow.cpp \
    udpservice.cpp \
    timer.c

DISTFILES += \
    UDP.pro.user

HEADERS += \
    fileinfolist.h \
    filetransfer.h \
    message.h \
    retransmissionthread.h \
    slidingwindow.h \
    sysglobalparam.h \
    types.h \
    udpservice.h

