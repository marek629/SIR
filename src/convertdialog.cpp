/*
 * This file is part of SIR, an open-source cross-platform Image tool
 * 2007  Rafael Sachetto
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
 * Program URL: http://sir.projet-libre.org/
 *
 */

#include "convertdialog.h"
#include "previewdialog.h"
#include "myqtreewidget.h"
#include "aboutdialog.h"
#include "optionsdialog.h"
#include "defines.h"
#include "rawutils.h"
#include "networkutils.h"
#include "messagebox.h"
#include "metadatadialog.h"
#include <QString>
#include <QDropEvent>
#include <QMenu>
#include <QPicture>
#include <QStringList>
#include <QDir>
#include <QFileDialog>
#include <QImageWriter>
#include <QTextCharFormat>
#include <QCompleter>
#include <QDirModel>
#include <QTranslator>
#include <QLocale>
#include <QMap>
#include <QIcon>
#include <QDesktopServices>
#include <QUrl>
#include <QWindowStateChangeEvent>

#include <QDebug>

ConvertDialog::ConvertDialog(QWidget *parent, QString args):QMainWindow(parent) {
    setupUi(this);
    numThreads = 1;
    this->args = args;
    lastDir = "";
    appTranslator = new QTranslator(this);
    qApp->installTranslator(appTranslator);
    statusList = new QMap<QString,int>();
    net = NULL;
    init();
}

ConvertDialog::~ConvertDialog() {
    writeWindowProperties();
    delete statusList;
    delete appTranslator;
    if(net)
        delete net;
    delete convertAction;
    delete removeAction;
    delete previewAction;
    delete metadataAction;
    delete ConvertThread::shared;
}

void ConvertDialog::createConnections() {
    // tree view's list menagement buttons & actions
    connect(addFilepushButton, SIGNAL(clicked()), this, SLOT(addFile()));
    connect(addDirpushButton, SIGNAL(clicked()), this, SLOT(addDir()));
    connect(RemovepushButton, SIGNAL(clicked()), this,
            SLOT(removeSelectedFromList()));
    connect(RemoveAllpushButton, SIGNAL(clicked()), this, SLOT(removeAll()));

    connect(actionAdd_File, SIGNAL(triggered()), this, SLOT(addFile()));
    connect(actionAdd_Dir, SIGNAL(triggered()), this, SLOT(addDir()));
    connect(actionRemoveAll, SIGNAL(triggered()), SLOT(removeAll()));

    // menu actions
    connect(actionExit, SIGNAL(triggered()), SLOT(close()));
    connect(actionAbout_Qt, SIGNAL(triggered()),qApp, SLOT(aboutQt()));
    connect(actionAbout_Sir, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionOptions, SIGNAL(triggered()), this, SLOT(setOptions()));
    connect(actionCheckforUpdates, SIGNAL(triggered()), SLOT(checkUpdates()));
    connect(actionSendInstall, SIGNAL(triggered()), SLOT(sendInstall()));

    // tree view events
    connect(filesTreeView, SIGNAL(customContextMenuRequested (QPoint)),
            SLOT(showMenu(QPoint)));
    connect(filesTreeView, SIGNAL(itemDoubleClicked ( QTreeWidgetItem *, int)),
            SLOT(showPreview(QTreeWidgetItem *, int)));
    connect(filesTreeView, SIGNAL(changed()), SLOT(updateTree()));

    // browse button
    connect(browseDestButton, SIGNAL(clicked()), SLOT(browseDestination()));

    // convert... & stop/exit buttons
    connect(convertButton, SIGNAL(clicked()), this, SLOT(convertAll()));
    connect(convertSelectedButton, SIGNAL(clicked()), SLOT(convertSelected()));
    connect(quitButton, SIGNAL(clicked()), SLOT(closeOrCancel()));

    // size tab
    connect(rotateCheckBox,SIGNAL(stateChanged (int)), SLOT(verify(int)));
    connect(sizeUnitComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setSizeUnit(int)));
    connectSizeLinesEdit();
}

void ConvertDialog::createActions()
{
    removeAction = new QAction(tr("Remove Selected"), this);
    removeAction->setStatusTip(tr("Remove selected images"));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeSelectedFromList()));

    convertAction = new QAction(tr("Convert Selected"), this);
    convertAction->setStatusTip(tr("Convert selected images"));
    connect(convertAction, SIGNAL(triggered()), this, SLOT(convertSelected()));

    previewAction = new QAction(tr("Show Image"), this);
    previewAction->setStatusTip(tr("Show preview selected image"));
    connect(previewAction, SIGNAL(triggered()), this, SLOT(previewAct()));

    metadataAction = new QAction(tr("Show Metadata"), this);
    metadataAction->setStatusTip(tr("Show metadata of selected image"));
    connect(metadataAction, SIGNAL(triggered()), this, SLOT(showMetadata()));
}

void ConvertDialog::checkUpdates() {
    net = new NetworkUtils();
    connect(net, SIGNAL(checkDone(QString*, bool)),
            SLOT(showUpdateResult(QString*, bool)));
    net->checkUpdates();
}

