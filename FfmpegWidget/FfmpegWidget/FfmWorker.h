#pragma once
#include <QObject>
#include <QFileDialog> 
#include <QDebug>
#include <QScrollBar>
#include <QThread>
#include <QProcess>
#include <QDateTime>
#include <QMessageBox>

#include <QUuid>

class Worker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(QStringList fullFnList, QString outDir);

public:
    int MakeDir(QString dir);
    int SaveVideoInfo(QString videoInfo, QString fileSzie, QString fileDir, QString excelFn, int row, int inOrOut);
    void ChangeExcel(int row, int col, QString value, QString excelFn);

signals:
    void resultReady(int num);
 signals:
    void ErrorReport(int num, QString error);

public:
   
    QString decThdCnt;
    QString encThdCnt;
    QString httpStr;
    QString keyStr;
};