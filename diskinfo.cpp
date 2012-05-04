#include <QDir>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_LINUX
#include <sys/statvfs.h>
#endif

#include "diskinfo.h"

quint64 diskSize(QString path) {
    quint64 free_space;
    if (path.isEmpty())
        return 0;
    QDir dir(path);
    if (!dir.exists())
        return 0;
    free_space = 0;

    #ifdef Q_OS_WIN
    const wchar_t *encodedPath
            = reinterpret_cast<const wchar_t *>(path.utf16());
    GetDiskFreeSpaceExW(encodedPath, (PULARGE_INTEGER)&free_space, 0, 0);
    #endif

    #ifdef Q_OS_LINUX
    struct statvfs buf;
    if (!statvfs(path.toAscii(), &buf)) {
        free_space = buf.f_bfree*buf.f_bsize;
    }
    #endif

    return free_space;
}

QString sizeToStr(quint64 size) {
    if (size > 10737418240LL) { //10Gb
        return QString::number(size/1024.0/1024.0/1024.0,0,2) + " Gb";
    }
    if (size > 1048576) { //1Mb
        return QString::number(size/1024.0/1024.0,0,2) + " Mb";
    }
    if (size > 10240) { //10Kb
        return QString::number(size/1024.0,0,2) + " kb";
    }
    return QString::number(size) + " b";
}
