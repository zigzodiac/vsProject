#include "FfmWorker.h"
#include <QAxObject>
#include "EncryptKey.h"


void Worker::doWork(QStringList fullFnList, QString outDir) {

	QProcess* p;
	QString result;
	/* ... here is the expensive or blocking operation ... */
	QFileInfo fileinfo;
	QString filename;
	QString outFn;
	QString current_date;
	QString diffDirName1;
	QString diffDirName2;
	QString outFullDir;
	QString outFullDir1;
	QString outFullDir2;
	QString fullOutFn1;
	QString fullOutFn2;
	bool isOk;
	int ret;

	QThread* ffmTh = new QThread;

	p = new QProcess(this);

	//QString cmd = "ffmpeg -i D:\\TestVideo\\test1.mp4 -s 960x720 -c:v libx264 -c:a copy  -f hls -hls_list_size 0  -hls_time 10  high/playlistHigh.m3u8 -s 720x480 -c:v libx264 -c:a copy  -f hls -hls_list_size 0  -hls_time 10 medium/playlistMedium.m3u8";
	QString program = "ffmpeg -threads " +decThdCnt+" -i ";
	QString sizeOpt1 = " -s 1280x720";
	QString sizeOpt2 = " -s 960x540";
	QString outOpt = " -threads " + encThdCnt +" -c:v libx264 -c:a copy -qmin 18 -qmax 31 ";
	QString SegOpt = " -c copy -f hls -hls_list_size 0 -hls_time 10 ";
	QString keyOpt = " -hls_key_info_file ";
	/*QMessageBox testMassage;*/
	
	/*fullOutFn2 = "D:/TestVideo/test1_20210104103254605_720_480p/test1_20210104103254605_720_480p.ts";

	fullOutFn2.replace('/', '\\');
	QString delCmdtest = "del /f /s /q  "+ fullOutFn2;
	system(delCmdtest.toStdString().c_str());*/


	////////////////
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString fullCmd;
	
	for (auto fn : fullFnList) {
		outFn.clear();
		fileinfo.setFile(fn);
		filename = fileinfo.fileName();
		current_date = current_date_time.toString("yyyyMMddhhmmsszzz");

		for (auto ch : filename) {
			if (ch == '.') {
				break;
			}
			else {
				outFn += ch;
			}
		}
		
		p->setWorkingDirectory(outDir);
		current_date = current_date_time.toString("yyyyMMddhhmmsszzz"); //yyyyMMddhhmmsszzzddd  d:ÖÜ
		outFn += '_';
		outFn += current_date;
		outFullDir = outDir + '/' + outFn;
		ret = MakeDir(outFullDir);

		diffDirName1 = "1280_720p";
		outFullDir1 = outFullDir + "/" + diffDirName1;
		ret = MakeDir(outFullDir1);
		fullOutFn1 = outFullDir1+ '/' + outFn +"_"+ diffDirName1+ ".ts";
		QString excelFn1 = outFullDir1 + '/' + outFn +"_"+ diffDirName1 + ".xlsx";

		diffDirName2 = "720_480p";
		outFullDir2 = outFullDir + '/' + diffDirName2;
		ret = MakeDir(outFullDir2);
		fullOutFn2 = outFullDir2 + '/' + outFn +"_"+ diffDirName2 + ".ts";
		QString excelFn2 = outFullDir2 + '/' + outFn + "_" + diffDirName2 + ".xlsx";
		
		// create key info for 1280x720
		QUuid id = QUuid::createUuid();
		QString strId = id.toString();
		strId.remove('-');
		QString str = strId.mid(8, 16);
		QString keystr = "aEPsRmS/W1tuZEmktdWtC7ALcE8VBGIV";
		int i = 1;
		//int strBase64Len;
		//char* strBase64 = base64Encode((const char*)(char*)str.toStdString().c_str(), 16, 0x100, &strBase64Len);
		char* dst = (char*)malloc(100);
		memset(dst, '\0', 100);
		int dstLen;
		int retVal;
		int outLen;
		retVal = TripleDESEncryt(str, keystr, dst, &outLen);
		if (retVal != 0) {
			return;
		}
		
		QString keyFn = outFullDir + '/' + outFn + "enc.key";
		QString encryptKey = outFullDir + '/' + outFn  + "encryptEnc.key";
		QFile fp(keyFn);

		isOk = fp.open(QIODevice::ReadWrite | QIODevice::Text);
		if (!isOk) {
			ErrorReport(-1, QStringLiteral("create enc key info failed"));
			continue;
		}
		QTextStream out(&fp);
		out << str;
		fp.close();

		// create key info for 720x480
		fp.setFileName(encryptKey);
		isOk = fp.open(QIODevice::ReadWrite | QIODevice::Text);
		if (!isOk) {
			ErrorReport(-1, QStringLiteral("create enc key info failed"));
			continue;
		}
		QTextStream out1(&fp);
		out1 << dst;
		fp.close();


		QString keyInfoFn = outFullDir + '/' + outFn  + "enc.keyinfo";
		//QString keyInfoFn2 = outFullDir2 + '/' + outFn + "enc.keyinfo";
		QString httpInfo = httpStr +"/" + outFn + "/" + outFn  + "encryptEnc.key";
		//QString httpInfo2 = "http://192.168.154.100:1935/video/" + outFn + diffDirName2 + "/" + "enc.key";
		fp.setFileName(keyInfoFn);
		isOk = fp.open(QIODevice::ReadWrite | QIODevice::Text);
		if (!isOk) {
			ErrorReport(-1, QStringLiteral("create enc key info failed"));
			continue;
		}
		QTextStream out2(&fp);
		out2 << httpInfo << "\n";
		out2 << keyFn;
		fp.close();

		/*fp.setFileName(keyInfoFn2);
		isOk = fp.open(QIODevice::ReadWrite | QIODevice::Text);
		if (!isOk) {
			ErrorReport(-1, QStringLiteral("create enc key info failed"));
			continue;
		}
		QTextStream out3(&fp);
		out3 << httpInfo2 << "\n";
		out3 << outFn + "enc.key";
		fp.close();*/


		QString p_stdout = "";
		p->setProcessChannelMode(QProcess::MergedChannels);
		emit resultReady(i);

		QString getInfoCmd = "ffmpeg -i " + fn;
		QString videoInfo;
		p->start(getInfoCmd);
		if(p->waitForFinished())
			videoInfo = QString::fromLocal8Bit(p->readAllStandardOutput());
		qDebug() << videoInfo;
		SaveVideoInfo(videoInfo, QString::number(fileinfo.size()), fn, excelFn1, 1, 0);
		SaveVideoInfo(videoInfo, QString::number(fileinfo.size()), fn, excelFn2, 1, 0);


		fullCmd = program + fn + sizeOpt1 + outOpt + fullOutFn1 + sizeOpt2 + outOpt +  fullOutFn2;
		qDebug() << fullCmd;
		p->start(fullCmd);
		//QString strTemp = QString::fromLocal8Bit(p->readAllStandardOutput());
		p_stdout = p->readAll();   //获取external程序输出/打印信息
		qDebug() << p_stdout;
		if (p->waitForFinished(-1))
		{
			p_stdout = p->readAll();   //获取external程序输出/打印信息
			qDebug() << p_stdout;
		}
		else {
			qDebug() << p->errorString();
		}
			
		
		QString outVideoInfo1;
		QString outVideoInfo2;
		QString getSegCmd1 = "ffmpeg -i " + fullOutFn1 + SegOpt + keyOpt +  keyInfoFn + " " + outFullDir1 + '/' + outFn + diffDirName1 + ".m3u8";
		QString getSegCmd2 = "ffmpeg -i " + fullOutFn2 + SegOpt + keyOpt +  keyInfoFn + " " + outFullDir2 + '/' + outFn + diffDirName2 + ".m3u8";
		qDebug() << getSegCmd1;
		qDebug() << getSegCmd2;
		p->setWorkingDirectory(outFullDir1);
		p->start(getSegCmd1);
		if (p->waitForFinished()) {
			outVideoInfo1 = QString::fromLocal8Bit(p->readAllStandardOutput());
			qDebug() <<"---------------"<< outVideoInfo1;
		}
		else {
			qDebug() <<"---------------"<< p->errorString();
		}

		p->setWorkingDirectory(outFullDir2);
		p->start(getSegCmd2);
		if (p->waitForFinished()) {
			outVideoInfo2 = QString::fromLocal8Bit(p->readAllStandardOutput());
			qDebug() << outVideoInfo2;
		}
		else {
			qDebug() << p->errorString();
		}

		QString getOutInfoCmd1 = "ffmpeg -i " + fullOutFn1;
		QString getOutInfoCmd2 = "ffmpeg -i " + fullOutFn2;
		fileinfo.setFile(fullOutFn1);
		p->setWorkingDirectory(outFullDir1);
		p->start(getOutInfoCmd1);
		if (p->waitForFinished()) {
			outVideoInfo1 = QString::fromLocal8Bit(p->readAllStandardOutput());
			qDebug() << outVideoInfo1;
			SaveVideoInfo(outVideoInfo1, QString::number(fileinfo.size()), outFullDir1 + '/' + outFn + diffDirName1 + ".m3u8", excelFn1, i, 1);
		}
		else {
			qDebug() << p->errorString();
		}

		fileinfo.setFile(fullOutFn2);
		p->setWorkingDirectory(outFullDir2);
		p->start(getOutInfoCmd2);
		if (p->waitForFinished()) {
			outVideoInfo2 = QString::fromLocal8Bit(p->readAllStandardOutput());
			qDebug() << outVideoInfo2;
			SaveVideoInfo(outVideoInfo2, QString::number(fileinfo.size()), outFullDir2 + '/' + outFn + diffDirName2 + ".m3u8", excelFn2, i, 1);
		}
		else {
			qDebug() << p->errorString();
		}

		///
		QString getPicCmd = "ffmpeg -i " + fullOutFn1 +" -r 30 -y -f  image2 -ss 00:00:01 -vframes 1  " + outFullDir + '/' + outFn + current_date + ".jpg";
		qDebug() << "---------------" << getPicCmd;
		p->start(getPicCmd);
		p->waitForFinished();
		outVideoInfo2 = QString::fromLocal8Bit(p->readAllStandardOutput());
		qDebug() << outVideoInfo2;

		fullOutFn1.replace('/', '\\');
		QString delCmd = "del /f /s /q  " + fullOutFn1;
		qDebug() << "---------------" << delCmd;
		system(delCmd.toStdString().c_str());
		/*p->start(delCmd);
		p->waitForFinished();
		outVideoInfo2 = QString::fromLocal8Bit(p->readAllStandardOutput());
		qDebug() << outVideoInfo2;*/

		/*getPicCmd = "ffmpeg - i " + fullOutFn2 + " -r 30 -y -f  image2 -ss 00:00:01 -vframes 1  " + outFullDir2 + '/' + outFn + diffDirName2 + ".jpg";
		qDebug() << "---------------" << getPicCmd;
		p->start(getPicCmd);
		p->waitForFinished();
		outVideoInfo2 = QString::fromLocal8Bit(p->readAllStandardOutput());
		qDebug() << outVideoInfo2;*/

		fullOutFn2.replace('/', '\\');
		delCmd = "del /f /s /q  " + fullOutFn2;
		qDebug() << "---------------" << delCmd;
		system(delCmd.toStdString().c_str()); 
		/*p->start(delCmd);
		p->waitForFinished();
		outVideoInfo2 = QString::fromLocal8Bit(p->readAllStandardOutput());
		qDebug() << outVideoInfo2;*/

		i++;
	}
	emit resultReady(0);
	
}


