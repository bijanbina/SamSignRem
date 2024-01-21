TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Sources/main.cpp \
        Sources/sr_backend.cpp

MOC_DIR = Build/.moc
RCC_DIR = Build/.rcc
OBJECTS_DIR = Build/.obj

HEADERS += \
    Sources/sr_backend.h
