#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog> 
#include <QDebug>
#include <QScrollBar>
#include <QThread>
#include <QProcess>
#include <QDateTime>
#include <QMessageBox>

#include "ui_FfmpegWidget.h"
#include "FfmWorker.h"

class FfmpegWidget : public QMainWindow
{
    Q_OBJECT

public:
    FfmpegWidget(QWidget *parent = Q_NULLPTR);

    void InitSigSlot();
    void InitWidget();
  public slots:
    void SctInput();
public slots:
    void handleResults(int num);
public slots:
    void handleError(int num, QString error);
    void SctOutDir();
    void FfmpegExe();
    int MakeDir(QString dir);


public:
    QStringList fullFnList;
    QString outDir;
    QProcess* p;

signals:
    void StartWorker(QStringList fullFnList, QString outDir);

private:
    Ui::FfmpegWidgetClass ui;
};
