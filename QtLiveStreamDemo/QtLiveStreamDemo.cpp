#include "QtLiveStreamDemo.h"


QtLiveStreamDemo::QtLiveStreamDemo(QWidget *parent)
    : QMainWindow(parent)
{
    VideoWidget* videoWidget = new VideoWidget;
    QAbstractButton* startLiveBotton = new QPushButton(tr("��ʼֱ��"));
    QAbstractButton* stopLiveBotton = new QPushButton(tr("ֱֹͣ��"));
    QBoxLayout* controlLayout = new QHBoxLayout;

    controlLayout->setMargin(0);
    controlLayout->addWidget(videoWidget);
    controlLayout->addWidget(startLiveBotton);
    controlLayout->addWidget(stopLiveBotton);
    setLayout(controlLayout);

   QThread  *t = new QThread;
    Worker *worker = new Worker;
    connect(t, &QThread::finished, worker, &QObject::deleteLater);//��ֹ�ڴ�й©
    connect(worker, SIGNAL(sig_GetOneFrame(QImage)), videoWidget, SLOT(slot_GetOneFrame(QImage)));
    worker->moveToThread(t);
    t->start();
    QTimer::singleShot(1, worker, SLOT(getImage()));//1����֮��ۺ����Ѿ����߳�������
    
    ui.setupUi(this);
}
