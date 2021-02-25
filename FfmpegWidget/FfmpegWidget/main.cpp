#include "FfmpegWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    FfmpegWidget w;
    w.InitSigSlot();
    w.InitWidget();
    w.show();
    return a.exec();
}
