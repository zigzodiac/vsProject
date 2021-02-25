#ifndef __FFMPEG_TRANSCODE_H__  
#define __FFMPEG_TRANSCODE_H__  

#include <string.h> 
#include <stdio.h>
#include <vector>
#include <map>
#include <list>

using namespace std;

extern "C"
{
#include "libavformat/avformat.h"  
#include "libavformat/avio.h"  
#include "libavcodec/avcodec.h"  
#include "libswscale/swscale.h"  
#include "libavutil/avutil.h"  
#include "libavutil/mathematics.h"  
#include "libswresample/swresample.h"  
#include "libavutil/opt.h"  
#include "libavutil/channel_layout.h"  
#include "libavutil/samplefmt.h"  
#include "libavdevice/avdevice.h"  //����ͷ����  
#include "libavfilter/avfilter.h"  
#include "libavutil/error.h"  
#include "libavutil/mathematics.h"    
#include "libavutil/time.h"    
#include "libavutil/fifo.h"  
#include "libavutil/audio_fifo.h"   //����������Ƭʱ���ز���������Ƶ�õ�  
#include "inttypes.h"  
#include "stdint.h"  
};

/*#pragma comment(lib,"avformat.lib")  
#pragma comment(lib,"avcodec.lib")  
#pragma comment(lib,"avdevice.lib")  
#pragma comment(lib,"avfilter.lib")  
#pragma comment(lib,"avutil.lib")  
#pragma comment(lib,"postproc.lib")  
#pragma comment(lib,"swresample.lib")  
#pragma comment(lib,"swscale.lib") */ 

//#define INPUTURL   "../in_stream/22.flv"   
//#define INPUTURL "../in_stream/������.The.Flash.S01E01.��Ӣ��Ļ.HDTVrip.624X352.mp4"  
//#define INPUTURL   "../in_stream/33.ts"   
//#define INPUTURL   "../in_stream/22mp4.mp4"   
//#define INPUTURL   "../in_stream/EYED0081.MOV"   
//#define INPUTURL   "../in_stream/���ٺ� - ���.mp3"   
//#define INPUTURL   "../in_stream/avier1.mp4"   
//#define INPUTURL   "../in_stream/������2Ԥ��Ƭ.mp4"   
//#define INPUTURL   "../in_stream/Class8���.m4v"   
#define INPUTURL   "../in_stream/9160_2.ts"   
//#define INPUTURL   "../in_stream/44.mp3"  
//#define INPUTURL   "../in_stream/ceshi.mp4"  
//#define INPUTURL   "../in_stream/33.mp4"  
//#define INPUTURL   "../in_stream/father.avi"  
//#define INPUTURL   "../in_stream/22.flv"  
//#define INPUTURL   "../in_stream/�������.wav"   
//#define INPUTURL   "../in_stream/Furious_7_2015_International_Trailer_2_5.1-1080p-HDTN.mp4"   
//#define INPUTURL   "../in_stream/Wildlife.wmv"   
//#define INPUTURL   "../in_stream/������Ů2.HD1280��������.mp4"   
//#define INPUTURL     "rtmp://221.228.193.50:1935/live/teststream1"   
#define OUTPUTURL  "../out_stream/output.flv" 
//http://10.69.112.96:8080/live/10flv/index.m3u8
#define OUTPUTURL10  "../out_stream/10.flv"
//#define OUTPUTURL10   "rtmp://10.69.112.96:1936/live/10flv"
#define OUTPUTURL11  "../out_stream/11.flv"  
//#define OUTPUTURL11   "rtmp://10.69.112.96:1936/live/11flv"
#define OUTPUTURL12  "../out_stream/12.flv"  
//#define OUTPUTURL12   "rtmp://10.69.112.96:1936/live/12flv"
//#define OUTPUTURL    "rtmp://221.228.193.50:1935/live/zwg"  
//#define OUTPUTURL    "rtmp://221.228.193.50:1935/live/zwg"  


