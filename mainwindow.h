#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfo>
#include <QWidget>

#include "settings.h"
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
    int prevTime;
    QModelIndex selectedIndexSrcList;
    Settings *settings;
    SrcDirItemModel *srcDirModel;
    int state;
    int time;
    QTimer *timer;
    ThreadCopy *threadCopy;
    Ui::MainWindow *ui;

    void cancelScan();
    QString getLastSelectedIgnore();
    QString listWidgetToSting(QListWidget *lw);
    void loadListWidgetFromString(QListWidget *lw, QString str);
    void loadSettings();
    void printFullOutPath(QString addPath);
    void refreshParentDirLevel(int level);
    void startCopy();
    bool selectSrcDir();
    void setTimeEstimate(int sec);
    void stop();

public slots:
    void addDirIgnoreList();
    void addFileIgnoreList();
    void addFilterExt();
    void addSrcDir();
    void addStringIgnoreList();
    void changeSettings();
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
    void timerTick();
    void outDirChanged();
    void saveSettings();
    void scanFinished();
    void setProgressBar(int val);
    void selectFilterExtList();
    void selectIngoreList();
    void selectOutputDir();
    void selectParentDirLevel(int level);
    void selectSrcDirList(QModelIndex index);
    void selectTab(int tabNum);
    void setEnabledFilter(bool enable);
    void setEnabledIgnore(bool enable);
    void showQuestionMsg(QString question);
    void runTimer();
    void updateDiskFreeSpace();

signals:
    void stopThread();

};

#endif // MAINWINDOW_H
