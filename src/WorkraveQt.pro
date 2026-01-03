QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Use lowercase binary name on Linux (standard convention)
unix:!macx {
    TARGET = workraveqt
} else {
    TARGET = WorkraveQt
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

win32:VERSION = 0.1.0.0
else:VERSION = 0.1.0

win32:RC_ICONS = images/sheep.ico

macx {
    ICON = images/sheep.icns
    QMAKE_INFO_PLIST = Info.plist
    OBJECTIVE_SOURCES += macos_platform.mm
    SOURCES += macos_platform.h
    QMAKE_LFLAGS += -framework CoreGraphics -framework Carbon
}

# https://stackoverflow.com/a/58434395/1806760
QMAKE_TARGET_DESCRIPTION = "WorkraveQt"

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BreakDialog.cpp \
    Configuration.cpp \
    Helpers.cpp \
    PreferencesDialog.cpp \
    ProcessDialog.cpp \
    ProcessListThread.cpp \
    QTrayDialog.cpp \
    TimerDialog.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    BreakDialog.h \
    Configuration.h \
    Helpers.h \
    MainWindow.h \
    PreferencesDialog.h \
    ProcessDialog.h \
    ProcessListThread.h \
    QTrayDialog.h \
    TimerDialog.h

FORMS += \
    BreakDialog.ui \
    MainWindow.ui \
    PreferencesDialog.ui \
    ProcessDialog.ui \
    TimerDialog.ui

win32:LIBS += -luser32

# Installation rules
unix:!macx:!qnx:!android {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    
    # Binary installation
    target.path = $$PREFIX/bin
    INSTALLS += target
    
    # Desktop file
    desktop.path = $$PREFIX/share/applications
    desktop.files = ../data/workraveqt.desktop
    INSTALLS += desktop
    
    # Autostart desktop file (only for system installs - user installs handle this in script)
    equals(PREFIX, "/usr") | equals(PREFIX, "/usr/local") {
        autostart.path = /etc/xdg/autostart
        autostart.files = ../data/workraveqt-autostart.desktop
        INSTALLS += autostart
    }
    
    # AppStream metainfo
    metainfo.path = $$PREFIX/share/metainfo
    metainfo.files = ../data/io.workrave.WorkraveQt.metainfo.xml
    INSTALLS += metainfo
    
    # SVG icon (scalable)
    icon_scalable.path = $$PREFIX/share/icons/hicolor/scalable/apps
    icon_scalable.files = images/sheep.svg
    icon_scalable.extra = cp $$PWD/images/sheep.svg $(INSTALL_ROOT)$$PREFIX/share/icons/hicolor/scalable/apps/workraveqt.svg
    INSTALLS += icon_scalable
}

# Other platforms
qnx {
    target.path = /tmp/$${TARGET}/bin
    INSTALLS += target
}
macx {
    target.path = /Applications
    INSTALLS += target
}

RESOURCES += \
    resources.qrc
