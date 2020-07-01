#pragma once

#include "QTrayDialog.h"

namespace Ui {
class BreakDialog;
}

class BreakDialog : public QTrayDialog
{
    Q_OBJECT

public:
    explicit BreakDialog(QWidget* parent = nullptr);
    ~BreakDialog();
    void setBreakDuration(int breakDuration);
    void setBreakProgress(int seconds);

private:
    Ui::BreakDialog* ui;
    int mBreakDuration = 0;
};

