#pragma once

//Use the same encoding protocol, convert to ts package format and compress

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QDir>
#include "Utils.h"
#include  "ConcatVideo.h"
#include "TransCoding.h"
#include "TransCodeCompress.h"
#include "ExtractVideo.h"


class VideoWorker : public QObject
{
	Q_OBJECT
public:
	VideoWorker();
	~VideoWorker();
	int MakeDir(QString dir);
signals:
	void sigVideoProcess(int videoId, int frameNum);
signals:
	void sigTaskInfo(QString taskName, QString time);


public slots:
	void ffmpegVideoPro();

	

public:
	int GetFrameForContent();
	int TranscodeCompress();
	int ffmpegConcatVideo();

public:
	VideoParam dstVideoParam;
	
	
	QStringList inFullFnList;
	QStringList outFullFnList;
	QStringList inFnListForConcat;
	QString outFnForConcat;
	QString outDir;
	QString outDirForConcat;
	bool threadStatus;
	int isConcatOrTran;
	QString taskUsedTime;
	QString taskName;
	TaskType  taskType;

};

