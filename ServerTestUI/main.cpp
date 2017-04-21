#include "servertestui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerTestUI w;
    w.show();
    return a.exec();
}
