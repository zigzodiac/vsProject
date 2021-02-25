#include "Worker.h"



Worker::Worker()
{
    threadSta = false;
	scrStmThdSta = false;
}

Worker::~Worker()
{
}

void Worker::CallbackUpdateQImage(uchar* RgbBuffer, int PicWidth, int Height)
{
    QImage tmpImg(RgbBuffer, PicWidth, Height, QImage::Format_RGB32);
    QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
    emit sig_GetOneFrame(image);  //发送信号
}

void Worker::YUVToRGB(AVFrame* RrameYUV, int PicWidth, int PicHeight)
{
	AVFrame* m_pFrameRGB;
	SwsContext* m_img_convert_ctx;
	uint8_t* m_rgbBuffer, * m_yuvBuffer;


	m_pFrameRGB = av_frame_alloc();
	int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, PicWidth, PicHeight);

	m_img_convert_ctx = sws_getContext(PicWidth, PicHeight, AV_PIX_FMT_YUV420P,
		PicWidth, PicHeight, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
}

void Worker::ExitThread()
{
    qDebug() << "current thread ID --- signal ThreadExit" << QThread::currentThreadId();
    threadSta = false;
 }



void Worker::FfmpegCameraStreaming() {

    AVFormatContext* pFormatCtx;
    AVPacket* pcaket;
    AVInputFormat* ifmt;//使用libavdevice的时候，唯一的不同在于需要首先查找用于输入的设备
    int i, videoindex;
    int numBytes;
    int ret, got_picture;
    AVCodecContext* pCodecCtx;
    AVCodec* pCodec;
    AVFrame* pFrame, * pFrameRGB;
    AVPacket* packet;
    uint8_t* out_buffer;
    qDebug() << "current thread ID --- ffmpeg_test:" << QThread::currentThreadId();

    av_register_all(); //初始化FFMPEG  调用了这个才能正常适用编码器和解码器
    avdevice_register_all();//初始化libavdevice并注册所有输入和输出设备
    pFormatCtx = avformat_alloc_context();//分配一个AVFormatContext,查找用于输入的设备

    //使用libavdevice读取数据,和直接打开视频文件比较类似,
    //  因为系统的设备也被FFmpeg认为是一种输入的格式（即AVInputFormat）
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();//获取当前可用摄像头
    qDebug() << cameras.size();
    QString cam_name = QString("video=") + cameras.at(0).description();//格式必须为"video=Integrated Camera"
    qDebug() << cam_name;
    QByteArray char_cam_name = cam_name.toLatin1();//将QString类型数据转化为 const char* 类型


    ifmt = av_find_input_format("dshow");//Libavdevice选择dshow（DirectShow）设备作为输入端


    //Set own video device's name
    if (avformat_open_input(&pFormatCtx, char_cam_name, ifmt, NULL) != 0) {//打开指定设备 —— cameras.at()
        qDebug() << "Couldn't open input stream.\n";
    }
    else {
        qDebug() << "Success open input stream —— " << char_cam_name;
    }

    //循环查找数据包包含的流信息，直到找到视频类型的流
    //  便将其记录下来 保存到videoStream变量中
    videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoindex = i;
            break;
        }
    }
    if (videoindex == -1)
    {
        qDebug() << "Couldn't find a video stream.\n";
    }
    else {
        qDebug() << "Success find a video stream!\n";

    }



    //查找对应的解码器并打开
    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    //    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);//软编码
    //    AVCodec * codec = avcodec_find_encoder_by_name("nvenc_h264");//硬编码
    if (pCodec == NULL)
    {
        qDebug() << ("Codec not found.\n");
    }
    else {

        qDebug() << "Codec found Successfuly!\n";
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)//打开解码器
    {
        qDebug() << ("Could not open codec.\n");
    }
    else {

        qDebug() << "Success open codec!\n";
    }


    //开始准备读取视频
    pFrame = av_frame_alloc();//分配一个AVFrame并将其字段设置为默认值
    pFrameRGB = av_frame_alloc();
    SwsContext *img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
        pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
        AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);//分配和返回一个SwsContext你需要它来执行使用swsscale（）的缩放/转换操作
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height);
    qDebug() << numBytes;
    out_buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture*)pFrameRGB, out_buffer, AV_PIX_FMT_RGB32,
        pCodecCtx->width, pCodecCtx->height);//根据指定的图像参数和提供的图像数据缓冲区设置图像域
    int y_size = pCodecCtx->width * pCodecCtx->height;
    packet = (AVPacket*)malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据
    av_dump_format(pFormatCtx, 0, QApplication::applicationDirPath().toLatin1(), 0); //输出视频信息

    //解码压缩
    threadSta = true;
    while (threadSta)
    {
        if (av_read_frame(pFormatCtx, packet) < 0)
        {

            break; //这里认为视频读取完了
        }
        if (packet->stream_index == videoindex) {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);//解码一帧视频数据
            if (ret < 0) {
                qDebug() << ("decode error.");
            }
            if (got_picture) {
                sws_scale(img_convert_ctx,
                    (uint8_t const* const*)pFrame->data,
                    pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                    pFrameRGB->linesize);//在 pFrame->data 中缩放图像切片，并将得到的缩放切片放在pFrameRGB->data图像中

            //把这个RGB数据 用QImage加载
                QImage tmpImg((uchar*)out_buffer, pCodecCtx->width, pCodecCtx->height, QImage::Format_RGB32);
                QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
                emit sig_GetOneFrame(image);  //发送信号
                QThread::msleep(10);

            }
        }
        av_free_packet(packet);  //释放资源,否则内存会一直上升
        QThread::msleep(10);

    }

    av_free(out_buffer);

    av_free(pFrameRGB);

    avcodec_close(pCodecCtx);

    avformat_close_input(&pFormatCtx);
}

