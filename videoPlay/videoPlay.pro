QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# 配置pkg-config
PKG_CONFIG = /usr/local/bin/pkg-config
CONFIG +=link_pkgconfig

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG+=sdk_no_version_check
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#如果在编译测试用例的时候出现下面这样的问题：
#error: symbol(s) not found for architecture x86_64
#error: linker command failed with exit code 1 (use -v to see invocation)

#那主要的原因还是库没有链接完整，第一个检查ffmpeg用到的库链接到了没有，如上面不能只写libavcodec.a，少了libavutil.a就有这样的问题。
#LIB_DIR=/usr/local/Cellar/ffmpeg/4.3.1_1/
#message("lib_dir is $$LIB_DIR")
INCLUDEPATH +=/usr/local/include

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    video.cpp \
    xvideowidget.cpp

HEADERS += \
    mainwindow.h \
    video.h \
    xvideowidget.h

FORMS += \
    mainwindow.ui


DESTDIR = ../bin
MOC_DIR = ../tmp/
RCC_DIR = ../tmp/
UI_DIR = ../tmp/
OBJECTS_DIR = ../tmp/

#PKGCONFIG += libavcodec libavutil libavformat libavdevice\
# libavresample libswresample  libavfilter libswscale
LIBS+=/usr/local/lib/libavcodec.dylib \
      /usr/local/lib/libavutil.dylib \
      /usr/local/lib/libavformat.dylib \
      /usr/local/lib/libavdevice.dylib \
      /usr/local/lib/libavresample.dylib \
      /usr/local/lib/libswresample.dylib \
      /usr/local/lib/libavfilter.dylib \
      /usr/local/lib/libswscale.dylib

#LIBS += "$$LIB_DIR/lib/libavcodec.a" \
#        "$$LIB_DIR/lib/libavutil.a" \
#        "$$LIB_DIR/lib/libavformat.a" \
#        "$$LIB_DIR/lib/libavdevice.a" \
#        "$$LIB_DIR/lib/libavresample.a" \
#        "$$LIB_DIR/lib/libswresample.a"\
#        "$$LIB_DIR/lib/libavfilter.a" \
#        "$$LIB_DIR/lib/libswscale.a" \
#        "$$LIB_DIR/lib/libpostproc.a"

TRANSLATIONS += \
    videoPlay_zh_CN.ts


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
