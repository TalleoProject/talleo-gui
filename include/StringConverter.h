// Copyright (c) 2021, The Talleo developers

#ifndef _STRING_CONVERTER_H
#define _STRING_CONVERTER_H
#include <string>
#include <QString>

inline std::string convertQStringToStdString(const QString &str) {
   return str.toLocal8Bit().constData();
}

inline QString convertStdStringToQString(const std::string &str) {
   return QString::fromLocal8Bit(str.c_str());
}
#endif
