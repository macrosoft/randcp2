#ifndef SOURCEFILES_H
#define SOURCEFILES_H
#include <QString>
#include <QVector>
#include "srcdiritemmodel.h"

class SourceFile
{
private:
    QString path;
    int dirIndex;
public:
    SourceFile(QString nPath, int nDirIndex);
    QString getPath();
    int getIndex();
};

class SourceFiles
{
private:
    SrcDirItemModel *srcDirModel;
    QVector<SourceFile*> files;
public:
    SourceFiles(SrcDirItemModel *nSrcDirModel);
    void add(QString path, int dirIndex);
    bool isEmpty();
    void getFile(QString &srcPath, QString &dstPath, int index);
    void getFirstFile(QString &srcPath, QString &dstPath);
    void getRndFile(QString &srcPath, QString &dstPath);
    int size();
    QString getDstPath(QString path, int index);
};

#endif // SOURCEFILES_H
