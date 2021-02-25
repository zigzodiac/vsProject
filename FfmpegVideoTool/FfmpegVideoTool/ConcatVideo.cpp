#include "ConcatVideo.h"

char* INPUT_FILE =  "D:\\TestVideo\\test1.mp4";
char* OUTPUT_FILE =  "D:\\TestVideo\\test1_playlist.m3u8";


#define TRUE 1
#define FALSE 0

AVRational time_base = { 1, AV_TIME_BASE };
typedef struct StreamContext {
    AVCodecContext* decCtx;
    AVCodecContext* encCtx;
    AVFrame* frame;
}StreamContext;

typedef struct VideoCodecDecoerParam {
    AVFormatContext* fmt_ctx;
    StreamContext* stream_ctx;
}VdCodecParam;


static void my_logoutput(void* ptr, int level, const char* fmt, va_list vl) {
    FILE* fp = fopen("my_log.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
}




std::list<VdCodecParam*>   vdCodecParamList;
static ThreadStatus thdSta;

int SetThreadStatus(ThreadStatus state) {
    thdSta = state;
    return 0;
}

int open_input_file(const char* filename, VdCodecParam* vdDecParam)
{
    int ret;
    unsigned int i;

    if ((ret = avformat_open_input(&vdDecParam->fmt_ctx, filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(vdDecParam->fmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }
    vdDecParam->stream_ctx = (StreamContext*)av_mallocz_array(vdDecParam->fmt_ctx->nb_streams, sizeof(StreamContext));
    StreamContext* stream_ctx = vdDecParam->stream_ctx;
    if (!stream_ctx)
        return AVERROR(ENOMEM);


    for (i = 0; i < vdDecParam->fmt_ctx->nb_streams; i++) {
        AVStream* stream = vdDecParam->fmt_ctx->streams[i];
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
                codec_ctx->framerate = av_guess_frame_rate(vdDecParam->fmt_ctx, stream, NULL);

           
            /* Open decoder */
            ret = avcodec_open2(codec_ctx, dec, NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
                return ret;
            }
        }

        codec_ctx->thread_count = 2;
        stream_ctx[i].decCtx = codec_ctx;
        /*stream_ctx[i].frame = av_frame_alloc();
        if (!stream_ctx[i].frame)
            return AVERROR(ENOMEM);*/
    }

    av_dump_format(vdDecParam->fmt_ctx, 0, filename, 0);
    return 0;
}


int open_output_file(const char* filename, VdCodecParam* vdDecParam, VdCodecParam *vdEncParam, VideoParam &conParam)
{
    AVStream* out_stream;
    AVStream* in_stream;
    AVCodecContext *dec_ctx, *enc_ctx;
    AVCodec* encoder;
    int ret;
    unsigned int i;

    avformat_alloc_output_context2(&vdEncParam->fmt_ctx, NULL, NULL, filename);
    if (!vdEncParam->fmt_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
        return AVERROR_UNKNOWN;
    }

    AVFormatContext* ifmt_ctx = vdDecParam->fmt_ctx;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        out_stream = avformat_new_stream(vdEncParam->fmt_ctx, NULL);
        if (!out_stream) {
            av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
            return AVERROR_UNKNOWN;
        }

        in_stream = ifmt_ctx->streams[i];
        dec_ctx = vdDecParam->stream_ctx[i].decCtx;

        if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
            || dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            /* in this example, we choose transcoding to same codec */
            encoder = avcodec_find_encoder(dec_ctx->codec_id);
            if (!encoder) {
                av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
                return AVERROR_INVALIDDATA;
            }
            enc_ctx = avcodec_alloc_context3(encoder);
            if (!enc_ctx) {
                av_log(NULL, AV_LOG_FATAL, "Failed to allocate the encoder context\n");
                return AVERROR(ENOMEM);
            }

            /* In this example, we transcode to same properties (picture size,
             * sample rate etc.). These properties can be changed for output
             * streams easily using filters */
            if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
                /*enc_ctx->height = dec_ctx->height;
                enc_ctx->width = dec_ctx->width;*/

                /*enc_ctx->height = conParam.height;
                enc_ctx->width = conParam.width;*/
                enc_ctx->height = dec_ctx->height;
                enc_ctx->width = dec_ctx->width;

                enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
                /* take first format from list of supported formats */
                if (encoder->pix_fmts)
                    enc_ctx->pix_fmt = encoder->pix_fmts[0];
                else
                    enc_ctx->pix_fmt = dec_ctx->pix_fmt;
                /* video time_base can be set to whatever is handy and supported by encoder */
                /*enc_ctx->time_base = av_inv_q(dec_ctx->framerate);*/
                enc_ctx->time_base = dec_ctx->time_base;
                enc_ctx->thread_count = 1;
            }
            else {
                enc_ctx->sample_rate = dec_ctx->sample_rate;
                enc_ctx->channel_layout = dec_ctx->channel_layout;
                enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
                /* take first format from list of supported formats */
                enc_ctx->sample_fmt = encoder->sample_fmts[0];
                /*enc_ctx->time_base = (AVRational){ 1, enc_ctx->sample_rate };*/
                enc_ctx->time_base.num = 1;
                enc_ctx->time_base.den = enc_ctx->sample_rate;
            }


            if (vdEncParam->fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            /* Third parameter can be used to pass settings to encoder */
            ret = avcodec_open2(enc_ctx, encoder, NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
                return ret;
            }
            ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to copy encoder parameters to output stream #%u\n", i);
                return ret;
            }

            /*out_stream->avg_frame_rate = in_stream->avg_frame_rate;
            out_stream->r_frame_rate = in_stream->r_frame_rate;
            out_stream->duration = in_stream->duration;*/
            //encoder->capabilities = (1 << 5);
            out_stream->time_base = in_stream->time_base;
            vdEncParam->stream_ctx[i].encCtx = enc_ctx;
        }
        else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
            return AVERROR_INVALIDDATA;
        }
        else {
            /* if this stream must be remuxed */
            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Copying parameters for stream #%u failed\n", i);
                return ret;
            }
            out_stream->time_base = in_stream->time_base;
        }

    }
    av_dump_format(vdEncParam->fmt_ctx, 0, filename, 1);

    if (!(vdEncParam->fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&vdEncParam->fmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", filename);
            return ret;
        }
    }

    av_opt_set(vdEncParam->fmt_ctx->priv_data, "hls_time", "10", 0);
    av_opt_set(vdEncParam->fmt_ctx->priv_data, "hls_list_size", "0", 0);

    /* init muxer, write output file header */
    ret = avformat_write_header(vdEncParam->fmt_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
        return ret;
    }

    return 0;
}



