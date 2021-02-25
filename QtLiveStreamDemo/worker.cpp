#include "worker.h"



void show_dshow_device() {
    AVFormatContext* pFormatCtx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options, "list_devices", "true", 0);
    AVInputFormat* iformat = av_find_input_format("dshow");
    printf("Device Info=============\n");
    avformat_open_input(&pFormatCtx, "video=dummy", iformat, &options);
    printf("========================\n");
}

void Worker::getImage()
{
    AVFormatContext* pFormatCtx;//
    AVInputFormat* ifmt;//
    int i, videoindex;
    int numBytes;
    int ret, got_picture;
    AVCodecContext* pCodecCtx;
    AVCodec* pCodec;
    AVFrame* pFrame, * pFrameRGB;
    AVPacket* packet;
    uint8_t* out_buffer;
    qDebug() << "current thread ID --- ffmpeg_test:" << QThread::currentThreadId();


    qDebug() << "Hello FFmpeg!";
    unsigned version = avcodec_version();//获取FFmpeg版本号 unsigned int 类型
    qDebug() << "version is:" << version;

    //2_初始化
    av_register_all(); //初始化FFMPEG  调用了这个才能正常适用编码器和解码器
    avformat_network_init();//初始化FFmpeg网络模块
    avdevice_register_all();//初始化libavdevice并注册所有输入和输出设备
    pFormatCtx = avformat_alloc_context();//分配一个AVFormatContext,查找用于输入的设备

    //3_选择并打开输入设备
    ifmt = av_find_input_format("dshow");
    //Set own video device's name
    if (avformat_open_input(&pFormatCtx, "video=Lenovo EasyCamera", ifmt, NULL) != 0) {
        printf("Couldn't open input stream.\n");
        goto error ;
    }

    //4_读取流信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("Couldn't find stream info stream.\n");
        goto error;
    }

    videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
        }
        break;
    }

    if (videoindex == -1) {
        printf("Couldn't video stream.\n");
        goto error;
    }
    //5_找到并打开解码器
    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

    if (pCodec == NULL)
    {
        printf("Codec not found.\n");
        goto error;
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        printf("Could not open codec.\n");
        goto error;
    }

    //6_开始准备读取视频
    pFrame = av_frame_alloc();//分配一个AVFrame并将其字段设置为默认值
    pFrameRGB = av_frame_alloc();
    SwsContext  *img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
        pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
        AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);  //分配和返回一个SwsContext你需要它来执行使用swsscale（）的缩放/转换操作
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height);
    qDebug() << numBytes;
    out_buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture*)pFrameRGB, out_buffer, AV_PIX_FMT_RGB32,
        pCodecCtx->width, pCodecCtx->height);//根据指定的图像参数和提供的图像数据缓冲区设置图像域
    int y_size = pCodecCtx->width * pCodecCtx->height;
    packet = (AVPacket*)malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据
    //av_dump_format(pFormatCtx, 0, QApplication::applicationDirPath().toLatin1(), 0); //输出视频信息



    //7_解码压缩
  
    while (1)
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


error:
    printf("error .\n");

    return;
}
