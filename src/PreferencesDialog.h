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
    void accept() override;

private:
    Ui::PreferencesDialog* ui;
    Configuration* mConfiguration;
    bool mStartupChecked = false;
};

