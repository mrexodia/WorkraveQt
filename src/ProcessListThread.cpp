#include "ProcessListThread.h"
#include <QProcess>

void ProcessListThread::run()
{
	QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
#if defined(Q_OS_WIN)
    process.start("wmic", { "process", "get", "commandline" });
#else
    process.start("ps", { "-e", "-o", "command" });
#endif // Q_OS_WIN
	process.waitForFinished(-1);
	QString s = process.readAllStandardOutput();

    s.replace("\r", "");
    auto x = s.split('\n', Qt::SkipEmptyParts);
    for(int i = 0; i < x.length(); i++)
    {
        auto p = x[i].trimmed();
        if(i == 0 || p.isEmpty())
            continue;
		mProcessList.append(std::move(p));
    }

    emit resultReady(mProcessList);
}