//����ö��
enum AVSampleFormat_t
{
	AV_SAMPLE_FMT_NONE_t = -1,
	AV_SAMPLE_FMT_U8_t,          ///< unsigned 8 bits  
	AV_SAMPLE_FMT_S16_t,         ///< signed 16 bits  
	AV_SAMPLE_FMT_S32_t,         ///< signed 32 bits  
	AV_SAMPLE_FMT_FLT_t,         ///< float  
	AV_SAMPLE_FMT_DBL_t,         ///< double  

	AV_SAMPLE_FMT_U8P_t,         ///< unsigned 8 bits, planar  
	AV_SAMPLE_FMT_S16P_t,        ///< signed 16 bits, planar  
	AV_SAMPLE_FMT_S32P_t,        ///< signed 32 bits, planar  
	AV_SAMPLE_FMT_FLTP_t,        ///< float, planar  
	AV_SAMPLE_FMT_DBLP_t,        ///< double, planar  

	AV_SAMPLE_FMT_NB_t           ///< Number of sample formats. DO NOT USE if linking dynamically  
};


#define OUT_AUDIO_ID            0                                                 //packet �е�ID ������ȼ�����Ƶ pocket ����Ƶ�� 0  ��Ƶ��1�������෴(Ӱ��add_out_stream˳��)  
#define OUT_VIDEO_ID            1  

//��·���ÿһ·����Ϣ�ṹ��
typedef struct Out_stream_info_t
{
	//user info
	int user_stream_id;                 //��·���ÿһ·��ID
	//video param  
	int m_dwWidth;
	int m_dwHeight;
	double m_dbFrameRate;               //֡��                                                    
	int m_video_codecID;
	int m_video_pixelfromat;
	int m_bit_rate;                     //����
	int m_gop_size;
	int m_max_b_frame;
	int m_thread_count;                 //��cpu�ں���Ŀ  
	//audio param  
	int m_dwChannelCount;               //����  
	AVSampleFormat_t m_dwBitsPerSample; //����  
	int m_dwFrequency;                  //������  
	int m_audio_codecID;

	//ffmpeg out pram
	AVAudioFifo* m_audiofifo;          //��Ƶ���pcm����  
	int64_t m_first_audio_pts;          //��һ֡����Ƶpts  
	int m_is_first_audio_pts;           //�Ƿ��Ѿ���¼��һ֡��Ƶʱ���  
	AVFormatContext* m_ocodec;         //�����context  
	int m_writeheader_seccess;          //дͷ�ɹ�Ҳ����д��ͷ֧��������д������Ƶ��ʽ��������ʵȵ�
	AVStream* m_ovideo_st;
	AVStream* m_oaudio_st;
	AVCodec* m_audio_codec;
	AVCodec* m_video_codec;
	AVPacket m_pkt;
	AVBitStreamFilterContext* m_vbsf_aac_adtstoasc;     //aac->adts to asc������  
	struct SwsContext* m_img_convert_ctx_video;
	int m_sws_flags;                    //��ֵ�㷨,˫���� 
	AVFrame* m_pout_video_frame;
	AVFrame* m_pout_audio_frame;
	SwrContext* m_swr_ctx;
	char m_outurlname[256];             //�����url��ַ

	Out_stream_info_t()
	{
		//user info
		user_stream_id = 0;             //��·���ÿһ·��ID
		//video param  
		m_dwWidth = 640;
		m_dwHeight = 480;
		m_dbFrameRate = 25;  //֡��                                                    
		m_video_codecID = (int)AV_CODEC_ID_H264;
		m_video_pixelfromat = (int)AV_PIX_FMT_YUV420P;
		m_bit_rate = 400000;                //����
		m_gop_size = 12;
		m_max_b_frame = 0;
		m_thread_count = 2;                 //��cpu�ں���Ŀ  
		//audio param  
		m_dwChannelCount = 2;               //����  
		m_dwBitsPerSample = AV_SAMPLE_FMT_S16_t; //����  
		m_dwFrequency = 44100;              //������  
		m_audio_codecID = (int)AV_CODEC_ID_AAC;

		//ffmpeg out pram  
		m_audiofifo = NULL;                 //��Ƶ���pcm����  
		m_first_audio_pts = 0;              //��һ֡����Ƶpts  
		m_is_first_audio_pts = 0;           //�Ƿ��Ѿ���¼��һ֡��Ƶʱ���  
		m_ocodec = NULL;                    //�����context 
		m_writeheader_seccess = 0;
		m_ovideo_st = NULL;
		m_oaudio_st = NULL;
		m_audio_codec = NULL;
		m_video_codec = NULL;
		//m_pkt;     
		m_vbsf_aac_adtstoasc = NULL;        //aac->adts to asc������  
		m_img_convert_ctx_video = NULL;
		m_sws_flags = SWS_BICUBIC;          //��ֵ�㷨,˫����  
		m_pout_video_frame = NULL;
		m_pout_audio_frame = NULL;
		m_swr_ctx = NULL;
		memset(m_outurlname, 0, 256);         //����
	}
}Out_stream_info;


