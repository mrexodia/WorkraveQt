#include "ProcessListThread.h"
#include <QProcess>

void ProcessListThread::run()
{
#ifdef Q_OS_WIN
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("wmic process get commandline");
    process.waitForFinished(-1);
    QString s = process.readAllStandardOutput();
#else
    QString s = "";
#endif // Q_OS_WIN

    s.replace("\r", "");
    auto x = s.split('\n', QString::SkipEmptyParts);
    for(int i = 0; i < x.length(); i++)
    {
        auto p = x[i].trimmed();
        if(i == 0 || p.isEmpty())
            continue;
        mProcessList.append(std::move(p));
    }

    emit resultReady(mProcessList);
}
