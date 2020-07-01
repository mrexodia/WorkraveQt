#include "TimerDialog.h"
#include "ui_TimerDialog.h"
#include "Helpers.h"

TimerDialog::TimerDialog(QWidget* parent) :
    QTrayDialog(parent),
    ui(new Ui::TimerDialog)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    ui->setupUi(this);
}

TimerDialog::~TimerDialog()
{
    delete ui;
}

static void setTimerProgress(QProgressBar* progressBar, QLabel* labelTimer, int progress)
{
    progressBar->setValue(progress);
    progressBar->update();
    labelTimer->setText(Helpers::timeFormat(progressBar->maximum() - progress));
}

void TimerDialog::setMicroBreakMaximum(int microBreakMaximum)
{
    ui->progressBarMicroBreak->setMaximum(microBreakMaximum);
    setMicroBreakProgress(0);
}

void TimerDialog::setMicroBreakProgress(int microBreakValue)
{
    setTimerProgress(ui->progressBarMicroBreak, ui->labelMicroBreakTimer, microBreakValue);;
}

void TimerDialog::setRestBreakMaximum(int restBreakMaximum)
{
    ui->progressBarRestBreak->setMaximum(restBreakMaximum);
    setRestBreakProgress(0);
}

void TimerDialog::setRestBreakProgress(int restBreakValue)
{
    setTimerProgress(ui->progressBarRestBreak, ui->labelRestBreakTimer, restBreakValue);
}
