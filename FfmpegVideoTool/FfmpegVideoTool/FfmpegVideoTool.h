#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog> 
#include <QDebug>
#include <QScrollBar>
#include <QThread>
#include <QMessageBox> 

#include "ui_FfmpegVideoTool.h"
#include "VideoWorker.h"
#include "LoginWidget.h"
#include "ExtractVideo.h"

typedef struct TaskInfo {
    QString taskName;
    QString taskDescription;
    QString taskConsumedTime;
}TaskInfo;

class FfmpegVideoTool : public QMainWindow
{
    Q_OBJECT

public:
    FfmpegVideoTool(QWidget *parent = Q_NULLPTR);
    void InitWidget();
    void ShowLoginWidget();
    int SetVideoParam(VideoWorker* worker);

 public slots:
    void OpenVideo();
public slots:
    void OneClickPro();
public slots:
    void VideoEdit();
public slots:
    void ShowVideoProgress(int videoId, int frameNum);
public slots:
    void SelectOutDir();

public slots:
    void StartVideoProTask();

public slots:
    void SetExtractVideoTime();
public slots:
    void SaveAndExit();
public slots:
    void UnsaveAndExit();

public slots:
    void Notify(QString taskName, QString time);

public:
    LoginWidget* loginWidget;
    VideoWorker* videoWorker;
    QThread* workerThread;

    QStringList selectedFiles;
    QList<TaskInfo> taskList;

private:
    Ui::FfmpegVideoToolClass ui;
    QAction* m_ActionToolBar1;
    QAction* m_ActionToolBar2;
    QAction* m_ActionToolBar3;
    QActionGroup* m_ToolBarActionGroup;


    QWidget* videoEditWidget;
    int rowCount;
    int isConcatOrTran;
};

