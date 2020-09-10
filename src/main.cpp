#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("WorkraveQt");
    QCoreApplication::setOrganizationDomain("workrave.io");
    QCoreApplication::setApplicationName("WorkraveQt");
    bool testConfiguration = false;
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "--testConfiguration") == 0)
            testConfiguration = true;
    }
    MainWindow w(testConfiguration);
    Q_UNUSED(w);
    return a.exec();
}