void ConvertDialog::sendInstall() {

    if(alreadSent) {
        QMessageBox::information(this, tr("Let us know!"),
                                 tr("You alread sent information about your " \
                                    "SIR installation. Thank you very much!")
                                 );

    }
    else {
        QMessageBox msgBox(QMessageBox::Question,
                           tr("Let us know!"),
                           tr("IMPORTANT: this action will not send any " \
                              "private data about you or your computer to SIR " \
                              "developers! It will only update a counter of " \
                              "unique installations of SIR.")
                           );

        QPushButton *connectButton = msgBox.addButton(tr("Go for it!"),
                                                      QMessageBox::ActionRole);

        QPushButton *abortButton = msgBox.addButton(tr("No thanks!"),
                                                    QMessageBox::ActionRole);

        msgBox.exec();

        if (msgBox.clickedButton() == connectButton) {
            net = new NetworkUtils();
            connect(net, SIGNAL(checkDone(QString*, bool)),
                    SLOT(showSendInstallResult(QString*, bool)));
            net->sendInstalltoSite();

        }
        else if(msgBox.clickedButton() == abortButton) {
            msgBox.close();
        }
    }


}

void ConvertDialog::showSendInstallResult(QString *result, bool error) {

    if(error) {
        QMessageBox::warning(this, tr("Let us know!"),
                             tr("There was an error while trying to " \
                                "connect with SIR website! " \
                                "Check your internet connection and " \
                                "try again later!"));
    }

    else {
        QMessageBox::information(this, tr("Let us know!"),
                                 tr("Thank you for let us know that you are "\
                                    "using SIR! You are the user number %1 of "\
                                    "this month!").arg(*result));

        QSettings settings("SIR");
        settings.beginGroup("Settings");
        settings.setValue("alreadSent", true);
        settings.endGroup();

    }

}

void ConvertDialog::showUpdateResult(QString *result, bool error) {

    if(error) {
        QMessageBox::warning(this, tr("SIR Updates"),
                             tr("There was an error while trying to " \
                                "connect with SIR update website! " \
                                "Check your internet connection and " \
                                "try again later!"));
    }

    else if(*result == "no_new_version" ) {
        QMessageBox::information(this, tr("SIR Updates"),
                                 tr("You have the lastest version of SIR!"));
    }
    else if(*result == "new_version" ) {

        QMessageBox msgBox(QMessageBox::Information,
                           tr("SIR Updates"),
                           tr("There is a new version of SIR available for "\
                              "download!")
                           );

        QPushButton *connectButton = msgBox.addButton(tr("Go to SIR website!"),
                                                      QMessageBox::ActionRole);

        QPushButton *abortButton = msgBox.addButton(tr("I will check later!"),
                                                    QMessageBox::ActionRole);

        msgBox.exec();

        if (msgBox.clickedButton() == connectButton) {
            // Open sir website in the default browser
            QUrl sirUrl("http://sir.projet-libre.org/static.php?page=download");
            QDesktopServices::openUrl(sirUrl);
        }
        else if(msgBox.clickedButton() == abortButton) {
            msgBox.close();
        }
    }
}

void ConvertDialog::setupThreads(int numThreads) {
    // clearing list of threads
    while (!convertThreads.isEmpty())
        delete convertThreads.takeFirst();

    for(int i = 0; i < numThreads; i++) {
        convertThreads.append(new ConvertThread(this, i));
    }

    for (int i = 0; i < numThreads; i++) {
        connect(convertThreads[i],
                SIGNAL(question(const QString &, int, const QString&)),this,
                SLOT(query(const QString &, int, const QString&)),
                Qt::QueuedConnection);

        connect(convertThreads[i],
                SIGNAL(imageStatus(const QStringList &, const QString &, int)),
                this,
                SLOT(setImageStatus(const QStringList &, const QString &, int)),
                Qt::QueuedConnection);

        connect(convertThreads[i], SIGNAL(getNextImage(int)),this,
                SLOT(giveNextImage(int)), Qt::QueuedConnection);
    }
}

void ConvertDialog::init() {

    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList list;

    foreach(QByteArray format, imageFormats)
    {
        list.append(QString(format));
    }

    targetFormatComboBox->insertItems(0,list);
    fileFilters = "*.";
    fileFilters.append(list.join(" *.").toLower());
    fileFilters.append(" *.jpg");
    fileFilters.append(" *.JPG");
    fileFilters.append(" *.JPEG");
    fileFilters.append(" *.Jpg");
    fileFilters.append(" *.Jpeg");

    RawUtils::createRawFilesList(rawFormats);
    readSettings();

    if (!args.isEmpty()) {
        argsList = args.split("**");
        initList();
    }

    QCompleter *completer = new QCompleter(this);
    QDirModel *dir = new QDirModel(completer);
    dir->setFilter(QDir::AllDirs);
    completer->setModel(dir);
    destFileEdit->setCompleter(completer);

    QStringList wordList;
    wordList << "web" << "thumb" << "thumbnail" << "resized" << "new";
    QCompleter *completer2 = new QCompleter(wordList, this);
    completer2->setCaseSensitivity(Qt::CaseInsensitive);
    destPrefixEdit->setCompleter(completer2);
    destSuffixEdit->setCompleter(completer2);

    convertProgressBar->setValue(0);
    createConnections();
    createActions();

    converting = false;
}

