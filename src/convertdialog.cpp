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
#include <QString>
#include <QDropEvent>
#include <QMenu>
#include <QPicture>
#include <QStringList>
#include <QDir>
#include <QFileDialog>
#include <QImageWriter>
#include <QTextCharFormat>
#include <QSettings>
#include <QCompleter>
#include <QDirModel>
#include <QRegExp>
#include <QTranslator>
#include <QLocale>
#include <QMap>
#include <QIcon>
#include <QDesktopServices>
#include <QUrl>

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
    delete statusList;
    delete appTranslator;
    if(net) delete net;
}

void ConvertDialog::createConnections() {
    connect(addFilepushButton, SIGNAL(clicked()), this, SLOT(addFile()));
    connect(addDirpushButton, SIGNAL(clicked()), this, SLOT(addDir()));
    connect(actionAdd_File, SIGNAL(triggered()), this, SLOT(addFile()));
    connect(actionAdd_Dir, SIGNAL(triggered()), this, SLOT(addDir()));

    connect(RemovepushButton, SIGNAL(clicked()), this,
            SLOT(removeSelectedFromList()));

    connect(RemoveAllpushButton, SIGNAL(clicked()), this, SLOT(removeAll()));

    connect(browseDestButton, SIGNAL(clicked()), this,
            SLOT(browseDestination()));

    connect(convertButton, SIGNAL(clicked()), this, SLOT(convert()));
    connect(convertSelectedButton, SIGNAL(clicked()), this,
            SLOT(convertSelected()));

    connect(rotateCheckBox,SIGNAL(stateChanged (int)), SLOT(verify(int)));
    connect(filesTreeView, SIGNAL(customContextMenuRequested (QPoint)),
            SLOT(showMenu(QPoint)));

    connect(filesTreeView, SIGNAL(itemDoubleClicked ( QTreeWidgetItem *, int)),
            SLOT(showPreview(QTreeWidgetItem *, int)));

    connect(quitButton, SIGNAL(clicked()), SLOT(closeOrCancel()));
    connect(actionExit, SIGNAL(triggered()), SLOT(close()));
    connect(actionAbout_Qt, SIGNAL(triggered()),qApp, SLOT(aboutQt()));
    connect(actionAbout_Sir, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionOptions, SIGNAL(triggered()), this, SLOT(setOptions()));
    connect(actionCheckforUpdates, SIGNAL(triggered()), this,
            SLOT(checkUpdates()));
    connect(filesTreeView, SIGNAL(changed()), SLOT(updateTree()));

    connect(actionSendInstall, SIGNAL(triggered()), SLOT(sendInstall()));
    connect(actionRemoveAll, SIGNAL(triggered()), SLOT(removeAll()));
    
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
        settings.beginGroup("MainWindow");
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

    convertThreads.clear();
    
    for(int i = 0; i < numThreads; i++) {
        convertThreads.append(new ConvertThread(this, i));
    }
    
    for (int i = 0; i < numThreads; i++) {
        connect(convertThreads[i],
                SIGNAL(overwriteQuestion(const QString &, int)),this,
                SLOT(queryOverwrite(const QString &, int)),
                Qt::QueuedConnection);

        connect(convertThreads[i],
                SIGNAL(imageStatus(const QStringList &, const QString &, int)),
                this,
                SLOT(setImageStatus(const QStringList &, const QString &, int)),
                Qt::QueuedConnection);

        connect(convertThreads[i], SIGNAL(getNextImage(int, bool)),this,
                SLOT(giveNextImage(int, bool)), Qt::QueuedConnection);
    }
    
}

