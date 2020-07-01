#include "BreakDialog.h"
#include "ui_BreakDialog.h"

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
    ui->labelTimer->setText(timeFormat(mBreakDuration));
}

void BreakDialog::setBreakProgress(int seconds)
{
    ui->progressBarTimer->setValue(seconds);
    ui->progressBarTimer->update();
    ui->labelTimer->setText(timeFormat(mBreakDuration - seconds));
}

QString BreakDialog::timeFormat(int seconds)
{
    auto minutes = seconds / 60;
    seconds = seconds % 60;
    return QString().sprintf("%d:%02d", minutes, seconds);
}
