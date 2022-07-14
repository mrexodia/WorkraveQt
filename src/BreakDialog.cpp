#include "BreakDialog.h"
#include "ui_BreakDialog.h"
#include "Helpers.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>

BreakDialog::BreakDialog(QWidget* parent) :
	QTrayDialog(parent),
	ui(new Ui::BreakDialog)
{
	ui->setupUi(this);
	setWindowFlag(Qt::WindowStaysOnTopHint);
	setWindowFlag(Qt::FramelessWindowHint);
	setWindowFlag(Qt::WindowFullscreenButtonHint, false);
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

void BreakDialog::setSuggestion(const QString& suggestion)
{
    ui->labelSuggestion->setText(tr("Suggestion: %1").arg(suggestion));
}

void BreakDialog::closeEvent(QCloseEvent* event)
{
	if(mForceClose)
		return QDialog::closeEvent(event);
	// Ignore close
	event->ignore();
}

void BreakDialog::keyPressEvent(QKeyEvent* event)
{
	if(event->key() == Qt::Key_Escape)
		event->ignore();
	else
		QDialog::keyPressEvent(event);
}

void BreakDialog::moveEvent(QMoveEvent* event)
{
	if(mFixedPos != QPoint(-1, -1) && event->pos() != mFixedPos)
	{
		QTimer::singleShot(0, [this]
		{
			move(mFixedPos);
		});
	}
	QDialog::moveEvent(event);
}

void BreakDialog::showEvent(QShowEvent* event)
{
    mFixedPos = pos();
    Helpers::startBreak();
    QDialog::showEvent(event);
}

void BreakDialog::hideEvent(QHideEvent* event)
{
    mFixedPos = QPoint(-1, -1);
    Helpers::stopBreak();
    QDialog::hideEvent(event);
}

#if defined(Q_OS_WIN)

#include <Windows.h>

bool BreakDialog::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    if(eventType == "windows_generic_MSG")
    {
        // This prevents users from bypassing the break dialog with Alt+Space -> Restore
        auto msg = (MSG*)message;
        if(msg->message == WM_SYSCOMMAND && msg->wParam == SC_RESTORE)
        {
            // https://docs.microsoft.com/en-us/windows/win32/menurc/wm-syscommand#return-value
            *result = 0;
            return true;
        }
    }
    return QTrayDialog::nativeEvent(eventType, message, result);
}

#endif // Q_OS_WIN

void BreakDialog::on_pushButtonLock_clicked()
{
	Helpers::lockScreen();
}

