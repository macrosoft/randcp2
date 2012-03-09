#ifndef ADDITIONALPATH_H
#define ADDITIONALPATH_H

#include <QString>

int getLevelParentDirs(QString path);
QString getAdditonalPath(QString fullPath, int level);

#endif // ADDITIONALPATH_H
