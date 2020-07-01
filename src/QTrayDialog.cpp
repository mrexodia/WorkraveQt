#include "QTrayDialog.h"

#include <QCloseEvent>
#include <QIcon>

QTrayDialog::QTrayDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
}

void QTrayDialog::closeEvent(QCloseEvent* event)
{
    event->ignore();
    hide();
}

void QTrayDialog::done(int r)
{
    Q_UNUSED(r);
    hide();
}
