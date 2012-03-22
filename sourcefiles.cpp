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

void SourceFiles::getFile(QString &srcPath, QString &dstPath, bool randMode) {
    int index = (randMode)? qrand()%files.size(): 0;
    SourceFile *file = files.at(index);
    files.remove(index);
    dstPath = srcPath = file->getPath();
    int srcLength = srcDirModel->getDir(file->getIndex()).length();
    dstPath = dstPath.remove(0, srcLength);
    QString additionalPath = srcDirModel->getPath(file->getIndex());
    dstPath = additionalPath + dstPath;
}

int SourceFiles::size() {
    return files.size();
}
