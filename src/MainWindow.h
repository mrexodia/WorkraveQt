#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

#include "PreferencesDialog.h"
#include "TimerDialog.h"
#include "BreakDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason);
    void tickTimeoutSlot();

    void on_actionExit_triggered();
    void on_actionPreferences_triggered();
    void on_actionOpen_triggered();
    void on_actionBreak_triggered();

private:
    Ui::MainWindow* ui = nullptr;
    QSystemTrayIcon* mTrayIcon = nullptr;
    QTimer* mTickTimer = nullptr;
    PreferencesDialog* mPreferencesDialog = nullptr;
    TimerDialog* mTimerDialog = nullptr;
    BreakDialog* mBreakDialog = nullptr;
    QRect mTimerDialogGeometry;
    int mMicroBreakTick = 0;
    bool mInMicroBreak = false;
    int mRestBreakTick = 0;
    bool mInRestBreak = false;

    int mMicroBreakCycle = 10;
    int mMicroBreakNotification = mMicroBreakCycle - 5;
    int mMicroBreakDuration = 15;

    int mRestBreakCycle = 60;
    int mRestBreakNotification = mRestBreakCycle - 20;
    int mRestBreakDuration = 15;
};
