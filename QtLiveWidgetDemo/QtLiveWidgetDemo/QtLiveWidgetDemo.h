#pragma once

#include <QtWidgets/QWidget>
#include <QDebug>
#include <QThread>
#include <QTimer>

#include "ui_QtLiveWidgetDemo.h"
#include "Worker.h"
#include "ScrStmWorker.h"

class QtLiveWidgetDemo : public QWidget
{
    Q_OBJECT

public:
    QtLiveWidgetDemo(QWidget *parent = Q_NULLPTR);
    void Init();
    void StartCamera();
    void CloseCamera();
    void CtrlCamera();

    void OpenScrStm();
    void CloseScrStm();
    void CtrlScrStm();

    Worker* worker;
    QThread* t;
    QThread* scrStmThd;
signals:
    void WorkerThreadExit();

public slots:
    void slot_GetOneFrame(QImage img);

private:
    bool isCameraOpen;
    bool isScreenStreaming;
    Ui::QtLiveWidgetDemoClass ui;
};
