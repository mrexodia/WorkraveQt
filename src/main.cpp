#include <cstdlib>
#include <QApplication>
#include <QProcess>

#include "Helpers.h"
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    // Improve debugging experience with Qt Creator (printf is flushed every time)
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("WorkraveQt");
    QCoreApplication::setOrganizationDomain("workrave.io");
    QCoreApplication::setApplicationName("WorkraveQt");

    // Test configuration with short timers for quick debugging
    // Disables saving to 'production' settings
    bool testConfiguration = false;
    bool resetConfiguration = false;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--testConfiguration") == 0)
            testConfiguration = true;
        else if (strcmp(argv[i], "--resetConfiguration") == 0)
            resetConfiguration = true;
    }

    MainWindow w(testConfiguration, resetConfiguration);
    Q_UNUSED(w);

    // Handle restart logic
    // Based on https://stackoverflow.com/a/33637676/1806760
    auto exit_code = a.exec();
    if (exit_code == Helpers::EXIT_RESTART)
    {
        auto process = new QProcess();
        process->start(QCoreApplication::applicationFilePath(), QCoreApplication::arguments(), QIODevice::NotOpen);
        return EXIT_SUCCESS;
    }
    return exit_code;
}
