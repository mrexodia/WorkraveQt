#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QSettings>
#include <QMessageBox>
#include <QRegularExpression>

#include "Helpers.h"

PreferencesDialog::PreferencesDialog(Configuration* configuration, QWidget* parent)
    : QTrayDialog(parent)
    , ui(new Ui::PreferencesDialog)
    , mConfiguration(configuration)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowFullscreenButtonHint, false);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setFixedSize(size());

    restoreDialogGeometry();

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
    // Thanks to Can for the correct logic!
    int h = seconds / (60 * 60);
    seconds -= h * (60 * 60);
    int m = seconds / 60;
    int s = seconds % 60;
    return QString("%1h%2m%3s").arg(h, 2, 10, QChar('0')).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}

void PreferencesDialog::showEvent(QShowEvent* event)
{
#define setting(x) ui->lineEdit##x->setText(secondsToHms(mConfiguration->m##x))
    setting(MicroBreakCycle);
    setting(MicroBreakNotification);
    setting(MicroBreakDuration);
    setting(RestBreakCycle);
    setting(RestBreakNotification);
    setting(RestBreakDuration);
#undef setting
    if (ui->checkBoxStartup->isEnabled())
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
    static QRegularExpression re("(\\d\\d)h(\\d\\d)m(\\d\\d)s");
    auto match = re.match(hms);
    if(!match.hasMatch())
        return false;
    int h = match.captured(1).toInt();
    if (h < 0 || h >= 24)
        return false;
    int m = match.captured(2).toInt();
    if (m < 0 || m >= 60)
        return false;
    int s = match.captured(3).toInt();
    if (s < 0 || s >= 60)
        return false;
    seconds = h * 3600 + m * 60 + s;
    return true;
}

void PreferencesDialog::accept()
{
    auto configBackup = *mConfiguration;

#define setting(x)                                                                                                   \
    if (!secondsFromHms(ui->lineEdit##x->text(), mConfiguration->m##x))                                              \
    {                                                                                                                \
        ui->lineEdit##x->setStyleSheet("QLineEdit { border: 2px solid red; }");                                      \
        QMessageBox::warning(this, tr("Error"), tr("Invalid data for: %1").arg(ui->lineEdit##x->placeholderText())); \
        return;                                                                                                      \
    }                                                                                                                \
    else                                                                                                             \
    {                                                                                                                \
        ui->lineEdit##x->setStyleSheet("");                                                                          \
    }                                                                                                                \

    setting(MicroBreakCycle);
    setting(MicroBreakNotification);
    setting(MicroBreakDuration);
    setting(RestBreakCycle);
    setting(RestBreakNotification);
    setting(RestBreakDuration);

    #undef setting

    if(mConfiguration->mMicroBreakCycle <= mConfiguration->mMicroBreakNotification)
    {
        *mConfiguration = configBackup;
        QMessageBox::warning(this, tr("Error"), tr("The micro break notification has to be before the cycle"));
        return;
    }

    if(mConfiguration->mRestBreakCycle <= mConfiguration->mRestBreakNotification)
    {
        *mConfiguration = configBackup;
        QMessageBox::warning(this, tr("Error"), tr("The rest break notification has to be before the cycle"));
        return;
    }

    if (ui->checkBoxStartup->isEnabled() && mStartupChecked != ui->checkBoxStartup->isChecked())
    {
#ifdef Q_OS_WIN
        // https://stackoverflow.com/a/38523870/1806760
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        if (ui->checkBoxStartup->isChecked())
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
    Helpers::restartApplication();
}