int Worker::MakeDir(QString dir)
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
	return 0;
}

int Worker::SaveVideoInfo(QString videoInfo, QString fileSzie, QString videoFn, QString excelFn, int row, int inOrOut)
{
	QString Duration;
	QString bitrate;
	QString fps;
	QString videoFmt;
	QString pixFmt;
	QFileInfo vFileInfo(videoFn);


	static bool IsColHeader = false;
	int posDur = videoInfo.indexOf("Duration:");
	if (posDur == -1)
		Duration = "null";
	Duration = videoInfo.mid(posDur + 10, 12);

	int posBr = videoInfo.indexOf("bitrate:", posDur) + 9;
	if (posBr == -1)
		bitrate = "null";
	int posBrEnd = videoInfo.indexOf("kb/s") + 3;
	posBrEnd == -1 ? bitrate = "null" : bitrate = videoInfo.mid(posBr, posBrEnd - posBr + 1);

	int start = videoInfo.indexOf("Video:", posBrEnd) + 7;
	int end = videoInfo.indexOf(" ", start);
	videoFmt = videoInfo.mid(start, end - start);

	start = videoInfo.indexOf(",", end);
	end = videoInfo.indexOf(",", start + 1);
	pixFmt = videoInfo.mid(start + 1, end - 1 - start);


	end = videoInfo.indexOf("fps", start);
	fps = videoInfo.mid(end - 4, 7);

	//QString fn = excelFn.replace('/', '\\');

	QAxObject* excel = new QAxObject(this);
	excel->setControl("Excel.Application");//连接Excel控件
	excel->dynamicCall("SetVisible (bool Visible)", "false");//不显示窗体
	excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
	QAxObject* workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
	QAxObject* workbook;
	
	QFileInfo fileInfo(excelFn);
	if (!fileInfo.isFile()) {
		workbooks->dynamicCall("Add");//新建一个工作簿
		workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
	}
	else {
		workbook = workbooks->querySubObject("Open(const QString&)", (excelFn));
	}
	

	QAxObject* worksheets = workbook->querySubObject("Sheets");//获取工作表集合
	QAxObject* worksheet = worksheets->querySubObject("Item(int)", 1);//获取工作表集合的工作表1，即sheet1


	QAxObject* usedrange = worksheet->querySubObject("UsedRange"); // sheet范围
	QAxObject* cells = usedrange->querySubObject("Columns");
	cells->dynamicCall("AutoFit");

	usedrange->setProperty("HorizontalAlignment", -4108);
	usedrange->setProperty("VerticalAlignment", -4108);

	int intRowStart = usedrange->property("Row").toInt(); // 起始行数   为1
	int intColStart = usedrange->property("Column").toInt();  // 起始列数 为1

	QAxObject* rows, * columns;
	rows = usedrange->querySubObject("Rows");  // 行
	columns = usedrange->querySubObject("Columns");  // 列


	int intRow = rows->property("Count").toInt(); // 行数
	int intCol = columns->property("Count").toInt();  // 列数

	int col = 0;
	inOrOut == 0 ? col = 0 : col = 10; 
	col > 0 ? IsColHeader = true : IsColHeader = false;


	if (!IsColHeader) {
		QAxObject* cell_1 = worksheet->querySubObject("Cells(int,int)", row, col + 1);
		QAxObject* cell_2 = worksheet->querySubObject("Cells(int,int)", row, col + 2);
		QAxObject* cell_3 = worksheet->querySubObject("Cells(int,int)", row, col + 3);
		QAxObject* cell_4 = worksheet->querySubObject("Cells(int,int)", row, col + 4);
		QAxObject* cell_5 = worksheet->querySubObject("Cells(int,int)", row, col + 5);
		QAxObject* cell_6 = worksheet->querySubObject("Cells(int,int)", row, col + 6);
		QAxObject* cell_7 = worksheet->querySubObject("Cells(int,int)", row, col + 7);
		QAxObject* cell_8 = worksheet->querySubObject("Cells(int,int)", row, col + 8);


		cell_1->setProperty("Value2", QStringLiteral("原文件名"));
		cell_2->setProperty("Value2", QStringLiteral("目录"));
		cell_3->setProperty("Value2", QStringLiteral("时长"));
		cell_4->setProperty("Value2", QStringLiteral("文件大小"));
		cell_5->setProperty("Value2", QStringLiteral("比特率"));
		cell_6->setProperty("Value2", QStringLiteral("视频协议"));
		cell_7->setProperty("Value2", QStringLiteral("帧率"));
		cell_8->setProperty("Value2", QStringLiteral("图像格式"));
		

		QAxObject* cell_11 = worksheet->querySubObject("Cells(int,int)", row, col + 10 + 1);
		QAxObject* cell_12 = worksheet->querySubObject("Cells(int,int)", row, col + 10 + 2);
		QAxObject* cell_13 = worksheet->querySubObject("Cells(int,int)", row, col + 10 + 3);
		QAxObject* cell_14 = worksheet->querySubObject("Cells(int,int)", row, col + 10 + 4);
		QAxObject* cell_15 = worksheet->querySubObject("Cells(int,int)", row, col + 10 + 5);
		QAxObject* cell_16 = worksheet->querySubObject("Cells(int,int)", row, col + 10  + 6);
		QAxObject* cell_17 = worksheet->querySubObject("Cells(int,int)", row, col + 10 + 7);
		QAxObject* cell_18 = worksheet->querySubObject("Cells(int,int)", row, col + 10 + 8);


		cell_11->setProperty("Value2", QStringLiteral("输出文件名"));
		cell_12->setProperty("Value2", QStringLiteral("目录"));
		cell_13->setProperty("Value2", QStringLiteral("时长"));
		cell_14->setProperty("Value2", QStringLiteral("文件大小"));
		cell_15->setProperty("Value2", QStringLiteral("比特率"));
		cell_16->setProperty("Value2", QStringLiteral("视频协议"));
		cell_17->setProperty("Value2", QStringLiteral("帧率"));
		cell_18->setProperty("Value2", QStringLiteral("图像格式"));
		

		
	}
	

	QAxObject* cell_1 = worksheet->querySubObject("Cells(int,int)", row + 1, col + 1);
	QAxObject* cell_2 = worksheet->querySubObject("Cells(int,int)", row + 1, col + 2);
	QAxObject* cell_3 = worksheet->querySubObject("Cells(int,int)", row + 1, col + 3);
	QAxObject* cell_4 = worksheet->querySubObject("Cells(int,int)", row + 1, col + 4);
	QAxObject* cell_5 = worksheet->querySubObject("Cells(int,int)", row + 1, col + 5);
	QAxObject* cell_6 = worksheet->querySubObject("Cells(int,int)", row + 1, col + 6);
	QAxObject* cell_7 = worksheet->querySubObject("Cells(int,int)", row + 1, col + 7);
	QAxObject* cell_8 = worksheet->querySubObject("Cells(int,int)", row + 1, col + 8);

	/*cell_1->setProperty("Value2", videoFn);
	cell_1->setProperty("Value2", Duration);
	cell_1->setProperty("Value2", fileSzie);
	cell_1->setProperty("Value2", bitrate);*/
	Duration += ' ';
	cell_1->dynamicCall("SetValue(conts QVariant&)", vFileInfo.fileName());
	cell_2->dynamicCall("SetValue(conts QVariant&)", vFileInfo.path());
	QVariant va(Duration);
	cell_3->setProperty("NumberFormatLocal", "hh:mm:ss.00");
	cell_3->setProperty("Value", va);

	cell_4->dynamicCall("SetValue(conts QVariant&)", fileSzie);
	cell_5->dynamicCall("SetValue(conts QVariant&)", bitrate);
	cell_6->dynamicCall("SetValue(conts QVariant&)", videoFmt);
	cell_7->dynamicCall("SetValue(conts QVariant&)", fps);
	cell_8->dynamicCall("SetValue(conts QVariant&)", pixFmt);

	

	workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(excelFn));//保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
	workbook->dynamicCall("Close()");//关闭工作簿
	excel->dynamicCall("Quit()");//关闭excel
	delete excel;
	excel = NULL;

	return 0;
}

