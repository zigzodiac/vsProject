#include "QtLiveWidgetDemo.h"

QtLiveWidgetDemo::QtLiveWidgetDemo(QWidget *parent)
    : QWidget(parent)
{
    isCameraOpen = false;
    isScreenStreaming = false;
    worker = nullptr;
    t = nullptr;
    ui.setupUi(this);

}

void QtLiveWidgetDemo::Init()
{
    worker = new Worker;
    connect(ui.OpenLive, &QPushButton::clicked, this, &QtLiveWidgetDemo::CtrlCamera);
    connect(ui.RecordScreen, &QPushButton::clicked, this, &QtLiveWidgetDemo::CtrlScrStm);
}



void QtLiveWidgetDemo::StartCamera()
{
    t = new QThread;
    qDebug() << "current thread ID --- StartCamera" << QThread::currentThreadId();

    connect(t, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, SIGNAL(sig_GetOneFrame(QImage)), this, SLOT(slot_GetOneFrame(QImage)));
    
    worker->moveToThread(t);
    connect(this, SIGNAL(WorkerThreadExit()), worker, SLOT(ExitThread()), Qt::QueuedConnection);
    t->start();

    QTimer::singleShot(1, worker, SLOT(FfmpegCameraStreaming()));
    ui.OpenLive->setText("CloseCamera");
}

void QtLiveWidgetDemo::CloseCamera()
{
    qDebug() << "current thread ID --- StartCamera" << QThread::currentThreadId();
    /*emit WorkerThreadExit();*/
    worker->threadSta = false;
    ui.OpenLive->setText("OpenCamera");

}

void QtLiveWidgetDemo::CtrlCamera()
{
    // close camera
    if (isScreenStreaming) {
        return ;
    }

    if (!isCameraOpen) {
        StartCamera();
        isCameraOpen = true;
    }// open camera
    else {
        CloseCamera();
        isCameraOpen = false;
    }
}


void QtLiveWidgetDemo::OpenScrStm()
{
    scrStmThd = new QThread;
    qDebug() << "current thread ID --- StartCamera" << QThread::currentThreadId();

    connect(scrStmThd, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, SIGNAL(SigUpdateFrame(QImage)), this, SLOT(slot_GetOneFrame(QImage)));
    worker->moveToThread(scrStmThd);
    scrStmThd->start();

    QTimer::singleShot(1, worker, SLOT(FfmpegScreenStreaming()));
    ui.OpenLive->setText("CloseScreenStreaming");
}

void QtLiveWidgetDemo::CloseScrStm()
{
    worker->scrStmThdSta = false;
    ui.RecordScreen->setText("OpenScreenStreaming");

}
void QtLiveWidgetDemo::CtrlScrStm()
{
    if (isCameraOpen) {
        return;
    }
    if (!isScreenStreaming) {
        OpenScrStm();
        isScreenStreaming = true;
    }
    else {
        CloseScrStm();
        isScreenStreaming = false;
    }
}

void QtLiveWidgetDemo::slot_GetOneFrame(QImage img) {
    ui.VideoLabel->setPixmap(QPixmap::fromImage(img));
}
