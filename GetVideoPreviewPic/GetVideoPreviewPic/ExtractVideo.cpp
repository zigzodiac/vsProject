#include"ExtractVideo.h"
#include <stdio.h>
#include <opencv2/core/mat.hpp>

#include "ImageComparison.h"
#include <iostream>

static AVFormatContext* ifmt_ctx;
typedef struct StreamContext {
    AVCodecContext* dec_ctx;
    AVCodecContext* enc_ctx;
    AVFrame* dec_frame;
} StreamContext;

static StreamContext* stream_ctx;
static char* outputDir = NULL;
static int diffDegree = 10;
int SetDiffDegree(int diff) {
    if (diff > 64 || diff < 0) {
        av_log(NULL, AV_LOG_ERROR, "diffDegree Out of range, max is 64 min is 0\n");
        return -81;
    }
    diffDegree = diff;
}

int  SetDir(char** dir) {
    outputDir = *dir;
    if (!outputDir) {
        av_log(NULL, AV_LOG_ERROR, "output dir is null\n");
        return -99;
    }
    return 0;
}

void SaveFrame(AVFrame* pFrame, int width, int height, int index)
{
    FILE* pFile;
    char szFilename[32];
    int y;
    sprintf_s(szFilename, "D:/TestVideo/picture/frame%d.jpg", index);
    errno_t err;
    err = fopen_s(&pFile,szFilename, "wb");

    if (pFile == NULL)
    {
        return;
    }
    fprintf(pFile, "P6%d %d 255", width, height);

    for (y = 0; y < height; y++)
    {
        fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);
    }
    fclose(pFile);
}

int savePicture(AVFrame* pFrame, char* out_name) {//编码保存图片
    int ret;
    int width = pFrame->width;
    int height = pFrame->height;
    AVCodecContext* pCodeCtx = NULL;


    AVFormatContext* pFormatCtx = avformat_alloc_context();
    // 设置输出文件格式
    pFormatCtx->oformat = av_guess_format("mjpeg", NULL, NULL);

    // 创建并初始化输出AVIOContext
    if (ret = avio_open(&pFormatCtx->pb, out_name, AVIO_FLAG_READ_WRITE) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Couldn't open output file\n");
        return ret;
    }

    // 构建一个新stream
    AVStream* pAVStream = avformat_new_stream(pFormatCtx, 0);
    if (pAVStream == NULL) {
        av_log(NULL, AV_LOG_ERROR, "Failed to create stream\n");
        return -80;
    }

    AVCodecParameters* parameters = pAVStream->codecpar;
    parameters->codec_id = pFormatCtx->oformat->video_codec;
    parameters->codec_type = AVMEDIA_TYPE_VIDEO;
    parameters->format = AV_PIX_FMT_YUVJ420P;
    parameters->width = pFrame->width;
    parameters->height = pFrame->height;

    AVCodec* pCodec = avcodec_find_encoder(pAVStream->codecpar->codec_id);

    if (!pCodec) {
        av_log(NULL, AV_LOG_ERROR, "Could not find encoder\n");
        return -1;
    }

    pCodeCtx = avcodec_alloc_context3(pCodec);
    if (!pCodeCtx) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    if ((ret = avcodec_parameters_to_context(pCodeCtx, pAVStream->codecpar)) < 0) {
        fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
            av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
        return ret;
    }

    pCodeCtx->time_base.num = 1;
    pCodeCtx->time_base.den = 25;

    if (ret = avcodec_open2(pCodeCtx, pCodec, NULL) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not open codec\n");
        return ret;
    }

    ret = avformat_write_header(pFormatCtx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "write_header fail\n");
        return ret;
    }

    int y_size = width * height;

    //Encode
    // 给AVPacket分配足够大的空间
    AVPacket pkt;
    av_new_packet(&pkt, y_size * 3);

    // 编码数据
    ret = avcodec_send_frame(pCodeCtx, pFrame);

    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not avcodec_send_frame\n");
        return ret;
    }
    // 得到编码后数据
    ret = avcodec_receive_packet(pCodeCtx, &pkt);

    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not avcodec_receive_packet\n");
        return ret;
    }

    ret = av_write_frame(pFormatCtx, &pkt);

    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not av_write_frame\n");
        return ret;
    }

    av_packet_unref(&pkt);

    //Write Trailer
    av_write_trailer(pFormatCtx);


    avcodec_close(pCodeCtx);
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);

    return 0;
}

