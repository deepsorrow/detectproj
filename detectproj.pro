QT -= gui

CONFIG -= app_bundle
CONFIG += c++1z

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    detection.cpp \
    heatmapping.cpp \
    filemanagement.cpp \
    masking.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    detection.h \
    heatmapping.h \
    filemanagement.h \
    masking.h

INCLUDEPATH += C:\opencv\build\include
INCLUDEPATH += D:\mysql-connector8.0.3\include

LIBS += C:\opencv-build\bin\libopencv_core343.dll
LIBS += C:\opencv-build\bin\libopencv_highgui343.dll
LIBS += C:\opencv-build\bin\libopencv_imgcodecs343.dll
LIBS += C:\opencv-build\bin\libopencv_imgproc343.dll
LIBS += C:\opencv-build\bin\libopencv_features2d343.dll
LIBS += C:\opencv-build\bin\libopencv_calib3d343.dll
LIBS += C:\opencv-build\bin\libopencv_objdetect343.dll
LIBS += C:\opencv-build\bin\libopencv_videoio343.dll



# more correct variant, how set includepath and libs for mingw
# add system variable: OPENCV_SDK_DIR=D:/opencv/build
# read http://doc.qt.io/qt-5/qmake-variable-reference.html#libs

#INCLUDEPATH += $$(OPENCV_SDK_DIR)/include

#LIBS += -L$$(OPENCV_SDK_DIR)/x86/mingw/lib \
#        -lopencv_core320        \
#        -lopencv_highgui320     \
#        -lopencv_imgcodecs320   \
#        -lopencv_imgproc320     \
#        -lopencv_features2d320  \
#        -lopencv_calib3d320