void ConvertDialog::browseDestination() {

    QString initialName = destFileEdit->text();

    if (initialName.isEmpty())
        initialName = QDir::homePath();

    QString fileName = QFileDialog::getExistingDirectory(
                           this,
                           tr("Choose a directory"),
                           initialName,
                           QFileDialog::ShowDirsOnly );

    fileName = QDir::convertSeparators(fileName);

    if (!fileName.isEmpty()) {
        destFileEdit->setText(fileName);
    }
}

void ConvertDialog::giveNextImage(int threadNum) {

    QTreeWidgetItem *item = NULL;

    if(convertedImages < numImages) {
        item = itemsToConvert[convertedImages];
        convertThreads[threadNum]->convertImage(item->text(0), item->text(1),
                                                item->text(2));
        convertThreads[threadNum]->confirmImage();
        convertedImages++;
    }
    else {
        convertThreads[threadNum]->setAcceptWork(false);
        convertThreads[threadNum]->confirmImage();
    }
}

void ConvertDialog::addDir() {

    if(lastDir == "") {
        lastDir = QDir::homePath();
    }

    QTreeWidgetItem *item;

    QString fileName = QFileDialog::getExistingDirectory(
                       this,
                       tr("Choose a directory"),
                       lastDir,
                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    fileName = QDir::convertSeparators(fileName);

    lastDir = fileName;

    if (!fileName.isEmpty()) {
        QDir sourceFolder(fileName,fileFilters);
        sourceFolder.setFilter( QDir::Files | QDir::NoSymLinks);

        QList<QFileInfo> list = sourceFolder.entryInfoList();
        QListIterator<QFileInfo> it(list);
        QFileInfo fi;

        while ( it.hasNext() ) {
            fi = it.next();
            QList<QString> itemList;
            itemList.append(fi.completeBaseName());
            itemList.append(fi.suffix());
            itemList.append(fi.path());
            itemList.append(tr("Not converted yet"));
            item = new QTreeWidgetItem(itemList);
            filesTreeView->addTopLevelItem(item);
            statusList->insert(fi.absoluteFilePath(), NOTCONVERTED);
        }

        convertButton->setEnabled(TRUE);
        convertSelectedButton->setEnabled(TRUE);
    }

    filesTreeView->resizeColumnToContents (0);
    filesTreeView->resizeColumnToContents (1);
    filesTreeView->resizeColumnToContents (2);
    filesTreeView->resizeColumnToContents (3);
}

void ConvertDialog::removeAll() {

    if (filesTreeView->topLevelItemCount() > 0) {
        filesTreeView->clear();
    }

    convertButton->setEnabled(FALSE);
    convertSelectedButton->setEnabled(FALSE);
    convertProgressBar->reset();
    statusList->clear();

}

void ConvertDialog::removeSelectedFromList() {

    QTreeWidgetItem *item;
    QString fileName;

    for (int i = 0; i < filesTreeView->topLevelItemCount(); i++)
    {

        if ((filesTreeView->topLevelItem(i))->isSelected()) {
            item = filesTreeView->takeTopLevelItem(i);

            fileName = item->text(2) + QDir::separator() +item->text(0) + ".";
            fileName += item->text(1);

            statusList->remove(fileName);
        }

    }

    if (filesTreeView->topLevelItemCount() == 0) {
        convertButton->setEnabled(FALSE);
        convertSelectedButton->setEnabled(FALSE);
    }
}

void ConvertDialog::addFile() {

    if(lastDir == "") {
        lastDir = QDir::homePath();
    }

    QString fileName;
    QString aux = tr("Images") + "(" + fileFilters + ")";

    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            tr("Select one or more files to open"),
                            lastDir,
                            aux
                        );


    QStringList::Iterator it = files.begin();
    QTreeWidgetItem *item;

    while ( it != files.end() ) {
        QList<QString> itemList;
        fileName = QDir::convertSeparators(*it);
        itemList.append(QFileInfo(fileName).completeBaseName());
        itemList.append(QFileInfo(fileName).suffix());
        itemList.append(QFileInfo(fileName).path());
        itemList.append(tr("Not converted yet"));
        statusList->insert(QFileInfo(fileName).absoluteFilePath(),
                           NOTCONVERTED);

        item = new QTreeWidgetItem(itemList);
        filesTreeView->addTopLevelItem(item);
        ++it;
        lastDir = QFileInfo(fileName).path();
    }

    convertButton->setEnabled(TRUE);
    convertSelectedButton->setEnabled(TRUE);
    filesTreeView->resizeColumnToContents (0);
    filesTreeView->resizeColumnToContents (1);
    filesTreeView->resizeColumnToContents (2);
    filesTreeView->resizeColumnToContents (3);
}

