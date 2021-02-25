#pragma once

int InitMat(int width, int height, AVPixelFormat fmt);
int SetMatData(uint8_t* out_buffer, int height, int width, AVPixelFormat fmt, int size, int flag);

double PicCompare();
int PictureShow();
