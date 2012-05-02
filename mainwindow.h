#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfo>
#include <QtGui/QListWidget>
#include <QSettings>
#include <QWidget>

#include "srcdiritemmodel.h"
#include "threadcopy.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    enum STATE {READY, SCANING, COPYING};

    QString additionalPath;
    QString newSrcDir;
    QModelIndex selectedIndexSrcList;
    QSettings *settings;
    SrcDirItemModel *srcDirModel;
    int state;
    ThreadCopy *threadCopy;
    Ui::MainWindow *ui;

    void cancelScan();
    QString getLastSelectedIgnoreDir();
    QString getOutputDir();
    QString listWidgetToSting(QListWidget *lw);
    void loadListWidgetFromString(QListWidget *lw, QString str);
    void loadSettings();
    void printFullOutPath(QString addPath);
    void refreshParentDirLevel(int level);
    void startCopy();
    bool selectSrcDir();
    QString sizeToStr(quint64 size);
    void stop();

public slots:
    void addDirIgnoreList();
    void addFileIgnoreList();
    void addFilterExt();
    void addSrcDir();
    void delFilterExtList();
    void delIgnoreList();
    void delSrcDirList();
    void displayFileQueue(int count);
    void doneCoping();
    void enableFileCount(bool enable);
    void enableFreeSpace(bool enable);
    void enableLimit(bool enable);
    void enableMaxDst(bool enable);
    void editIgnoreList();
    void editSrcDirList();
    void editSrcDirListLineEdit(QString path);
    void log(QString msg);
    void pressStartButton();
    void refreshOutPath();
    void saveSettings();
    void scanFinished();
    void selectFilterExtList();
    void selectIngoreList();
    void selectOutputDir();
    void selectParentDirLevel(int level);
    void selectSrcDirList(QModelIndex index);
    void selectTab(int tabNum);
    void setEnabledFilter(bool enable);
    void setEnabledIgnore(bool enable);
    void updateDiskFreeSpace();

signals:
    void stopThread();

};

#endif // MAINWINDOW_H
