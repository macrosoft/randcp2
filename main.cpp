#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    QString translateFile = "randcp2_" + QLocale::system().name() + ".qm";
    translator.load(translateFile);
    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
