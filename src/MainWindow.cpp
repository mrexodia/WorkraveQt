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
    qDebug() << "mMicroBreakTick" << mMicroBreakTick;

    const int microBreakCycle = 10;
    const int microBreakNotification = microBreakCycle - 5;
    const int microBreakDuration = 15;
    assert(microBreakCycle > microBreakNotification);

    if(mInBreak)
    {
        mBreakDialog->setBreakProgress(mMicroBreakTick);
        if(mMicroBreakTick == microBreakDuration)
        {
            mInBreak = false;
            mBreakDialog->hide();
            mMicroBreakTick = 0;
        }
    }
    else
    {
        if(mMicroBreakTick == microBreakNotification)
        {
            auto secondsLeft = microBreakCycle - microBreakNotification;
            QString message = tr("Micro break in %1 seconds").arg(secondsLeft);
            if(secondsLeft == 1)
                message = tr("Micro break in 1 second");
            mTrayIcon->showMessage(QString(), message, QSystemTrayIcon::Information, 1000);
        }
        else if(mMicroBreakTick == microBreakCycle)
        {
            mMicroBreakTick = 0;
            mInBreak = true;
            mBreakDialog->setBreakDuration(microBreakDuration);
            mBreakDialog->setWindowTitle("Micro break");
            mBreakDialog->show();
            mBreakDialog->activateWindow();
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    mTimerDialog->close();
    mBreakDialog->close();
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
