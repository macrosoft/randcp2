#include <QtGui>

#ifdef Q_OS_LINUX
#include <QMessageBox>
#include <QProcess>
#endif

#include <QHashIterator>
#include "threadcopy.h"
#include "diskinfo.h"

ThreadCopy::ThreadCopy(Settings *pSettings, SrcDirItemModel *pSrcDirModel,
                       QListWidget *pFilterListWidget,
                       QListWidget *pIgnoreListWidget, int nSleep,
                       QObject *parent) :
    QThread(parent) {
    settings = pSettings;
    outputDir = settings->getOutputDir();
    srcDirModel = pSrcDirModel;
    enableFilter = settings->getBool(Settings::EN_EXTFILTER);
    filterListWidget = pFilterListWidget;
    enableIgnore = settings->getBool(Settings::EN_IGNOREFILTER);
    ignoreListWidget = pIgnoreListWidget;
    stopFlag = false;
    setSleep(nSleep);
    answer = 0;
    #ifdef Q_OS_WIN
    wildcard.setCaseSensitivity(Qt::CaseInsensitive);
    #endif
    wildcard.setPatternSyntax(QRegExp::Wildcard);
    allwaysTryOtherFile = false;
    progressCtrl = new ProgressControl(settings);
}

ThreadCopy::~ThreadCopy() {
    sourceFiles->~SourceFiles();
    progressCtrl->~ProgressControl();
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
    switch (settings->getInt(Settings::MODE)) {
    case Settings::SHUFFLE:
        scan();
        copy();
        break;
    case Settings::SYNCHRONIZE:
        scanOutput(outputDir);
        scan();
        deleteOldFiles();
        copy();
        break;
    }
    deleteLater();
}

void ThreadCopy::setAnswer(int ans) {
    answer = ans;
}

//private

bool ThreadCopy::checkFile(QFileInfo file, int index) {
    QString filePath = file.absoluteFilePath();
    if (!checkFileFilter(filePath) || !checkFileIgnore(filePath))
        return false;
    QFileInfo dstFile(outputDir +
                      sourceFiles->getDstPath(file.absoluteFilePath(), index));
    if (dstFile.exists() &&
            qAbs((dstFile.lastModified().secsTo(file.lastModified()))) < 5) {
        outputFiles.remove(dstFile.absoluteFilePath());
        return false;
    }
    return true;
}

bool ThreadCopy::checkFileFilter(QString file) {
    if (!enableFilter)
        return true;
    for (int i=0; i < filterListWidget->count(); i++) {
        wildcard.setPattern("*." + filterListWidget->item(i)->text());
        if (wildcard.exactMatch(file)) {
            return true;
        }
    }
    return false;
}

bool ThreadCopy::checkFileIgnore(QString file) {
    if (!enableIgnore)
        return true;
    for (int i=0; i < ignoreListWidget->count(); i++) {
        wildcard.setPattern(QDir::fromNativeSeparators(
                          ignoreListWidget->item(i)->text()) + "*");
        if (wildcard.exactMatch(file)) {
            return false;
        }
    }
    return true;
}

int ThreadCopy::checkLimits(QFileInfo srcFileInfo) {
    progressCtrl->checkLimits(srcFileInfo);
    for (int i = 0; i < progressCtrl->getLimitsCount(); i++) {
        if (progressCtrl->limitIsReached(i)) {
            if (allwaysTryOtherFile)
                return TRY_OTHER_FILE;
            showQuestion(progressCtrl->getTextQuestion(i, srcFileInfo));
            if (answer == QMessageBox::Yes)
                return TRY_OTHER_FILE;
            if (answer == QMessageBox::YesToAll) {
                allwaysTryOtherFile = true;
                return TRY_OTHER_FILE;
            }
            emit print(progressCtrl->say(i));
            return LIMIT_REACHED;
        }
    }
    return LIMIT_OK;
}

void ThreadCopy::copy() {
    progressCtrl->prepare(sourceFiles->size(), getDirSize(outputDir));
    emit runTimer();
    quint64 copiedFileSize = 0;
    outDirSize = getDirSize(outputDir);
    while (!sourceFiles->isEmpty()) {
        if (getStopFlag()) {
            emit print(tr("***Stoped!***"));
            break;
        }
        QString srcFile, dstFile;
        if (settings->getInt(Settings::MODE) == Settings::SHUFFLE)
            sourceFiles->getRndFile(srcFile, dstFile);
        else
            sourceFiles->getFirstFile(srcFile, dstFile);
        emit fileQueueChanged(sourceFiles->size());
        dstFile = outputDir + dstFile;
        QFileInfo dstFileInfo(dstFile);
        if (dstFileInfo.exists()) {
            emit print(tr("Remove %1").arg(QDir::toNativeSeparators(dstFile)));
            QFile dest(dstFile);
            dest.remove();
        }
        QFileInfo srcFileInfo(srcFile);
        int check = checkLimits(srcFileInfo);
        if (check == LIMIT_REACHED) {
            emit progressChanged(100);
            break;
        } else if (check == TRY_OTHER_FILE) {
            emit progressChanged(progressCtrl->getMax());
            continue;
        }
        copiedFileSize += srcFileInfo.size();
        emit print(tr("Copy %1").arg(QDir::toNativeSeparators(srcFile)));
        emit print(tr("to %2").arg(QDir::toNativeSeparators(dstFile)));
        emit print(QString());
        QString newPath = dstFileInfo.path();
        dstFileInfo.dir().mkpath(newPath);
        QFile::copy(srcFile,dstFile);

        #ifdef Q_OS_LINUX
        QProcess *touch = new QProcess();
        QString modifyTime =
                srcFileInfo.lastModified().toString("yyMMddhhmm.ss");
        touch->start("touch",
                QStringList() << "-m" << "-t" << modifyTime << dstFile);
        touch->waitForFinished();
        touch->deleteLater();
        #endif
        progressCtrl->step(srcFileInfo);
        if (progressCtrl->fileCountLimitIsReached()) {
            emit progressChanged(100);
            emit print(tr("The max file amount is reached."));
            break;
        }
        emit changeDiskFreeSpace();
        emit progressChanged(progressCtrl->getMax());
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

float ThreadCopy::getProgressMax() {
    return progressCtrl->getRealMax();
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
                   .arg(srcCount));
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
    if (!checkFileIgnore(QDir::fromNativeSeparators(pathDir)))
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

void ThreadCopy::showQuestion(QString q) {
    emit question(q);
    questionMutex.lock();
    questionWait.wait(&questionMutex);
    questionMutex.unlock();
}

//public slots

void ThreadCopy::setSleep(int nSleep) {
    QMutexLocker ml(&mutex);
    sleepTime = nSleep;
}

void ThreadCopy::stop() {
    QMutexLocker ml(&mutex);
    stopFlag = true;
}
