QT += core gui widgets concurrent

TARGET = ImageFilter
TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    filter2d.cpp \
    imageinfowidget.cpp \
    tiffcompressiondialog.cpp \
    tiffsaver.cpp \

HEADERS += \
    mainwindow.h \
    filter2d.h \
    imageinfowidget.h \
    tiffcompressiondialog.h \
    tiffsaver.h \

QMAKE_CXXFLAGS += -Wall -Wextra

# LibTIFF
unix {
    LIBS += -ltiff
    INCLUDEPATH += /usr/include
}

win32 {
    # Для Windows нужно скачать LibTIFF или использовать vcpkg
    # vcpkg install tiff:x64-windows
    LIBS += -ltiff
    INCLUDEPATH += C:/vcpkg/installed/x64-windows/include
    LIBPATH += C:/vcpkg/installed/x64-windows/lib
}

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

unix:!macx {
    target.path = /usr/local/bin
    INSTALLS += target
}
