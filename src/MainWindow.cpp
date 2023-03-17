#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Helpers.h"
#include "ProcessDialog.h"

#include <QCloseEvent>
#include <QEvent>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(bool testConfiguration, bool resetConfiguration, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mTrayIcon(new QSystemTrayIcon(this))
    , mPreferencesDialog(new PreferencesDialog(&mConfiguration, nullptr))
    , mTimerDialog(new TimerDialog(this))
    , mBreakDialog(new BreakDialog(nullptr))
    , mProcessDialog(new ProcessDialog(this))
{
    if ((mConfiguration.mIsTestConfiguration = testConfiguration))
    {
        mConfiguration.mMicroBreakCycle = 10;
        mConfiguration.mMicroBreakNotification = mConfiguration.mMicroBreakCycle - 5;
        mConfiguration.mMicroBreakDuration = 15;

        mConfiguration.mRestBreakCycle = 60;
        mConfiguration.mRestBreakNotification = mConfiguration.mRestBreakCycle - 20;
        mConfiguration.mRestBreakDuration = 20;
    }
    else if(resetConfiguration)
    {
        mConfiguration.save();
    }

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

    mTimerDialog->setMicroBreakMaximum(mConfiguration.mMicroBreakCycle);
    mTimerDialog->setRestBreakMaximum(mConfiguration.mRestBreakCycle);
    mTimerDialog->setIdleMaximum(mConfiguration.mMicroBreakDuration);

    // Add suggestions

    QFile file("suggestions.json");
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open suggestions.json, using default suggestions";

        mMicroBreakSuggestions.append(tr("Stand up and get a glass of water"));
        mMicroBreakSuggestions.append(tr("Take off your glasses and relax your eyes")); // mrfearless
        mRestBreakSuggestions.append(tr("Make a cup of tea!"));
    } else {
        QString read = file.readAll();

        QJsonDocument doc = QJsonDocument::fromJson(read.toUtf8());
        QJsonObject obj = doc.object();

        for (auto suggestion : obj.value(QString("miniBreakIdeas")).toObject()) {
            mMicroBreakSuggestions.append(suggestion.toObject()["text"].toString());
        }

        for (auto suggestion : obj.value(QString("longBreakIdeas")).toObject()) {
            mMicroBreakSuggestions.append(suggestion.toObject()["text"].toString());
        }
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason)
{
#ifdef Q_OS_WIN
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
#else
	Q_UNUSED(reason);
#endif // Q_OS_WIN
}

void MainWindow::tickTimeoutSlot()
{
    if(mPaused || mProcessDialog->isGameRunning())
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

    auto idleTime = Helpers::getIdleTimeS();
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

    //qDebug() << "mMicroBreakTick" << mMicroBreakTick << "mRestBreakTick" << mRestBreakTick << "idleTime" << idleTime;

    auto startBreak = [this](const QString& type, int duration)
    {
        mBreakDialog->setBreakDuration(duration);
        mBreakDialog->setWindowTitle(type);
        mBreakDialog->move(0, 0);
        mBreakDialog->showMaximized();
        mBreakDialog->setSuggestion("");

        if (mInMicroBreak) {
            mBreakDialog->setSuggestion(mMicroBreakSuggestions[mSuggestionIndex++ % mMicroBreakSuggestions.size()]);
        } else {
            mBreakDialog->setSuggestion(mRestBreakSuggestions[mSuggestionIndex++ % mRestBreakSuggestions.size()]);
        }

        setBlocked(true);
    };

    auto notifyBreak = [this](const QString& type, int secondsLeft)
    {
        QString message = tr("%1 in %2 seconds").arg(type).arg(secondsLeft);
        if(secondsLeft == 1)
            message = tr("%1 in 1 second").arg(type);
        mTrayIcon->showMessage(QString(), message, QSystemTrayIcon::Information, 5000);
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
    mPreferencesDialog->close();
    mTimerDialog->close();
    mBreakDialog->setForceClose();
    mBreakDialog->close();
    mProcessDialog->setForceClose();
    mProcessDialog->close();
    close();
#if defined(Q_OS_MAC)
    // NOTE: this is necessary on macos for some reason
    QApplication::exit();
#endif // Q_OS_MAC
}

void MainWindow::on_actionPreferences_triggered()
{
    mPreferencesDialog->show();
}

void MainWindow::on_actionOpen_triggered()
{
	if(mTimerDialog->isVisible())
	{
		mTimerDialog->hide();
	}
	else
	{
		mTimerDialog->show();
	}
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

void MainWindow::on_action_Game_whitelist_triggered()
{
    mProcessDialog->show();
}
