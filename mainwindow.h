#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QListWidget>
#include <QWidget>
#include <QSettings>
#include <QFileInfo>
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

    int state;
    Ui::MainWindow *ui;
    QSettings *settings;
    SrcDirItemModel *srcDirModel;
    ThreadCopy *threadCopy;
    QString newSrcDir;
    QString additionalPath;
    QModelIndex selectedIndexSrcList;
    void loadSettings();
    bool selectSrcDir();
    QString listWidgetToSting(QListWidget *lw);
    void loadListWidgetFromString(QListWidget *lw, QString str);
    QString sizeToStr(quint64 size);
    QString getLastSelectedIgnoreDir();
    void refreshParentDirLevel(int level);
    void startCopy();
    void cancelScan();
    void stop();
public slots:
    void selectOutputDir();
    void selectParentDirLevel(int level);
    void saveSettings();
    void addSrcDir();
    void selectSrcDirList(QModelIndex index);
    void editSrcDirList();
    void delSrcDirList();
    void setEnabledFilter(bool enable);
    void addFilterExt();
    void selectFilterExtList();
    void delFilterExtList();
    void setEnabledIgnore(bool enable);
    void addFileIgnoreList();
    void addDirIgnoreList();
    void selectIngoreList();
    void delIgnoreList();
    void editIgnoreList();
    void enableFileCount(bool enable);
    void enableFreeSpace(bool enable);
    void enableLimit(bool enable);
    void enableMaxDst(bool enable);
    void selectTab(int tabNum);
    void pressStartButton();
    void log(QString msg);
    void displayFileQueue(int count);
    void doneCoping();
    void updateDiskFreeSpace();
    void scanFinished();
    void refreshOutPath();
signals:
    void stopThread();
};

#endif // MAINWINDOW_H
