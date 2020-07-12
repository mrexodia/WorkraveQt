#include "MainWindow.h"
#include "ui_MainWindow.h"

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
    if(!testConfiguration)
        mConfiguration.load();
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
    if(mPaused)
        return;

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
        if(mRestBreakTick == mConfiguration.mRestBreakDuration)
        {
            mInRestBreak = false;
            mRestBreakTick = 0;
            mBreakDialog->hide();
            mTimerDialog->setRestBreakMaximum(mConfiguration.mRestBreakCycle);
            mTimerDialog->setMicroBreakMaximum(mConfiguration.mMicroBreakCycle);
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
        else if(mRestBreakTick == mConfiguration.mRestBreakNotification)
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
            mInMicroBreak = false;
            mMicroBreakTick = 0;
            mBreakDialog->hide();
            mTimerDialog->setMicroBreakMaximum(mConfiguration.mMicroBreakCycle);
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
        else if(mMicroBreakTick == mConfiguration.mMicroBreakNotification)
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

void MainWindow::on_actionPause_triggered()
{
    mPaused = !mPaused;
    ui->actionPause->setText(mPaused ? tr("Resume") : tr("Pause"));
}