void ConvertDialog::convertAll() {
    itemsToConvert.clear();
    for (int i=0; i<filesTreeView->topLevelItemCount(); i++)
        itemsToConvert.append( filesTreeView->topLevelItem(i) );
    if (itemsToConvert.isEmpty()) {
        QMessageBox::warning(this, "SIR",
                             tr("Please add at least one image file." ));
        return;
    }
    convert();
}

void ConvertDialog::convertSelected() {
    itemsToConvert = filesTreeView->selectedItems();
    if (itemsToConvert.isEmpty()) {
        QMessageBox::warning( this, "SIR",
                              tr("Please select at least one image file." ));
        return;
    }
    convert();
}

void ConvertDialog::convert() {
    resetAnswers();
    bool hasWidth = false;
    bool hasHeight = false;

    int w = 0;
    int h = 0;

    QDir destFolder(destFileEdit->text());

    if (!widthLineEdit->text().isEmpty()) {
        w = widthLineEdit->text().toInt();
        hasWidth = true;
    }
    if (!heightLineEdit->text().isEmpty()) {
        h = heightLineEdit->text().toInt();
        hasHeight = true;
    }

    if (!destFolder.exists()) {
        switch ( QMessageBox::question(this, tr("Create Folder? -- SIR"),
                                       tr("The folder %1 do not exists."
                                          "Do you want to create it?").arg(
                                           destFolder.absolutePath() ),
                                       tr("&Yes"), tr("&No") , 0, 1 ) ) {

            case 0:
                if (!destFolder.mkdir(destFolder.absolutePath())) {
                    QMessageBox::warning(this, "SIR",
                                         tr("Unable to create target folder.") );
                    return;
                }
            break;
            case 1:
            return;
        }
    }

    QTreeWidgetItem * item;

    numImages = itemsToConvert.count();
    convertedImages = 0;
    int nt = numThreads;

    quitButton->setText(tr("Cancel"));
    converting = true;
    this->setCursor(Qt::WaitCursor);

    if(numImages < nt) {
        nt = numImages;
    }

    //Start the worker threads
    setupThreads(nt);

    convertProgressBar->setRange(0,itemsToConvert.count());
    convertProgressBar->setValue(0);

    convertSelectedButton->setEnabled(false);
    convertButton->setEnabled(false);

    if (sizeUnitComboBox->currentIndex() == 2) {
        int multiplier = 1024;
        if (fileSizeComboBox->currentIndex() == 1)
            multiplier *= 1024;
        ConvertThread::setDesiredSize( fileSizeSpinBox->value() * multiplier );
    }
    else
        ConvertThread::setDesiredSize( w, h, (sizeUnitComboBox->currentIndex() == 1),
                                       hasWidth, hasHeight,
                                       maintainCheckBox->isChecked() );
    QString desiredFormat = targetFormatComboBox->currentText().toLower();
    ConvertThread::setDesiredFormat( desiredFormat );
    ConvertThread::setDesiredRotation( rotateCheckBox->isChecked(),
                                           rotateLineEdit->text().toDouble() );
    ConvertThread::setQuality(qualitySpinBox->value());
    ConvertThread::setDestPrefix(destPrefixEdit->text());
    ConvertThread::setDestSuffix(destSuffixEdit->text());
    ConvertThread::setDestFolder(destFolder);
    ConvertThread::setOverwriteAll(false);

    //Gives a image to each thread convert
    for(int i = 0; i < nt; i++) {
        convertThreads[i]->setAcceptWork( true );
        item = itemsToConvert[convertedImages];
        convertThreads[i]->convertImage(item->text(0), item->text(1),
                                        item->text(2));
        convertedImages++;
    }
}

void ConvertDialog::verify() {

    if (rotateCheckBox->isChecked()) {
        rotateLineEdit->setEnabled(TRUE);
    }
    else {
        rotateLineEdit->setEnabled(FALSE);
    }
}

void ConvertDialog::showPreview(QTreeWidgetItem *item, int col) {

    Q_UNUSED(col);

    QString imagePath = makeImagePath(item);
    QStringList *list = makeList();
    int index = list->indexOf(imagePath);

    previewForm = new PreviewDialog(this, list, index);
    previewForm->show();
}

void ConvertDialog::showMetadata() {
    QString imagePath = makeImagePath(treeMenuItem);
    QStringList *list = makeList();
    int index = list->indexOf(imagePath);

    metadataForm = new MetadataDialog(this, list, index);
    metadataForm->show();
}

