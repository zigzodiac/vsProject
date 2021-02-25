#pragma once

#include <QtWidgets/QMainWindow>
#include <QBoxLayout> 
#include <QThread>
#include <QTimer>
#include <QPushButton> 
#include "ui_QtLiveStreamDemo.h"
#include "VideoWidget.h"
#include "worker.h"

class QtLiveStreamDemo : public QMainWindow
{
    Q_OBJECT

public:
    QtLiveStreamDemo(QWidget *parent = Q_NULLPTR);

private:
    Ui::QtLiveStreamDemoClass ui;


private slots:
        void startLive();
};
