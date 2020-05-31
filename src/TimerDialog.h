#pragma once

#include "QTrayDialog.h"

namespace Ui {
class TimerDialog;
}

class TimerDialog : public QTrayDialog
{
    Q_OBJECT

public:
    explicit TimerDialog(QWidget* parent = nullptr);
    ~TimerDialog();

private:
    Ui::TimerDialog* ui;
};