extern AVFormatContext* m_icodec;                         //������context  
extern int m_in_dbFrameRate;                              //��������֡��
extern int m_in_video_stream_idx;                         //����������Ƶ���к�  
extern int m_in_audio_stream_idx;                         //����������Ƶ���к�
extern int m_in_video_starttime;                          //����������Ƶ��ʼʱ��
extern int m_in_audio_starttime;                          //����������Ƶ��ʼʱ��
extern AVPacket m_in_pkt;                                 //��ȡ�����ļ�packet
extern map<int, Out_stream_info*> m_list_out_stream_info;  //��·�����list

//��ʼ��demux
int ffmpeg_init_demux(char* inurlname, AVFormatContext** iframe_c);
//�ͷ�demux
int ffmpeg_uinit_demux(AVFormatContext* iframe_c);
//��ʼ��mux:list,ԭʼ��ֻ��Ҫcopy��
int ffmpeg_init_mux(map<int, Out_stream_info*> list_out_stream_info, int original_user_stream_id);
//�ͷ�mux,ԭʼ��ֻ��Ҫcopy�Ĳ��ô򿪱�����
int ffmpeg_uinit_mux(map<int, Out_stream_info*> list_out_stream_info, int original_user_stream_id);

//for mux copy 
AVStream* ffmpeg_add_out_stream(AVFormatContext* output_format_context, AVMediaType codec_type_t);
//for codec  
AVStream* ffmpeg_add_out_stream2(Out_stream_info* out_stream_info, AVMediaType codec_type_t, AVCodec** codec);
int ffmpeg_init_decode(int stream_type);
int ffmpeg_init_code(int stream_type, AVStream* out_stream, AVCodec* out_codec);
int ffmpeg_uinit_decode(int stream_type);
int ffmpeg_uinit_code(int stream_type, AVStream* out_stream);
//ת������,ԭʼ��ֻ��Ҫcopy��
int ffmpeg_transcode(int original_user_stream_id);


//������ת�����������õ�
int ffmpeg_perform_decode(int stream_type, AVFrame* picture);
int ffmpeg_perform_code2(Out_stream_info* out_stream_info, int stream_type, AVFrame* picture);  //����AVAudioFifo 
void ffmpeg_perform_yuv_conversion(Out_stream_info* out_stream_info, AVFrame* pinframe, AVFrame* poutframe);
SwrContext* ffmpeg_init_pcm_resample(Out_stream_info* out_stream_info, AVFrame* in_frame, AVFrame* out_frame);
int ffmpeg_preform_pcm_resample(Out_stream_info* out_stream_info, SwrContext* pSwrCtx, AVFrame* in_frame, AVFrame* out_frame);
void ffmpeg_uinit_pcm_resample(SwrContext* swr_ctx, AVAudioFifo* audiofifo);
void ffmpeg_write_frame(Out_stream_info* out_stream_info, int ID, AVPacket pkt_t);        //copy
void ffmpeg_write_frame2(Out_stream_info* out_stream_info, int ID, AVPacket pkt_t);       //codec

#endif