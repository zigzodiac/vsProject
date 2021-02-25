#include "VideoWorker.h"

VideoWorker::VideoWorker()
{
	threadStatus = false;
}

VideoWorker::~VideoWorker()
{

}

int VideoWorker::MakeDir(QString dir)
{
	QDir dir1;
	int ret;
	//创建目录
	//返回bool类型判断是否创建成功
	if (dir1.mkdir(dir))  //这里只能一级一级创建不能一次性创建多级目录
	{
		qDebug() << "new error" << endl;
	}
	else
	{
		qDebug() << "new complten" << endl;
	}

	//目录是否存在
	if (!dir1.exists())
	{
		return -1;
		qDebug() << "dir不存在";
	}
	else
	{
		qDebug() << "dir存在";
	}
	return 0;
}
int VideoWorker::TranscodeCompress() {
	QDateTime startTime, endTime, dutation;
	startTime = QDateTime::currentDateTime();
	int ret;
	QString outDir = "D:/TestVideo/";
	QString outFullDir;
	QString outFullFn;
	QDir dir1;
	//创建目录
	//返回bool类型判断是否创建成功


	QString outFn;
	QString diffDirName;
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyyMMddhhmmsszzzddd");
	QFileInfo fileinfo;
	QString filename;
	if (inFullFnList.isEmpty())
		return -1;
	for (auto fn : inFullFnList) {
		fileinfo.setFile(fn);
		filename = fileinfo.fileName();
		for (auto ch : filename) {
			if (ch == '.') {
				break;
			}
			else {
				outFn += ch;
			}
		}
		current_date = current_date_time.toString("yyyyMMddhhmmsszzz"); //yyyyMMddhhmmsszzzddd  d:周
		outFn += '_';
		outFn += current_date;
		diffDirName = "1280_720p";
		outFullDir = outDir + outFn + diffDirName;
		ret = MakeDir(outFullDir);

		if (ret != 0)
			continue;
		outFullFn = outFullDir + '/' + outFn + diffDirName + ".m3u8";

		ret = transcode(fn, outFullFn, 1280, 720);

		endTime = QDateTime::currentDateTime();

		qint64 diffs = startTime.secsTo(endTime);
		qDebug() << "transcodeCompress video consume time " << diffs << endl;
		return ret;
	}
}

int VideoWorker::GetFrameForContent() {
	int ret;

	ret = OpenVideo(dstVideoParam.inFnlist.first().toStdString().c_str());
	if (ret < 0) {
		qDebug() << "open video file fail";
	}
	ret = GetVideoPic();
	return ret;
}

int VideoWorker::ffmpegConcatVideo() {

	int ret;

	QDateTime startTime, endTime, dutation;
	startTime = QDateTime::currentDateTime();
	//dstVideoParam.outFn = dstVideoParam.outDir + "/" + "concat_test.mp4";
	//select first video file extension
	QFileInfo fileInfo = QFileInfo(dstVideoParam.inFnlist.first());
	dstVideoParam.outFn = dstVideoParam.outDir + "/" + "concat_test." + fileInfo.suffix();
	ret = ConcatVideo(dstVideoParam);
	if (ret < 0)
		return ret;
	endTime = QDateTime::currentDateTime();

	qint64 diffs = startTime.secsTo(endTime);
	taskUsedTime = QDateTime::fromTime_t(diffs).toString("dd-hh:mm:ss");
	qDebug() << "concat video consume time "<< taskUsedTime << endl;
	return ret;
	
}

void VideoWorker::ffmpegVideoPro()
{
	int ret;
	switch (taskType)
	{
	case TRANSCODE:
		ret = TranscodeCompress();
	case CONCAT:
		ret = ffmpegConcatVideo();
	case GET_KEY_PIC:
		ret = GetFrameForContent();
	default:
		break;
	}
}

