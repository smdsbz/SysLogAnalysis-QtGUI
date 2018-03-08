#-------------------------------------------------
#
# Project created by QtCreator 2018-03-05T22:40:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SyslogAnalysis
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    utils/LogClass/LogDate.cpp \
    utils/LogClass/LogMessage.cpp \
    utils/LogClass/LogRecord.cpp \
    utils/LogClass/LogRecord_iterator.cpp \
    datequerydialog.cpp \
    stringquerydialog.cpp \
    createlogrecorddialog.cpp \
    analysisdisplaydialog.cpp

HEADERS += \
        mainwindow.h \
    utils/FPTree/FPTree.h \
    utils/HashTable/HashCell.h \
    utils/HashTable/HashTable.h \
    utils/LogClass/LogClass.h \
    utils/StorageGraph/StorageGraph.h \
    datequerydialog.h \
    stringquerydialog.h \
    createlogrecorddialog.h \
    analysisdisplaydialog.h

FORMS += \
        mainwindow.ui \
    datequerydialog.ui \
    stringquerydialog.ui \
    createlogrecorddialog.ui \
    analysisdisplaydialog.ui
