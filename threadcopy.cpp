#include <QtGui>

#ifdef Q_OS_LINUX
#include <QProcess>
#endif

#include <QHashIterator>
#include "threadcopy.h"
#include "diskinfo.h"

ThreadCopy::ThreadCopy(QString nOutputDir, SrcDirItemModel *pSrcDirModel,
                       int nMode, bool enableFilterFlag,
                       QListWidget *pFilterListWidget, bool enableIgnoreFlag,
                       QListWidget *pIgnoreListWidget, bool enableFileCountFlag,
                       int nMaxFileCount, bool enableMinFreeSpaceFlag,
                       float nMinFreeSpace, bool enableLimitFlag,
                       float nLimit, bool enableMaxDstFlag,
                       float nMaxDst, int nSleep, QObject *parent) :
    QThread(parent)
{
    outputDir = nOutputDir;
    srcDirModel = pSrcDirModel;
    mode = nMode;
    enableFilter = enableFilterFlag;
    filterListWidget = pFilterListWidget;
    enableIgnore = enableIgnoreFlag;
    ignoreListWidget = pIgnoreListWidget;
    enableFileCount = enableFileCountFlag;
    maxFileCount = nMaxFileCount;
    enableMinFreeSpace = enableMinFreeSpaceFlag;
    minFreeSpace = nMinFreeSpace;
    enableLimit = enableLimitFlag;
    limit = nLimit;
    enableMaxDst = enableMaxDstFlag;
    maxDst = nMaxDst;
    stopFlag = false;
    setSleep(nSleep);
}

ThreadCopy::~ThreadCopy() {
    sourceFiles->~SourceFiles();
    emit done();
}

//public

void ThreadCopy::run() {
    sourceFiles = new SourceFiles(srcDirModel);
    QFileInfo outDir(outputDir);
    if (!outDir.exists()) {
        if (outDir.dir().mkpath(outputDir)) {
            emit print(tr("Creating output directory"));
            emit changeDiskFreeSpace();
        } else {
            emit print(tr("Can not create output directory :("));
            deleteLater();
            return;
        }
    }
    switch (mode) {
    case SHUFFLE:
        scan();
        copy();
        break;
    case SYNCHRONIZE:
        scanOutput(outputDir);
        scan();
        deleteOldFiles();
        copy();
        break;
    }
    deleteLater();
}

//private

bool ThreadCopy::checkFile(QFileInfo file, int index) {
    bool filtred = checkFileFilter(file.absoluteFilePath());
    if (!filtred)
        return false;
    QRegExp rx("");
    if (enableIgnore) {
        for (int i=0; i < ignoreListWidget->count(); i++) {
            rx.setPattern(QDir::fromNativeSeparators(
                              ignoreListWidget->item(i)->text()) + "*");
                if (rx.exactMatch(file.absoluteFilePath())) {
                    filtred = false;
                    break;
                }
        }
    }
    QFileInfo dstFile(outputDir +
                      sourceFiles->getDstPath(file.absoluteFilePath(), index));

    if (dstFile.exists() &&
            qAbs((dstFile.lastModified().secsTo(file.lastModified()))) < 5) {
        outputFiles.remove(dstFile.absoluteFilePath());
        return false;
    }
    return filtred;
}

bool ThreadCopy::checkFileFilter(QString file) {
    QRegExp rx("");
    rx.setPatternSyntax(QRegExp::Wildcard);
    bool filtred = !enableFilter;
    if (enableFilter) {
        for (int i=0; i < filterListWidget->count(); i++) {
            rx.setPattern("*." + filterListWidget->item(i)->text());
            if (rx.exactMatch(file)) {
                filtred = true;
                break;
            }
        }
    }
    return filtred;
}

