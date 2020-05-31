#pragma once

#include <QDialog>

class QTrayDialog : public QDialog
{
public:
    QTrayDialog(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;
    void done(int r) override;
};