void Worker::FfmpegScreenStreaming()
{
	AVFormatContext* ifmtCtx = NULL;
	AVFormatContext* ofmtCtx = NULL;
	AVPacket pkt;
	AVFrame* pFrame, * pFrameYUV, *pFrameRGB;
	SwsContext* pImgConvertCtx, *pImgToRgbConvertCtx;
	AVDictionary* params = NULL;
	AVCodec* pCodec;
	AVCodecContext* pCodecCtx;
	unsigned char* outBuffer, *rgbOutBuffer;
	AVCodecContext* pH264CodecCtx;
	AVCodec* pH264Codec;

	int ret = 0;
	unsigned int i = 0;
	int videoIndex = -1;
	int frameIndex = 0;

	const char* inFilename = "desktop";//输入URL
	const char* outFilename = "rtmp://192.168.154.100:1935/live/123"; //输出URL
	const char* ofmtName = NULL;

	avdevice_register_all();
	avformat_network_init();

	AVInputFormat* ifmt = av_find_input_format("gdigrab");
	if (!ifmt)
	{
		printf("can't find input device\n");
		goto end;
	}

	// 1. 打开输入
	// 1.1 打开输入文件，获取封装格式相关信息
	if ((ret = avformat_open_input(&ifmtCtx, inFilename, ifmt, 0)) < 0)
	{
		printf("can't open input file: %s\n", inFilename);
		goto end;
	}

	// 1.2 解码一段数据，获取流相关信息
	if ((ret = avformat_find_stream_info(ifmtCtx, 0)) < 0)
	{
		printf("failed to retrieve input stream information\n");
		goto end;
	}

	// 1.3 获取输入ctx
	for (i = 0; i < ifmtCtx->nb_streams; ++i)
	{
		if (ifmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoIndex = i;
			break;
		}
	}

	qDebug() << ("%s:%d, videoIndex = %d\n", __FUNCTION__, __LINE__, videoIndex);

	av_dump_format(ifmtCtx, 0, inFilename, 0);

	// 1.4 查找输入解码器
	pCodec = avcodec_find_decoder(ifmtCtx->streams[videoIndex]->codecpar->codec_id);
	if (!pCodec)
	{
		qDebug() << ("can't find codec\n");
		goto end;
	}

	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!pCodecCtx)
	{
		qDebug() << ("can't alloc codec context\n");
		goto end;
	}

	avcodec_parameters_to_context(pCodecCtx, ifmtCtx->streams[videoIndex]->codecpar);

	//  1.5 打开输入解码器
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("can't open codec\n");
		goto end;
	}


	// 1.6 查找H264编码器
	pH264Codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!pH264Codec)
	{
		qDebug() << ("can't find h264 codec.\n");
		goto end;
	}

	// 1.6.1 设置参数
	pH264CodecCtx = avcodec_alloc_context3(pH264Codec);
	pH264CodecCtx->codec_id = AV_CODEC_ID_H264;
	pH264CodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pH264CodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pH264CodecCtx->width = pCodecCtx->width;
	pH264CodecCtx->height = pCodecCtx->height;
	pH264CodecCtx->time_base.num = 1;
	pH264CodecCtx->time_base.den = 25;	//帧率（即一秒钟多少张图片）
	pH264CodecCtx->bit_rate = 400000;	//比特率（调节这个大小可以改变编码后视频的质量）
	pH264CodecCtx->gop_size = 250;
	pH264CodecCtx->qmin = 10;
	pH264CodecCtx->qmax = 51;
	//some formats want stream headers to be separate
