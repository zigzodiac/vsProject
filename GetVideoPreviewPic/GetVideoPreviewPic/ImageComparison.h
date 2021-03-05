#pragma once
#include <libavutil/pixfmt.h>
#include <stdint.h>
#include <inttypes.h>

bool compareFacesByHist();
bool compareFacesByHist£±();
int InitMat(int width, int height, AVPixelFormat fmt);
int SetMatData(uint8_t* out_buffer, int height, int width, AVPixelFormat fmt, int size, int flag);
int PerceptualHash();
int pHashValueCompare();
double PicCompare();
int PictureShow();
float CheckBlankScreen(int flag, float &result);