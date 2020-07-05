#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

PreferencesDialog::PreferencesDialog(Configuration* configuration, QWidget* parent) :
    QTrayDialog(parent),
    ui(new Ui::PreferencesDialog),
    mConfiguration(configuration)
{
    ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::showEvent(QShowEvent* event)
{
#define setting(x) ui->spinBox ## x->setValue(mConfiguration->m ## x)
    setting(MicroBreakCycle);
    setting(MicroBreakNotification);
    setting(MicroBreakDuration);
    setting(RestBreakCycle);
    setting(RestBreakNotification);
    setting(RestBreakDuration);
#undef setting
    QTrayDialog::showEvent(event);
}

void PreferencesDialog::on_buttonBox_accepted()
{
#define setting(x) mConfiguration->m ## x = ui->spinBox ## x->value()
    setting(MicroBreakCycle);
    setting(MicroBreakNotification);
    setting(MicroBreakDuration);
    setting(RestBreakCycle);
    setting(RestBreakNotification);
    setting(RestBreakDuration);
#undef setting
    mConfiguration->save();
    mConfiguration->dump();
}
