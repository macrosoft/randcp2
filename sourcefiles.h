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
    void getFile(QString &srcPath, QString &dstPath, bool randMode);
    int size();
};

#endif // SOURCEFILES_H
