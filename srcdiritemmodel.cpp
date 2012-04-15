#include <QDir>
#include <QFileInfo>
#include <QMimeData>
#include <QUrl>

#include "srcdiritemmodel.h"
#include "additionalpath.h"

SrcDirItemModel::SrcDirItemModel(QObject *parent) :
    QAbstractListModel(parent)
{
    dirLevel = 0;
}

SrcDirItemModel::~SrcDirItemModel() {
}

//public

void SrcDirItemModel::addDir(QString dir, QString path) {
    dirs.append(dir);
    additionalPath.append(path);
    QModelIndex index;
    emit dataChanged(index,index);
}

QVariant SrcDirItemModel::data(const QModelIndex &index, int role) const {
    if (index.isValid() and index.row() < dirs.size() and index.row() >= 0) {
        return role == Qt::DisplayRole? dirs.at(index.row()): QVariant();
    } else
        return QVariant();
}


void SrcDirItemModel::delDir(const QModelIndex &index) {
    dirs.removeAt(index.row());
    additionalPath.removeAt(index.row());
    emit dataChanged(index,index);
}

QString SrcDirItemModel::dirAt(int i) {
    if (i < dirs.count())
        return dirs.at(i);
    else
        return QString();
}

QString SrcDirItemModel::getAdditionalPath(const int index) const {
    return additionalPath.at(index);
}

QString SrcDirItemModel::getAdditionalPath(const QModelIndex &index) const {
    return additionalPath.at(index.row());
}

QString SrcDirItemModel::getDir(const int index) const {
    return dirs.at(index);
}

QString SrcDirItemModel::lastDir() {
    if (dirs.size()) {
        return dirs.at(dirs.size() - 1);
    } else {
        return QString();
    }
}

void SrcDirItemModel::load(QString dirsStr, QString pathStr) {
    dirs << dirsStr.split("////");
    additionalPath << pathStr.split("////");
    QModelIndex index;
    emit dataChanged(index,index);
    if (dirs.at(0).isEmpty()) {
        dirs.clear();
        additionalPath.clear();
    }
}

int SrcDirItemModel::rowCount(const QModelIndex &parent) const {
    return dirs.size();
}

QString SrcDirItemModel::serializeDirs() const {
    return dirs.join("////");
}

QString SrcDirItemModel::serializePaths()  const {
    return additionalPath.join("////");
}

void SrcDirItemModel::updateDir(QString dir, QString path,
                                const QModelIndex &index) {
    if (dirs.size() > index.row()) {
        dirs[index.row()] = dir;
        additionalPath[index.row()] = path;
        emit dataChanged(index,index);
    }
}
//protected

bool SrcDirItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                   int row, int column,
                                   const QModelIndex &parent) {
    if (action == Qt::IgnoreAction)
        return true;
    if (!data->hasFormat("text/uri-list"))
        return false;
    if (column > 0)
        return false;
    QList<QUrl> urlList = data->urls();
    foreach (QUrl url, urlList) {
        QString dirName = url.toLocalFile();
        if (QFileInfo(dirName).isDir())
            addDir(QDir::toNativeSeparators(dirName),
                   QDir::toNativeSeparators(
                       getAdditonalPath(dirName, dirLevel)));
    }
    return true;
}

Qt::ItemFlags SrcDirItemModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    if (index.isValid())
        return Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

Qt::DropActions SrcDirItemModel::supportedDropActions() const {
    return Qt::CopyAction;
}

QMimeData *SrcDirItemModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *mimeData = new QMimeData();
    return mimeData;
}


QStringList SrcDirItemModel::mimeTypes() const {
    return QStringList("text/uri-list");
}

//public slots

void SrcDirItemModel::setDirLevel(int level) {
    dirLevel = level;
}
