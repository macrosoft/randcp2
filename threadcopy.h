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

private:
    bool enableFileCount;
    bool enableFilter;
    bool enableIgnore;
    bool enableLimit;
    bool enableMaxDst;
    bool enableMinFreeSpace;
    QListWidget *filterListWidget;
    QListWidget *ignoreListWidget;
    float limit;
    float maxDst;
    int maxFileCount;
    float minFreeSpace;
    int mode;
    QMutex mutex;
    QString outputDir;
    QHash<QString, bool> outputFiles;
    SourceFiles *sourceFiles;
    int sleepTime;
    SrcDirItemModel *srcDirModel;
    bool stopFlag;

    bool checkFile(QFileInfo file, int index);
    bool checkFileFilter(QString file);
    void copy();
    void deleteOldFiles();
    quint64 getDirSize(QString path);
    int getSleep();
    bool getStopFlag();
    void scan();
    int scanDir(QString pathDir, int index);
    void scanOutput(QString pathDir);

public slots:
    void setSleep(int nSleep);
    void stop();

signals:
    void changeDiskFreeSpace();
    void done();
    void fileQueueChanged(int);
    void print(QString);
    void scanFinished();

};

#endif // THREADCOPY_H
