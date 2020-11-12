#include "TimerDialog.h"
#include "ui_TimerDialog.h"
#include "Helpers.h"

#include <QSettings>
#include <QCloseEvent>

TimerDialog::TimerDialog(QWidget* parent) :
    QTrayDialog(parent),
    ui(new Ui::TimerDialog)
{
    ui->setupUi(this);
	setWindowFlag(Qt::WindowStaysOnTopHint);
	setWindowFlag(Qt::WindowFullscreenButtonHint, false);
	setFixedSize(size());

    QSettings settings;
	restoreGeometry(settings.value("TimerDialog/geometry").toByteArray());
    if(settings.value("TimerDialog/visible").toBool())
        setVisible(true);
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

void TimerDialog::setIdleMaximum(int idleMaximum)
{
    ui->progressBarIdle->setMaximum(idleMaximum);
    setIdleProgress(0);
}

void TimerDialog::setIdleProgress(int idleValue)
{
	setTimerProgress(ui->progressBarIdle, ui->labelIdleTimer, idleValue);
}

void TimerDialog::setForceClose()
{
	mForceClose = true;
}

void TimerDialog::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("TimerDialog/geometry", saveGeometry());
    settings.setValue("TimerDialog/visible", isVisible());
	if(mForceClose)
	{
		QDialog::closeEvent(event);
	}
	else
	{
		event->ignore();
		hide();
	}
}
