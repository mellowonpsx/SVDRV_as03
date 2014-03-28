TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    camera.cpp \
    angle.cpp

HEADERS += \
    camera.h \
    angle.h

#INCLUDEPATH += -L/usr/lib/
LIBS += -lglut
LIBS += -lGL
LIBS += -lGLU
LIBS += -lSDL
LIBS += -lSDL_image
