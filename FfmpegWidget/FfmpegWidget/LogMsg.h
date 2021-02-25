#pragma once

#include <QMutex> 
#include <QString>
#include <QDateTime> 
#include <QFile> 
#include <QTextStream> 
void outputMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg);


