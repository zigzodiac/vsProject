#pragma once


#include "ffmpegInf.h"
#include <stdint.h>
#include <inttypes.h>
#include <QString>

//static char av_error[AV_ERROR_MAX_STRING_SIZE] = { 0 };
int transcode(QString inFn, QString outFn, int width, int height);
int GetOutFileInfo();

typedef struct {
    int width;
    int height;
}OutFmt;

