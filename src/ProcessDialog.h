#pragma once

#include "QTrayDialog.h"
#include <QStringList>
#include <QSet>

namespace Ui
{
class ProcessDialog;
}

class ProcessDialog : public QTrayDialog
{
    Q_OBJECT

public:
    explicit ProcessDialog(QWidget* parent = nullptr);
    ~ProcessDialog();
    void setForceClose();
    bool isGameRunning() const { return mIsGameRunning; }

public slots:
    void resultReadySlot(const QStringList& processList);
    void refreshProcessList();

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private slots:
    void on_buttonRefresh_clicked();
    void on_buttonFilter_clicked();
    void on_buttonAddRule_clicked();
    void on_editFilter_textChanged(const QString& text);

private:
    void updateProcessList(const QString& filter = QString());
    QStringList getRules() const;

private:
    Ui::ProcessDialog* ui = nullptr;
    QStringList mProcessList;
    QSet<QThread*> mWorkerThreads;
    bool mForceClose = false;
    QTimer* mTimer = nullptr;
    bool mIsGameRunning = false;
};