int VideoProcess(QStringList dirlist, VideoParam& videoParam) {

    
    AVPacket pkt;
    AVFormatContext* input_fmtctx = NULL;
    AVFormatContext* output_fmtctx = NULL;
    AVCodecContext* enc_ctx = NULL;
    AVCodecContext* dec_ctx = NULL;
    AVCodec* encoder = NULL;
    AVBitStreamFilterContext* vbsf = NULL;
    int ret = 0;
    int i = 0;
    int have_key = 0;
    static int first_pkt = 1;
    av_log_set_level(48);
    av_register_all();

    if (avformat_open_input(&input_fmtctx, INPUT_FILE, NULL, NULL) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open the file %s\n", "./a.mp4");
        return -ENOENT;
    }

    if (avformat_find_stream_info(input_fmtctx, 0) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to retrieve input stream information\n");
        return -EINVAL;
    }

    av_dump_format(input_fmtctx, NULL, INPUT_FILE, 0);

    if (avformat_alloc_output_context2(&output_fmtctx, NULL, FORMAT, OUTPUT_FILE) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open the file %s \n", OUTPUT_FILE);
        return -ENOENT;
    }

    /* Process transcode parameters */
    for (i = 0; i < input_fmtctx->nb_streams; i++) {
        AVStream* out_stream = NULL;
        AVStream* in_stream = NULL;

        in_stream = input_fmtctx->streams[i];
        out_stream = avformat_new_stream(output_fmtctx, in_stream->codec->codec);
        if (out_stream < 0) {
            av_log(NULL, AV_LOG_ERROR, "Alloc new Stream error\n");
            return -EINVAL;
        }

        avcodec_copy_context(output_fmtctx->streams[i]->codec, input_fmtctx->streams[i]->codec);

        out_stream->codec->codec_tag = 0;
        if (output_fmtctx->oformat->flags & AVFMT_GLOBALHEADER) {
            out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    //vbsf = av_bitstream_filter_init("h264_mp4toannexb");


    av_dump_format(output_fmtctx, NULL, OUTPUT_FILE, 1);

    if (avio_open2(&output_fmtctx->pb, OUTPUT_FILE, AVIO_FLAG_WRITE, &output_fmtctx->interrupt_callback, NULL) < 0) {
        av_log(NULL, AV_LOG_ERROR, "cannot open the output file '%s' \n", "./fuck.m3u8");
        return -ENOENT;
    }

    av_opt_set(output_fmtctx->priv_data, "hls_time", "10", 0);

    if ((ret = avformat_write_header(output_fmtctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, " Cannot write the header for the file '%s' ret = %d\n", "fuck.ts", ret);
        return -ENOENT;
    }

    const AVBitStreamFilter* bsfilter = av_bsf_get_by_name("h264_mp4toannexb");
    AVBSFContext* bsf_ctx = NULL;
    // 2 初始化过滤器上下文
    av_bsf_alloc(bsfilter, &bsf_ctx); //AVBSFContext;
    // 3 添加解码器属性
    avcodec_parameters_copy(bsf_ctx->par_in, input_fmtctx->streams[0]->codecpar);
    av_bsf_init(bsf_ctx);

    while (1) {
        AVStream* in_stream = NULL;
        AVStream* out_stream = NULL;
        av_usleep(1000);

        ret = av_read_frame(input_fmtctx, &pkt);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "read frame error %d \n", ret);
            break;
        }

        if (pkt.pts == AV_NOPTS_VALUE && first_pkt) {
            pkt.pts = pkt.dts;
            first_pkt = 0;
        }

        in_stream = input_fmtctx->streams[pkt.stream_index];
        out_stream = output_fmtctx->streams[pkt.stream_index];

        //send packet to h264_mp4toannexb
        if (av_bsf_send_packet(bsf_ctx, &pkt) != 0) {
            av_packet_unref(&pkt);
            continue;
        }
        while (av_bsf_receive_packet(bsf_ctx, &pkt) == 0)
        {
            //out_pkt_count++;
            // printf("fwrite size:%d\n", pkt->size);
            
        }

        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;

        if (pkt.stream_index == 0) {

            AVPacket fpkt = pkt;
            int a = av_bitstream_filter_filter(vbsf,
                out_stream->codec, NULL, &fpkt.data, &fpkt.size,
                pkt.data, pkt.size, pkt.flags & AV_PKT_FLAG_KEY);
            pkt.data = fpkt.data;
            pkt.size = fpkt.size;

        }
        av_log(NULL, AV_LOG_ERROR, "write pkt dts %lld pts %lld duration %lld  strean_index %d\n", pkt.dts, pkt.pts, pkt.duration, pkt.stream_index);
        ret = av_write_frame(output_fmtctx, &pkt);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Muxing Error\n");
            break;
        }
        av_free_packet(&pkt);
    }

    av_write_trailer(output_fmtctx);
    avformat_close_input(&input_fmtctx);
    avio_close(output_fmtctx->pb);
    avformat_free_context(output_fmtctx);
    av_bitstream_filter_close(vbsf);
    vbsf = NULL;

    return 0;
}