int OpenVideo(const char* filename)
{

    int ret;
    unsigned int i;

    ifmt_ctx = NULL;
    if ((ret = avformat_open_input(&ifmt_ctx, filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    stream_ctx = (StreamContext*)av_mallocz_array(ifmt_ctx->nb_streams, sizeof(*stream_ctx));
    if (!stream_ctx)
        return AVERROR(ENOMEM);

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream* stream = ifmt_ctx->streams[i];
        AVCodec* dec = avcodec_find_decoder(stream->codecpar->codec_id);
        AVCodecContext* codec_ctx;
        if (!dec) {
            av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
            return AVERROR_DECODER_NOT_FOUND;
        }
        codec_ctx = avcodec_alloc_context3(dec);
        if (!codec_ctx) {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
            return AVERROR(ENOMEM);
        }
        ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
                "for stream #%u\n", i);
            return ret;
        }
        /* Reencode video & audio and remux subtitles etc. */
        if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
            || codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
                codec_ctx->framerate = av_guess_frame_rate(ifmt_ctx, stream, NULL);
            /* Open decoder */
            ret = avcodec_open2(codec_ctx, dec, NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
                return ret;
            }
        }
        stream_ctx[i].dec_ctx = codec_ctx;
        stream_ctx[i].dec_frame = av_frame_alloc();
        if (!stream_ctx[i].dec_frame)
            return AVERROR(ENOMEM);
    }

    av_dump_format(ifmt_ctx, 0, filename, 0);
    return 0;
}

