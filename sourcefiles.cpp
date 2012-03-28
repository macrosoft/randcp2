#include <QTime>

#include "sourcefiles.h"


SourceFile::SourceFile(QString nPath, int nDirIndex):
    path(nPath),
    dirIndex(nDirIndex) {
}

int SourceFile::getIndex() {
    return dirIndex;
}

QString SourceFile::getPath() {
    return path;
}

SourceFiles::SourceFiles(SrcDirItemModel *nSrcDirModel):
    srcDirModel(nSrcDirModel) {
    qsrand(QTime::currentTime().msec());
}

void SourceFiles::add(QString path,int dirIndex) {
    files.append(new SourceFile(path, dirIndex));
}


QString SourceFiles::getDstPath(QString path, int index) {
    int srcLength = srcDirModel->getDir(index).length();
    QString dstPath =  path.remove(0, srcLength);
    QString additionalPath = srcDirModel->getAdditionalPath(index);
    dstPath = additionalPath + dstPath;
    return dstPath;
}

void SourceFiles::getFile(QString &srcPath, QString &dstPath, int index) {
    SourceFile *file = files.at(index);
    files.remove(index);
    srcPath = file->getPath();
    dstPath = getDstPath(file->getPath(), file->getIndex());
}

void SourceFiles::getFirstFile(QString &srcPath, QString &dstPath) {
    getFile(srcPath, dstPath, 0);
}

void SourceFiles::getRndFile(QString &srcPath, QString &dstPath) {
    int rand = qrand()%files.size();
    getFile(srcPath, dstPath, rand);
}

bool SourceFiles::isEmpty() {
    return files.isEmpty();
}

int SourceFiles::size() {
    return files.size();
}
