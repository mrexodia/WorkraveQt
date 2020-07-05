#pragma once

#include "QTrayDialog.h"
#include "Configuration.h"

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QTrayDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(Configuration* configuration, QWidget* parent = nullptr);
    ~PreferencesDialog();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::PreferencesDialog* ui;
    Configuration* mConfiguration;
};

