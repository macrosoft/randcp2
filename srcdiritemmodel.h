#ifndef SRCDIRITEMMODEL_H
#define SRCDIRITEMMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class SrcDirItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SrcDirItemModel(QObject *parent = 0);
    ~SrcDirItemModel();
    void addDir(QString dir, QString path);
    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const;
    void delDir(const QModelIndex &index);
    QString dirAt(int i);
    QString getAdditionalPath(const int index) const;
    QString getAdditionalPath(const QModelIndex &index) const;
    QString getDir(const int index) const;
    QString lastDir();
    void load(QString dirsStr, QString pathStr);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QString serializeDirs() const;
    QString serializePaths() const;
    void updateDir(QString dir, QString path, const QModelIndex &index);

protected:
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    virtual QStringList mimeTypes() const;

private:
    QStringList additionalPath;
    int dirLevel;
    QStringList dirs;
signals:

public slots:
    void setDirLevel(int level);
};

#endif // SRCDIRITEMMODEL_H
