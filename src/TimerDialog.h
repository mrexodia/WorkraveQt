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
    void setMicroBreakMaximum(int microBreakMaximum);
    void setMicroBreakProgress(int microBreakValue);
    void setRestBreakMaximum(int restBreakMaximum);
    void setRestBreakProgress(int restBreakValue);
    void setIdleMaximum(int idleMaximum);
    void setIdleProgress(int idleValue);

private:
    Ui::TimerDialog* ui;
};

