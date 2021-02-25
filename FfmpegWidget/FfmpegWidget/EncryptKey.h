#pragma once
//#include <stdio.h>
//#include <stdlib.h>
#include <QString>

#include <openssl/des.h>
#include <QByteArray> 
/************************************************************************
** �������ã�
** 3des-ecb���ܷ�ʽ��
** 24λ��Կ������24λ���Ҳ�0x00��
** ��������8λ���룬���뷽ʽΪ����1λ��һ��0x01,��2λ������0x02,...
** ������8λ����ģ����油�˸�0x08��
************************************************************************/
#define ENCRYPT 1

#define keyStrLen 24
int ThreeTrip_DES(unsigned char* data, int datalen, unsigned char* key, int keylen,
    unsigned char* outdata, int* outdatalen);
int TripleDESEncryt(QString str, QString keyStr, char* dstKey, int* outLen);

char* base64Encode(const char* buffer, int length, bool newLine, int *outLen);