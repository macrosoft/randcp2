#include "diskinfo.h"
#include <QDir>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_LINUX
#include <sys/statvfs.h>
#endif

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
