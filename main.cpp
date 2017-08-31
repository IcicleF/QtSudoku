#include "MainWindow.h"
#include "KeyboardFilter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    KeyboardFilter filter;
    w.installEventFilter(&filter);

    w.show();

    return a.exec();
}
