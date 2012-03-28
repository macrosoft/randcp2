#ifndef THREADCOPY_H
#define THREADCOPY_H

#include <QThread>
#include <QtGui>
#include <QFileInfo>
#include <QHash>
#include "srcdiritemmodel.h"
#include "sourcefiles.h"

class ThreadCopy : public QThread
{
    Q_OBJECT
private:
    QString outputDir;
    bool enableFilter;
    QHash<QString, bool> outputFiles;
    QListWidget *filterListWidget;
    bool enableIgnore;
    QListWidget *ignoreListWidget;
    int mode;
    bool enableFileCount;
    int maxFileCount;
    bool enableMinFreeSpace;
    float minFreeSpace;
    bool enableLimit;
    float limit;
    bool enableMaxDst;
    float maxDst;
    SourceFiles *sourceFiles;
    SrcDirItemModel *srcDirModel;
    QMutex mutex;
    int sleepTime;
    bool stopFlag;
    void deleteOldFiles();
    void scan();
    int scanDir(QString pathDir, int index);
    void scanOutput(QString pathDir);
    bool checkFile(QFileInfo file, int index);
    bool checkFileFilter(QString file);
    quint64 getDirSize(QString path);
    void copy();
    int getSleep();
    bool getStopFlag();
public:
    enum {SHUFFLE, SYNCHRONIZE};
    ThreadCopy(QString nOutputDir, SrcDirItemModel *pSrcDirModel, int nMode,
               bool enableFilterFlag, QListWidget *pFilterListWidget,
               bool enableIgnoreFlag, QListWidget *pIgnoreListWidget,
               bool enableFileCountFlag, int nMaxFileCount,
               bool enableMinFreeSpaceFlag, float nMinFreeSpace,
               bool enableLimitFlag, float nLimit,
               bool enableMaxDstFlag, float nMaxDst, int nSleep,
               QObject *parent = 0);
    ~ThreadCopy();
    void run();
signals:
    void print(QString);
    void fileQueueChanged(int);
    void changeDiskFreeSpace();
    void scanFinished();
    void done();
public slots:
    void setSleep(int nSleep);
    void stop();
};

#endif // THREADCOPY_H
