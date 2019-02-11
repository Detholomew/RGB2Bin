#-------------------------------------------------
#
# Project created by QtCreator 2019-02-11T03:02:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RGB2Bin_Green
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
        kernel.cu

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui


## OpenCV401
DEPENDPATH += $$PWD\..\OPENCV
INCLUDEPATH += $$PWD\..\OPENCV
CONFIG(release, debug|release) {
    LIBS += -L$$PWD\..\OPENCV -lopencv_world401
}
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD\..\OPENCV -lopencv_world401d
}


#----------------------------------------------------------------
#-------------------------Cuda setup-----------------------------
#----------------------------------------------------------------

# project build directories
DESTDIR     = $$PWD
OBJECTS_DIR = $$DESTDIR/obj

# C++ flags
QMAKE_CXXFLAGS_RELEASE =-O3

# Cuda sources
CUDA_SOURCES += kernel.cu

# Path to cuda toolkit install
CUDA_DIR      = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.0"

# Path to header and libs files
INCLUDEPATH  += $$CUDA_DIR/include  \
                    "C:\ProgramData\NVIDIA Corporation\CUDA Samples\v10.0\common\inc"
QMAKE_LIBDIR += $$CUDA_DIR/lib/x64

SYSTEM_TYPE = 64            # '32' or '64', depending on your system

# libs used in your code
LIBS += -lcuda -lcudart

# GPU architecture
CUDA_ARCH     = sm_30

# Here are some NVCC flags I've always used by default.
NVCCFLAGS     = --use_fast_math


# Prepare the extra compiler configuration (taken from the nvidia forum - i'm not an expert in this part)
CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')


# MSVCRT link option (static or dynamic, it must be the same with your Qt SDK link option)
MSVCRT_LINK_FLAG_DEBUG = "/MDd"
MSVCRT_LINK_FLAG_RELEASE = "/MD"


# Tell Qt that we want add more stuff to the Makefile
QMAKE_EXTRA_COMPILERS += cuda

# Configuration of the Cuda compiler
CONFIG(debug, debug|release) {
    # Debug mode
    cuda_d.input = CUDA_SOURCES
    cuda_d.output = $$OBJECTS_DIR/${QMAKE_FILE_BASE}.obj
    cuda_d.commands = $$CUDA_DIR/bin/nvcc.exe -D_DEBUG $$NVCC_OPTIONS $$CUDA_INC $$LIBS --machine $$SYSTEM_TYPE \
                     -arch=$$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME} -Xcompiler $$MSVCRT_LINK_FLAG_DEBUG
    cuda_d.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda_d
}
else {
    # Release mode
    cuda.input = CUDA_SOURCES
    cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}.obj
    cuda.commands = $$CUDA_DIR/bin/nvcc.exe $$NVCC_OPTIONS $$CUDA_INC $$LIBS --machine $$SYSTEM_TYPE \
                    -arch=$$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME} -Xcompiler $$MSVCRT_LINK_FLAG_RELEASE
    cuda.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda
}

DISTFILES += \
    kernel.cu


