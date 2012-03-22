#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegExp>
#include "additionalpath.h"
#include "diskinfo.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    additionalPath.clear();
    srcDirModel = new SrcDirItemModel();
    connect(ui->parentDirSpinBox, SIGNAL(valueChanged(int)),
            srcDirModel, SLOT(setDirLevel(int)));
    ui->srcListView->setModel(srcDirModel);
    ui->srcListView->setAcceptDrops(true);
    ui->srcListView->setDropIndicatorShown(false);
    loadSettings();
    threadCopy = NULL;
    state = READY;
}

MainWindow::~MainWindow() {
    delete ui;
    settings->deleteLater();
}

void MainWindow::selectOutputDir() {
    QString outputDir = QFileDialog::getExistingDirectory(this,
                                tr("Select output directory"),
                                ui->outDirLineEdit->text());
    if (!outputDir.isEmpty())
        ui->outDirLineEdit->setText(QDir::toNativeSeparators(outputDir));
}

bool MainWindow::selectSrcDir() {
    QString dir = QString();
    if  (selectedIndexSrcList.isValid())
        dir = srcDirModel->data(selectedIndexSrcList).toString();
    else
        dir = srcDirModel->lastDir();
    QString srcDir = QFileDialog::getExistingDirectory(this,
                                tr("Select source directory"),dir);
    if (!srcDir.isEmpty()) {
        newSrcDir = QDir::toNativeSeparators(srcDir);
        refreshParentDirLevel(ui->parentDirSpinBox->value());
        return true;
    }
    return false;
}

void MainWindow::refreshParentDirLevel(int level) {
    additionalPath = getAdditonalPath(newSrcDir, level);
    QString outDir = QDir::toNativeSeparators(getOutputDir());
    printFullOutPath(additionalPath);
}

void MainWindow::refreshOutPath() {
    refreshParentDirLevel(ui->parentDirSpinBox->value());
}

void MainWindow::selectParentDirLevel(int level) {
    refreshParentDirLevel(level);
    if (!newSrcDir.isEmpty())
        srcDirModel->updateDir(newSrcDir, additionalPath,
                               selectedIndexSrcList);
}

void MainWindow::loadSettings() {
    settings = new QSettings("randcp2.ini",QSettings::IniFormat);
    ui->outDirLineEdit->setText(settings->value("outdir").toString());
    srcDirModel->load(settings->value("srcdir").toString(),
                      settings->value("srcpath").toString());
    ui->filterCheckBox->setChecked(settings->value("enextfilter").toBool());
    ui->filterGroupBox->setEnabled(settings->value("enextfilter").toBool());
    loadListWidgetFromString(ui->filterListWidget,
                             settings->value("extfilter").toString());
    ui->ignoreCheckBox->setChecked(settings->value("enignorefilter").toBool());
    ui->ignoreGroupBox->setEnabled(settings->value("enignorefilter").toBool());
    loadListWidgetFromString(ui->ignoreListWidget,
                             settings->value("ignorefilter").toString());
    ui->fileCountCheckBox->setChecked(
                settings->value("enablemaxfilecount").toBool());
    ui->fileCountSpinBox->setEnabled(
                settings->value("enablemaxfilecount").toBool());
    ui->fileCountSpinBox->setValue(settings->value("ablemaxfilecount").toInt());
    ui->minFreeSpaceCheckBox->setChecked(
                settings->value("enminfreespace").toBool());
    ui->freeSpaceSpinBox->setEnabled(
                settings->value("enminfreespace").toBool());
    ui->freeSpaceSpinBox->setValue(settings->value("minfreespace").toDouble());
    ui->limitCheckBox->setChecked(settings->value("enlimit").toBool());
    ui->limitSpinBox->setEnabled(settings->value("enlimit").toBool());
    ui->limitSpinBox->setValue(settings->value("limit").toDouble());
    ui->maxDstCheckBox->setChecked(settings->value("enmaxdst").toBool());
    ui->maxDstSpinBox->setEnabled(settings->value("enmaxdst").toBool());
    ui->maxDstSpinBox->setValue(settings->value("maxdst").toDouble());
}

