#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("WorkraveQt");
    QCoreApplication::setOrganizationDomain("workraveqt.io");
    QCoreApplication::setApplicationName("WorkraveQt");
    MainWindow w;
    return a.exec();
}
