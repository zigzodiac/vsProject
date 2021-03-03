#ifndef FFMPEGINTF
#define FFMPEGINTF


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "libavutil/samplefmt.h"  
#include <libavutil/avutil.h>
#include <libavfilter/avfilter.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include "libavfilter/avfilter.h"  
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/timestamp.h>
#include <libavutil/error.h>

}


static char av_error[AV_ERROR_MAX_STRING_SIZE] = { 0 };
#define av_err2str(errnum) av_make_error_string(av_error, AV_ERROR_MAX_STRING_SIZE, errnum)

#endif



