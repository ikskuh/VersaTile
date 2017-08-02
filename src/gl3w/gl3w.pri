SOURCES += \
	$$PWD/src/gl3w.c

INCLUDEPATH += $$PWD/include
DEPENDPATH  += $$PWD/include

win32 {
    LIBS += -lOpenGL32
}
!win32 {
    LIBS += -ldl -lGL
}