void MainWindow::saveSettings() {
    settings->setValue("outdir",ui->outDirLineEdit->text());
    settings->setValue("srcdir",srcDirModel->serializeDirs());
    settings->setValue("srcpath",srcDirModel->serializePaths());
    settings->setValue("enextfilter", ui->filterCheckBox->checkState());
    settings->setValue("extfilter",listWidgetToSting(ui->filterListWidget));
    settings->setValue("enignorefilter", ui->ignoreCheckBox->checkState());
    settings->setValue("ignorefilter",listWidgetToSting(ui->ignoreListWidget));
    settings->setValue("enablemaxfilecount",
                       ui->fileCountCheckBox->checkState());
    settings->setValue("maxfilecount",ui->fileCountSpinBox->value());
    settings->setValue("enminfreespace",
                       ui->minFreeSpaceCheckBox->checkState());
    settings->setValue("minfreespace",ui->freeSpaceSpinBox->value());
    settings->setValue("enlimit",
                       ui->limitCheckBox->checkState());
    settings->setValue("limit",ui->limitSpinBox->value());
    settings->setValue("enmaxdst",
                       ui->maxDstCheckBox->checkState());
    settings->setValue("maxdst",ui->maxDstSpinBox->value());
    settings->sync();
}

void MainWindow::addSrcDir() {
    if (selectSrcDir()) {
        srcDirModel->addDir(newSrcDir, additionalPath);
        ui->srcLineEdit->clear();
        ui->srcStatusLabel->clear();
        ui->parentDirSpinBox->setEnabled(false);
    }
}

void MainWindow::selectSrcDirList(QModelIndex index) {
    selectedIndexSrcList = index;
    newSrcDir = srcDirModel->data(index).toString();
    ui->srcLineEdit->setText(newSrcDir);
    QString path = QDir::fromNativeSeparators(srcDirModel->getPath(index));
    printFullOutPath(path);
    ui->parentDirSpinBox->setEnabled(true);
    ui->parentDirSpinBox->setMaximum(getLevelParentDirs(newSrcDir));
    ui->parentDirSpinBox->setValue(path.count('/'));
    ui->editSrcButton->setEnabled(true);
    ui->delSrcButton->setEnabled(true);
}

void MainWindow::editSrcDirList() {
    if (selectSrcDir()) {
        srcDirModel->updateDir(newSrcDir, additionalPath,
                               selectedIndexSrcList);
        ui->editSrcButton->setEnabled(false);
        ui->delSrcButton->setEnabled(false);
    }
}

void MainWindow::delSrcDirList() {
    srcDirModel->delDir(selectedIndexSrcList);
    ui->srcLineEdit->clear();
    ui->parentDirSpinBox->setValue(0);
    ui->editSrcButton->setEnabled(false);
    ui->delSrcButton->setEnabled(false);
}

void MainWindow::setEnabledFilter(bool enable) {
    ui->filterGroupBox->setEnabled(enable);
}

void MainWindow::addFilterExt() {
    if (!ui->filterLineEdit->text().isEmpty() &&
            !ui->filterLineEdit->text().count('/')) {
        ui->filterListWidget->addItem(ui->filterLineEdit->text());
        ui->filterLineEdit->clear();
    }
}

void MainWindow::selectFilterExtList() {
    ui->delFilterButton->setEnabled(true);
}

void MainWindow::delFilterExtList() {
    ui->filterListWidget->takeItem(ui->filterListWidget->currentRow());
    ui->delFilterButton->setEnabled(false);
    ui->filterLineEdit->clear();
}

QString MainWindow::listWidgetToSting(QListWidget *lw) {
    QString str = QString();
    for (int i=0; i < lw->count(); i++) {
        if (i)
            str += "////";
        str += lw->item(i)->text();
    }
    return str;
}

