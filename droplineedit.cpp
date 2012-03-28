#include <QDir>
#include <QDragEnterEvent>
#include <QFileInfo>
#include <QUrl>

#include "droplineedit.h"

DropLineEdit::DropLineEdit(QWidget *parent) :
    QLineEdit(parent) {
}

void DropLineEdit::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void DropLineEdit::dropEvent(QDropEvent *event) {
    QUrl url = event->mimeData()->urls().at(0);
    QString dir = url.toLocalFile();
    if (QFileInfo(dir).isDir())
        setText(QDir::toNativeSeparators(dir));
}
