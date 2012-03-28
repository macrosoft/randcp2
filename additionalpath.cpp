#include <QDir>

#include "additionalpath.h"

QString getAdditonalPath(QString fullPath, int level) {
    if (level) {
        QString path = QDir::fromNativeSeparators(fullPath);
        int max_level = getLevelParentDirs(path);
        if (level > max_level)
            level = max_level;
        int i, level_count = 0;
        for (i=path.length() - 1; i>=0; i--) {
            if (path[i]=='/')
                if (++level_count == level) {
                    break;
                }
        }
        path.remove(0,i);
        return path;
    } else {
        return QString();
    }
}

int getLevelParentDirs(QString path) {
    QString stdSepPath = QDir::fromNativeSeparators(path);
    int countParentDir = stdSepPath.count('/');
    #ifdef Q_OS_WIN
    if ((stdSepPath.length() == 3 && stdSepPath[2] == '/') ||
        (stdSepPath.length() > 2 &&
         stdSepPath[0] == '/' && stdSepPath[1] == '/'))
        countParentDir--;
    #endif

    #ifdef Q_OS_LINUX
    if (stdSepPath.length() == 1)
        countParentDir--;
    #endif
    return countParentDir;
}
