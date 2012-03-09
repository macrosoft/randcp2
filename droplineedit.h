#ifndef DROPLINEEDIT_H
#define DROPLINEEDIT_H

#include <QLineEdit>

class DropLineEdit : public QLineEdit {
    Q_OBJECT
protected:
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dropEvent(QDropEvent *);
public:
    explicit DropLineEdit(QWidget *parent = 0);

signals:

public slots:

};

#endif // DROPLINEEDIT_H