void ConvertDialog::init() {

    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList list;
    
    foreach(QByteArray format, imageFormats)
    {
        list.append(*new QString(format));
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
    
    convertProgressBar->setValue(0);
    filesTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    createConnections();
	
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

void ConvertDialog::giveNextImage(int threadNum, bool onlySelected) {

    QTreeWidgetItem *item = NULL;
    
    if(convertedImages < numImages) {

        if(onlySelected) {
            item = selectedItems[convertedImages];
        }
        else {
            item = filesTreeView->topLevelItem(convertedImages);
        }

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

void ConvertDialog::convert() {

    bool hasWidth = false;
    bool hasHeight = false;
    QMatrix m;
    
    int w = 0;
    int h = 0;
    QString imagePath;
    
    QImage destImg;
    
    QDir destFolder(destFileEdit->text());
    
    QString destPrefix = destPrefixEdit->text();
    
    if (!widthLineEdit->text().isEmpty()) {
        w = widthLineEdit->text().toInt();
        hasWidth = true;
    }
    
    if (!heightLineEdit->text().isEmpty()) {
        h = heightLineEdit->text().toInt();
        hasHeight = true;
    }
    
    if (!destFolder.exists()) {
    
        switch ( QMessageBox::question(
                 this,
                 tr("Create Folder? -- SIR"),
                 tr("The folder %1 do not exists."
                    "Do you want to create it?").arg(destFolder.absolutePath()),
                    tr("&Yes"), tr("&No") , 0, 1 ) ) {
        
            case 0:
            if (!destFolder.mkdir(destFolder.absolutePath())) {
                QMessageBox::warning(
                        this, "SIR",
                        tr("Unable to create target folder.")
                        );
                return;
            }
            break;
            
            case 1:
            return;
        }        
    }
    
    QTreeWidgetItem *item;
    
    convertProgressBar->setRange(0,filesTreeView->topLevelItemCount());
    convertProgressBar->setValue(0);
    numImages = filesTreeView->topLevelItemCount();
    convertedImages = 0;
    int nt = numThreads;
    
    if (numImages > 0) {
    
        if(numImages < nt) {
            nt = numImages;
        }
     
		quitButton->setText(tr("Cancel"));
		converting = true;
		this->setCursor(Qt::WaitCursor);    
		convertButton->setEnabled(false);
	 
        //Start the worker threads
        setupThreads(nt);
        
        //Gives a image to each thread convert
        for(int i = 0; i < nt; i++) {
            convertThreads[i]->setDesiredSize( w, h, hasWidth, hasHeight,
                                               maintainCheckBox->isChecked() );
            QString desiredFormat = targetFormatComboBox->currentText().toLower();
            convertThreads[i]->setDesiredFormat( desiredFormat );

            convertThreads[i]->setDesiredRotation(rotateCheckBox->isChecked(),
                                                  rotateLineEdit->text().toDouble());

            convertThreads[i]->setQuality( qualitySpinBox->value() );
            convertThreads[i]->setDestPrefix( destPrefix );
            convertThreads[i]->setDestFolder( destFolder );
            convertThreads[i]->setOverwriteAll( false );
            convertThreads[i]->setAcceptWork( true );
            item = filesTreeView->topLevelItem(convertedImages);
            convertThreads[i]->convertImage(item->text(0), item->text(1),
                                            item->text(2));
            convertedImages++;
        }
        
    }
    else {
    
        QMessageBox::warning(
            this, "SIR",
            tr("Please add at least one image file." ));
            
        convertButton->setEnabled(TRUE);
        
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

void ConvertDialog::convertSelected() {

    bool hasWidth = false;
    bool hasHeight = false;
    QMatrix m;
    
    int w = 0;
    int h = 0;
    QString imagePath;
    
    QImage destImg;
    
    QDir destFolder(destFileEdit->text());
    
    QString destPrefix = destPrefixEdit->text();
    
    if (!widthLineEdit->text().isEmpty()) {
        w = widthLineEdit->text().toInt();
        hasWidth = true;
    }
    
    if (!heightLineEdit->text().isEmpty()) {
        h = heightLineEdit->text().toInt();
        hasHeight = true;
    }
    
    if (!destFolder.exists()) {
    
        switch ( QMessageBox::question(
                     this,
                     tr("Create Folder? -- SIR"),
                     tr("The folder %1 do not exists."
                     "Do you want to create it?").arg(destFolder.absolutePath()),
                     tr("&Yes"), tr("&No") , 0, 1 ) ) {
        
            case 0:
            if (!destFolder.mkdir(destFolder.absolutePath())) {
                QMessageBox::warning(
                    this, "SIR",
                    tr("Unable to create target folder.") );
                return;
            }
            break;
            case 1:
            return;
        }
        
    }
    
	QTreeWidgetItem * item;
    
    selectedItems = filesTreeView->selectedItems();
    
    numImages = selectedItems.count();
    convertedImages = 0;
    int nt = numThreads;
    
    if (!selectedItems.isEmpty()) {
	
		quitButton->setText(tr("Cancel"));
		converting = true;
		this->setCursor(Qt::WaitCursor);
    
        if(numImages < nt) {
            nt = numImages;
        }
        
        //Start the worker threads
        setupThreads(nt);
        
        convertProgressBar->setRange(0,selectedItems.count());
        convertProgressBar->setValue(0);
        
        convertSelectedButton->setEnabled(false);
        convertButton->setEnabled(false);
        
        
        //Gives a image to each thread convert
        for(int i = 0; i < nt; i++) {
            convertThreads[i]->setDesiredSize( w, h, hasWidth, hasHeight,
                                               maintainCheckBox->isChecked() );
            QString desiredFormat = targetFormatComboBox->currentText().toLower();
            convertThreads[i]->setDesiredFormat( desiredFormat );
            convertThreads[i]->setDesiredRotation( rotateCheckBox->isChecked(),
                                                   rotateLineEdit->text().toDouble() );
            convertThreads[i]->setQuality( qualitySpinBox->value() );
            convertThreads[i]->setDestPrefix( destPrefix );
            convertThreads[i]->setDestFolder( destFolder );
            convertThreads[i]->setOverwriteAll( false );
            convertThreads[i]->setAcceptWork( true );
            item = selectedItems[convertedImages];
            convertThreads[i]->convertImage(item->text(0), item->text(1),
                                            item->text(2),  true);
            convertedImages++;
        }
        
    }
    else {
    
        QMessageBox::warning(
            this, "SIR",
            tr("Please select at least one image file." ));
            
        convertButton->setEnabled(TRUE);
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
    
    QString imagePath;
    int index;
    QStringList *list;
    list = makeList();
    
    if (!item->text(2).endsWith("/")) {
        imagePath = item->text(2) + QDir::separator() +item->text(0) + ".";
        imagePath += item->text(1);
    }
    else {
        imagePath = item->text(2) +item->text(0) + "." +item->text(1);
    }
    
    imagePath = QDir::fromNativeSeparators(imagePath);
    
    QRegExp exp(imagePath);
    index = list->indexOf(exp,0);
    previewForm = new PreviewDialog(this, list, index);
    previewForm->show();
    
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

    QAction *convert, *remove;
    QTreeWidgetItem *item = filesTreeView->itemAt(point);
                            
    if (item) {
        convert = new QAction(tr("Convert Selected"), this);
        convert->setStatusTip(tr("Convert the selected images"));
        connect(convert, SIGNAL(triggered()), this, SLOT(convertSelected()));
        remove = new QAction(tr("Remove Selected"), this);
        remove->setStatusTip(tr("Remove the selected images"));

        connect(remove, SIGNAL(triggered()), this,
                SLOT(removeSelectedFromList()));

        QMenu contextMenu(this);
        contextMenu.addAction(convert);
        contextMenu.addAction(remove);
        contextMenu.exec(QCursor::pos());
    }
}

void ConvertDialog::about() {

    AboutDialog *about = new AboutDialog();
    about->setVersion( QString("Version ")+VERSION );
    //about->retranslateUi(about);
    about->exec();
    delete about;
    
}

void ConvertDialog::setOptions() {

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
    destFileEdit->setText(settings.value("targetFolder",
                                         QDir::homePath()).toString());

    targetFormatComboBox->setCurrentIndex(settings.value("targetFormat",
                                                         0).toInt());

    widthLineEdit->setText(settings.value("width", "800").toString());
    heightLineEdit->setText(settings.value("height", "600").toString());
    destPrefixEdit->setText(settings.value("targetPrefix", "web").toString());
    qualitySpinBox->setValue(settings.value("quality", 100).toInt());
    numThreads = settings.value("cores", 1).toInt();
    
    QString selectedTranslationFile = ":/translations/";
    selectedTranslationFile += settings.value("languageFileName",
                                              defaultLanguage).toString();

    appTranslator->load(selectedTranslationFile);
    
    alreadSent = settings.value("alreadSent",false).toBool();

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
            if(fileFilters.contains(ext)) {
                fileFilters.remove(ext);
            }
        }
    }

    retranslateStrings();
    settings.endGroup();
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

void ConvertDialog::queryOverwrite(const QString& targetFile, int tid) {

    ConvertThread::shared->overwriteMutex.lock();

    if (ConvertThread::shared->noOverwriteAll) {
        ConvertThread::shared->overwriteMutex.unlock();
        convertThreads[tid]->confirmOverwrite(3);
    }
    else if(!ConvertThread::shared->overwriteAll) {
    
        int result = MessageBox::question(
                         this,
                         tr("Overwrite File? -- SIR"),
                         tr("A file called %1 already exists."
                            "Do you want to overwrite it?").arg( targetFile) );
                         
                         
        ConvertThread::shared->overwriteResult = result;
        
        if (result == MessageBox::YesToAll)
            ConvertThread::shared->overwriteAll = true;
        else if (result == MessageBox::NoToAll)
            ConvertThread::shared->noOverwriteAll = true;
        else if (result == MessageBox::Cancel)
            ConvertThread::shared->abort = true;
        
        ConvertThread::shared->overwriteMutex.unlock();
        convertThreads[tid]->confirmOverwrite(result);
    }
    else {
        ConvertThread::shared->overwriteMutex.unlock();
        convertThreads[tid]->confirmOverwrite(2);
    }
    
}

void ConvertDialog::retranslateStrings() {

    QList<QString> itemList;
    QString fileName;
    
    retranslateUi(this);
    
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

