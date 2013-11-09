#-------------------------------------------------
#
# Project created by QtCreator 2013-11-05T01:30:56
#
#-------------------------------------------------

QT       += core gui sql

TARGET = chouti
TEMPLATE = app

CONFIG += qaxcontainer

#ICON = ./images/computer.png

SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp \
    newtestform.cpp \
    newsubjectform.cpp \
    manageuserform.cpp \
    docreadwriter.cpp

HEADERS  += mainwindow.h \
    logindialog.h \
    newtestform.h \
    newsubjectform.h \
    manageuserform.h \
    docreadwriter.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    newtestform.ui \
    newsubjectform.ui \
    manageuserform.ui

OTHER_FILES += \
    ReadMe.txt \
    logo.rc \
    favicon.ico \
    question.sql \
    paper.sql \
    subject.sql

RESOURCES += \
    res.qrc

RC_FILE += \
    logo.rc
