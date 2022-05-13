
QT       += core gui network multimedia
win32 {
INCLUDEPATH += "C:\dev\lib\rapidjson-1.1.0\include"
}

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
    aboutui.cpp \
    core/Biots.cpp \
    core/Brain.cpp \
    core/Connect.cpp \
    core/Environ.cpp \
    core/Etools.cpp \
    core/Genotype.cpp \
    core/PosAndSpeed.cpp \
    core/Rand.cpp \
    core/Settings.cpp \
    core/SoundRegistry.cpp \
    dockviewbiot.cpp \
    environmentarea.cpp \
    main.cpp \
    mainwindow.cpp \
    networking.cpp \
    networkui.cpp \
    settingsbiot.cpp \
    settingshabitat.cpp \
    settingsnetwork.cpp \
    settingsui.cpp

HEADERS += \
    aboutui.h \
    core/Biots.h \
    core/Brain.h \
    core/Connect.h \
    core/Environ.h \
    core/Etools.h \
    core/Genotype.h \
    core/PosAndSpeed.h \
    core/Rand.h \
    core/Settings.h \
    core/SoundRegistry.h \
    core/json.h \
    dockviewbiot.h \
    environmentarea.h \
    mainwindow.h \
    networking.h \
    networkui.h \
    settingsbiot.h \
    settingshabitat.h \
    settingsnetwork.h \
    settingsui.h

FORMS += \
    aboutui.ui \
    dockviewbiot.ui \
    mainwindow.ui \
    networkui.ui \
    settingsbiot.ui \
    settingshabitat.ui \
    settingsnetwork.ui \
    settingsui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ui.qrc

RC_ICONS = res/icon.ico
