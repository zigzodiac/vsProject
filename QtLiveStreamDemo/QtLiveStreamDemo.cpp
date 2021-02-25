#include "QtLiveStreamDemo.h"


QtLiveStreamDemo::QtLiveStreamDemo(QWidget *parent)
    : QMainWindow(parent)
{
    VideoWidget* videoWidget = new VideoWidget;
    QAbstractButton* startLiveBotton = new QPushButton(tr("开始直播"));
    QAbstractButton* stopLiveBotton = new QPushButton(tr("停止直播"));
    QBoxLayout* controlLayout = new QHBoxLayout;

    controlLayout->setMargin(0);
    controlLayout->addWidget(videoWidget);
    controlLayout->addWidget(startLiveBotton);
    controlLayout->addWidget(stopLiveBotton);
    setLayout(controlLayout);

   QThread  *t = new QThread;
    Worker *worker = new Worker;
    connect(t, &QThread::finished, worker, &QObject::deleteLater);//防止内存泄漏
    connect(worker, SIGNAL(sig_GetOneFrame(QImage)), videoWidget, SLOT(slot_GetOneFrame(QImage)));
    worker->moveToThread(t);
    t->start();
    QTimer::singleShot(1, worker, SLOT(getImage()));//1毫秒之后槽函数已经在线程中运行
    
    ui.setupUi(this);
}
