TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Sources/backend.cpp \
        Sources/main.cpp \
        Sources/sr_lz4.cpp \
        Sources/sr_processor.cpp \
        Sources/sr_tar.cpp

MOC_DIR = Build/.moc
RCC_DIR = Build/.rcc
OBJECTS_DIR = Build/.obj

LIBS += -lKernel32 -lUser32 -lole32 \
             -luuid -loleaut32 -loleacc \
             -lDwmapi -lPsapi -lSetupapi \
             -lPowrProf -lPdh

HEADERS += \
    Sources/backend.h \
    Sources/sr_lz4.h \
    Sources/sr_processor.h \
    Sources/sr_tar.h