int GetVideoPic()
{
    int ret;
    AVPacket packet;
    packet.data = NULL;
    packet.size = 0;

    enum AVMediaType type;
    unsigned int stream_index;
    unsigned int i;
    int got_frame;
    AVRational secondTimeBase;
    secondTimeBase.num = 1;
    secondTimeBase.den = 1000;

    static struct SwsContext* img_convert_ctx;
    img_convert_ctx = sws_getContext(stream_ctx[0].dec_ctx->width, stream_ctx[0].dec_ctx->height,
        stream_ctx[0].dec_ctx->pix_fmt, stream_ctx[0].dec_ctx->width, stream_ctx[0].dec_ctx->height,
        AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL); //1.图像色彩空间转换；2.分辨率缩放；3.前后图像滤波处理

    AVFrame* pFrameRGB = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, stream_ctx[0].dec_ctx->width, stream_ctx[0].dec_ctx->height, 1);

    uint8_t* out_buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));//内存分配
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize,out_buffer, AV_PIX_FMT_BGR24,
        stream_ctx[0].dec_ctx->width, stream_ctx[0].dec_ctx->height, 1); //为已经分配的空间的结构体AVPicture挂上一段用于保存数据的空间，这个结构体中有一个指针数组data[4]，挂在这个数组里
    pFrameRGB->height = stream_ctx[0].dec_ctx->height;
    pFrameRGB->width = stream_ctx[0].dec_ctx->width;
    pFrameRGB->format = AV_PIX_FMT_BGR24;
    int index = 0;
    int keyFrameIndex = 0;
    char buf[1024];

    InitMat(stream_ctx[0].dec_ctx->width, stream_ctx[0].dec_ctx->height, AV_PIX_FMT_BGR24);
    int picIsSet = 0;
    int curKeyFrame = 0;
    int64_t frameTime = 0;
    float pictDarknessDegree = 0.0;
    while (1) {
        if ((ret = av_read_frame(ifmt_ctx, &packet)) < 0)
            break;
        if (packet.flags != AV_PKT_FLAG_KEY || packet.stream_index != 0) {
            continue;
        }
        stream_index = packet.stream_index;
        type = ifmt_ctx->streams[packet.stream_index]->codecpar->codec_type;

        if (type != AVMEDIA_TYPE_VIDEO)
            continue;

        av_log(NULL, AV_LOG_DEBUG, "Demuxer gave frame of stream_index %u\n",
            stream_index);

        av_packet_rescale_ts(&packet,
            ifmt_ctx->streams[stream_index]->time_base,
            stream_ctx[stream_index].dec_ctx->time_base);

        ret = avcodec_send_packet(stream_ctx[stream_index].dec_ctx, &packet);

        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
            break;
        }

        while (ret >= 0) {
            ret = avcodec_receive_frame(stream_ctx[stream_index].dec_ctx, stream_ctx[stream_index].dec_frame);
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                break;
            else if (ret < 0)
                goto end;
            sws_scale(img_convert_ctx, (uint8_t const* const*)stream_ctx[stream_index].dec_frame->data,
                stream_ctx[stream_index].dec_frame->linesize, 0, stream_ctx[0].dec_ctx->height, pFrameRGB->data,
                pFrameRGB->linesize);
            if (!stream_ctx[stream_index].dec_frame->key_frame)
                continue;
            printf("The currently processing key frame index is %d\r", keyFrameIndex++);
            if (!picIsSet) {
                SetMatData(out_buffer, stream_ctx[0].dec_ctx->height, stream_ctx[0].dec_ctx->width, AV_PIX_FMT_BGR24, numBytes, 1);
                ret = CheckBlankScreen(1, pictDarknessDegree);
                if(ret < 0)
                    av_log(NULL, AV_LOG_ERROR, "CheckBlankScreen failed, Error code: %d\n", ret);
                if (pictDarknessDegree > 0.85)
                    continue;
                frameTime = av_rescale_q(stream_ctx[stream_index].dec_frame->pts, stream_ctx[stream_index].dec_ctx->time_base, secondTimeBase);
                snprintf(buf, sizeof(buf), "%s/%I64d.jpg", outputDir, frameTime);
                savePicture(stream_ctx[stream_index].dec_frame, buf); //保存为jpg图片
                index++;
                picIsSet = 1;
            }
            else {
                SetMatData(out_buffer, stream_ctx[0].dec_ctx->height, stream_ctx[0].dec_ctx->width, AV_PIX_FMT_BGR24, numBytes, 2);
                //double result = PicCompare();
                //std::cout << "current frame key frame idex is " << curKeyFrame++ << std::endl;
                int result = pHashValueCompare();
                if (result >= diffDegree) {
                    SetMatData(out_buffer, stream_ctx[0].dec_ctx->height, stream_ctx[0].dec_ctx->width, AV_PIX_FMT_BGR24, numBytes, 1);
                    ret = CheckBlankScreen(2, pictDarknessDegree);
                    if (ret < 0)
                        av_log(NULL, AV_LOG_ERROR, "CheckBlankScreen failed, Error code: %d\n", ret);
                    if (pictDarknessDegree > 0.85)
                       continue;
                    frameTime = av_rescale_q(stream_ctx[stream_index].dec_frame->pts, stream_ctx[stream_index].dec_ctx->time_base, secondTimeBase);
                    snprintf(buf, sizeof(buf), "%s/%I64d.jpg", outputDir, frameTime);
                    savePicture(stream_ctx[stream_index].dec_frame, buf); //保存为jpg图片
                    index++;
                }
            }
        }
        av_packet_unref(&packet);
    }
    std::cout << std::endl;
    std::cout << "Number of previewed pictures is  " << index << std::endl;
    

end:
    av_packet_unref(&packet);
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        avcodec_free_context(&stream_ctx[i].dec_ctx);
        av_frame_free(&stream_ctx[i].dec_frame);
    }

    av_free(stream_ctx);
    avformat_close_input(&ifmt_ctx);
    if (ret < 0)
        av_log(NULL, AV_LOG_ERROR, "Error occurred: %s\n", av_err2str(ret));
    return 0;
}


int GetVideoDuration()
{
    AVRational secondTimeBase = { 1, 1 };
    int ret = av_rescale_q(ifmt_ctx->streams[0]->duration, ifmt_ctx->streams[0]->time_base, secondTimeBase);
    return ret;
}
