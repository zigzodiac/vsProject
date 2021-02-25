#include "FfmpegVideoTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FfmpegVideoTool w;
    w.InitWidget();
    w.show();
    return a.exec();
}
