#include "MyApplication.h"
#include <QDebug>
#include <QIcon>
#include <QMenu>

MyApplication::MyApplication(int argc, char** argv)
    : QApplication(argc, argv)
    , mTrayIcon(new QSystemTrayIcon(this))
    , mTrayMenu(new QMenu())
{
    mTrayMenu->addAction("E&xit", this, &MyApplication::exitSlot);
    mTrayIcon->setContextMenu(mTrayMenu);
    mTrayIcon->setIcon(QIcon(":/sheep.svg"));
    mTrayIcon->setToolTip(applicationName());
    mTrayIcon->show();
    connect(mTrayIcon, &QSystemTrayIcon::activated, this, &MyApplication::trayIconActivatedSlot);
    mMainWindow = new MainWindow();
}

void MyApplication::trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick && !mMainWindow->isVisible())
    {
        mMainWindow->showNormal();
        mMainWindow->activateWindow();
    }
}

void MyApplication::exitSlot()
{
    mMainWindow->exit();
}
