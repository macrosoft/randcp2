#ifndef SOURCEFILES_H
#define SOURCEFILES_H

#include <QString>
#include <QVector>

#include "settings.h"
#include "srcdiritemmodel.h"

class SourceFile
{
private:
    QString path;
    int dirIndex;

public:
    SourceFile(QString nPath, int nDirIndex);
    int getIndex();
    QString getPath();
};

class SourceFiles
{
private:
    QVector<SourceFile*> files;
    SrcDirItemModel *srcDirModel;
    Settings *settings;

public:
    SourceFiles(Settings *pSettings, SrcDirItemModel *nSrcDirModel);
    void add(QString path, int dirIndex);
    QString getDstPath(QString path, int index);
    void getFile(QString &srcPath, QString &dstPath, int index);
    void getFirstFile(QString &srcPath, QString &dstPath);
    void getRndFile(QString &srcPath, QString &dstPath);
    bool isEmpty();
    int size();
};

#endif // SOURCEFILES_H
