#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("ruessel");
    a.setApplicationName("boxview");
    MainWindow w;
    w.show();
    return a.exec();
}
