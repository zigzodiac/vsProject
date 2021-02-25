#pragma once
#include<QStringList>

#include "ffmpegInf.h"

#include <iostream>
#include <list>

#define FORMAT "mpegts"
#include "Utils.h"

int VideoProcess(QStringList dirlist, VideoParam& dstVideoParam);
int ConcatVideo(VideoParam& dstVideoParam);
