#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include "mapQGraphicsView.h"
#include <QObject>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
