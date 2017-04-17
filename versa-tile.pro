TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += sdl2

include(/home/felix/projects/gl3w/gl3w.pri)

SOURCES += main.cpp \
    imgui/imgui_draw.cpp \
    imgui/imgui.cpp \
    imgui/imgui_impl_sdl_gl3.cpp

INCLUDEPATH += $$PWD/imgui/

HEADERS += \
    imgui/imconfig.h \
    imgui/imgui.h \
    imgui/imgui_impl_sdl_gl3.h \
    imgui/imgui_internal.h \
    imgui/stb_rect_pack.h \
    imgui/stb_textedit.h \
    imgui/stb_truetype.h

sdl2
{
  message(Include SDL2)
  QMAKE_CFLAGS   += `pkg-config --cflags sdl2`
  QMAKE_CXXFLAGS += `pkg-config --cflags sdl2`
  LIBS           += `pkg-config --libs sdl2`
}
