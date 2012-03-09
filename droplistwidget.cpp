#include <QDragEnterEvent>
#include <QDir>
#include <QUrl>
#include "droplistwidget.h"

DropListWidget::DropListWidget(QWidget *parent) :
    QListWidget(parent) {
    setAcceptDrops(true);
}

Qt::DropActions DropListWidget::supportedDropActions() const {
    return Qt::CopyAction;
}

QStringList DropListWidget::mimeTypes() const {
    return QStringList("text/uri-list");
}

bool DropListWidget::dropMimeData(int index, const QMimeData *data,
                          Qt::DropAction action) {
    QList<QUrl> urlList = data->urls();
    foreach (QUrl url, urlList) {
        addItem(QDir::toNativeSeparators(url.toLocalFile()));
    }
    return true;
}
