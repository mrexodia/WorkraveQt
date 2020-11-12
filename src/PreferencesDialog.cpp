#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"
#include <QSettings>
#include <QMessageBox>

PreferencesDialog::PreferencesDialog(Configuration* configuration, QWidget* parent) :
    QTrayDialog(parent),
    ui(new Ui::PreferencesDialog),
    mConfiguration(configuration)
{
    ui->setupUi(this);
	setWindowFlag(Qt::WindowFullscreenButtonHint, false);
	setFixedSize(size());

#if !defined(Q_OS_WIN)
    ui->checkBoxStartup->setEnabled(false);
#endif // Q_OS_WIN
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

static QString secondsToHms(int seconds)
{
    int h = seconds / 3600;
    int m = seconds / 60;
    int s = seconds % 60;
    return QString().sprintf("%02dh%02dm%02ds", h, m, s);
}

void PreferencesDialog::showEvent(QShowEvent* event)
{
#define setting(x) ui->lineEdit ## x->setText(secondsToHms(mConfiguration->m ## x))
    setting(MicroBreakCycle);
    setting(MicroBreakNotification);
    setting(MicroBreakDuration);
    setting(RestBreakCycle);
    setting(RestBreakNotification);
    setting(RestBreakDuration);
#undef setting
    if(ui->checkBoxStartup->isEnabled())
    {
#ifdef Q_OS_WIN
        // https://stackoverflow.com/a/38523870/1806760
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        ui->checkBoxStartup->setChecked(settings.contains(QCoreApplication::applicationName()));
#endif // Q_OS_WIN
    }
    mStartupChecked = ui->checkBoxStartup->isChecked();
    QTrayDialog::showEvent(event);
}

static bool secondsFromHms(const QString& hms, int& seconds)
{
    QRegExp rx("(\\d\\d)h(\\d\\d)m(\\d\\d)s");
    if(!rx.exactMatch(hms))
        return false;
    int h = rx.cap(1).toInt();
    if(h < 0 || h >= 24)
        return false;
    int m = rx.cap(2).toInt();
    if(m < 0 || m >= 60)
        return false;
    int s = rx.cap(3).toInt();
    if(s < 0 || s >= 60)
        return false;
    seconds = h * 3600 + m * 60 + s;
    return true;
}

void PreferencesDialog::accept()
{
#define setting(x) \
    if(!secondsFromHms(ui->lineEdit ## x->text(), mConfiguration->m ## x)) \
    { \
        ui->lineEdit ## x->setStyleSheet("QLineEdit { border: 2px solid red; }"); \
        QMessageBox::warning(this, tr("Error"), tr("Invalid data for: %1").arg(ui->lineEdit ## x->placeholderText())); \
        return; \
    } \
    else \
    { \
        ui->lineEdit ## x->setStyleSheet(""); \
    } \
    /**/

    setting(MicroBreakCycle);
    setting(MicroBreakNotification);
    setting(MicroBreakDuration);
    setting(RestBreakCycle);
    setting(RestBreakNotification);
    setting(RestBreakDuration);

#undef setting

    if(ui->checkBoxStartup->isEnabled() && mStartupChecked != ui->checkBoxStartup->isChecked())
    {
#ifdef Q_OS_WIN
        // https://stackoverflow.com/a/38523870/1806760
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        if(ui->checkBoxStartup->isChecked())
        {
            settings.setValue(QCoreApplication::applicationName(), QCoreApplication::applicationFilePath().replace('/', '\\'));
        }
        else
        {
            settings.remove(QCoreApplication::applicationName());
        }
#endif // Q_OS_WIN
    }
    mConfiguration->save();
    mConfiguration->dump();
    QDialog::accept();
}
