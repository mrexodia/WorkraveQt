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
    mLastTimeout = time(nullptr);
    connect(mTickTimer, &QTimer::timeout, this, &MainWindow::tickTimeoutSlot);
    mTickTimer->start();

    mTimerDialog->setMicroBreakMaximum(mConfiguration.mMicroBreakCycle);
    mTimerDialog->setRestBreakMaximum(mConfiguration.mRestBreakCycle);
    mTimerDialog->setIdleMaximum(mConfiguration.mMicroBreakDuration);

    // Add suggestions

    QFile file("suggestions.json");
    if(file.open(QIODevice::ReadOnly))
    {
        QString read = file.readAll();

        QJsonDocument doc = QJsonDocument::fromJson(read.toUtf8());
        QJsonObject obj = doc.object();

        for (auto suggestion : obj.value(QString("miniBreakIdeas")).toObject())
        {
            mMicroBreakSuggestions.append(suggestion.toObject()["text"].toString());
        }

        for (auto suggestion : obj.value(QString("longBreakIdeas")).toObject())
        {
            mMicroBreakSuggestions.append(suggestion.toObject()["text"].toString());
        }
    }
    else
    {
        qDebug() << "Failed to open suggestions.json, using default suggestions";
    }

    // Make sure there is always at least one suggestion
    if(mMicroBreakSuggestions.empty())
    {
        mMicroBreakSuggestions.append(tr("Stand up and get a glass of water"));
        mMicroBreakSuggestions.append(tr("Take off your glasses and relax your eyes")); // mrfearless
        mMicroBreakSuggestions.append(tr("Switch your desk between sitting and standing"));
    }
    if(mRestBreakSuggestions.empty())
    {
        mRestBreakSuggestions.append(tr("Make a cup of tea!"));
        mRestBreakSuggestions.append(tr("Do some streches"));
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
    // Store the absolute time we last executed this callback. If the computer locks
    // we can use this to calculate the time drift since the last callback.
    auto previousTimeout = mLastTimeout;
    mLastTimeout = time(nullptr);

    // Detect idle time and time drift
    auto timeDrift = (int)(mLastTimeout - previousTimeout);
    auto hasTimeDrifted = timeDrift > 2;
    auto idleTime = Helpers::getIdleTimeS();
    if(hasTimeDrifted)
    {
        qDebug() << "Detected time drift (lock screen/standby?):" << timeDrift << "idleTime:" << idleTime;
        idleTime = qMax(idleTime, timeDrift);
    }
    else
    {
        timeDrift = 1;
    }

    if(mPaused)
    {
        // Restart the application if the user has been idle for longer than a rest break,
        // this prevents a permanent pause state if the user goes to sleep.
        if(idleTime > mConfiguration.mRestBreakDuration)
        {
            Helpers::restartApplication();
        }

        // Ignore all timer events when paused
        return;
    }

    if(mProcessDialog->isGameRunning())
    {
        // Hide the break dialog if a game started running
        if(mBreakDialog->isVisible())
        {
            mBreakDialog->showNormal();
            mBreakDialog->hide();
        }

        // TODO: make it so the break dialog will pop up as soon as the game stops running

        // Ignore all timer events when a game is running
        return;
    }

    auto resetRestBreak = [this]()
    {
        mActiveBreak = BreakType::None;
        mRestBreakTick = 0;
        mBreakDialog->showNormal();
        mBreakDialog->hide();
        setBlocked(false);
        mTimerDialog->setRestBreakMaximum(mConfiguration.mRestBreakCycle);
        mTimerDialog->setMicroBreakMaximum(mConfiguration.mMicroBreakCycle);
    };

    auto resetMicroBreak = [this]()
    {
        mActiveBreak = BreakType::None;
        mMicroBreakTick = 0;
        mBreakDialog->showNormal();
        mBreakDialog->hide();
        setBlocked(false);
        mTimerDialog->setMicroBreakMaximum(mConfiguration.mMicroBreakCycle);
    };

    auto startBreak = [this](BreakType type)
    {
        mActiveBreak = type;
        switch(type)
        {
        case BreakType::Micro:
            mBreakDialog->setBreakDuration(mConfiguration.mMicroBreakDuration);
            mBreakDialog->setSuggestion(mMicroBreakSuggestions[mSuggestionIndex++ % mMicroBreakSuggestions.size()]);
            break;
        case BreakType::Rest:
            mBreakDialog->setBreakDuration(mConfiguration.mRestBreakDuration);
            mBreakDialog->setSuggestion(mRestBreakSuggestions[mSuggestionIndex++ % mRestBreakSuggestions.size()]);
            break;
        case BreakType::None:
            QMessageBox::critical(this, tr("Error"), "Cannot start break with BreakType::None");
            return;
        }

        mBreakDialog->setWindowTitle(breakTypeName(type));
        mBreakDialog->move(0, 0);
        mBreakDialog->showMaximized();

        setBlocked(true);
    };

    auto notifyBreak = [this](BreakType type)
    {
        int secondsLeft = 0;
        switch(type)
        {
        case BreakType::Micro:
        {
            secondsLeft = mConfiguration.mMicroBreakCycle - mConfiguration.mMicroBreakNotification;
            const int secondsUntilRestBreakNotification = mConfiguration.mRestBreakNotification - mRestBreakTick;
            if(secondsUntilRestBreakNotification <= secondsLeft)
            {
                // Skip the micro break notification if a rest break notification comes right away
                return;
            }
        }
            break;
        case BreakType::Rest:
            secondsLeft = mConfiguration.mRestBreakCycle - mConfiguration.mRestBreakNotification;
            break;
        case BreakType::None:
            QMessageBox::critical(this, tr("Error"), "Cannot notify break with BreakType::None");
            return;
        }

        auto name = breakTypeName(type);
        QString message = tr("%1 in %2 seconds").arg(name).arg(secondsLeft);
        if(secondsLeft == 1)
        {
            message = tr("%1 in 1 second").arg(name);
        }
        mTrayIcon->showMessage(QString(), message, QSystemTrayIcon::Information, 5000);
    };

    //qDebug() << "mMicroBreakTick:" << mMicroBreakTick << "mRestBreakTick:" << mRestBreakTick << "idleTime:" << idleTime << "idleState:" << (int)mIdleState;

    auto isIdle = idleTime > mConfiguration.mIdleThreshold;
    switch(mActiveBreak)
    {
    case BreakType::None:
    {
        // Idle timer state management
        if(idleTime > mConfiguration.mRestBreakDuration)
        {
            if(mIdleState != BreakType::None)
            {
                resetRestBreak();
                mIdleState = BreakType::None;
                mTimerDialog->setIdleMaximum(0);
            }

            // Spin the progress bar
            mTimerDialog->setIdleProgress(0);
        }
        else if(idleTime > mConfiguration.mMicroBreakDuration)
        {
            if(mIdleState != BreakType::Rest)
            {
                resetMicroBreak();
                mIdleState = BreakType::Rest;
                mTimerDialog->setIdleMaximum(mConfiguration.mRestBreakDuration);
            }
            mTimerDialog->setIdleProgress(idleTime);
        }
        else
        {
            if(mIdleState != BreakType::Micro)
            {
                mIdleState = BreakType::Micro;
                mTimerDialog->setIdleMaximum(mConfiguration.mMicroBreakDuration);
            }
            mTimerDialog->setIdleProgress(idleTime);
        }

        // Advance the break ticks (taking into account possible time drift)
        if(!isIdle)
        {
            mMicroBreakTick += timeDrift;
            mRestBreakTick += timeDrift;
        }

        // Once the rest break notification is shown, reset the micro break timer
        // NOTE: this is an attempt to fix overlapping breaks
        if(mRestBreakTick >= mConfiguration.mRestBreakNotification)
        {
            mMicroBreakTick = 0;
            mTimerDialog->setMicroBreakMaximum(0);
        }

        // Update the micro break state
        assert(mMicroBreakTick <= mConfiguration.mMicroBreakCycle);
        mTimerDialog->setMicroBreakProgress(mMicroBreakTick);
        if(mMicroBreakTick >= mConfiguration.mMicroBreakCycle)
        {
            mMicroBreakTick = 0;
            startBreak(BreakType::Micro);
            mTimerDialog->setMicroBreakMaximum(0);
        }
        else if(mMicroBreakTick == mConfiguration.mMicroBreakNotification && !isIdle)
        {
            notifyBreak(BreakType::Micro);
        }

        // Update the rest break state
        mTimerDialog->setRestBreakProgress(mRestBreakTick);
        if(mRestBreakTick >= mConfiguration.mRestBreakCycle)
        {
            mRestBreakTick = 0;
            startBreak(BreakType::Rest);
            mTimerDialog->setRestBreakMaximum(0);
        }
        else if(mRestBreakTick == mConfiguration.mRestBreakNotification && !isIdle)
        {
            notifyBreak(BreakType::Rest);
        }
    }
    break;

    case BreakType::Micro:
    {
        // Advance micro break progress
        mMicroBreakTick += timeDrift;
        mBreakDialog->setBreakProgress(mMicroBreakTick);
        if(mMicroBreakTick >= mConfiguration.mRestBreakDuration)
        {
            // NOTE: This can happen if the time drift was very long
            resetRestBreak();
            mMicroBreakTick = 0;
        }
        else if(mMicroBreakTick >= mConfiguration.mMicroBreakDuration)
        {
            resetMicroBreak();
        }
    }
    break;

    case BreakType::Rest:
    {
        // Advance rest break progress
        mRestBreakTick += timeDrift;
        mBreakDialog->setBreakProgress(mRestBreakTick);
        if(mRestBreakTick >= mConfiguration.mRestBreakDuration)
        {
            resetRestBreak();
        }
    }
    break;
    }
}

void MainWindow::on_actionExit_triggered()
{
    if(!mConfiguration.mIsTestConfiguration && mBlocked-- > 0)
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

QString MainWindow::breakTypeName(BreakType type) const
{
    switch(type)
    {
    case BreakType::None:
        return tr("No break");
    case BreakType::Micro:
        return tr("Micro break");
    case BreakType::Rest:
        return tr("Rest break");
    }
    return "<invalid>";
}

void MainWindow::on_action_Game_whitelist_triggered()
{
    mProcessDialog->show();
}
