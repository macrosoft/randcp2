#ifndef DISKINFO_H
#define DISKINFO_H
#include <QtCore>

quint64 diskSize(QString path);
QString sizeToStr(quint64 size);

#endif // DISKINFO_H
