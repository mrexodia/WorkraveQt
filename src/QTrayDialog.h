#pragma once

#include <QDialog>

class QTrayDialog : public QDialog
{
    Q_OBJECT

public:
    QTrayDialog(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;
    void done(int r) override;
    void restoreDialogGeometry();
    void saveDialogGeometry();

private:
    QString geometryKey() const;
};