int ConcatVideo2(QStringList fnList, QString outFn) {

    av_register_all();
    AVFormatContext* pInCtx;
    AVFormatContext* pOutCtx;

    AVInputFormat* pInFmt = av_find_input_format("concat");
    avformat_open_input(&pInCtx, "file_concat.txt", pInFmt, NULL);

    if (!pInCtx) {
        return -1;
    }
    
    avformat_find_stream_info(pInCtx, NULL);
    av_dump_format(pInCtx, 0, NULL, 0);

}
int init_filter(AVBSFContext** bsf_ctx, AVFormatContext* fmt_ctx, int stream_Index) {

    int ret = 0;
    const AVBitStreamFilter* bsfilter = av_bsf_get_by_name("h264_mp4toannexb");
    // 2 初始化过滤器上下文
    ret = av_bsf_alloc(bsfilter, bsf_ctx); //AVBSFContext;

    ret = avcodec_parameters_copy((*bsf_ctx)->par_in, fmt_ctx->streams[stream_Index]->codecpar);
    ret = av_bsf_init(*bsf_ctx);
    return 0;
}

int filter_packet(AVBSFContext* bsf_ctx, int stream_index, AVPacket* pkt) {
    int ret = 0;
    ret = av_bsf_send_packet(bsf_ctx, pkt);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "h264_mp4toannexb filter "
            "failed to send input packet\n");
        return ret;
    }

    while (!ret)
        ret = av_bsf_receive_packet(bsf_ctx, pkt);

    if (ret < 0 && (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)) {
        av_log(NULL, AV_LOG_ERROR, "h264_mp4toannexb filter "
            "failed to receive output packet\n");
        return ret;
    }
    return 0;
}
int ConcatVideo(VideoParam  &conVideoParam) {
    //av_log_set_level(48);
    //av_log_set_callback(my_logoutput);
    QStringList fnList = conVideoParam.inFnlist;
    QString outFn = conVideoParam.outFn;
    VdCodecParam *curVideoParam, *nextVideoParam;
    int ret = 0;

    if (fnList.size() <= 1) {
        return -1;
    }

    av_register_all();
    for (QString fn : fnList) {
        VdCodecParam* vdDecparam = (VdCodecParam*)av_mallocz(sizeof(vdDecparam));
        ret = open_input_file(fn.toStdString().c_str(), vdDecparam);
        if (ret < 0)
            return -1;
        vdCodecParamList.push_back(vdDecparam);
    }

    auto it = vdCodecParamList.begin();
    auto next = it;
    auto end = vdCodecParamList.end();
    for (; it != end; it++) {
        next++;
        if (next == end)
            break;

        curVideoParam = *it;
        nextVideoParam = *next;
        if (curVideoParam->stream_ctx[0].decCtx->codec_id != nextVideoParam->stream_ctx[0].decCtx->codec_id &&
            curVideoParam->stream_ctx[0].decCtx->height != nextVideoParam->stream_ctx[0].decCtx->height &&
            curVideoParam->stream_ctx[0].decCtx->width != nextVideoParam->stream_ctx[0].decCtx->width &&
            curVideoParam->stream_ctx[0].decCtx->pix_fmt != nextVideoParam->stream_ctx[0].decCtx->pix_fmt) {
            printf("error video format or codec is diffrent");
            return -1;
        }

        if (curVideoParam->stream_ctx[1].decCtx->codec_id != nextVideoParam->stream_ctx[1].decCtx->codec_id &&
            curVideoParam->stream_ctx[1].decCtx->sample_fmt != nextVideoParam->stream_ctx[1].decCtx->sample_fmt &&
            curVideoParam->stream_ctx[1].decCtx->channels != nextVideoParam->stream_ctx[1].decCtx->channels &&
            curVideoParam->stream_ctx[1].decCtx->channel_layout != nextVideoParam->stream_ctx[1].decCtx->channel_layout) {
            printf("error audio format or codec is diffrent");
            return -1;
        }
    }
    VdCodecParam* vdDecParam = *vdCodecParamList.begin();
    VdCodecParam* vdEncParam = (VdCodecParam*)av_mallocz(sizeof(VdCodecParam));
    vdEncParam->stream_ctx = (StreamContext*)av_mallocz(2*sizeof(StreamContext));
    

    ret = open_output_file(outFn.toStdString().c_str(), vdDecParam, vdEncParam, conVideoParam);
    if (ret < 0)
        return -1;

    AVPacket packet;
    packet.data = NULL;
    packet.size = 0;
    VdCodecParam *param;
    AVStream* st;
    int stream_index;
    int64_t curAudioPts = 0, curAudioDts = 0;
    int64_t curVideoPts = 0, curVideoDts = 0, nextPkgPts;
    int64_t audioDura = 0, videoDura = 0, otherDura = 0;
    int64_t delta = 0;
    int64_t delta_audio = 0;

    AVBSFContext* bsf_ctx = NULL;
    init_filter(&bsf_ctx, vdCodecParamList.begin()._Ptr->_Myval->fmt_ctx, 0);
    thdSta = RUNNING;
    int i = 0;
    for (auto inCtx : vdCodecParamList) {
        i > 0 ? delta += videoDura : delta = 0;
        i > 0 ? delta_audio += audioDura : delta_audio = 0;
        while (thdSta == RUNNING)
        {
            if ((ret = av_read_frame(inCtx->fmt_ctx, &packet)) < 0) {
                if (ret == AVERROR_EOF) {
                    break;
                }
                else if (ret < 0) {
                    break;
                }
            }
            if (packet.stream_index == 0) {
                filter_packet(bsf_ctx, 0, &packet);
            }

            st = inCtx->fmt_ctx->streams[packet.stream_index];
            av_log(NULL, AV_LOG_DEBUG, "file:%d stream:%d pts:%s pts_time:%s dts:%s dts_time:%s",
                i, packet.stream_index,
                av_ts2str(packet.pts), av_ts2timestr(packet.pts, &st->time_base),
                av_ts2str(packet.dts), av_ts2timestr(packet.dts, &st->time_base));

            //delta = av_rescale_q(videoDura, time_base, inCtx->fmt_ctx->streams[packet.stream_index]->time_base);
            //delta_audio = av_rescale_q(audioDura, time_base, inCtx->fmt_ctx->streams[packet.stream_index]->time_base);

         

            if (packet.stream_index == 0) {
                if (packet.pts != AV_NOPTS_VALUE)
                    packet.dts += delta;
                if (packet.dts != AV_NOPTS_VALUE)
                    packet.pts += delta;
            }
            else if(packet.stream_index == 1) {
                if (packet.pts != AV_NOPTS_VALUE)
                    packet.dts += delta_audio;
                if (packet.dts != AV_NOPTS_VALUE)
                    packet.pts += delta_audio;
            }
            
            
            av_log(NULL, AV_LOG_DEBUG, " -> pts:%s pts_time:%s dts:%s dts_time:%s\n",
                av_ts2str(packet.pts), av_ts2timestr(packet.pts, &st->time_base),
                av_ts2str(packet.dts), av_ts2timestr(packet.dts, &st->time_base));
            stream_index = packet.stream_index;
            StreamContext *stream_ctx = &inCtx->stream_ctx[stream_index];
            //av_packet_rescale_ts(&packet, inCtx->fmt_ctx->streams[stream_index]->time_base, stream_ctx->decCtx->time_base);
           
            ret = av_interleaved_write_frame(vdEncParam->fmt_ctx, &packet);
            if (ret < 0)
                goto end;
            av_packet_unref(&packet);
        }

        videoDura += inCtx->fmt_ctx->streams[0]->duration;
        audioDura += av_rescale_q(videoDura, inCtx->fmt_ctx->streams[0]->time_base, inCtx->fmt_ctx->streams[1]->time_base);
        //audioDura += inCtx->fmt_ctx->streams[1]->duration;

        curVideoDts = curVideoPts += videoDura;
        curAudioDts = curAudioPts += audioDura;
        i++;
    }
    av_write_trailer(vdEncParam->fmt_ctx);
end:
    //av_free_packet(&packet);

    for (auto vdCtx : vdCodecParamList) {
        avcodec_free_context(&vdCtx->stream_ctx->decCtx);
        avformat_close_input(&vdCtx->fmt_ctx);
    }
    if (vdEncParam->fmt_ctx && !(vdEncParam->fmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&vdEncParam->fmt_ctx->pb);
    avformat_free_context(vdEncParam->fmt_ctx);

    return 0;
}