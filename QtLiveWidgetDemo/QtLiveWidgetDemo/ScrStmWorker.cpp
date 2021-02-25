#include "ScrStmWorker.h"

ScrStmWorker::ScrStmWorker()
{
	scrStmThdSta = false;
}

ScrStmWorker::~ScrStmWorker()
{
}

void ScrStmWorker::FfmpegScreenStreaming()
{
	AVFormatContext* ifmtCtx = NULL;
	AVFormatContext* ofmtCtx = NULL;
	AVPacket pkt;
	AVFrame* pFrame, * pFrameYUV, * pFrameRGB;
	SwsContext* pImgConvertCtx, * pImgToRgbConvertCtx;
	AVDictionary* params = NULL;
	AVCodec* pCodec;
	AVCodecContext* pCodecCtx;
	unsigned char* outBuffer, * rgbOutBuffer;
	AVCodecContext* pH264CodecCtx;
	AVCodec* pH264Codec;

	int ret = 0;
	unsigned int i = 0;
	int videoIndex = -1;
	int frameIndex = 0;

	const char* inFilename = "desktop";//����URL
	const char* outFilename = "rtmp://192.168.31.29/live/123"; //���URL
	const char* ofmtName = NULL;

	avdevice_register_all();
	avformat_network_init();

	AVInputFormat* ifmt = av_find_input_format("gdigrab");
	if (!ifmt)
	{
		printf("can't find input device\n");
		goto end;
	}

	// 1. ������
	// 1.1 �������ļ�����ȡ��װ��ʽ�����Ϣ
	if ((ret = avformat_open_input(&ifmtCtx, inFilename, ifmt, 0)) < 0)
	{
		printf("can't open input file: %s\n", inFilename);
		goto end;
	}

	// 1.2 ����һ�����ݣ���ȡ�������Ϣ
	if ((ret = avformat_find_stream_info(ifmtCtx, 0)) < 0)
	{
		printf("failed to retrieve input stream information\n");
		goto end;
	}

	// 1.3 ��ȡ����ctx
	for (i = 0; i < ifmtCtx->nb_streams; ++i)
	{
		if (ifmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoIndex = i;
			break;
		}
	}

	printf("%s:%d, videoIndex = %d\n", __FUNCTION__, __LINE__, videoIndex);

	av_dump_format(ifmtCtx, 0, inFilename, 0);

	// 1.4 �������������
	pCodec = avcodec_find_decoder(ifmtCtx->streams[videoIndex]->codecpar->codec_id);
	if (!pCodec)
	{
		printf("can't find codec\n");
		goto end;
	}

	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!pCodecCtx)
	{
		printf("can't alloc codec context\n");
		goto end;
	}

	avcodec_parameters_to_context(pCodecCtx, ifmtCtx->streams[videoIndex]->codecpar);

	//  1.5 �����������
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("can't open codec\n");
		goto end;
	}


	// 1.6 ����H264������
	pH264Codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!pH264Codec)
	{
		printf("can't find h264 codec.\n");
		goto end;
	}

	// 1.6.1 ���ò���
	pH264CodecCtx = avcodec_alloc_context3(pH264Codec);
	pH264CodecCtx->codec_id = AV_CODEC_ID_H264;
	pH264CodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pH264CodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pH264CodecCtx->width = pCodecCtx->width;
	pH264CodecCtx->height = pCodecCtx->height;
	pH264CodecCtx->time_base.num = 1;
	pH264CodecCtx->time_base.den = 25;	//֡�ʣ���һ���Ӷ�����ͼƬ��
	pH264CodecCtx->bit_rate = 400000;	//�����ʣ����������С���Ըı�������Ƶ��������
	pH264CodecCtx->gop_size = 250;
	pH264CodecCtx->qmin = 10;
	pH264CodecCtx->qmax = 51;
	//some formats want stream headers to be separate
//	if (pH264CodecCtx->flags & AVFMT_GLOBALHEADER)
	{
		pH264CodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}


	// 1.7 ��H.264������
	av_dict_set(&params, "preset", "superfast", 0);
	av_dict_set(&params, "tune", "zerolatency", 0);	//ʵ��ʵʱ����
	if (avcodec_open2(pH264CodecCtx, pH264Codec, &params) < 0)
	{
		printf("can't open video encoder.\n");
		goto end;
	}

	// 2. �����
	// 2.1 �������ctx
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
		printf("can't create output context\n");
		goto end;
	}

	// 2.2 ���������
	for (i = 0; i < ifmtCtx->nb_streams; ++i)
	{
		AVStream* outStream = avformat_new_stream(ofmtCtx, NULL);
		if (!outStream)
		{
			printf("failed to allocate output stream\n");
			goto end;
		}

		avcodec_parameters_from_context(outStream->codecpar, pH264CodecCtx);
	}

	av_dump_format(ofmtCtx, 0, outFilename, 1);

	if (!(ofmtCtx->oformat->flags & AVFMT_NOFILE))
	{
		// 2.3 ��������ʼ��һ��AVIOContext, ���Է���URL��outFilename��ָ������Դ
		ret = avio_open(&ofmtCtx->pb, outFilename, AVIO_FLAG_WRITE);
		if (ret < 0)
		{
			printf("can't open output URL: %s\n", outFilename);
			goto end;
		}
	}

	// 3. ���ݴ���
	// 3.1 д����ļ�
	ret = avformat_write_header(ofmtCtx, NULL);
	if (ret < 0)
	{
		printf("Error accourred when opening output file\n");
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
		// 3.2 ����������ȡһ��packet
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
				printf("Decode error.\n");
				goto end;
			}

			if (avcodec_receive_frame(pCodecCtx, pFrame) >= 0)
			{
				sws_scale(pImgConvertCtx,
					(const unsigned char* const*)pFrame->data,
					pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
					pFrameYUV->linesize);

				sws_scale(pImgToRgbConvertCtx,
					(const unsigned char* const*)pFrame->data,
					pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
					pFrameRGB->linesize);

				pFrameYUV->format = pCodecCtx->pix_fmt;
				pFrameYUV->width = pCodecCtx->width;
				pFrameYUV->height = pCodecCtx->height;

				pFrameRGB->format = pCodecCtx->pix_fmt;
				pFrameRGB->width = pCodecCtx->width;
				pFrameRGB->height = pCodecCtx->height;

				QImage tmpImg((uchar*)rgbOutBuffer, pCodecCtx->width, pCodecCtx->height, QImage::Format_RGB32);
				QImage image = tmpImg.copy(); //��ͼ����һ�� ���ݸ�������ʾ
				emit SigUpdateFrame(image);  //�����ź�

				ret = avcodec_send_frame(pH264CodecCtx, pFrameYUV);
				if (ret < 0)
				{
					printf("failed to encode.\n");
					goto end;
				}

				if (avcodec_receive_packet(pH264CodecCtx, &pkt) >= 0)
				{
					// �������DTS,PTS
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
		return;
	}

	return;
}
