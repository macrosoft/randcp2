#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
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
    for (int i = 0; i < ui->ignoreListWidget->count(); i++) {
        ui->ignoreListWidget->item(i)->setFlags(
                    ui->ignoreListWidget->item(i)->flags() |
                    Qt::ItemIsEditable);
    }
    threadCopy = NULL;
    state = READY;
}

MainWindow::~MainWindow() {
    delete ui;
    settings->~Settings();
}

//private

void MainWindow::cancelScan() {
    log(tr("***Scanning canceled!***"));
    displayFileQueue(0);
    emit stopThread();
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

void MainWindow::loadSettings() {
    settings = new Settings();
    ui->outDirLineEdit->setText(settings->getString(Settings::OUTDIR));
    srcDirModel->load(settings->getString(Settings::SRCDIR),
                      settings->getString(Settings::SRCPATH));
    ui->filterCheckBox->setChecked(settings->getBool(Settings::EN_EXTFILTER));
    ui->filterGroupBox->setEnabled(settings->getBool(Settings::EN_EXTFILTER));
    loadListWidgetFromString(ui->filterListWidget,
                             settings->getString(Settings::EXTFILTER));
    ui->ignoreCheckBox->setChecked(
                settings->getBool(Settings::EN_IGNOREFILTER));
    ui->ignoreGroupBox->setEnabled(
                settings->getBool(Settings::EN_IGNOREFILTER));
    loadListWidgetFromString(ui->ignoreListWidget,
                             settings->getString(Settings::IGNOREFILTER));
    if (settings->getInt(Settings::MODE, 0) == Settings::SHUFFLE)
        ui->rndModeRadioButton->setChecked(true);
    else
        ui->synchModeRadioButton->setChecked(true);
    ui->fileCountCheckBox->setChecked(
                settings->getBool(Settings::EN_MAXFILECOUNT));
    ui->fileCountSpinBox->setEnabled(
                settings->getBool(Settings::EN_MAXFILECOUNT));
    ui->fileCountSpinBox->setValue(settings->getInt(Settings::MAXFILECOUNT));
    ui->minFreeSpaceCheckBox->setChecked(
                settings->getBool(Settings::EN_MINFREESPACE));
    ui->freeSpaceSpinBox->setEnabled(
                settings->getBool(Settings::EN_MINFREESPACE));
    ui->freeSpaceSpinBox->setValue(settings->getDouble(Settings::MINFREESPACE));
    ui->limitCheckBox->setChecked(settings->getBool(Settings::EN_LIMIT));
    ui->limitSpinBox->setEnabled(settings->getBool(Settings::EN_LIMIT));
    ui->limitSpinBox->setValue(settings->getDouble(Settings::LIMIT));
    ui->maxDstCheckBox->setChecked(settings->getBool(Settings::EN_MAXDST));
    ui->maxDstSpinBox->setEnabled(settings->getBool(Settings::EN_MAXDST));
    ui->maxDstSpinBox->setValue(settings->getDouble(Settings::MAXDST));
    connect(ui->rndModeRadioButton, SIGNAL(toggled(bool)), settings,
            SLOT(setMode(bool)));
}

void MainWindow::printFullOutPath(QString addPath) {
    ui->srcStatusLabel->setText(settings->getOutputDir() +
                                QDir::toNativeSeparators(addPath) +
                                QDir::separator() + "...");
}

void MainWindow::refreshParentDirLevel(int level) {
    additionalPath = getAdditonalPath(newSrcDir, level);
    QString outDir = QDir::toNativeSeparators(settings->getOutputDir());
    printFullOutPath(additionalPath);
}

void MainWindow::startCopy() {
    if (!ui->outDirLineEdit->text().isEmpty()) {
        threadCopy = new ThreadCopy(settings, srcDirModel, ui->filterListWidget,
                                    ui->ignoreListWidget,
                                    ui->sleepSpinBox->value(), this);
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
        connect(threadCopy, SIGNAL(question(QString)),
                SLOT(showQuestionMsg(QString)));
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

void MainWindow::stop() {
    ui->startButton->setEnabled(false);
    emit stopThread();
}

//public slots

void MainWindow::addDirIgnoreList() {
    QString ignoreDir = QFileDialog::getExistingDirectory(this,
                        tr("Select ignore directory"),
                        getLastSelectedIgnoreDir());
    if (!ignoreDir.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem(
                    QDir::toNativeSeparators(ignoreDir));
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->ignoreListWidget->addItem(item);
        ui->delIgnoreButton->setEnabled(false);
        ui->editIgnoreButton->setEnabled(false);
    }
}

void MainWindow::addFileIgnoreList() {
    QString fileName = QFileDialog::getOpenFileName(this,
                            tr("Select ignore file"),
                            getLastSelectedIgnoreDir());
    if (!fileName.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem(
                    QDir::toNativeSeparators(fileName));
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->ignoreListWidget->addItem(item);
    }
}

void MainWindow::addFilterExt() {
    if (!ui->filterLineEdit->text().isEmpty() &&
            !ui->filterLineEdit->text().count('/')) {
        ui->filterListWidget->addItem(ui->filterLineEdit->text());
        ui->filterLineEdit->clear();
    }
}

void MainWindow::addSrcDir() {
    if (selectSrcDir()) {
        srcDirModel->addDir(newSrcDir, additionalPath);
        ui->srcLineEdit->clear();
        ui->srcStatusLabel->clear();
        ui->parentDirSpinBox->setEnabled(false);
    }
}

void MainWindow::changeSettings() {
    QObject *s = sender();
    if (s == ui->filterCheckBox)
        settings->setBool(Settings::EN_EXTFILTER,
                          ui->filterCheckBox->checkState());
    else if (s == ui->ignoreCheckBox)
        settings->setBool(Settings::EN_IGNOREFILTER,
                          ui->ignoreCheckBox->checkState());
    else if (s == ui->fileCountCheckBox)
        settings->setBool(Settings::EN_MAXFILECOUNT,
                          ui->fileCountCheckBox->checkState());
    else if (s == ui->fileCountSpinBox)
        settings->setInt(Settings::MAXFILECOUNT, ui->fileCountSpinBox->value());
    else if (s == ui->minFreeSpaceCheckBox)
        settings->setBool(Settings::EN_MINFREESPACE,
                          ui->minFreeSpaceCheckBox->checkState());
    else if (s == ui->freeSpaceSpinBox)
        settings->setDouble(
                    Settings::MINFREESPACE, ui->freeSpaceSpinBox->value());
    else if (s == ui->limitCheckBox)
        settings->setBool(Settings::EN_LIMIT,
                          ui->limitCheckBox->checkState());
    else if (s == ui->limitSpinBox)
        settings->setDouble(Settings::LIMIT, ui->limitSpinBox->value());
    else if (s == ui->maxDstCheckBox)
        settings->setBool(Settings::EN_MAXDST,
                          ui->maxDstCheckBox->checkState());
    else if (s == ui->maxDstSpinBox)
        settings->setDouble(Settings::MAXDST, ui->maxDstSpinBox->value());
}

void MainWindow::delFilterExtList() {
    ui->filterListWidget->takeItem(ui->filterListWidget->currentRow());
    ui->delFilterButton->setEnabled(false);
    ui->filterLineEdit->clear();
}

void MainWindow::delSrcDirList() {
    srcDirModel->delDir(selectedIndexSrcList);
    ui->srcLineEdit->clear();
    ui->parentDirSpinBox->setValue(0);
    ui->editSrcButton->setEnabled(false);
    ui->delSrcButton->setEnabled(false);
}

void MainWindow::displayFileQueue(int count) {
    ui->filesLabel->setText(tr("Files in queue: %1").arg(count));
}

void MainWindow::doneCoping() {
    for (int i=0; i < ui->tabWidget->count()-1; i++)
        ui->tabWidget->setTabEnabled(i, true);
    ui->startButton->setEnabled(true);
    ui->startButton->setText(tr("Start"));
    log(tr("Done!"));
    state = READY;
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

void MainWindow::editSrcDirList() {
    if (selectSrcDir()) {
        srcDirModel->updateDir(newSrcDir, additionalPath,
                               selectedIndexSrcList);
        ui->srcLineEdit->setText(newSrcDir);
        ui->editSrcButton->setEnabled(false);
        ui->delSrcButton->setEnabled(false);
    }
}

void MainWindow::editSrcDirListLineEdit(QString path) {
    if (selectedIndexSrcList.row() >= 0 && path.length() > 0) {
        bool rootDisk = false;
        #ifdef Q_OS_WIN
        if (path.length() == 3 and path[1] == ':')
            rootDisk = true;
        #endif
        if (!rootDisk &&
            (path[path.length()-1] == '/' || path[path.length()-1] == '\\'))
            path.remove(path.length() - 1 , 1);
        path = QDir::toNativeSeparators(path);
        srcDirModel->updateDir(path, additionalPath,
                               selectedIndexSrcList);
    }
}

void MainWindow::log(QString msg) {
    ui->logTextEdit->appendPlainText(msg);
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

void MainWindow::outDirChanged() {
    refreshParentDirLevel(ui->parentDirSpinBox->value());
    settings->setString(Settings::OUTDIR, ui->outDirLineEdit->text());
}

void MainWindow::saveSettings() {
    settings->setString(Settings::SRCDIR, srcDirModel->serializeDirs());
    settings->setString(Settings::SRCPATH, srcDirModel->serializePaths());
    settings->setString(Settings::EXTFILTER,
                        listWidgetToSting(ui->filterListWidget));
    settings->setString(Settings::IGNOREFILTER,
                        listWidgetToSting(ui->ignoreListWidget));
    settings->sync();
}

void MainWindow::scanFinished() {
    log(tr("Scanning finnished."));
    ui->startButton->setText(tr("Stop"));
    state = COPYING;
}

void MainWindow::selectFilterExtList() {
    ui->delFilterButton->setEnabled(true);
}

void MainWindow::selectIngoreList() {
    ui->delIgnoreButton->setEnabled(true);
    ui->editIgnoreButton->setEnabled(true);
}

void MainWindow::selectOutputDir() {
    QString outputDir = QFileDialog::getExistingDirectory(this,
                                tr("Select output directory"),
                                ui->outDirLineEdit->text());
    if (!outputDir.isEmpty())
        ui->outDirLineEdit->setText(QDir::toNativeSeparators(outputDir));
}

void MainWindow::selectParentDirLevel(int level) {
    refreshParentDirLevel(level);
    if (!newSrcDir.isEmpty())
        srcDirModel->updateDir(newSrcDir, additionalPath,
                               selectedIndexSrcList);
}

void MainWindow::selectSrcDirList(QModelIndex index) {
    selectedIndexSrcList = index;
    newSrcDir = srcDirModel->data(index).toString();
    ui->srcLineEdit->setText(newSrcDir);
    QString path =
            QDir::fromNativeSeparators(srcDirModel->getAdditionalPath(index));
    printFullOutPath(path);
    ui->parentDirSpinBox->setEnabled(true);
    ui->parentDirSpinBox->setMaximum(getLevelParentDirs(newSrcDir));
    ui->parentDirSpinBox->setValue(path.count('/'));
    ui->editSrcButton->setEnabled(true);
    ui->delSrcButton->setEnabled(true);
}

void MainWindow::selectTab(int tabNum) {
    if (tabNum == 3) {
        ui->frameBottom->setEnabled(false);
        updateDiskFreeSpace();
    } else {
        ui->frameBottom->setEnabled(true);
    }
}

void MainWindow::setEnabledFilter(bool enable) {
    ui->filterGroupBox->setEnabled(enable);
}

void MainWindow::setEnabledIgnore(bool enable) {
    ui->ignoreGroupBox->setEnabled(enable);
}

void MainWindow::showQuestionMsg(QString question) {
    QMessageBox *msg = new QMessageBox(QMessageBox::Information,
                            "Limit is reached", question,
                            QMessageBox::Yes | QMessageBox::No);
    int answer = msg->exec();
    threadCopy->setAnswer(answer);
    threadCopy->questionWait.wakeAll();
}

void MainWindow::updateDiskFreeSpace() {
    QString freeSpace = sizeToStr(diskSize(ui->outDirLineEdit->text()));
    ui->freeSpaceLabel->setText(tr("Free disk space: ") + freeSpace);
}

//signals

void MainWindow::delIgnoreList() {
    ui->ignoreListWidget->takeItem(ui->ignoreListWidget->currentRow());
    ui->delIgnoreButton->setEnabled(false);
    ui->editIgnoreButton->setEnabled(false);
}

