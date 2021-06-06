#include <QApplication>
#include <QtCore/QDebug>
#include "mainwindow.h"

int debug_print = 0;

int main(int argc, char *argv[])
{
    if (debug_print) qDebug() << "Hello Qt World!";

    if (debug_print) qDebug() << "QApplication()";
    QApplication a(argc, argv);
    if (debug_print) qDebug() << "QApplication() done";

    if (debug_print) qDebug() << "MainWindow().";
    MainWindow w;
    if (debug_print) qDebug() << "MainWindow() done.";

    if (debug_print) qDebug() << "MainWindow() show.";
    w.show();
    if (debug_print) qDebug() << "MainWindow() show done.";

    return a.exec();
}