void ConvertDialog::initList() {

    QStringList::Iterator it2 = argsList.begin();
    QString fileName;
    QTreeWidgetItem *item;

    for ( ; it2 != argsList.end(); ++it2 ) {
        fileName = *it2;
        fileName = QDir::convertSeparators(fileName);

        if (!fileName.isEmpty() && QFileInfo(fileName).exists()) {
            //Directory
            if (QFileInfo(fileName).isDir()) {
                QDir sourceFolder(fileName,fileFilters);
                sourceFolder.setFilter( QDir::Files | QDir::NoSymLinks);

                QList<QFileInfo> list = sourceFolder.entryInfoList();
                QListIterator<QFileInfo> it(list);
                QFileInfo fi;

                while ( it.hasNext() ) {
                    fi = it.next();
                    QList<QString> itemList;
                    itemList.append(fi.completeBaseName());
                    itemList.append(fi.suffix());
                    itemList.append(fi.path());
                    itemList.append(tr("Not converted yet"));
                    item = new QTreeWidgetItem(itemList);
                    filesTreeView->addTopLevelItem(item);
                    statusList->insert(fi.absoluteFilePath(), NOTCONVERTED);
                }
            }
            //File
            else {

                int comp = QString::compare("",QFileInfo(fileName).suffix());

                if((fileFilters.contains(QFileInfo(fileName).suffix()))
                    && (comp !=0)) {

                    QList<QString> itemList;
                    itemList.append(QFileInfo(fileName).completeBaseName());
                    itemList.append(QFileInfo(fileName).suffix());
                    itemList.append(QFileInfo(fileName).path());
                    itemList.append(tr("Not converted yet"));
                    item = new QTreeWidgetItem(itemList);
                    filesTreeView->addTopLevelItem(item);
                    statusList->insert(QFileInfo(fileName).absoluteFilePath(),
                                       NOTCONVERTED);
                }
            }
        }
    }

    if (filesTreeView->topLevelItemCount() > 0) {
        convertButton->setEnabled(TRUE);
        convertSelectedButton->setEnabled(TRUE);
    }

    filesTreeView->resizeColumnToContents (0);
    filesTreeView->resizeColumnToContents (1);
    filesTreeView->resizeColumnToContents (2);
    filesTreeView->resizeColumnToContents (3);
}

void ConvertDialog::verify(int status) {

    if (status == Qt::Checked ) {
        rotateLineEdit->setEnabled(TRUE);
    }
    else {
        rotateLineEdit->setEnabled(FALSE);
    }

}

void ConvertDialog::showMenu(const QPoint & point) {

    treeMenuItem = filesTreeView->itemAt(point);

    if (treeMenuItem) {
        QMenu contextMenu(this);
        contextMenu.addAction(previewAction);
        contextMenu.addAction(metadataAction);
        contextMenu.addSeparator();
        contextMenu.addAction(convertAction);
        contextMenu.addAction(removeAction);
        contextMenu.setDefaultAction(previewAction);
        contextMenu.exec(QCursor::pos());
    }
}

void ConvertDialog::previewAct()
{
    showPreview(treeMenuItem, 0);
}

void ConvertDialog::about() {

    AboutDialog *about = new AboutDialog();
    about->setVersion( QString("Version ")+VERSION );
    //about->retranslateUi(about);
    about->exec();
    delete about;

}

void ConvertDialog::setOptions() {
    writeWindowProperties();
    OptionsDialog *options = new OptionsDialog(this);
    connect( options, SIGNAL( ok() ), SLOT( readSettings() ) );
    options->exec();
    delete options;

}

