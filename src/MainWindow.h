#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

#include "PreferencesDialog.h"
#include "TimerDialog.h"
#include "BreakDialog.h"
#include "ProcessDialog.h"
#include "Configuration.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(bool testConfiguration, bool resetConfiguration, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason);
    void tickTimeoutSlot();

    void on_actionExit_triggered();
    void on_actionPreferences_triggered();
    void on_actionOpen_triggered();
    void on_actionBreak_triggered();
    void on_actionPause_triggered();
    void on_action_Game_whitelist_triggered();

private:
    void setBlocked(bool blocked);

private:
    Ui::MainWindow* ui = nullptr;
    QSystemTrayIcon* mTrayIcon = nullptr;
    QTimer* mTickTimer = nullptr;
    PreferencesDialog* mPreferencesDialog = nullptr;
    TimerDialog* mTimerDialog = nullptr;
    BreakDialog* mBreakDialog = nullptr;
    ProcessDialog* mProcessDialog = nullptr;
    int mMicroBreakTick = 0;
    bool mInMicroBreak = false;
    int mRestBreakTick = 0;
    bool mInRestBreak = false;
    int mIdleMaximum = 0;
    QVector<QString> mSuggestions;
    int mSuggestionIndex = 0;

    Configuration mConfiguration;

    bool mPaused = false;
    int mBlocked = 0;
};
