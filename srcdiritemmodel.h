#ifndef SRCDIRITEMMODEL_H
#define SRCDIRITEMMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class SrcDirItemModel : public QAbstractListModel
{
    Q_OBJECT
protected:
    QStringList dirs;
    QStringList additionalPath;
    int dirLevel;

    virtual Qt::DropActions supportedDropActions() const;
    virtual  Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QStringList mimeTypes() const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent);
public:
    explicit SrcDirItemModel(QObject *parent = 0);
    ~SrcDirItemModel();
    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    void addDir(QString dir, QString path);
    void updateDir(QString dir, QString path, const QModelIndex &index);
    void delDir(const QModelIndex &index);
    QString getAdditionalPath(const QModelIndex &index) const;
    QString getAdditionalPath(const int index) const;
    QString getDir(const int index) const;
    QString serializeDirs() const;
    QString serializePaths() const;
    void load(QString dirsStr, QString pathStr);
    QString dirAt(int i);
    QString lastDir();
signals:

public slots:
    void setDirLevel(int level);
};

#endif // SRCDIRITEMMODEL_H
