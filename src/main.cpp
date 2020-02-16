#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    gst_init (&argc, &argv);
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    w.resizeWindowsManual();
    return a.exec();
}
