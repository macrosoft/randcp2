#include "diskinfo.h"
#include "progresscontrol.h"


ProgressControl::ProgressControl(Settings *pSettings) {
    settings = pSettings;
}

ProgressControl::~ProgressControl() {
}

void ProgressControl::checkLimits(QFileInfo srcFileInfo) {
    limits[QUEUE_LIMIT].val++;
    quint64 outDiskSize = diskSize(settings->getOutputDir());
    limits[DISK_SIZE_LIMIT].reached = outDiskSize < (quint64)srcFileInfo.size();
    limits[RESERVED_SPACE_LIMIT].reached = limits[RESERVED_SPACE_LIMIT].max <=
            limits[RESERVED_SPACE_LIMIT].val + srcFileInfo.size();;
    limits[COPIED_SIZE_LIMIT].reached = limits[COPIED_SIZE_LIMIT].max <=
            limits[COPIED_SIZE_LIMIT].val + srcFileInfo.size();
    limits[DEST_SIZE_LIMIT].reached = limits[DEST_SIZE_LIMIT].max <=
            limits[DEST_SIZE_LIMIT].min +
            limits[DEST_SIZE_LIMIT].val + srcFileInfo.size();
}

bool ProgressControl::fileCountLimitIsReached() {
    return limits[FILE_COUNT_LIMIT].max <= limits[FILE_COUNT_LIMIT].val;
}

QString ProgressControl::getTextQuestion(int limit, QFileInfo srcFileInfo) {
    switch (limit) {
    case DISK_SIZE_LIMIT:
        return QObject::tr("Can`t copy file %1 (%2). %3 free. Try other file?")
                .arg(QDir::toNativeSeparators(srcFileInfo.absoluteFilePath()))
                .arg(sizeToStr(srcFileInfo.size()))
                .arg(sizeToStr(diskSize(settings->getOutputDir())));
    case RESERVED_SPACE_LIMIT:
        return QObject::tr("Can`t copy file %1 (%2). Try other file?")
                .arg(QDir::toNativeSeparators(srcFileInfo.absoluteFilePath()))
                .arg(sizeToStr(srcFileInfo.size()));
    case COPIED_SIZE_LIMIT:
        return QObject::tr("Can`t copy file %1 (%2). Try other file?")
                .arg(QDir::toNativeSeparators(srcFileInfo.absoluteFilePath()))
                .arg(sizeToStr(srcFileInfo.size()));
    case DEST_SIZE_LIMIT:
        return QObject::tr("Can`t copy file %1 (%2). Try other file?")
                .arg(QDir::toNativeSeparators(srcFileInfo.absoluteFilePath()))
                .arg(sizeToStr(srcFileInfo.size()));
    }
    return QString();
}

int ProgressControl::getLimitsCount() {
    return LIMITS_COUNT;
}

int ProgressControl::getMax() {
    int max = 0;
    for (int i = 0; i < FULL_LIMITS_COUNT; i++) {
        if (limits[i].enable) {
            float diff = (limits[i].max - limits[i].min);
            if (diff <= 0) {
                return 100;
            }
            int val = qRound(limits[i].val*100/diff);
            max = (max > val)? max: val;
        }
    }
    return max;
}

bool ProgressControl::limitIsReached(int numb) {
    return limits[numb].enable && limits[numb].reached;
}

void ProgressControl::prepare(int sourceFileCount, quint64 outDirSize) {
    for (int i = 0; i < FULL_LIMITS_COUNT; i++) {
        limits[i].enable = false;
        limits[i].reached = false;
        limits[i].min = 0;
        limits[i].max = 0;
        limits[i].val = 0;
    }
    quint64 outDiskSize = diskSize(settings->getOutputDir());
    limits[DISK_SIZE_LIMIT].enable = true;
    limits[DISK_SIZE_LIMIT].echo = QObject::tr("No free disk space!");
    limits[DISK_SIZE_LIMIT].max = outDiskSize;
    limits[RESERVED_SPACE_LIMIT].enable =
            settings->getBool(Settings::EN_MINFREESPACE);
    limits[RESERVED_SPACE_LIMIT].echo =
            QObject::tr("The min free space amount is reached.");
    if (limits[RESERVED_SPACE_LIMIT].enable)
        limits[RESERVED_SPACE_LIMIT].max = outDiskSize -
                settings->getDouble(Settings::MINFREESPACE)*1024*1024;
    limits[COPIED_SIZE_LIMIT].enable = settings->getBool(Settings::EN_LIMIT);
    limits[COPIED_SIZE_LIMIT].echo =
            QObject::tr("The max copied file size amount is reached.");
    if (limits[COPIED_SIZE_LIMIT].enable)
        limits[COPIED_SIZE_LIMIT].max =
                settings->getDouble(Settings::SIZE_LIMIT)*1024*1024;;
    limits[DEST_SIZE_LIMIT].enable = settings->getBool(Settings::EN_MAXDST);
    limits[DEST_SIZE_LIMIT].echo =
            QObject::tr("The max output directory size amount is reached.");
    if (limits[DEST_SIZE_LIMIT].enable) {
        limits[DEST_SIZE_LIMIT].min = outDirSize;
        limits[DEST_SIZE_LIMIT].max =
                settings->getDouble(Settings::MAXDST)*1024*1024;
    }
    limits[FILE_COUNT_LIMIT].enable =
            settings->getBool(Settings::EN_MAXFILECOUNT);
    if (limits[FILE_COUNT_LIMIT].enable) {
        limits[FILE_COUNT_LIMIT].max =
                settings->getInt(Settings::MAXFILECOUNT);
    }
    limits[QUEUE_LIMIT].enable = true;
    limits[QUEUE_LIMIT].max = sourceFileCount;
}

QString ProgressControl::say(int numb) {
    return limits[numb].echo;
}


void ProgressControl::step(QFileInfo srcFileInfo) {
    limits[DISK_SIZE_LIMIT].val =
        limits[RESERVED_SPACE_LIMIT].val =
        limits[DEST_SIZE_LIMIT].val =
        limits[COPIED_SIZE_LIMIT].val += srcFileInfo.size();
    limits[FILE_COUNT_LIMIT].val++;
}