void ConvertDialog::readSettings() {

    QString locale = QLocale::system().name();

    QString defaultLanguage = "sir_"+locale+".qm";

    QSettings settings("SIR");
    settings.beginGroup("MainWindow");
    if (settings.value("cores",-1).toInt() != -1) {
        //Old format settings from SIR 2.1 - need migration to SIR 2.2 format
        QFile configFile(settings.fileName());
        if (configFile.open(QIODevice::ReadWrite)) {
            QString fileContent = configFile.read(configFile.size());
            fileContent.replace("[MainWindow]","[Settings]");
            configFile.resize(fileContent.size());
            if (configFile.seek(0))
                configFile.write(fileContent.toAscii());
            else
                qWarning("ConvertDialog: settings migration failed");
            configFile.close();
        }
    }
    else {
        this->move( settings.value("possition",this->pos()).toPoint() );
        this->resize( settings.value("size",this->size()).toSize() );
        if (settings.value("maximized",false).toBool())
            this->showMaximized();
    }
    settings.endGroup(); // MainWindow

    //Old format settings from SIR 2.2
    QString testStr = "Errare humanum est.";
    if (settings.value("Settings/width", testStr).toString() != testStr) {
        // migrate settings from SIR 2.2
        bool sent = settings.value("MainWindow/alreadSent",false).toBool();
        settings.remove("MainWindow/alreadSent");
        settings.setValue("Settings/alreadSent",sent);
        sizeWidthString = settings.value("Settings/width", "800").toString();
        settings.remove("Settings/width");
        settings.setValue("Size/widthPx",sizeWidthString);
        sizeHeightString = settings.value("Settings/height", "600").toString();
        settings.remove("Settings/height");
        settings.setValue("Size/heightPx",sizeHeightString);
        bool metadata = settings.value("Settings/metadata",true).toBool();
        settings.remove("Settings/metadata");
        settings.setValue("Metadata/metadata",metadata);
        bool saveMetadata = settings.value("Settings/saveMetadata",true).toBool();
        settings.remove("Settings/saveMetadata");
        settings.setValue("Metadata/saveMetadata",saveMetadata);
        rawEnabled = settings.value("Settings/raw", false).toBool();
        settings.remove("Settings/raw");
        settings.setValue("Raw/raw",rawEnabled);
    }

    settings.beginGroup("Settings");

    destFileEdit->setText(settings.value("targetFolder",
                                         QDir::homePath()).toString());

    targetFormatComboBox->setCurrentIndex(settings.value("targetFormat",
                                                         0).toInt());

    destPrefixEdit->setText(settings.value("targetPrefix", "web").toString());
    destSuffixEdit->setText(settings.value("targetSuffix", "thumb").toString());
    qualitySpinBox->setValue(settings.value("quality", 100).toInt());

    numThreads = settings.value("cores", 0).toInt();
    if (numThreads == 0)
        numThreads = OptionsDialog::detectCoresCount();

    QString selectedTranslationFile = ":/translations/";
    selectedTranslationFile += settings.value("languageFileName",
                                              defaultLanguage).toString();

    appTranslator->load(selectedTranslationFile);

    alreadSent = settings.value("alreadSent",false).toBool();

    retranslateStrings();
    settings.endGroup(); // Settings

    settings.beginGroup("Size");
    int sizeUnitIndex = settings.value("sizeUnit", 0).toInt();
    setSizeUnit(sizeUnitIndex);
    sizeUnitComboBox->setCurrentIndex(sizeUnitIndex);
    fileSizeSpinBox->setValue(settings.value("fileSizeValue", 300.).toDouble());
    fileSizeComboBox->setCurrentIndex(settings.value("fileSizeUnit", 0).toInt());
    if (sizeUnitComboBox->currentIndex() == 1) {
        sizeWidthString = settings.value("widthPx", "800").toString();
        sizeHeightString = settings.value("heightPx", "600").toString();
        widthLineEdit->setText(sizeWidthString);
        widthLineEdit->setText(settings.value("widthPercent", "100").toString());
        heightLineEdit->setText(sizeHeightString);
        heightLineEdit->setText(settings.value("heightPercent", "100").toString());
    }
    else if (sizeUnitComboBox->currentIndex() == 0) {
        sizeWidthString = settings.value("widthPercent", "100").toString();
        sizeHeightString = settings.value("heightPercent", "100").toString();
        widthLineEdit->setText(sizeWidthString);
        widthLineEdit->setText(settings.value("widthPx", "800").toString());
        heightLineEdit->setText(sizeHeightString);
        heightLineEdit->setText(settings.value("heightPx", "600").toString());
    }
    settings.endGroup(); // Size

    settings.beginGroup("Raw");
    rawEnabled = settings.value("raw", false).toBool();
    if(rawEnabled) {
        foreach(QString ext, rawFormats) {
            if(!fileFilters.contains(ext)) {
                fileFilters.append(ext);
            }
        }
    }
    else {
        foreach(QString ext, rawFormats) {
            if(fileFilters.contains(ext) && ext != " *.tif") {
                fileFilters.remove(ext);
            }
        }
    }
    settings.endGroup(); // Raw

    settings.beginGroup("Metadata");
    bool metadataEnabled = settings.value("metadata",true).toBool();
    MetadataUtils::Metadata::setEnabled(metadataEnabled);
    ConvertThread::setMetadataEnabled(metadataEnabled);
    bool saveMetadata = settings.value("saveMetadata",true).toBool();
    MetadataUtils::Metadata::setSave(saveMetadata);
    ConvertThread::setSaveMetadata(saveMetadata);
    if (saveMetadata) {
        ConvertThread::setRealRotate(settings.value("realRotate",false).toBool());
        ConvertThread::setUpdateThumbnail(
                    settings.value("updateThumbnail",true).toBool() );
        ConvertThread::setRotateThumbnail(
                    settings.value("rotateThumbnail",false).toBool() );
    }
    else
        ConvertThread::setRealRotate(true);
    settings.endGroup(); // Metadata

    settings.beginGroup("Exif");
    bool exifOverwrite;

    exifOverwrite = settings.value("artistOverwrite",false).toBool();
    MetadataUtils::Exif::setArtistOverwrite(exifOverwrite);
    if (exifOverwrite)
        MetadataUtils::Exif::setArtistString( MetadataUtils::String(
                    settings.value("artistMap").toMap().keys().first() ) );

    exifOverwrite = settings.value("copyrightOverwrite",false).toBool();
    MetadataUtils::Exif::setCopyrightOverwrite(exifOverwrite);
    if (exifOverwrite)
        MetadataUtils::Exif::setCopyrightString( MetadataUtils::String(
                    settings.value("copyrightMap").toMap().keys().first() ) );

    exifOverwrite = settings.value("userCommentOverwrite",false).toBool();
    MetadataUtils::Exif::setUserCommentOverwrite(exifOverwrite);
    if (exifOverwrite)
        MetadataUtils::Exif::setUserCommentString( MetadataUtils::String(
                    settings.value("userCommentMap").toMap().keys().first() ) );

    settings.endGroup(); // Exif
}

