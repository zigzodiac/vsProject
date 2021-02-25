#include <QtWidgets/QApplication>
#include "QtLiveWidgetDemo.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtLiveWidgetDemo w;
    w.show();
    w.Init();
    return a.exec();
}
