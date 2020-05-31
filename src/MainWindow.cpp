#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QCloseEvent>
#include <QEvent>
#include <QMessageBox>

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
}

MainWindow::~MainWindow()
{
    delete ui;
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
