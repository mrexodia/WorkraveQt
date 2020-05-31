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

private:
    Ui::BreakDialog* ui;
};

