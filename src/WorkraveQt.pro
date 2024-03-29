QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
