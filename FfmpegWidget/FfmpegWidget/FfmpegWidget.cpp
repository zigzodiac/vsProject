#include "FfmpegWidget.h"

FfmpegWidget::FfmpegWidget(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

void FfmpegWidget::SctInput()
{
    qDebug() << "OPen File" << endl;
    QFileDialog* fileDialog = new QFileDialog(this);

    fileDialog->setWindowTitle(QStringLiteral("选择文件"));

    fileDialog->setDirectory(".");

    //fileDialog->setNameFilter(tr("Images(*.png *.jpg *.jpeg *.bmp)"));
    fileDialog->setNameFilter(tr("text(*.mp4)"));
  
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
  
    fileDialog->setViewMode(QFileDialog::Detail);

    if (fileDialog->exec())
    {
        fullFnList = fileDialog->selectedFiles();
    }

	QString str = QString::number(fullFnList.size());

	str += " videos have been selected";
	ui.inDirLineEdit->setText(str);

}

void FfmpegWidget::InitSigSlot()
{
	connect(ui.sctInFilesBtn, SIGNAL(clicked()), this, SLOT(SctInput()));
	connect(ui.sctOutDirBtn, SIGNAL(clicked()), this, SLOT(SctOutDir()));
	connect(ui.proBtn, SIGNAL(clicked()), this, SLOT(FfmpegExe()));
}

void FfmpegWidget::InitWidget()
{
	ui.lineEditSize->setText(QStringLiteral("1280x720, 720x480"));
	ui.lineEditQa->setText(QStringLiteral("默认"));
	ui.lineEditFmt->setText(QStringLiteral("m3u8"));
	ui.decLineEdit->setText("4");
	ui.encLineEdit->setText("2");
	qDebug() << ui.encLineEdit->text();
	qDebug() << ui.decLineEdit->text();

}

void FfmpegWidget::SctOutDir()
{
    outDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", 
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	ui.outDirLineEdit->setText(outDir);
}

void FfmpegWidget::handleResults(int num) 
{
	if (num == 0) {
		ui.label_3->setText(QStringLiteral("已经处理完所有视频"));
		QMessageBox box;
		box.setText(QStringLiteral("视频处理任务已完成"));
		box.exec();
		ui.curStr->setText("");

		ui.proBtn->setAttribute(Qt::WA_TransparentForMouseEvents, false);
		ui.sctInFilesBtn->setAttribute(Qt::WA_TransparentForMouseEvents, false);
		ui.sctOutDirBtn->setAttribute(Qt::WA_TransparentForMouseEvents, false);
		return;
	}
	int len = fullFnList.size();
	QString curFn = fullFnList[num - 1];
	QFileInfo fileinfo;
	fileinfo.setFile(curFn);
	 QString curstr = QStringLiteral("正在处理视频") + fileinfo.fileName();
	ui.curStr->setText(curstr);
	
	ui.label_3->setText(QStringLiteral("正在处理第") + QString::number(num) + QStringLiteral("个视频"));


}

void FfmpegWidget::handleError(int num, QString error) {
	QMessageBox::warning(this, QStringLiteral("异常"), error);
}


void FfmpegWidget::FfmpegExe()
{

	if (outDir.isEmpty() || fullFnList.isEmpty()) {
		QMessageBox box;
		box.setText(QStringLiteral("请选择输入输出目录"));
		box.exec();
		return;
	}
	else {
		ui.proBtn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
		ui.sctInFilesBtn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	    ui.sctOutDirBtn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	}

	if (ui.decLineEdit->text().toInt() > 10 || ui.decLineEdit->text().toInt() < 1 || 
		ui.encLineEdit->text().toInt() > 10 || ui.encLineEdit->text().toInt() < 1)
	{
		QMessageBox box;
		box.setText(QStringLiteral("解码线程和编码线程数最大为10，最小为1"));
		box.exec();
		return;
	}

	//ui.radioButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);

	Worker* worker = new Worker;
	QThread *workerThread = new QThread;

	worker->decThdCnt = ui.decLineEdit->text();
	worker->encThdCnt = ui.encLineEdit->text();
	worker->httpStr = ui.urlLineEdit->text();

	if (worker->httpStr.isEmpty()) {
		QMessageBox box;
		box.setText(QStringLiteral("密钥url 不能为空"));
		box.exec();
		return;
	}


	worker->moveToThread(workerThread);
	connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
	connect(this, SIGNAL(StartWorker(QStringList, QString)), worker, SLOT(doWork(QStringList, QString)));
	connect(worker, &Worker::resultReady, this, &FfmpegWidget::handleResults);
	connect(worker, &Worker::ErrorReport, this, &FfmpegWidget::handleError);

	emit StartWorker(fullFnList, outDir);
	workerThread->start();

	}


int FfmpegWidget::MakeDir(QString dir)
{
	QDir dir1;
	int ret;

	if (dir1.mkdir(dir))
	{
		qDebug() << "new error" << endl;
	}
	else
	{
		qDebug() << "new complten" << endl;
	}

	if (!dir1.exists())
	{
		return -1;
		qDebug() << "目录不存在";
	}
	else
	{
		qDebug() << "目录存在";
	}
	return 0;
}