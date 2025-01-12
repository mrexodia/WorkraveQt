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
    void setSuggestion(const QString& suggestion);

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
#if defined(Q_OS_WIN)
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#endif // Q_OS_WIN

private slots:
    void on_pushButtonLock_clicked();

private:
    Ui::BreakDialog* ui;
    int mBreakDuration = 0;
    bool mForceClose = false;
    QPoint mFixedPos;
};

