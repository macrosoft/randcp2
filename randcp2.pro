#-------------------------------------------------
#
# Project created by QtCreator 2012-01-25T15:40:11
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = randcp2
TEMPLATE = app


SOURCES += main.cpp\
    sourcefiles.cpp \
    mainwindow.cpp \
    diskinfo.cpp \
    srcdiritemmodel.cpp \
    threadcopy.cpp \
    additionalpath.cpp \
    droplistwidget.cpp \
    droplineedit.cpp \
    settings.cpp \
    progresscontrol.cpp

HEADERS  += mainwindowdf.h \
    sourcefiles.h \
    mainwindow.h \
    srcdiritemmodel.h \
    diskinfo.h \
    threadcopy.h \
    additionalpath.h \
    droplistwidget.h \
    droplineedit.h \
    settings.h \
    progresscontrol.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += /usr/include/x86_64-linux-gnu

TRANSLATIONS = randcp2_ru.ts