void MainWindow::loadListWidgetFromString(QListWidget *lw, QString str) {
    QStringList list = str.split("////");
    lw->clear();
    if (list.count() == 1 && list.at(0).isEmpty())
        return;
    for (int i=0; i < list.count(); i++) {
        lw->addItem(list.at(i));
    }
}

void MainWindow::setEnabledIgnore(bool enable) {
    ui->ignoreGroupBox->setEnabled(enable);
}

QString MainWindow::getLastSelectedIgnoreDir() {
    int row = ui->ignoreListWidget->currentRow();
    if (row >= 0)
        return ui->ignoreListWidget->item(row)->text();
    else if (ui->ignoreListWidget->count() > 0)
        return ui->ignoreListWidget->item(
                    ui->ignoreListWidget->count()-1)->text();
    return QString();
}

void MainWindow::addFileIgnoreList() {
    QString fileName = QFileDialog::getOpenFileName(this,
                            tr("Select ignore file"),
                            getLastSelectedIgnoreDir());
    if (!fileName.isEmpty())
        ui->ignoreListWidget->addItem(QDir::toNativeSeparators(fileName));
}

void MainWindow::addDirIgnoreList() {

    QString ignoreDir = QFileDialog::getExistingDirectory(this,
                        tr("Select ignore directory"),
                        getLastSelectedIgnoreDir());
    if (!ignoreDir.isEmpty()) {
        ui->ignoreListWidget->addItem(QDir::toNativeSeparators(ignoreDir));
        ui->delIgnoreButton->setEnabled(false);
        ui->editIgnoreButton->setEnabled(false);
    }
}

void MainWindow::selectIngoreList() {
    ui->delIgnoreButton->setEnabled(true);
    ui->editIgnoreButton->setEnabled(true);
}

void MainWindow::delIgnoreList() {
    ui->ignoreListWidget->takeItem(ui->ignoreListWidget->currentRow());
    ui->delIgnoreButton->setEnabled(false);
    ui->editIgnoreButton->setEnabled(false);
}

void MainWindow::editIgnoreList() {
    QString oldPath = ui->ignoreListWidget->currentItem()->text();
    QFileInfo oldFile(oldPath);
    QString newPath = QString();
    if (oldFile.isDir()) {
        newPath = QFileDialog::getExistingDirectory(this,
                                    tr("Select ignore directory"),
                                    oldPath);
    } else {
        newPath = QFileDialog::getOpenFileName(this,
                                tr("Select ignore file"),
                                oldFile.path());
    }
    if (!newPath.isEmpty())
        ui->ignoreListWidget->currentItem()->setText(
                    QDir::toNativeSeparators(newPath));
}

void MainWindow::enableFileCount(bool enable) {
    ui->fileCountSpinBox->setEnabled(enable);
}

void MainWindow::enableFreeSpace(bool enable) {
    ui->freeSpaceSpinBox->setEnabled(enable);
}

void MainWindow::enableLimit(bool enable) {
    ui->limitSpinBox->setEnabled(enable);
}

void MainWindow::enableMaxDst(bool enable) {
    ui->maxDstSpinBox->setEnabled(enable);
}

void MainWindow::selectTab(int tabNum) {
    if (tabNum == 3) {
        ui->frameBottom->setEnabled(false);
        updateDiskFreeSpace();
    } else {
        ui->frameBottom->setEnabled(true);
    }
}

void MainWindow::updateDiskFreeSpace() {
    QString freeSpace = sizeToStr(diskSize(ui->outDirLineEdit->text()));
    ui->freeSpaceLabel->setText(tr("Free disk space: ") + freeSpace);
}

void MainWindow::pressStartButton() {
    switch (state) {
        case READY: startCopy();
        break;
        case SCANING: cancelScan();
        break;
        case COPYING: stop();
        break;
    }
}

