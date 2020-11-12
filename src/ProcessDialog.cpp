#include "ProcessDialog.h"
#include "ProcessListThread.h"
#include "ui_ProcessDialog.h"

#include <QSettings>
#include <QTimer>

ProcessDialog::ProcessDialog(QWidget *parent) :
    QTrayDialog(parent),
    ui(new Ui::ProcessDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowFullscreenButtonHint, false);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, false);
    setWindowFlag(Qt::WindowMinMaxButtonsHint, true);

    for(const auto& rule : QSettings().value("GameRules").toStringList())
        ui->editRules->appendPlainText(rule);

    // Automatically refresh the process list every 10 seconds
    mTimer = new QTimer(this);
    mTimer->setInterval(5000);
    connect(mTimer, &QTimer::timeout, this, &ProcessDialog::refreshProcessList);
    mTimer->start();
    refreshProcessList();
}

ProcessDialog::~ProcessDialog()
{
    delete ui;
}

void ProcessDialog::setForceClose()
{
    mForceClose = true;
}

void ProcessDialog::resultReadySlot(const QStringList& processList)
{
    mProcessList = processList;
    updateProcessList(ui->editFilter->text());
    mIsGameRunning = false;
    auto rules = getRules();
    for(const QString& process : mProcessList)
    {
        for(const QString& rule : rules)
        {
            if(process.contains(rule, Qt::CaseInsensitive))
            {
                mIsGameRunning = true;
                return;
            }
        }
    }
}

void ProcessDialog::refreshProcessList()
{
    auto workerThread = new ProcessListThread(this);
    connect(workerThread, &ProcessListThread::resultReady, this, &ProcessDialog::resultReadySlot);
    workerThread->start();
    mWorkerThreads.append(workerThread);
}

void ProcessDialog::closeEvent(QCloseEvent* event)
{
    if(mForceClose)
    {
        mTimer->stop();
        for(QThread* thread : mWorkerThreads)
            thread->wait();
    }
    QSettings().setValue("GameRules", getRules());
    QTrayDialog::closeEvent(event);
}

void ProcessDialog::on_buttonRefresh_clicked()
{
    refreshProcessList();
}

void ProcessDialog::on_buttonFilter_clicked()
{
    updateProcessList(ui->editFilter->text());
}

void ProcessDialog::on_buttonAddRule_clicked()
{
    ui->editRules->appendPlainText(ui->editFilter->text());
}

void ProcessDialog::updateProcessList(const QString& filter)
{
    ui->editProcesses->clear();
    for(QString& s : mProcessList)
    {
        if(filter.isEmpty())
        {
            ui->editProcesses->appendPlainText(s);
        }
        else if(s.contains(filter, Qt::CaseInsensitive))
        {
            ui->editProcesses->appendPlainText(s);
        }
    }

    auto cursor = ui->editProcesses->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    ui->editProcesses->setTextCursor(cursor);
}

QStringList ProcessDialog::getRules() const
{
    auto l = ui->editRules->toPlainText().split('\n', QString::SkipEmptyParts);
    QStringList list;
    for(const auto& x : l)
    {
        auto t = x.trimmed();
        if(t.isEmpty())
            continue;
        list << t;
    }
    return l;
}

void ProcessDialog::on_editFilter_textChanged(const QString &arg1)
{
    ui->buttonAddRule->setEnabled(!arg1.isEmpty());
}

void ProcessDialog::on_editFilter_editingFinished()
{
    updateProcessList(ui->editFilter->text());
}
