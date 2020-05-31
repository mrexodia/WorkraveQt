#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QCloseEvent>
#include <QEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::exit()
{
    mIsExiting = true;
    close();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if(mIsExiting)
    {
        QMainWindow::closeEvent(event);
    }
    else
    {
        event->ignore();
        hide();
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        if (isMinimized())
            setWindowFlags(windowFlags() | Qt::Tool);
        else
            setWindowFlags(windowFlags() & ~Qt::Tool);
    }
    return QMainWindow::changeEvent(event);
}
