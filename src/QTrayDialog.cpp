#include "QTrayDialog.h"

#include <QCloseEvent>
#include <QIcon>

QTrayDialog::QTrayDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::MSWindowsFixedSizeDialogHint);
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
