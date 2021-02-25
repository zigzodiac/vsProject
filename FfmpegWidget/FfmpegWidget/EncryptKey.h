#pragma once
//#include <stdio.h>
//#include <stdlib.h>
#include <QString>

#include <openssl/des.h>
#include <QByteArray> 
/************************************************************************
** 本例采用：
** 3des-ecb加密方式；
** 24位密钥，不足24位的右补0x00；
** 加密内容8位补齐，补齐方式为：少1位补一个0x01,少2位补两个0x02,...
** 本身已8位对齐的，后面补八个0x08。
************************************************************************/
#define ENCRYPT 1

#define keyStrLen 24
int ThreeTrip_DES(unsigned char* data, int datalen, unsigned char* key, int keylen,
    unsigned char* outdata, int* outdatalen);
int TripleDESEncryt(QString str, QString keyStr, char* dstKey, int* outLen);

char* base64Encode(const char* buffer, int length, bool newLine, int *outLen);