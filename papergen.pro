#-------------------------------------------------
#
# Project created by QtCreator 2013-11-05T01:30:56
#
#-------------------------------------------------

QT       += core gui sql network

TARGET = papergen
TEMPLATE = app

CONFIG += qaxcontainer

ICON = ./images/computer.png

SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp \
    newtestform.cpp \
    newsubjectform.cpp \
    manageuserform.cpp \
    docreadwriter.cpp \
    firstsettingsdialog.cpp \
    question.cpp \
    newsubject.cpp \
    subjectmanager.cpp \
    newquestion.cpp \
    modifyquestion.cpp

HEADERS  += mainwindow.h \
    logindialog.h \
    newtestform.h \
    newsubjectform.h \
    manageuserform.h \
    docreadwriter.h \
    firstsettingsdialog.h \
    defs.h \
    question.h \
    newsubject.h \
    subjectmanager.h \
    newquestion.h \
    modifyquestion.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    newtestform.ui \
    newsubjectform.ui \
    manageuserform.ui \
    firstsettingsdialog.ui \
    newsubject.ui \
    subjectmanager.ui \
    newquestion.ui \
    modifyquestion.ui

OTHER_FILES += \
    ReadMe.txt \
    logo.rc \
    favicon.ico

RESOURCES += \
    res.qrc

RC_FILE += \
    logo.rc
