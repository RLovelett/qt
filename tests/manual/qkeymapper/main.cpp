#include <QtGui/QApplication>
#include "mainwindow.h"
#include "widget.h"
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget *widget = new Widget();
    MainWindow w;
    w.setCentralWidget(widget);
    w.show();

    return a.exec();
}
