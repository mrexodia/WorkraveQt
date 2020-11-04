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
    void setForceClose();

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void on_pushButtonLock_clicked();

private:
    Ui::BreakDialog* ui;
    int mBreakDuration = 0;
    bool mForceClose = false;
};

