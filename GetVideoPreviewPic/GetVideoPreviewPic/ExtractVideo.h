#pragma once

#include"FfmpegIntf.h"

int SetDiffDegree(int diff);
int SetDir(char** dir);
int OpenVideo(const char* filename);
int GetVideoPic();

int GetVideoDuration();