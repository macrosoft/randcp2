#ifndef PROGRESSCONTROL_H
#define PROGRESSCONTROL_H

#include <QFileInfo>
#include <QMutex>

#include "settings.h"


class ProgressControl {
public:
    ProgressControl(Settings *pSettings);
    ~ProgressControl();

    void checkLimits(QFileInfo srcFileInfo);
    bool fileCountLimitIsReached();
    QString getTextQuestion(int limit, QFileInfo srcFileInfo);
    int getLimitsCount();
    int getMax();
    float getRealMax();
    bool limitIsReached(int numb);
    void prepare(int sourceFileCount, quint64 outDirSize);
    QString say(int numb);
    void step(QFileInfo srcFileInfo);
private:
    struct Limit {
        bool enable;
        bool reached;
        QString echo;
        float min;
        float max;
        float val;
    };

    enum {DISK_SIZE_LIMIT, RESERVED_SPACE_LIMIT, COPIED_SIZE_LIMIT,
          DEST_SIZE_LIMIT, FILE_COUNT_LIMIT, QUEUE_LIMIT};

    static const int LIMITS_COUNT = 4; // without QUEUE_LIMIT
    static const int FULL_LIMITS_COUNT = LIMITS_COUNT + 2;
        // with QUEUE_LIMIT and FILE_COUNT_LIMIT
    Limit limits[FULL_LIMITS_COUNT];
    QMutex mutex;
    Settings *settings;
};

#endif // PROGRESSCONTROL_H
