#pragma once
#include <QObject>
#include <QImage>
#include <QThread>
#include <QtMultimedia/QCameraInfo>
#include <QtWidgets/QApplication>

#ifndef FFMPEGINTF
#include "FfmpegIntf.h"
#endif // FFMPEGINTF


class ScrStmWorker :public QObject
{
    Q_OBJECT

public:
    ScrStmWorker();
    ~ScrStmWorker();

    bool scrStmThdSta;
signals:
    void SigUpdateFrame(QImage);

protected slots:
    void FfmpegScreenStreaming();
};