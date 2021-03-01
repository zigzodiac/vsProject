#pragma once
#include <libavutil/pixfmt.h>
#include <stdint.h>
#include <inttypes.h>

bool compareFacesByHist();
bool compareFacesByHist£±();
int InitMat(int width, int height, AVPixelFormat fmt);
int SetMatData(uint8_t* out_buffer, int height, int width, AVPixelFormat fmt, int size, int flag);
int PerceptualHash();
double PicCompare();
int PictureShow();
