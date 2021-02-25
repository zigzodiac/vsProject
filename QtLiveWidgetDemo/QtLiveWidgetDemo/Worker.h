#pragma once
#include <QObject>
#include <QImage>
#include <QThread>
#include <QtMultimedia/QCameraInfo>
#include <QtWidgets/QApplication>

#ifndef FFMPEGINTF
#include "FfmpegIntf.h"
#endif // FFMPEGINTF


class Worker :public QObject
{
    Q_OBJECT

public:
    Worker();
    ~Worker();
   
    bool threadSta;
    bool scrStmThdSta;
    void CallbackUpdateQImage(uchar* RgbBuffer, int PicWidth, int Height);
    void YUVToRGB(AVFrame* RrameYUV, int PicWidth, int PicHeight);  //YUV420p to RGB32
signals:
    void sig_GetOneFrame(QImage);
signals:
    void SigUpdateFrame(QImage);

protected slots:
    void FfmpegCameraStreaming();
    void FfmpegScreenStreaming();
    void ExitThread();
};