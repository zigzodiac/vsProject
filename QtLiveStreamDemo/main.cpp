#include "QtLiveStreamDemo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtLiveStreamDemo w;
    w.show();
    return a.exec();
}
