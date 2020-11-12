#pragma once

#include <QThread>
#include <QStringList>

class ProcessListThread : public QThread
{
    Q_OBJECT

public:
    using QThread::QThread;

protected:
    void run() override;

signals:
    void resultReady(const QStringList& processList);

private:
    QStringList mProcessList;
};
