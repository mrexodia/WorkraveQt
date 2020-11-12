#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Helpers.h"

#include <QCloseEvent>
#include <QEvent>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(bool testConfiguration, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mTrayIcon(new QSystemTrayIcon(this))
    , mPreferencesDialog(new PreferencesDialog(&mConfiguration, nullptr))
    , mTimerDialog(new TimerDialog(this))
    , mBreakDialog(new BreakDialog(nullptr))
{
    if(testConfiguration)
    {
        mConfiguration.mMicroBreakCycle = 10;
        mConfiguration.mMicroBreakNotification = mConfiguration.mMicroBreakCycle - 5;
        mConfiguration.mMicroBreakDuration = 15;

        mConfiguration.mRestBreakCycle = 60;
        mConfiguration.mRestBreakNotification = mConfiguration.mRestBreakCycle - 20;
        mConfiguration.mRestBreakDuration = 20;
    }
    else
    {
        mConfiguration.load();
    }
    mConfiguration.dump();

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

    assert(mConfiguration.mMicroBreakCycle > mConfiguration.mMicroBreakNotification);
    //assert(mConfiguration.mMicroBreakCycle - mConfiguration.mMicroBreakNotification < mConfiguration.mMicroBreakDuration);

    mTimerDialog->setMicroBreakMaximum(mConfiguration.mMicroBreakCycle);
    mTimerDialog->setRestBreakMaximum(mConfiguration.mRestBreakCycle);
    mTimerDialog->setIdleMaximum(mConfiguration.mMicroBreakDuration);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger)
    {
        if(mTimerDialog->isVisible())
        {
            mTimerDialog->setVisible(false);
        }
        else
        {
            mTimerDialog->setVisible(true);
        }
    }
}

void MainWindow::tickTimeoutSlot()
{
    if(mPaused)
        return;

    auto resetRestBreak = [this]()
    {
        mInRestBreak = false;
        mRestBreakTick = 0;
        mBreakDialog->hide();
        setBlocked(false);
        mTimerDialog->setRestBreakMaximum(mConfiguration.mRestBreakCycle);
        mTimerDialog->setMicroBreakMaximum(mConfiguration.mMicroBreakCycle);
    };

    auto resetMicroBreak = [this]()
    {
        mInMicroBreak = false;
        mMicroBreakTick = 0;
        mBreakDialog->hide();
        setBlocked(false);
        mTimerDialog->setMicroBreakMaximum(mConfiguration.mMicroBreakCycle);
    };

    // TODO: move to settings?
    constexpr int idleThreshold = 5;

    auto idleTime = Helpers::getIdleTimeMs();
    auto isIdle = idleTime > idleThreshold;

    if(!mInRestBreak && !mInMicroBreak && isIdle)
    {
        // The user is idle, pause other timers
        if(idleTime > mConfiguration.mMicroBreakDuration && mIdleMaximum != mConfiguration.mRestBreakDuration)
        {
            resetMicroBreak();
            mIdleMaximum = mConfiguration.mRestBreakDuration;
            mTimerDialog->setIdleMaximum(mIdleMaximum);
        }
        if(idleTime > mConfiguration.mRestBreakDuration && mIdleMaximum != 0)
        {
            resetRestBreak();
            mIdleMaximum = 0;
            mTimerDialog->setIdleMaximum(mIdleMaximum);
        }
    }
    else
    {
        mMicroBreakTick += 1;
        mRestBreakTick += 1;
    }

    if(idleTime <= idleThreshold)
    {
        mIdleMaximum = mConfiguration.mMicroBreakDuration;
        mTimerDialog->setIdleMaximum(mIdleMaximum);
    }

    if(mIdleMaximum != 0 && !mInMicroBreak && !mInRestBreak)
    {
        mTimerDialog->setIdleProgress(idleTime);
    }

    qDebug() << "mMicroBreakTick" << mMicroBreakTick << "mRestBreakTick" << mRestBreakTick << "idleTime" << idleTime;

    auto startBreak = [this](const QString& type, int duration)
    {
        mBreakDialog->setBreakDuration(duration);
        mBreakDialog->setWindowTitle(type);
        mBreakDialog->move(0, 0);
        mBreakDialog->showMaximized();
        setBlocked(true);
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
        if(mRestBreakTick == mConfiguration.mRestBreakDuration)
        {
            resetRestBreak();
        }
    }
    else
    {
        assert(mRestBreakTick <= mConfiguration.mRestBreakCycle);

        // Once the rest break notification is shown, reset the micro break timer
        if(mRestBreakTick >= mConfiguration.mRestBreakNotification)
        {
            mMicroBreakTick = 0;
            mTimerDialog->setMicroBreakMaximum(0);
        }

        auto breakType = tr("Rest break");
        mTimerDialog->setRestBreakProgress(mRestBreakTick);
        if(mRestBreakTick == mConfiguration.mRestBreakCycle)
        {
            mInRestBreak = true;
            mRestBreakTick = 0;
            startBreak(breakType, mConfiguration.mRestBreakDuration);
            mTimerDialog->setRestBreakMaximum(0);
        }
        else if(mRestBreakTick == mConfiguration.mRestBreakNotification && !isIdle)
        {
            const int secondsLeft = mConfiguration.mRestBreakCycle - mConfiguration.mRestBreakNotification;
            notifyBreak(breakType, secondsLeft);
        }
    }

    if(mInMicroBreak)
    {
        assert(!mInRestBreak);
        mRestBreakTick -= 1;

        mBreakDialog->setBreakProgress(mMicroBreakTick);
        if(mMicroBreakTick == mConfiguration.mMicroBreakDuration)
        {
            resetMicroBreak();
        }
    }
    else
    {
        assert(mMicroBreakTick <= mConfiguration.mMicroBreakCycle);

        auto breakType = tr("Micro break");
        mTimerDialog->setMicroBreakProgress(mMicroBreakTick);
        if(mMicroBreakTick == mConfiguration.mMicroBreakCycle)
        {
            mInMicroBreak = true;
            mMicroBreakTick = 0;
            startBreak(breakType, mConfiguration.mMicroBreakDuration);
            mTimerDialog->setMicroBreakMaximum(0);
        }
        else if(mMicroBreakTick == mConfiguration.mMicroBreakNotification && !isIdle)
        {
            const int secondsUntilRestBreakNotification = mConfiguration.mRestBreakNotification - mRestBreakTick;
            const int secondsLeft = mConfiguration.mMicroBreakCycle - mConfiguration.mMicroBreakNotification;
            if(secondsUntilRestBreakNotification > secondsLeft)
            {
                notifyBreak(breakType, secondsLeft);
            }
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    if(mBlocked-- > 0)
        return;
    mTimerDialog->close();
    mBreakDialog->setForceClose();
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

void MainWindow::on_actionPause_triggered()
{
    mPaused = !mPaused;
    ui->actionPause->setText(mPaused ? tr("Resume") : tr("Pause"));
}

void MainWindow::setBlocked(bool blocked)
{
    mBlocked = blocked ? 10 : 0;
}
