#include "FfmpegVideoTool.h"
#include "ImageComparison.h"


QStringList videoSizeOpts = { "1280x720 720x780","1280x720", "720x480" };
QStringList videoFmtOpts = { "m3u8", "mp4", "ts" };
QStringList videoQualityOpts = { "高", "中", "低" };


FfmpegVideoTool::FfmpegVideoTool(QWidget *parent)
    : QMainWindow(parent)
{
    rowCount = 0;
    videoWorker = nullptr;
    workerThread = nullptr;
    ui.setupUi(this);
}

void FfmpegVideoTool::InitWidget()
{
    ui.extractSetWidget->hide();
    
    //login widget
    loginWidget = new LoginWidget;
    loginWidget->InitWidget();
    //set combox format
  ui.comboBoxFormat->setEditable(true);
  //ui.comboBoxDir->setEditable(true);
  ui.comboBoxQuality->setEditable(true);
  //ui.comboBoxUrl->setEditable(true);

  //scollbar
  //ui.avProcessHScrollBar->setRange(0, 100);
  
     ui.comboBoxFormat->addItems(videoFmtOpts);
     ui.comboBoxQuality->addItems(videoQualityOpts);
     ui.comboBoxSize->addItems(videoSizeOpts);
 
    //set tablewidget
    QStringList header;
    header << "FileName" << "Format";
    ui.VideoTableWidget->setRowCount(0);
    ui.VideoTableWidget->setColumnCount(2);
    /*ui.VideoTableWidget->setMinimumWidth(100);*/
    ui.VideoTableWidget->setHorizontalHeaderLabels(header);
    ui.VideoTableWidget->horizontalHeader()->resizeSection(0, 150);
    ui.VideoTableWidget->horizontalHeader()->setFixedHeight(25);
    
    ui.VideoTableWidget->setShowGrid(false);//hide grid

    ui.VideoTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ScrollBar
    ui.VideoTableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
        "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
        "QScrollBar::handle:hover{background:gray;}"
        "QScrollBar::sub-line{background:transparent;}"
        "QScrollBar::add-line{background:transparent;}");
    //add toolbar
    m_ToolBarActionGroup = new QActionGroup(this);
    QList<QAction*> m_ToolBarList;

    m_ActionToolBar1 = new QAction(QStringLiteral("添加视频"));
    m_ActionToolBar2 = new QAction(QStringLiteral("任务列表"));
    m_ActionToolBar3 = new QAction(QStringLiteral("Help"));
    m_ActionToolBar1->setCheckable(true);
    m_ActionToolBar2->setCheckable(true);
    m_ActionToolBar3->setCheckable(true);
    m_ToolBarList.append(m_ActionToolBar1);
    m_ToolBarList.append(m_ActionToolBar2);
    m_ToolBarList.append(m_ActionToolBar3);
    m_ToolBarActionGroup->addAction(m_ActionToolBar1);
    m_ToolBarActionGroup->addAction(m_ActionToolBar2);
    m_ToolBarActionGroup->addAction(m_ActionToolBar3);
    ui.toolBar->addActions(m_ToolBarList);
    //ui.toolBar->setStyleSheet("background-color:rgb(84,170,255);color:rgb(204,204,204)");
    ui.toolBar->setStyleSheet("font: 15pt 'Agency FB'");
    qDebug() << "OPen FileDialog" << endl;

    //init
    videoWorker = new VideoWorker;
    workerThread = new QThread;

    //connect signal and slot
    connect(ui.oneClickProBtn, SIGNAL(clicked()), this, SLOT(OneClickPro()));
    connect(m_ActionToolBar1, SIGNAL(triggered()), this, SLOT(OpenVideo()));
    connect(ui.selectOutDirBtn, SIGNAL(clicked()), this, SLOT(SelectOutDir()));
    connect(m_ActionToolBar3, SIGNAL(triggered()), this, SLOT(VideoEdit()));
    connect(ui.startVideoProBtn, SIGNAL(clicked()), this, SLOT(StartVideoProTask()));

    connect(ui.selTimeBtn, SIGNAL(clicked()), this, SLOT(SetExtractVideoTime()));
    connect(ui.saveAndExitBtn, SIGNAL(clicked()), this, SLOT(SaveAndExit()));
    connect(ui.unsaveAndExitBtn, SIGNAL(clicked()), this, SLOT(UnsaveAndExit()));
   
}

void FfmpegVideoTool::ShowLoginWidget() {
    loginWidget->show();
}



