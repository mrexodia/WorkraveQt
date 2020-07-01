#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QCloseEvent>
#include <QEvent>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mTrayIcon(new QSystemTrayIcon(this))
    , mPreferencesDialog(new PreferencesDialog(nullptr))
    , mTimerDialog(new TimerDialog(this))
    , mBreakDialog(new BreakDialog(nullptr))
{
    ui->setupUi(this);
    mTrayIcon->setIcon(windowIcon());
    mTrayIcon->setContextMenu(ui->menuTray);
    mTrayIcon->setToolTip(QApplication::applicationName());
    mTrayIcon->show();
    connect(mTrayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivatedSlot);
    mTickTimer = new QTimer(this);
    mTickTimer->setInterval(1000);
    connect(mTickTimer, &QTimer::timeout, this, &MainWindow::tickTimeoutSlot);
    mTickTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
    {
        if(mTimerDialog->isVisible())
        {
            mTimerDialogGeometry = mTimerDialog->geometry();
            mTimerDialog->setVisible(false);
        }
        else
        {
            if(!mTimerDialogGeometry.isNull())
                mTimerDialog->setGeometry(mTimerDialogGeometry);
            mTimerDialog->setVisible(true);
        }
    }
}

void MainWindow::tickTimeoutSlot()
{
    mMicroBreakTick += 1;
    if(mMicroBreakTick == 10)
    {
        mTrayIcon->showMessage("Title", "Micro break in 10 seconds", QSystemTrayIcon::Information, 1000);
    }
    else if(mMicroBreakTick == 15)
    {
        // TODO: register a proper microbreak
        mMicroBreakTick = 0;
        mBreakDialog->show();
    }
    qDebug() << "timeout";
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionPreferences_triggered()
{
    mPreferencesDialog->show();
}

void MainWindow::on_actionOpen_triggered()
{
    mTimerDialog->show();
}

void MainWindow::on_actionBreak_triggered()
{
    mBreakDialog->show();
}
