#-------------------------------------------------
#
# Project created by QtCreator 2017-04-24T21:05:26
#
#-------------------------------------------------

QT        += core gui

linux {
    CONFIG    += link_pkgconfig
    PKGCONFIG += assimp
}
win32 {
    LIBS += -L$$quote(C:\Users\Felix\Desktop\assimp\build\x64) assimp-vc140-mt.lib IrrXML.lib zlibstatic.lib
    INCLUDEPATH += $$quote(C:\Users\Felix\Desktop\assimp\include)
    DEPENDPATH  += $$quote(C:\Users\Felix\Desktop\assimp\include)
    INCLUDEPATH += $$quote(C:\Users\Felix\Desktop\assimp\build\include)
    DEPENDPATH  += $$quote(C:\Users\Felix\Desktop\assimp\build\include)

    INCLUDEPATH += $$quote(../glm/)
    DEPENDPATH += $$quote(../glm/)
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = versa-tile
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    modeleditorview.cpp \
    tilesetviewer.cpp \
    mesh.cpp \
    optionsdialog.cpp \
    openspritesheetdialog.cpp

HEADERS  += mainwindow.h \
    modeleditorview.h \
    tilesetviewer.h \
    mesh.h \
    optionsdialog.h \
    openspritesheetdialog.h

FORMS    += mainwindow.ui \
    optionsdialog.ui \
    openspritesheetdialog.ui

RESOURCES += \
    data.qrc

DISTFILES += \
	data/tilesets/requirements.txt

include(gl3w/gl3w.pri);