void FfmpegVideoTool::OpenVideo()
{
    qDebug() << "OPen File-------" << endl;
    printf("open file --------");
    QFileDialog* fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(QStringLiteral("选择文件"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    //fileDialog->setNameFilter(tr("Images(*.png *.jpg *.jpeg *.bmp)"));
    fileDialog->setNameFilter(tr("text(*.mp4)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    QStringList fullFileNames;
    QStringList fileNames;
    QFileInfo fileinfo;
    QString tmp;
    QString outFullFn;
    int videoNum = 0;
    int raw = 0;
    int col = 0;

    qDebug() << " video  edit" << endl;
    if (fileDialog->exec())
    {
        fullFileNames = fileDialog->selectedFiles();
    }
    
    for (auto fullFn: fullFileNames)
    {
        fileinfo = QFileInfo(fullFn);
        tmp = fileinfo.fileName();
        /*for (int i = 0; i < tmp.size(); i++) {
            if (tmp[i] == '.') {
                break;
            }
            else if (tmp[i] >= '0' && tmp[i] <= '9') {

            }
            else {
                tmp.clear();
                break;
            }
        }*/

        if (!tmp.isEmpty()) {
            selectedFiles.append(fullFn);
            ui.VideoTableWidget->insertRow(rowCount);
            ui.VideoTableWidget->setItem(rowCount, col, new QTableWidgetItem(tmp));
            ui.VideoTableWidget->setItem(rowCount, col + 1, new QTableWidgetItem("Unknow"));
            rowCount++;
        }
        qDebug() << tmp << endl;
    }
}

void FfmpegVideoTool::SelectOutDir(){
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    QFileDialog* dirDialog = new QFileDialog(this);
    QString outDir;

    if(btn->objectName() == "selectOutDirBtn") {
        //ui.VideoTableWidget->setCurrentItem(NULL);
        outDir = dirDialog->getExistingDirectory(this, QStringLiteral("选择输出目录"), "./", QFileDialog::ShowDirsOnly);
        ui.outDirEditForTran->setText(outDir);
    }
}

void FfmpegVideoTool::OneClickPro() {

    /*videoWorker = new VideoWorker;
    workerThread = new QThread;*/
    qDebug() << " OneClickPro dir" << endl;
    QList<QTableWidgetItem*>items = ui.VideoTableWidget->selectedItems();
    int count = items.count() / 2;
    if (count == 0)
        return;

    QStringList dirList;
    QFileInfo fileinfo;
    videoWorker->taskType = TRANSCODE;
    if (videoWorker->inFullFnList.isEmpty()) {
   
    }
    for (int i = 0; i < count; i++)
    { 
        int row = ui.VideoTableWidget->row(items.at(i));//获取选中的行
        QString filename = ui.VideoTableWidget->item(row, 0)->text();
        for (auto fn : selectedFiles) {
            fileinfo.setFile(fn);
            if (filename == fileinfo.fileName()) {
                videoWorker->inFullFnList.append(fn);
            }
        }
    }
    for (auto tmp : videoWorker->inFullFnList) {
        qDebug() << " selected dir" << tmp << endl;
    }
    
    
    connect(workerThread, &QThread::finished, videoWorker, &QObject::deleteLater);
    /*connect(videoWorker, SIGNAL(sigNumOfFramesProcessed(int num)), this, SLOT(ShowProNumOfFrames(int num)));*/
    
  
    connect(workerThread, SIGNAL(started()), videoWorker, SLOT(ffmpegVideoPro()));
    connect(videoWorker, SIGNAL(sigVideoProcess(int, int)), this, SLOT(ShowVideoProgress(int, int)));
    connect(videoWorker, SIGNAL(sigTaskInfo(QString, QString)), this, SLOT(Notify(QString, QString)));
    videoWorker->moveToThread(workerThread);
    workerThread->start();
   
}

void FfmpegVideoTool::VideoEdit() {
    qDebug() << " video  edit" << endl;
}

void FfmpegVideoTool::ShowVideoProgress(int videoId, int frameNum) {
    qDebug() << "processed num of frames" << endl;
}

void FfmpegVideoTool::StartVideoProTask() {
    int ret;
    VideoWorker* worker = new VideoWorker;
    QThread* thread = new QThread;
    
    switch (ui.tabWidget->currentIndex())
    {
    case 0:
        ret =  PictureShow();
    case 1:
        worker->taskType = TRANSCODE;
    case 2:
        worker->taskType = CONCAT;
    case 3:
        worker->taskType = EXTRACT;
    case 4:
        worker->taskType = GET_KEY_PIC;
    default:
        break;
    }


    
    ret = SetVideoParam(worker);
    if (ret < 0) {
        return;
    }

    for (auto tmp : worker->inFullFnList) {
        qDebug() << " selected dir" << tmp << endl;
    }
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    /*connect(videoWorker, SIGNAL(sigNumOfFramesProcessed(int num)), this, SLOT(ShowProNumOfFrames(int num)));*/

    connect(thread, SIGNAL(started()), worker, SLOT(ffmpegVideoPro()));
    //connect(worker, SIGNAL(sigVideoProcess(int, int)), this, SLOT(ShowVideoProgress(int, int)));
    worker->moveToThread(thread);
    thread->start();
}

int FfmpegVideoTool::SetVideoParam(VideoWorker* worker) {
    
    QList<QTableWidgetItem*>items = ui.VideoTableWidget->selectedItems();
    QString outDir = ui.outDirEditForTran->text();

    if (items.isEmpty() || outDir.isEmpty()) {
        QMessageBox::warning(NULL, "warning", QStringLiteral("请选择视频或设置目录"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return -1;
    }

    QFileInfo fileinfo;
    int count = items.count();
    worker->dstVideoParam.inFnlist.clear();

    for (int i = 0; i < count; i= i+2)
    {
        int row = ui.VideoTableWidget->row(items.at(i));//获取选中的行
        QString filename = ui.VideoTableWidget->item(row, 0)->text();
        for (auto fn : selectedFiles) {
            fileinfo.setFile(fn);
            if (filename == fileinfo.fileName()) {
                worker->dstVideoParam.inFnlist.append(fn);
            }
        }
    }

    worker->dstVideoParam.outDir = outDir;


}

void FfmpegVideoTool::Notify(QString taskName, QString time) {
    QMessageBox::warning(NULL, "warning", time, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

void FfmpegVideoTool::SetExtractVideoTime() {
    ui.extractSetWidget->show();
    selectedFiles.clear();
    if (selectedFiles.isEmpty()) {
        QMessageBox::warning(NULL, "warning", QStringLiteral("Content"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }

}

void FfmpegVideoTool::SaveAndExit() {
    ui.extractSetWidget->hide();
}

void FfmpegVideoTool::UnsaveAndExit() {
    ui.extractSetWidget->hide();
}