void MainWindow::stop() {
    ui->startButton->setEnabled(false);
    emit stopThread();
}

void MainWindow::cancelScan() {
    log(tr("***Scanning canceled!***"));
    displayFileQueue(0);
    emit stopThread();
}

void MainWindow::startCopy() {
    if (!ui->outDirLineEdit->text().isEmpty()) {
        threadCopy = new ThreadCopy(getOutputDir(),
                                    srcDirModel,
                                    ui->filterCheckBox->checkState(),
                                    ui->filterListWidget,
                                    ui->ignoreCheckBox->checkState(),
                                    ui->ignoreListWidget,
                                    ui->fileCountCheckBox->checkState(),
                                    ui->fileCountSpinBox->value(),
                                    ui->minFreeSpaceCheckBox->checkState(),
                                    ui->freeSpaceSpinBox->value(),
                                    ui->limitCheckBox->checkState(),
                                    ui->limitSpinBox->value(),
                                    ui->maxDstCheckBox->checkState(),
                                    ui->maxDstSpinBox->value(),
                                    ui->sleepSpinBox->value(),
                                    this);
        connect(threadCopy, SIGNAL(print(QString)), SLOT(log(QString)));
        connect(threadCopy, SIGNAL(fileQueueChanged(int)),
                SLOT(displayFileQueue(int)));
        connect(threadCopy, SIGNAL(changeDiskFreeSpace()),
                SLOT(updateDiskFreeSpace()));
        connect(threadCopy, SIGNAL(done()), SLOT(doneCoping()));
        connect(threadCopy, SIGNAL(scanFinished()), SLOT(scanFinished()));
        connect(this, SIGNAL(stopThread()), threadCopy, SLOT(stop()));
        connect(ui->sleepSpinBox, SIGNAL(valueChanged(int)),
                threadCopy, SLOT(setSleep(int)));
        for (int i=0; i < ui->tabWidget->count()-1; i++)
            ui->tabWidget->setTabEnabled(i, false);
        ui->logTextEdit->clear();
        state = SCANING;
        ui->startButton->setText(tr("Cancel"));
        threadCopy->start();
    } else {
        QMessageBox::warning(0,tr("Can't start copying files"),
                             tr("Select the output directory"));
    }
}

void MainWindow::scanFinished() {
    log(tr("Scanning finnished."));
    ui->startButton->setText(tr("Stop"));
    state = COPYING;
}

void MainWindow::doneCoping() {
    for (int i=0; i < ui->tabWidget->count()-1; i++)
        ui->tabWidget->setTabEnabled(i, true);
    ui->startButton->setEnabled(true);
    ui->startButton->setText(tr("Start"));
    log(tr("Done!"));
    state = READY;
}

QString MainWindow::sizeToStr(quint64 size) {
    if (size > 10737418240LL) { //10Gb
        return QString::number(size/1024.0/1024.0/1024.0,0,2) + " Gb";
    }
    if (size > 1048576) { //1Mb
        return QString::number(size/1024.0/1024.0,0,2) + " Mb";
    }
    if (size > 10240) { //10Kb
        return QString::number(size/1024.0,0,2) + " kb";
    }
    return QString::number(size) + " b";
}


void MainWindow::log(QString msg) {
    ui->logTextEdit->appendPlainText(msg);
}

void MainWindow::displayFileQueue(int count) {
    ui->filesLabel->setText(tr("Files in queue: %1").arg(count));
}

void MainWindow::printFullOutPath(QString addPath) {
    ui->srcStatusLabel->setText(getOutputDir()+
                                QDir::toNativeSeparators(addPath) +
                                QDir::separator() + "...");
}


QString MainWindow::getOutputDir() {
    QString outDir = ui->outDirLineEdit->text();
#ifdef Q_OS_WIN
    if (outDir.length() == 3 and outDir[1] == ':')
        outDir.remove(2,1);
#endif
    return outDir;
}
