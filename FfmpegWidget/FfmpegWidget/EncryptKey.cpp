#include "EncryptKey.h"
#include <openssl/evp.h>
#include <openssl/x509.h>

unsigned char cipherData[100];
char* outData;
//3DES
int ThreeTrip_DES(unsigned char* data, int datalen, unsigned char* key, int keylen,
    unsigned char* outdata, int* outdatalen)
{
    unsigned char outbuf[1024];
    int outlen, tmplen;

    //8字节一个分组的长度
    unsigned char iv[8] = { 0,0,0,0,0,0,0,0 }; 




    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_CIPHER_CTX_init(ctx);
    EVP_EncryptInit_ex(ctx, EVP_des_ede_ecb(), NULL, key, iv);//3DES算法 EVP_des_ede_cbc\EVP_des_ede3_ecb\EVP_des_cbc

    EVP_CIPHER_CTX_set_padding(ctx, 1);

    if (!EVP_EncryptUpdate(ctx, outbuf, &outlen, (unsigned char*)data, datalen))
    {
        EVP_CIPHER_CTX_cleanup(ctx);
        return -1;
    }
    //注意，传入给下面函数的输出缓存参数必须注意不能覆盖了原来的加密输出的数据
    if (!EVP_EncryptFinal_ex(ctx, outbuf + outlen, &tmplen))
    {
        EVP_CIPHER_CTX_cleanup(ctx);
        return -2;
    }
    outlen += tmplen;
    EVP_CIPHER_CTX_cleanup(ctx);

    int outLen;
    char* dataBase64 = base64Encode((const char*)(char*)outbuf, outlen, BIO_FLAGS_BASE64_NO_NL, &outLen);
    printf("after  encrypt and base64 %s:\n", dataBase64);
    //返回数据
   /* *outdatalen = outlen;
    memcpy(outdata, outbuf, outlen);*/

    return 0;
}




int TripleDESEncryt(QString str, QString keyStr, char *dstKey, int *outLen)
{
    
    QByteArray keyByte = keyStr.toLocal8Bit();
    const char* keyByteStr = keyByte.data();

    QByteArray text = str.toLocal8Bit();
    QByteArray by = text.toBase64();
    char* data = (char*)malloc(by.size() + 1);
    memset(data, '\0', by.size() + 1);
    if (data == NULL)
        return -1;
    memcpy(data, by.data(), by.size());
    
    int data_len = by.size();
    QByteArray tempKey = keyStr.toLocal8Bit();
    const char* k = tempKey.data();
    int docontinue = 1;

    
    unsigned char ch;
    unsigned char* src = NULL; /* 补齐后的明文 */
    unsigned char* dst = NULL; /* 解密后的明文 */
    int len;
    unsigned char tmp[8];
    unsigned char in[8];
    unsigned char out[8];
    int key_len;
#define LEN_OF_KEY 24
    unsigned char key[LEN_OF_KEY]; /* 补齐后的密钥 */
    unsigned char block_key[9];
    DES_key_schedule ks, ks2, ks3;
    /* 构造补齐后的密钥 */
    
    key_len = strlen(k);
    int copyLen;
    if (key_len > LEN_OF_KEY) {
        copyLen = LEN_OF_KEY;
        memcpy(key, k, copyLen);
    }
    else {
        memset(key + key_len, 0x00, LEN_OF_KEY - key_len);
    }
    
    int data_rest = data_len % 8;
    //data_rest != 0 ? len = data_len + (8 - data_rest) : len = data_len;
    len = len = data_len + (8 - data_rest);
    ch = 8 - data_rest;
    src = (unsigned char*)malloc(len);
    dst = (unsigned char*)malloc(len);
    if (NULL == src || NULL == dst)
    {
        docontinue = 0;
    }
    if (docontinue)
    {
        int count;
        int i;
        /* 构造补齐后的加密内容 */
        memset(src, 0, len);
        memcpy(src, data, data_len);
       memset(src + data_len, ch, 8 - data_rest);
        /* 密钥置换 */
        memset(block_key, 0, sizeof(block_key));
        memcpy(block_key, keyByteStr + 0, 8);
        DES_set_key_unchecked((DES_cblock*)block_key, &ks);
        memcpy(block_key, keyByteStr + 8, 8);
        DES_set_key_unchecked((DES_cblock*)block_key, &ks2);
        memcpy(block_key, keyByteStr + 16, 8);
        DES_set_key_unchecked((DES_cblock*)block_key, &ks3);
        printf("before encrypt:\n");
        for (i = 0; i < len; i++)
        {
            printf("0x%.2X ", *(src + i));
        }
        printf("\n");
        /* 循环加密/解密，每8字节一次 */
        count = len / 8;
        for (i = 0; i < count; i++)
        {
            memset(out, 0, 8);
            memcpy(in, src + 8 * i, 8);
            /* 加密 */
            DES_ecb3_encrypt((const_DES_cblock*)in, (DES_cblock*)out, &ks, &ks2, &ks3, DES_ENCRYPT);
            memcpy(dst + 8 * i, out, 8);
        }
        printf("after decrypt :\n");
        for (i = 0; i < len; i++)
        {
            printf("0x%.2X ", *(dst + i));
        }
        printf("\n");
    }
   
    char* dataBase64 = base64Encode((const char*)(char*)dst, len, BIO_FLAGS_BASE64_NO_NL, outLen);

    if (dataBase64 != NULL && outLen != NULL) {
        memcpy(dstKey, dataBase64, *outLen);
        free(dataBase64);
        dataBase64 = NULL;
    }

    if (NULL != src)
    {
        free(src);
        src = NULL;
    }
    if (NULL != dst)
    {
        free(dst);
        dst = NULL;
    }
    return 0;
}

 char* base64Encode(const char* buffer, int length, bool newLine, int *outLen)
{
    BIO* bmem = NULL;
    BIO* b64 = NULL;
    BUF_MEM* bptr;
    b64 = BIO_new(BIO_f_base64());
    if (!newLine) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, buffer, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    BIO_set_close(b64, BIO_NOCLOSE);
    char* buff = (char*)malloc(bptr->length + 1);
    memcpy(buff, bptr->data, bptr->length);
    buff[bptr->length] = 0;
    if (outLen != NULL) {
        *outLen = bptr->length;
    }
    BIO_free_all(b64);
    return buff;
}
static char* base64Decode(char* input, int length, bool newLine)
{
    BIO* b64 = NULL;
    BIO* bmem = NULL;
    char* buffer = (char*)malloc(length);
    memset(buffer, 0, length);
    b64 = BIO_new(BIO_f_base64());
    if (!newLine) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new_mem_buf(input, length);
    bmem = BIO_push(b64, bmem);
    BIO_read(bmem, buffer, length);
    BIO_free_all(bmem);
    return buffer;
}