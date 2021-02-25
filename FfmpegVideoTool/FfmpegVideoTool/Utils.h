#include <QString>
#include <QStringList>
#ifndef UTILS
#define UTILS

#pragma once
enum ThreadStatus {
    READY = 0,
    RUNNING,
    BLOCKED,
    DEAD
};

enum TaskType {
    TRANSCODE = 0,
    CONCAT,
    EXTRACT,
    GET_KEY_PIC
};

typedef struct VideoParam {
    TaskType  taskType;
    QStringList inFnlist;
    QString outFn;
    QString outDir;
    QString Format;
    int rate;
    int width;
    int height;
    int quality;

    //m3u8
    int hls_list_size;
    int hls_time;
}VideoParam;

#endif