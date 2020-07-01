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

    assert(mMicroBreakCycle > mMicroBreakNotification);
    assert(mMicroBreakCycle - mMicroBreakNotification < mMicroBreakDuration);

    mTimerDialog->setMicroBreakMaximum(mMicroBreakCycle);
    mTimerDialog->setRestBreakMaximum(mRestBreakCycle);
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
    mRestBreakTick += 1;
    qDebug() << "mMicroBreakTick" << mMicroBreakTick << "mRestBreakTick" << mRestBreakTick;

    auto startBreak = [this](const QString& type, int duration)
    {
        mBreakDialog->setBreakDuration(duration);
        mBreakDialog->setWindowTitle(type);
        mBreakDialog->show();
        mBreakDialog->activateWindow();
        mBreakDialog->setFocus();
    };

    auto notifyBreak = [this](const QString& type, int secondsLeft)
    {
        QString message = tr("%1 in %2 seconds").arg(type).arg(secondsLeft);
        if(secondsLeft == 1)
            message = tr("%1 in 1 second").arg(type);
        mTrayIcon->showMessage(QString(), message, QSystemTrayIcon::Information, 1000);
    };

    if(mInRestBreak)
    {
        assert(!mInMicroBreak);
        mMicroBreakTick -= 1;

        mBreakDialog->setBreakProgress(mRestBreakTick);
        if(mRestBreakTick == mRestBreakDuration)
        {
            mInRestBreak = false;
            mRestBreakTick = 0;
            mBreakDialog->hide();
            mTimerDialog->setRestBreakMaximum(mRestBreakCycle);
            mTimerDialog->setMicroBreakMaximum(mMicroBreakCycle);
        }
    }
    else
    {
        assert(mRestBreakTick <= mRestBreakCycle);

        // Once the rest break notification is shown, reset the micro break timer
        if(mRestBreakTick >= mRestBreakNotification)
        {
            mMicroBreakTick = 0;
            mTimerDialog->setMicroBreakMaximum(0);
        }

        auto breakType = tr("Rest break");
        mTimerDialog->setRestBreakProgress(mRestBreakTick);
        if(mRestBreakTick == mRestBreakCycle)
        {
            mInRestBreak = true;
            mRestBreakTick = 0;
            startBreak(breakType, mRestBreakDuration);
            mTimerDialog->setRestBreakMaximum(0);
        }
        else if(mRestBreakTick == mRestBreakNotification)
        {
            const int secondsLeft = mRestBreakCycle - mRestBreakNotification;
            notifyBreak(breakType, secondsLeft);
        }
    }

    if(mInMicroBreak)
    {
        assert(!mInRestBreak);
        mRestBreakTick -= 1;

        mBreakDialog->setBreakProgress(mMicroBreakTick);
        if(mMicroBreakTick == mMicroBreakDuration)
        {
            mInMicroBreak = false;
            mMicroBreakTick = 0;
            mBreakDialog->hide();
            mTimerDialog->setMicroBreakMaximum(mMicroBreakCycle);
        }
    }
    else
    {
        assert(mMicroBreakTick <= mMicroBreakCycle);

        auto breakType = tr("Micro break");
        mTimerDialog->setMicroBreakProgress(mMicroBreakTick);
        if(mMicroBreakTick == mMicroBreakCycle)
        {
            mInMicroBreak = true;
            mMicroBreakTick = 0;
            startBreak(breakType, mMicroBreakDuration);
            mTimerDialog->setMicroBreakMaximum(0);
        }
        else if(mMicroBreakTick == mMicroBreakNotification)
        {
            const int secondsUntilRestBreakNotification = mRestBreakNotification - mRestBreakTick;
            const int secondsLeft = mMicroBreakCycle - mMicroBreakNotification;
            if(secondsUntilRestBreakNotification > secondsLeft)
            {
                notifyBreak(breakType, secondsLeft);
            }
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
