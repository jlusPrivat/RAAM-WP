QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
    controller/language.cpp \
    controller/maincontroller.cpp \
    main.cpp \
    model/outputdevice.cpp \
    view/aboutview.cpp \
    view/clientview.cpp \
    view/mainwindow.cpp \
    model/audiomanager.cpp \
    view/settingsview.cpp

HEADERS += \
    controller/language.h \
    controller/maincontroller.h \
    model/outputdevice.h \
    view/aboutview.h \
    view/clientview.h \
    view/mainwindow.h \
    model/audiomanager.h \
    utils/SafeRelease.h \
    utils/makros.h \
    view/settingsview.h

LIBS += \
    "$$PWD\..\libs\Ole32.Lib" \
    "$$PWD\..\libs\mmdevapi.lib" \
    "$$PWD\..\libs\Uuid.Lib"

RESOURCES += \
    "$$PWD\..\resources\resources.qrc"

TRANSLATIONS += \
    "$$PWD\..\resources\translations\raam-wp_en.ts" \
    "$$PWD\..\resources\translations\raam-wp_de.ts"

RC_ICONS = "$$PWD\..\resources\imgs\logo.ico"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