void ConvertDialog::changeEvent(QEvent *e) {

    if (e->type()!=QEvent::WindowStateChange)
        return;

    QWindowStateChangeEvent *event = (QWindowStateChangeEvent*)(e);
    if (event->oldState()==Qt::WindowNoState) {
        windowSize = this->size();
        windowPossition = this->pos();
    }
}

QStringList * ConvertDialog::makeList() {
    int count = filesTreeView->topLevelItemCount();
    QStringList *list = new QStringList();


    if (count > 0) {

        QString imagePath;
        QTreeWidgetItem *item;

        for (int i = 0; i < count; i++) {

            item = filesTreeView->topLevelItem(i);

            if (!item->text(2).endsWith("/")) {
                imagePath = item->text(2) + QDir::separator() +item->text(0);
                imagePath += "." + item->text(1);
            }
            else {
                imagePath = item->text(2) +item->text(0) + "." +item->text(1);
            }

            imagePath = QDir::fromNativeSeparators(imagePath);

            list->append(imagePath);
        }

    }

    return list;
}

QString ConvertDialog::makeImagePath(QTreeWidgetItem *item)
{
    QString imagePath;
    if (!item->text(2).endsWith("/")) {
        imagePath = item->text(2) + QDir::separator() +item->text(0) + ".";
        imagePath += item->text(1);
    }
    else {
        imagePath = item->text(2) +item->text(0) + "." +item->text(1);
    }

    return QDir::fromNativeSeparators(imagePath);
}

void ConvertDialog::updateTree() {

    if (filesTreeView->topLevelItemCount() > 0) {
        convertButton->setEnabled(TRUE);
        convertSelectedButton->setEnabled(TRUE);
    }

    filesTreeView->resizeColumnToContents (0);
    filesTreeView->resizeColumnToContents (1);
    filesTreeView->resizeColumnToContents (2);
    filesTreeView->resizeColumnToContents (3);
}

void ConvertDialog::setImageStatus(const QStringList& imageData,
                                   const QString& status, int statusNum) {


    if(statusNum != CONVERTING) {
        //We don't want to update the status bar if the statusValue is
        //CONVERTING
        convertProgressBar->setValue(convertProgressBar->value()+1);
    }
    int count = filesTreeView->topLevelItemCount();
    QString fileName;

    for (int i = 0; i < count; i++)
    {
        QTreeWidgetItem *item = filesTreeView->topLevelItem(i);
        if(item->text(0) == imageData.at(0) && item->text(1) == imageData.at(1)
            && item->text(2) == imageData.at(2)) {
            item->setText(3, status);
            fileName = item->text(2) + QDir::separator() +item->text(0) + ".";
            fileName += item->text(1);
            statusList->insert(fileName,statusNum);
            break;
        }
    }

    if(convertProgressBar->value() == convertProgressBar->maximum()) {
        updateInterface();
    }
}

void ConvertDialog::query(const QString& targetFile, int tid, const QString& whatToDo) {

    QueryData data = {targetFile,tid};
    QString what = whatToDo.toLower();
    if (what == "overwrite")
        overwriteQueue.enqueue(data);
    else if (what == "enlarge")
        enlargeQueue.enqueue(data);
    else {
        qWarning("ConvertDialog::query(): bad \"whatToDo\" argument");
        convertThreads[tid]->confirmEnlarge(1);
        convertThreads[tid]->confirmOverwrite(1);
        return;
    }
    static bool flag = true;
    static int id = -1;
    if (id == -1)
        id = tid;
    if (tid==id && flag) {
        flag = false;
        while (!enlargeQueue.isEmpty())
            questionEnlarge(enlargeQueue.dequeue());
        while (!overwriteQueue.isEmpty())
            questionOverwrite(overwriteQueue.dequeue());
        id = -1;
        flag = true;
    }
}

void ConvertDialog::questionOverwrite(QueryData data) {

    if (ConvertThread::shared->noOverwriteAll)
        convertThreads[data.tid]->confirmOverwrite(3);
    else if(ConvertThread::shared->abort)
        convertThreads[data.tid]->confirmOverwrite(4);
    else if(!ConvertThread::shared->overwriteAll) {

        int result = MessageBox::question(
                         this,
                         tr("Overwrite File? -- SIR"),
                         tr("A file called %1 already exists."
                            "Do you want to overwrite it?").arg( data.filePath) );

        if (result == MessageBox::YesToAll)
            ConvertThread::shared->overwriteAll = true;
        else if (result == MessageBox::NoToAll)
            ConvertThread::shared->noOverwriteAll = true;
        else if (result == MessageBox::Cancel)
            ConvertThread::shared->abort = true;

        convertThreads[data.tid]->confirmOverwrite(result);
    }
    else
        convertThreads[data.tid]->confirmOverwrite(2);
}