void ThreadCopy::copy() {
    int fileCount = 0;
    quint64 copiedFileSize = 0;
    quint64 outDirSize = getDirSize(outputDir);
    while (!sourceFiles->isEmpty()) {
        if (getStopFlag()) {
            emit print(tr("***Stoped!***"));
            break;
        }
        QString srcFile, dstFile;
        if (mode == SHUFFLE)
            sourceFiles->getRndFile(srcFile, dstFile);
        else
            sourceFiles->getFirstFile(srcFile, dstFile);
        dstFile = outputDir + dstFile;
        QFileInfo dstFileInfo(dstFile);
        if (dstFileInfo.exists()) {
            emit print(tr("Remove %1").arg(QDir::toNativeSeparators(dstFile)));
            QFile dest(dstFile);
            dest.remove();
        }
        QFileInfo srcFileInfo(srcFile);
        if (diskSize(outputDir) < (quint64)srcFileInfo.size()) {
            emit print(tr("No free disk space!"));
            break;
        }
        if (enableMinFreeSpace) {
            quint64 reserved_free_space = diskSize(outputDir)
                    - srcFileInfo.size();
             if (reserved_free_space
                     < minFreeSpace*1024*1024) {
                emit print(tr("The min free space amount is reached."));
                break;
            }
        }
        if (enableLimit) {
            if (copiedFileSize + srcFileInfo.size() >
                    limit*1024*1024) {
                emit print(tr("The max copied file size amount is reached."));
                break;
            }
        }
        if (enableMaxDst) {
            if (outDirSize + copiedFileSize + srcFileInfo.size() >
                    maxDst*1024*1024) {
                emit print(tr(
                           "The max output directory size amount is reached."));
                break;
            }
        }
        copiedFileSize += srcFileInfo.size();
        emit print(tr("Copy %1").arg(QDir::toNativeSeparators(srcFile)));
        emit print(tr("to %2").arg(QDir::toNativeSeparators(dstFile)));
        emit print(QString());
        emit fileQueueChanged(sourceFiles->size());
        QString newPath = dstFileInfo.path();
        dstFileInfo.dir().mkpath(newPath);
        QFile::copy(srcFile,dstFile);

        #ifdef Q_OS_LINUX
        QProcess *touch = new QProcess(this);
        QString modifyTime =
                srcFileInfo.lastModified().toString("yyMMddhhmm.ss");
        touch->start("touch",
                QStringList() << "-m" << "-t" << modifyTime << dstFile);
        touch->waitForFinished();
        touch->deleteLater();
        #endif

        if (enableFileCount && ++fileCount >= maxFileCount) {
            emit print(tr("The max file amount is reached."));
            break;
        }
        emit changeDiskFreeSpace();
        this->msleep(getSleep());
    }
}

void ThreadCopy::deleteOldFiles() {
    QHashIterator<QString, bool> i(outputFiles);
    while (i.hasNext()) {
        if (getStopFlag())
            return;
        i.next();
        emit print(tr("Remove %1").arg(QDir::toNativeSeparators(i.key())));
        QFile file(i.key());
        file.remove();
    }
    emit print(QString());
}

quint64 ThreadCopy::getDirSize(QString path) {
    QDir dir(path);
    quint64 size = 0;
    QFileInfoList files = dir.entryInfoList(QDir::Dirs | QDir::Files
                                            | QDir::NoSymLinks
                                            | QDir::NoDotAndDotDot);
    for (int i = 0; i < files.size(); i++) {
        QFileInfo f = files.at(i);
        if (f.isDir())
            size += getDirSize(f.filePath());
        else
            size += f.size();
    }

    return size;
}

int ThreadCopy::getSleep() {
    QMutexLocker ml(&mutex);
    return sleepTime;
}

bool ThreadCopy::getStopFlag() {
    QMutexLocker ml(&mutex);
    return stopFlag;
}

void ThreadCopy::scan() {
    int srcCount = srcDirModel->rowCount();
    int filesFound = 0;
    for (int i=0; i<srcCount; i++) {
        emit print(QString(tr("Scanning directories: %1 of %2")).arg(i+1)
                   .arg(srcCount).toAscii());
        emit print(QString(tr("Scanning: %1")).arg(srcDirModel->dirAt(i)));
        filesFound = scanDir(srcDirModel->dirAt(i), i);
        emit print(QString(tr("Files found: %1")).arg(filesFound));
        if (getStopFlag())
            return;
    }
    emit scanFinished();
}

int ThreadCopy::scanDir(QString pathDir, int index) {
    if (getStopFlag())
        return 0;
    QDir dir(pathDir);
    int filesFound = 0;
    QFileInfoList files = dir.entryInfoList(QDir::Files);
    for (int i = 0; i < files.size(); i++) {
        QFileInfo f = files.at(i);
        if (checkFile(f, index)) {
            sourceFiles->add(f.absoluteFilePath(), index);
            filesFound++;
            emit fileQueueChanged(sourceFiles->size());
        }
    }
    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirs.size(); i++) {
         QFileInfo d = dirs.at(i);
         filesFound += scanDir(d.absoluteFilePath(), index);
    }
    return filesFound;
}

void ThreadCopy::scanOutput(QString pathDir) {
    if (getStopFlag())
        return;
    QDir dir(pathDir);
    QFileInfoList files = dir.entryInfoList(QDir::Files);
    for (int i = 0; i < files.size(); i++) {
        QString fileName = files.at(i).absoluteFilePath();
        if (checkFileFilter(fileName))
            outputFiles.insert(fileName, true);
    }
    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirs.size(); i++) {
             QFileInfo d = dirs.at(i);
             scanOutput(d.absoluteFilePath());
    }
}

//slots

void ThreadCopy::setSleep(int nSleep) {
    QMutexLocker ml(&mutex);
    sleepTime = nSleep;
}

void ThreadCopy::stop() {
    QMutexLocker ml(&mutex);
    stopFlag = true;
}

