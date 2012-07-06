#ifndef THREADCOPY_H
#define THREADCOPY_H

#include <QThread>
#include <QtGui>
#include <QFileInfo>
#include <QHash>
#include "settings.h"
#include "sourcefiles.h"
#include "srcdiritemmodel.h"

class ThreadCopy : public QThread
{
    Q_OBJECT

public:
    QWaitCondition questionWait;


    ThreadCopy(Settings *pSettings, SrcDirItemModel *pSrcDirModel,
               QListWidget *pFilterListWidget, QListWidget *pIgnoreListWidget,
               int nSleep, QObject *parent);
    ~ThreadCopy();
    void run();
    void setAnswer(int ans);

private:
    struct Limits {
        bool enable;
        bool value;
        QString echo;
    };
    enum {LIMIT_REACHED, TRY_OTHER_FILE, LIMIT_OK};
    enum {DISK_SIZE_LIMIT, RESERVED_SPACE_LIMIT, COPIED_SIZE_LIMIT,
          DEST_SIZE_LIMIT};
    static const int LIMITS_COUNT = 4;

    bool allwaysTryOtherFile;
    int answer;
    bool enableFileCount;
    bool enableFilter;
    bool enableIgnore;
    QListWidget *filterListWidget;
    QListWidget *ignoreListWidget;
    float limit;
    Limits limits[LIMITS_COUNT];
    float maxDst;
    float minFreeSpace;
    QMutex mutex;
    QString outputDir;
    quint64 outDirSize;
    QHash<QString, bool> outputFiles;
    Settings *settings;
    SourceFiles *sourceFiles;
    int sleepTime;
    SrcDirItemModel *srcDirModel;
    bool stopFlag;
    QRegExp wildcard;
    QMutex questionMutex;
    bool checkFile(QFileInfo file, int index);
    bool checkFileFilter(QString file);
    bool checkFileIgnore(QString file);
    int checkLimits(QFileInfo srcFileInfo, int copiedFileSize);
    void copy();
    void deleteOldFiles();
    quint64 getDirSize(QString path);
    int getSleep();
    bool getStopFlag();
    QString getTextQuestion(int limit, QFileInfo srcFileInfo);
    void prepareLimitsTable();
    void scan();
    int scanDir(QString pathDir, int index);
    void scanOutput(QString pathDir);
    void showQuestion(QString q);

public slots:
    void setSleep(int nSleep);
    void stop();

signals:
    void changeDiskFreeSpace();
    void done();
    void fileQueueChanged(int);
    void print(QString);
    void scanFinished();
    void question(QString);
};

#endif // THREADCOPY_H
