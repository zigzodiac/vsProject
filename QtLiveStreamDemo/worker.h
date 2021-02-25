#pragma once
#include <QDebug> 
#include <QThread>
#include <QImage>
#include <QObject>



extern "C" {
 #include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

class Worker : public QObject {
	Q_OBJECT
public:
	Worker(QObject* parent = nullptr) {}
	
signals:
	void sig_GetOneFrame(QImage);

public slots:
	void getImage();

};
