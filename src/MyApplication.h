#pragma once

#include <QObject>
#include <QApplication>
#include <QSystemTrayIcon>

#include "MainWindow.h"

class MyApplication : public QApplication
{
    Q_OBJECT
public:
    MyApplication(int argc, char** argv);

private slots:
    void trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason);
    void exitSlot();

private:
    QSystemTrayIcon* mTrayIcon = nullptr;
    QMenu* mTrayMenu = nullptr;
    MainWindow* mMainWindow = nullptr;
};

