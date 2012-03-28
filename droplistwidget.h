#ifndef DROPLISTWIDGET_H
#define DROPLISTWIDGET_H

#include <QListWidget>

class DropListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit DropListWidget(QWidget *parent = 0);
    virtual bool dropMimeData(int index, const QMimeData *data,
                              Qt::DropAction action);
    virtual QStringList mimeTypes() const;
    virtual Qt::DropActions supportedDropActions() const;

signals:

public slots:

};


class DropExtListWidget : public DropListWidget {
    Q_OBJECT
public:
    explicit DropExtListWidget(QWidget *parent = 0);
    virtual bool dropMimeData(int index, const QMimeData *data,
                              Qt::DropAction action);
};

#endif // DROPLISTWIDGET_H