void Worker::ChangeExcel(int row, int col, QString value, QString excelFn)
{
	QAxObject* excel = new QAxObject(this);
	excel->setControl("Excel.Application");//连接Excel控件
	excel->dynamicCall("SetVisible (bool Visible)", "false");//不显示窗体
	excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
	QAxObject* workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
	QAxObject* workbook;

	QFileInfo fileInfo(excelFn);
	if (fileInfo.isFile()) {
		workbook = workbooks->querySubObject("Open(const QString&)", (excelFn));
	}
	else {
		qDebug() << excelFn << "is not exsit";
	}


	QAxObject* worksheets = workbook->querySubObject("Sheets");//获取工作表集合
	QAxObject* worksheet = worksheets->querySubObject("Item(int)", 1);//获取工作表集合的工作表1，即sheet1


	QAxObject* usedrange = worksheet->querySubObject("UsedRange"); // sheet范围
	int intRowStart = usedrange->property("Row").toInt(); // 起始行数   为1
	int intColStart = usedrange->property("Column").toInt();  // 起始列数 为1

	QAxObject* rows, * columns;
	rows = usedrange->querySubObject("Rows");  // 行
	columns = usedrange->querySubObject("Columns");  // 列


	int intRow = rows->property("Count").toInt(); // 行数
	int intCol = columns->property("Count").toInt();  // 列数

	QAxObject* cell = worksheet->querySubObject("Cells(int,int)", row, col);
	cell->dynamicCall("SetValue(conts QVariant&)", value);
}
