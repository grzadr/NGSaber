#-------------------------------------------------
#
# Project created by QtCreator 2015-11-26T10:01:50
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NGSaber
TEMPLATE = app

QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

QMAKE_CXXFLAGS += -O2 -finline-small-functions -momit-leaf-frame-pointer

SOURCES += main.cpp\
	src/Windows/mainwindow.cpp \
    src/Windows/helpwindow.cpp \
    src/Windows/singlereadwindow.cpp \
    src/Windows/pairedreadwindow.cpp \
    src/Windows/processwindow.cpp \
	src/parameters.cpp \
    src/launcher.cpp \
    src/fastqfile.cpp \
    src/readsingle.cpp \
    src/fastqpair.cpp \
    src/readpaired.cpp \

HEADERS  += include/mainwindow.h \
    include/helpwindow.h \
    include/singlereadwindow.h \
    include/pairedreadwindow.h \
	include/processwindow.h \
    include/parameters.h \
    include/launcher.h \
    include/fastqfile.h \
    include/readsingle.h \
    include/fastqpair.h \
    include/readpaired.h \
    include/version.h

FORMS    += src/Windows/mainwindow.ui \
    src/Windows/helpwindow.ui \
    src/Windows/singlereadwindow.ui \
    src/Windows/pairedreadwindow.ui \
	src/Windows/processwindow.ui

RESOURCES += \
    images/images.qrc

DISTFILES +=
