#include "sourcefiles.h"
#include <QTime>

SourceFile::SourceFile(QString nPath, int nDirIndex):
    path(nPath),
    dirIndex(nDirIndex) {
}

QString SourceFile::getPath() {
    return path;
}


int SourceFile::getIndex() {
    return dirIndex;
}

SourceFiles::SourceFiles(SrcDirItemModel *nSrcDirModel):
    srcDirModel(nSrcDirModel) {
    qsrand(QTime::currentTime().msec());
}

void SourceFiles::add(QString path,int dirIndex) {
    files.append(new SourceFile(path, dirIndex));
}

bool SourceFiles::isEmpty() {
    return files.isEmpty();
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

int SourceFiles::size() {
    return files.size();
}
