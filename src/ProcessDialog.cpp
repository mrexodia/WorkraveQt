#include "ProcessDialog.h"
#include "ProcessListThread.h"
#include "ui_ProcessDialog.h"

#include <QSettings>
#include <QTimer>

ProcessDialog::ProcessDialog(QWidget* parent)
    : QTrayDialog(parent)
    , ui(new Ui::ProcessDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowFullscreenButtonHint, false);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, false);
    setWindowFlag(Qt::WindowMinMaxButtonsHint, true);

    restoreDialogGeometry();

    for (const auto& rule : QSettings().value("GameRules").toStringList())
        ui->editRules->appendPlainText(rule);

    // Automatically refresh the process list every 5 seconds
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
    for (const QString& process : mProcessList)
    {
        for (const QString& rule : rules)
        {
            if (process.contains(rule, Qt::CaseInsensitive))
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
    connect(workerThread, &QThread::finished, [this, workerThread] {
        workerThread->deleteLater();
        mWorkerThreads.remove(workerThread);
    });
    workerThread->start();
    mWorkerThreads.insert(workerThread);
}

void ProcessDialog::closeEvent(QCloseEvent* event)
{
    if (mForceClose)
    {
        mTimer->stop();
        for (QThread* thread : mWorkerThreads)
            thread->wait();
    }
    QSettings().setValue("GameRules", getRules());
    QTrayDialog::closeEvent(event);
}

void ProcessDialog::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);

    mTimer->stop();
}

void ProcessDialog::hideEvent(QHideEvent* event)
{
    Q_UNUSED(event);

    mTimer->start();
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
    for (QString& s : mProcessList)
    {
        if (filter.isEmpty())
        {
            ui->editProcesses->appendPlainText(s);
        }
        else if (s.contains(filter, Qt::CaseInsensitive))
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
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    auto l = ui->editRules->toPlainText().split('\n', QString::SkipEmptyParts);
#else
    auto l = ui->editRules->toPlainText().split('\n', Qt::SkipEmptyParts);
#endif // QT_VERSION
    QStringList list;
    for (const auto& x : l)
    {
        auto t = x.trimmed();
        if (t.isEmpty())
            continue;
        list << t;
    }
    return l;
}

void ProcessDialog::on_editFilter_textChanged(const QString& text)
{
    ui->buttonAddRule->setEnabled(!text.isEmpty());
    updateProcessList(text);
}
