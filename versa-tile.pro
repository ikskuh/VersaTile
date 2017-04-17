TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += sdl2 sdl2_image

include(/home/felix/projects/gl3w/gl3w.pri)
include(/home/felix/projects/nativefiledialog/nativefiledialog.pri)

SOURCES += main.cpp \
    imgui/imgui_draw.cpp \
    imgui/imgui.cpp \
    imgui/imgui_impl_sdl_gl3.cpp \
    system.cpp \
    mesheditor.cpp \
    texture.cpp \
    iconstorage.cpp \
    syslog.cpp \
    imext.cpp \
    glstate.cpp

INCLUDEPATH += $$PWD/imgui/

HEADERS += \
    imgui/imconfig.h \
    imgui/imgui.h \
    imgui/imgui_impl_sdl_gl3.h \
    imgui/imgui_internal.h \
    imgui/stb_rect_pack.h \
    imgui/stb_textedit.h \
    imgui/stb_truetype.h \
    imgui/system.h \
    mesheditor.h \
    texture.h \
    imext.h \
    iconstorage.h \
    syslog.h \
    glstate.h

sdl2
{
  message(Include SDL2)
  QMAKE_CFLAGS   += $$system(pkg-config --cflags sdl2 SDL2_image)
  QMAKE_CXXFLAGS += $$system(pkg-config --cflags sdl2 SDL2_image)
  LIBS           += $$system(pkg-config --libs sdl2 SDL2_image)
}