void ConvertDialog::questionEnlarge(QueryData data) {

    if (ConvertThread::shared->noEnlargeAll)
        convertThreads[data.tid]->confirmEnlarge(3);
    else if (ConvertThread::shared->abort)
        convertThreads[data.tid]->confirmEnlarge(4);
    else if (!ConvertThread::shared->enlargeAll) {

        int result = MessageBox::question(
                    this,
                    tr("Enlarge File? - SIR"),
                    tr("A file called %1 is smaller than the requested size. "
                       "Enlargement can cause deterioration of picture quality. "
                       "Do you want enlarge it?").arg(data.filePath));

        if (result == MessageBox::YesToAll)
            ConvertThread::shared->enlargeAll = true;
        else if (result == MessageBox::NoToAll)
            ConvertThread::shared->noEnlargeAll = true;
        else if (result == MessageBox::Cancel)
            ConvertThread::shared->abort = true;

        convertThreads[data.tid]->confirmEnlarge(result);
    }
    else
        convertThreads[data.tid]->confirmEnlarge(2);
}

void ConvertDialog::retranslateStrings() {

    QList<QString> itemList;
    QString fileName;
    int sizeUnitIndex = sizeUnitComboBox->currentIndex();
    int fileSizeIndex = fileSizeComboBox->currentIndex();
    retranslateUi(this);
    // restoring nulled indexes
    sizeUnitComboBox->setCurrentIndex(sizeUnitIndex);
    fileSizeComboBox->setCurrentIndex(fileSizeIndex);

    itemList.append(tr("Name"));
    itemList.append(tr("Ext"));
    itemList.append(tr("Path"));
    itemList.append(tr("Status"));
    filesTreeView->setHeaderLabels(itemList);

    QTreeWidgetItemIterator it(filesTreeView);
    int count;
    count = 0;

    while (*it)
    {
        fileName = (*it)->text(2) + QDir::separator() +(*it)->text(0) + ".";
        fileName += (*it)->text(1);

        switch(statusList->value(fileName)) {
            case CONVERTED:
            (*it)->setText(3,tr("Converted"));
            break;
            case SKIPPED:
            (*it)->setText(3,tr("Skipped"));
            break;
            case FAILED:
            (*it)->setText(3,tr("Failed to convert"));
            break;
            case NOTCONVERTED:
            (*it)->setText(3,tr("Not converted yet"));
            break;
            case CONVERTING:
            (*it)->setText(3,tr("Converting"));
            break;
            case CANCELLED:
            (*it)->setText(3,tr("Cancelled"));
            break;
        }
        count++;
        ++it;
    }
}

void ConvertDialog::closeOrCancel() {

    if (converting) {
        stopConvertThreads();
        updateInterface();
        setCanceled();
    }
    else{
        close();
    }

}

void ConvertDialog::stopConvertThreads() {
        for (int i = 0; i < numThreads; i++) {
                convertThreads[i]->terminate();
        }
}

void ConvertDialog::updateInterface() {
        converting = false;
        convertSelectedButton->setEnabled(true);
    convertButton->setEnabled(true);
    filesTreeView->resizeColumnToContents (0);
    filesTreeView->resizeColumnToContents (1);
    filesTreeView->resizeColumnToContents (2);
    filesTreeView->resizeColumnToContents (3);
        setCursor(Qt::ArrowCursor);
        quitButton->setText(tr("Quit"));
}

void ConvertDialog::setCanceled() {

        int count = filesTreeView->topLevelItemCount();
        QTreeWidgetItem *item = new QTreeWidgetItem();

        QString status = tr("Cancelled");

        QString converted = tr("Converted");

        for(int i = 0; i < count; i++) {
            item = filesTreeView->topLevelItem(i);
            if(item->text(3) != converted){
                item->setText(3, status);
                        }
                }
}

void ConvertDialog::setSizeUnit(int index) {
    if (index < 0)
        return;
    static int lastIndex = index;
    if (index == 2) { // bytes
        geometryWidget->hide();
        fileSizeWidget->show();
    }
    else { // px or %
        fileSizeWidget->hide();
        geometryWidget->show();
        disconnectSizeLinesEdit();
        if (lastIndex != index) {
            QString tmp = sizeWidthString;
            sizeWidthString = widthLineEdit->text();
            widthLineEdit->setText(tmp);
            tmp = sizeHeightString;
            sizeHeightString = heightLineEdit->text();
            heightLineEdit->setText(tmp);
        }
        if (maintainCheckBox->isChecked() && index == 1) // %
            heightLineEdit->setText( widthLineEdit->text() );
        connectSizeLinesEdit();
        lastIndex = index;
    }
}

void ConvertDialog::sizeChanged(const QString &value) {
    // size unit is % and maintainCheckBox is checked
    if (sizeUnitComboBox->currentIndex() == 1 && maintainCheckBox->isChecked()) {
        QString senderName = sender()->objectName();
        if (senderName == "widthLineEdit")
            heightLineEdit->setText(value);
        else if(senderName == "heightLineEdit")
            widthLineEdit->setText(value);
    }
}
