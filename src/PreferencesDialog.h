#pragma once

#include "QTrayDialog.h"

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QTrayDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget* parent = nullptr);
    ~PreferencesDialog();

private:
    Ui::PreferencesDialog* ui;
};

