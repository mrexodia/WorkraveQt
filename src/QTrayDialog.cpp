#include "QTrayDialog.h"

#include <QCloseEvent>
#include <QIcon>
#include <QScreen>
#include <QGuiApplication>
#include <QSettings>

QTrayDialog::QTrayDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
}

QString QTrayDialog::geometryKey() const
{
    return QString("%1/geometry").arg(metaObject()->className());
}

void QTrayDialog::restoreDialogGeometry()
{
    QSettings settings;
    QByteArray savedGeometry = settings.value(geometryKey()).toByteArray();
    if (!savedGeometry.isEmpty()) {
        restoreGeometry(savedGeometry);
    } else {
        // Center on primary screen by default (workaround for XWayland)
        if (QScreen* screen = QGuiApplication::primaryScreen()) {
            QRect screenGeometry = screen->availableGeometry();
            move(screenGeometry.center() - rect().center());
        }
    }
}

void QTrayDialog::saveDialogGeometry()
{
    QSettings settings;
    settings.setValue(geometryKey(), saveGeometry());
}

void QTrayDialog::closeEvent(QCloseEvent* event)
{
    saveDialogGeometry();
    event->ignore();
    hide();
}

void QTrayDialog::done(int r)
{
    Q_UNUSED(r);
    hide();
}
