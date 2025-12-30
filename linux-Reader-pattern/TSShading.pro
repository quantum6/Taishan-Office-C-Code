#QT -= gui

TEMPLATE = lib
#TEMPLATE = app
DEFINES += TSSHADING_LIBRARY
QTPLUGIN += qjpeg
#CONFIG += c++11

VERSION = 1.0.3

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    tsshading.cpp \
    tso_appreader_read_util_TSShading.cpp

HEADERS += \
    TSShading_global.h \
    tsshading.h \
    tso_appreader_read_util_TSShading.h
win32 {
        INCLUDEPATH += windows_include
}
unix{
        INCLUDEPATH += linux_include
}

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

QMAKE_LFLAGS += "-Wl,-rpath=./lib"


#unix:!macx: LIBS += -L$$PWD/opt/Qt5.15.2/lib/ -llibqtlibpng



#unix:!macx: LIBS += -L$$PWD/../../../../opt/Qt5.15.2/lib/ -lqtlibpng

#INCLUDEPATH += $$PWD/../../../../opt/Qt5.15.2/include
#DEPENDPATH += $$PWD/../../../../opt/Qt5.15.2/include

#unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../opt/Qt5.15.2/lib/libqtlibpng.a