//	if (pH264CodecCtx->flags & AVFMT_GLOBALHEADER)
	{
		pH264CodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}


	// 1.7 打开H.264编码器
	av_dict_set(&params, "preset", "superfast", 0);
	av_dict_set(&params, "tune", "zerolatency", 0);	//实现实时编码
	if (avcodec_open2(pH264CodecCtx, pH264Codec, &params) < 0)
	{
		qDebug() << ("can't open video encoder.\n");
		goto end;
	}

	// 2. 打开输出
	// 2.1 分配输出ctx
	if (strstr(outFilename, "rtmp://"))
	{
		ofmtName = "flv";
	}
	else if (strstr(outFilename, "udp://"))
	{
		ofmtName = "mpegts";
	}
	else
	{
		ofmtName = NULL;
	}

	avformat_alloc_output_context2(&ofmtCtx, NULL, ofmtName, outFilename);
	if (!ofmtCtx)
	{
		qDebug() << ("can't create output context.");
		goto end;
	}

	// 2.2 创建输出流
	for (i = 0; i < ifmtCtx->nb_streams; ++i)
	{
		AVStream* outStream = avformat_new_stream(ofmtCtx, NULL);
		if (!outStream)
		{
			qDebug() << ("failed to allocate output stream\n");
			goto end;
		}

		avcodec_parameters_from_context(outStream->codecpar, pH264CodecCtx);
	}

	av_dump_format(ofmtCtx, 0, outFilename, 1);

	if (!(ofmtCtx->oformat->flags & AVFMT_NOFILE))
	{
		// 2.3 创建并初始化一个AVIOContext, 用以访问URL（outFilename）指定的资源
		ret = avio_open(&ofmtCtx->pb, outFilename, AVIO_FLAG_WRITE);
		if (ret < 0)
		{
			qDebug() << ("can't open output URL: %s\n", outFilename);
			goto end;
		}
	}
	qDebug() << ("Sucess open output URL: %s\n", outFilename);
	// 3. 数据处理
	// 3.1 写输出文件
	ret = avformat_write_header(ofmtCtx, NULL);
	if (ret < 0)
	{
		qDebug() << ("Error accourred when opening output file\n");
		goto end;
	}


	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	pFrameRGB = av_frame_alloc();

	outBuffer = (unsigned char*)av_malloc(
		av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width,
			pCodecCtx->height, 1));
	rgbOutBuffer = (unsigned char*)av_malloc(
		av_image_get_buffer_size(AV_PIX_FMT_RGB32, pCodecCtx->width,
			pCodecCtx->height, 1));

	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, outBuffer,
		AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
	av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, rgbOutBuffer,
		AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);

	pImgConvertCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
		pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
		AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	pImgToRgbConvertCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
		pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
		AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

	scrStmThdSta = true;
	while (scrStmThdSta)
	{
		// 3.2 从输入流读取一个packet
		ret = av_read_frame(ifmtCtx, &pkt);
		if (ret < 0)
		{
			break;
		}
		
		if (pkt.stream_index == videoIndex)
		{
			ret = avcodec_send_packet(pCodecCtx, &pkt);
			if (ret < 0)
			{
				qDebug() << ("Decode error.\n");
				goto end;
			}

			if (avcodec_receive_frame(pCodecCtx, pFrame) >= 0)
			{
				sws_scale(pImgConvertCtx,
					(const unsigned char* const*)pFrame->data,
					pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
					pFrameYUV->linesize);

				/*sws_scale(pImgToRgbConvertCtx,
					(const unsigned char* const*)pFrame->data,
					pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
					pFrameRGB->linesize);*/

				pFrameYUV->format = pCodecCtx->pix_fmt;
				pFrameYUV->width = pCodecCtx->width;
				pFrameYUV->height = pCodecCtx->height;

				pFrameRGB->format = pCodecCtx->pix_fmt;
				pFrameRGB->width = pCodecCtx->width;
				pFrameRGB->height = pCodecCtx->height;

				//QImage tmpImg((uchar*)rgbOutBuffer, pCodecCtx->width, pCodecCtx->height, QImage::Format_RGB32);
				//QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
				//emit SigUpdateFrame(image);  //发送信号

				ret = avcodec_send_frame(pH264CodecCtx, pFrameYUV);
				if (ret < 0)
				{
					qDebug() << ("failed to encode.\n");
					goto end;
				}

				if (avcodec_receive_packet(pH264CodecCtx, &pkt) >= 0)
				{
					// 设置输出DTS,PTS
					pkt.pts = pkt.dts = frameIndex * (ofmtCtx->streams[0]->time_base.den) / ofmtCtx->streams[0]->time_base.num / 25;
					frameIndex++;

					ret = av_interleaved_write_frame(ofmtCtx, &pkt);
					if (ret < 0)
					{
						printf("send packet failed: %d\n", ret);
					}
					else
					{
						printf("send %5d packet successfully!\n", frameIndex);
					}
				}
			}
		}

		av_packet_unref(&pkt);
	}

	av_write_trailer(ofmtCtx);

end:
	avformat_close_input(&ifmtCtx);

	/* close output */
	if (ofmtCtx && !(ofmtCtx->oformat->flags & AVFMT_NOFILE)) {
		avio_closep(&ofmtCtx->pb);
	}
	
	av_frame_free(&pFrameRGB);
	av_frame_free(&pFrameYUV);
	av_free(outBuffer);
	av_free(rgbOutBuffer);
	sws_freeContext(pImgConvertCtx);
	sws_freeContext(pImgToRgbConvertCtx);

	avformat_free_context(ofmtCtx);

	if (ret < 0 && ret != AVERROR_EOF) {
		printf("Error occurred\n");
		return ;
	}

	return;
}
