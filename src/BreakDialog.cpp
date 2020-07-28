#include "BreakDialog.h"
#include "ui_BreakDialog.h"
#include "Helpers.h"

#include <QMessageBox>
#include <QCloseEvent>

BreakDialog::BreakDialog(QWidget* parent) :
    QTrayDialog(parent),
    ui(new Ui::BreakDialog)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    ui->setupUi(this);
}

BreakDialog::~BreakDialog()
{
    delete ui;
}

void BreakDialog::setBreakDuration(int breakDuration)
{
    mBreakDuration = breakDuration;
    ui->progressBarTimer->setMaximum(breakDuration);
    ui->progressBarTimer->reset();
    ui->progressBarTimer->update();
    ui->labelTimer->setText(Helpers::timeFormat(mBreakDuration));
}

void BreakDialog::setBreakProgress(int seconds)
{
    ui->progressBarTimer->setValue(seconds);
    ui->progressBarTimer->update();
    ui->labelTimer->setText(Helpers::timeFormat(mBreakDuration - seconds));
}

void BreakDialog::setForceClose()
{
    mForceClose = true;
}

void BreakDialog::closeEvent(QCloseEvent* event)
{
    if(mForceClose)
        return QDialog::closeEvent(event);
    // Ignore close
    event->ignore();
}

void BreakDialog::on_pushButtonLock_clicked()
{
    QMessageBox::information(this, "TODO", "Not yet implemented!");
}

