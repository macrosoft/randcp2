#ifndef THREADCOPY_H
#define THREADCOPY_H

#include <QThread>
#include <QtGui>
#include <QFileInfo>
#include "srcdiritemmodel.h"
#include "sourcefiles.h"

class ThreadCopy : public QThread
{
    Q_OBJECT
private:
    QString outputDir;
    bool enableFilter;
    QListWidget *filterListWidget;
    bool enableIgnore;
    QListWidget *ignoreListWidget;
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
    void scan();
    int scanDir(QString pathDir, int index);
    bool checkFile(QFileInfo file);
    quint64 getDirSize(QString path);
    void copy();
    int getSleep();
    bool getStopFlag();
public:
    ThreadCopy(QString nOutputDir, SrcDirItemModel *pSrcDirModel,
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
