/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2012  Marek Jędryka   <jedryka89@gmail.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Program URL: http://sir.projet-libre.org/
 */

#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QCompleter>
#include <QDirModel>
#include <QTranslator>
#include <QDesktopServices>
#include <QUrl>
#include <QWindowStateChangeEvent>
#include <QDebug>
#include <QLibraryInfo>
#include <cmath>
#include "convertdialog.h"
#include "widgets/treewidget.h"
#include "widgets/aboutdialog.h"
#include "widgets/optionsdialog.h"
#include "widgets/options/generalgroupbox.h"
#include "defines.h"
#include "rawutils.h"
#include "networkutils.h"
#include "widgets/messagebox.h"
#include "metadatautils.h"
#include "selection.h"

/** Default constuctor.
  *
  * Sets up window with saved settings like window state, position
  * and convertion preferences.
  * \param parent Pointer to parent object.
  * \param args String containing aplication argv tables records
  *     separated by ** (double star-sign).
  * \sa init()
  */
ConvertDialog::ConvertDialog(QWidget *parent, QString args) : QMainWindow(parent) {
    setupUi(this);
    this->args = args;
    qtTranslator = new QTranslator(this);
    appTranslator = new QTranslator(this);
    qApp->installTranslator(qtTranslator);
    qApp->installTranslator(appTranslator);
    net = NULL;
    sharedInfo = ConvertThread::sharedInfo();
    init();
}

/** Destructor.\n
  * Writes window state and position, dealocates dynamic allocated memory
  * and clears temporary directory.
  * \sa writeWindowProperties() clearTempDir()
  */
ConvertDialog::~ConvertDialog() {
    writeWindowProperties();
    delete qtTranslator;
    delete appTranslator;
    if(net)
        delete net;
    clearTempDir();
}

/** Connects UI signals to corresponding slots. */
void ConvertDialog::createConnections() {
    // tree view's list menagement buttons
    connect(addFilepushButton, SIGNAL(clicked()),
            filesTreeWidget, SLOT(addFile()));
    connect(addDirpushButton, SIGNAL(clicked()), filesTreeWidget, SLOT(addDir()));
    connect(removePushButton, SIGNAL(clicked()),
            filesTreeWidget, SLOT(removeSelectedFromList()));
    connect(removeAllPushButton, SIGNAL(clicked()),
            filesTreeWidget, SLOT(removeAll()));
    // & actions
    connect(actionAdd_File, SIGNAL(triggered()), filesTreeWidget, SLOT(addFile()));
    connect(actionAdd_Dir, SIGNAL(triggered()), filesTreeWidget, SLOT(addDir()));
    connect(actionSelect, SIGNAL(triggered()), SLOT(showSelectionDialog()));
    connect(actionImport_files, SIGNAL(triggered()), SLOT(showSelectionDialog()));
    connect(actionRemoveAll, SIGNAL(triggered()),
            filesTreeWidget, SLOT(removeAll()));

    // menu actions
    connect(actionExit, SIGNAL(triggered()), SLOT(close()));
    connect(actionAbout_Qt, SIGNAL(triggered()),qApp, SLOT(aboutQt()));
    connect(actionAbout_Sir, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionOptions, SIGNAL(triggered()), this, SLOT(setOptions()));
    connect(actionCheckforUpdates, SIGNAL(triggered()), SLOT(checkUpdates()));
    connect(actionSendInstall, SIGNAL(triggered()), SLOT(sendInstall()));

    // tree view events
    connect(filesTreeWidget, SIGNAL(itemSelectionChanged()),
            detailsBrowser, SLOT(showDetails()));

    // browse button
    connect(browseDestButton, SIGNAL(clicked()), SLOT(browseDestination()));

    // convert... & stop/exit buttons
    connect(convertButton, SIGNAL(clicked()), this, SLOT(convertAll()));
    connect(convertSelectedButton, SIGNAL(clicked()), SLOT(convertSelected()));
    connect(quitButton, SIGNAL(clicked()), SLOT(closeOrCancel()));

    // size tab
    connect(rotateCheckBox,SIGNAL(stateChanged (int)), SLOT(verifyRotate(int)));
    connect(sizeUnitComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setSizeUnit(int)));
    if (maintainCheckBox->isChecked() && sizeUnitComboBox->currentIndex()==1) // %
        connectSizeLinesEdit();

    // quality spin box & slider
    connect(qualitySpinBox, SIGNAL(valueChanged(int)), qualitySlider, SLOT(setValue(int)));
    connect(qualitySlider, SIGNAL(valueChanged(int)), qualitySpinBox, SLOT(setValue(int)));
}

/** Check updates on SIR website.
  * \sa showUpdateResult()
  */
void ConvertDialog::checkUpdates() {
    net = new NetworkUtils();
    connect(net, SIGNAL(checkDone(QString*, bool)),
            SLOT(showUpdateResult(QString*, bool)));
    net->checkUpdates();
}

/** Notify SIR installation on project website if alread didn't notified;
  * otherwise show message box about alread sended.
  * \sa showSendInstallResult()
  */
void ConvertDialog::sendInstall() {

    if(alreadySent) {
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

/** Show message box about notified installation.
  * \sa sendInstall()
  */
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
        alreadySent = true;
        Settings::instance().settings.alreadySent = alreadySent;
    }
}

/** Show result of checkUpdates() in message box.
  * \sa checkUpdates()
  */
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

/** Setups \a numThreads worker threads. */
void ConvertDialog::setupThreads(int numThreads) {
    // clearing list of threads
    while (!convertThreads.isEmpty())
        delete convertThreads.takeFirst();
    // setup threads
    for(int i = 0; i < numThreads; i++) {
        // creating new thread
        convertThreads.append(new ConvertThread(this, i));
        // connecting
        connect(convertThreads[i],
                SIGNAL(question(const QString &, Question)),
                SLOT(query(const QString &, Question)),
                Qt::BlockingQueuedConnection);
        connect(convertThreads[i],
                SIGNAL(imageStatus(const QStringList &, const QString &, int)),
                SLOT(setImageStatus(const QStringList &, const QString &, int)),
                Qt::QueuedConnection);
        connect(convertThreads[i], SIGNAL(getNextImage(int)),
                SLOT(giveNextImage(int)), Qt::BlockingQueuedConnection);
    }
}

/** Creates lists of write and read supported images including raw images,
  * restore saved settings in last session, setups completers for lines edit
  * and creates connections and actions.
  * \sa loadSettings() createConnections()
  */
void ConvertDialog::init() {

    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList list;
    // target format combo box setup
    foreach (QByteArray format, imageFormats)
        list.append(QString(format));
    targetFormatComboBox->insertItems(0,list);
    // read file filters setup
    imageFormats = QImageReader::supportedImageFormats();
    foreach (QByteArray format, imageFormats) {
        QString str(format);
        if (!list.contains(str))
            list.append(str);
    }
    fileFilters = "*.";
    fileFilters.append(list.join(" *.").toLower());
    fileFilters.append(" *.jpg");
    fileFilters.append(" *.JPG");
    fileFilters.append(" *.JPEG");
    fileFilters.append(" *.Jpg");
    fileFilters.append(" *.Jpeg");

    RawUtils::createRawFilesList(rawFormats);
    loadSettings();

    if (!args.isEmpty())
        filesTreeWidget->initList(args.split("**"));

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

    verticalSplitter->setStretchFactor(0,1000);
    horizontalSplitter->setStretchFactor(0,1000);

    convertProgressBar->setValue(0);
    createConnections();

    converting = false;
}

/** Browse destination directory button slot.
  * Choose destination directory in QFileDialog.
  *
  * Path from neighboring line edit is setting as origin directory in QFileDialog.
  * If the path is empty origin directory will be set to home path.
  */
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

/** Gives next image for worker thread legitimazed \a threadNum thread ID. */
void ConvertDialog::giveNextImage(int threadNum) {

    QTreeWidgetItem *item = NULL;

    if(convertedImages < numImages) {
        item = itemsToConvert[convertedImages];
        convertThreads[threadNum]->convertImage(item->text(NameColumn),
                                                item->text(ExtColumn),
                                                item->text(PathColumn));
        convertedImages++;
    }
    else {
        convertThreads[threadNum]->setAcceptWork(false);
    }
}

/** Enables convertion push buttons if \a enable is true; otherwise disables it. */
void ConvertDialog::enableConvertButtons(bool enable) {
    convertButton->setEnabled(enable);
    convertSelectedButton->setEnabled(enable);
}

/** Convert all button slot.
  * Load all items from tree widget into list of images to convert and
  * call #convert() function.
  * \sa convertSelected()
  */
void ConvertDialog::convertAll() {
    itemsToConvert.clear();
    for (int i=0; i<filesTreeWidget->topLevelItemCount(); i++)
        itemsToConvert.append( filesTreeWidget->topLevelItem(i) );
    if (itemsToConvert.isEmpty()) {
        QMessageBox::warning(this, "SIR",
                             tr("Please add at least one image file." ));
        return;
    }
    convert();
}

/** Convert selected button slot.
  * Load selected items from tree widget into list of images to convert and
  * call #convert() function.
  * \sa convertAll()
  */
void ConvertDialog::convertSelected() {
    itemsToConvert = filesTreeWidget->selectedItems();
    if (itemsToConvert.isEmpty()) {
        QMessageBox::warning( this, "SIR",
                              tr("Please select at least one image file." ));
        return;
    }
    convert();
}

/** Reset answers, setups and starts convertion threads.
  * \sa resetAnswers() ConvertThread
  */
void ConvertDialog::convert() {
    resetAnswers();
    bool hasWidth = false;
    bool hasHeight = false;

    int w = 0;
    int h = 0;

    QDir destFolder(destFileEdit->text());

    if (widthDoubleSpinBox->value() != widthDoubleSpinBox->minimum()) {
        w = widthDoubleSpinBox->value();
        hasWidth = true;
    }
    if (heightDoubleSpinBox->value() != heightDoubleSpinBox->minimum()) {
        h = heightDoubleSpinBox->value();
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

    enableConvertButtons(false);

    if (sizeUnitComboBox->currentIndex() == 2) {
        int multiplier = 1024;
        if (fileSizeComboBox->currentIndex() == 1) // MiB
            multiplier *= 1024;
        sharedInfo->setDesiredSize( fileSizeSpinBox->value() * multiplier );
    }
    else
        sharedInfo->setDesiredSize( w, h, (sizeUnitComboBox->currentIndex() == 1),
                                       hasWidth, hasHeight,
                                       maintainCheckBox->isChecked() );
    QString desiredFormat = targetFormatComboBox->currentText().toLower();
    sharedInfo->setDesiredFormat( desiredFormat );
    sharedInfo->setDesiredFlip(flipComboBox->currentIndex());
    sharedInfo->setDesiredRotation( rotateCheckBox->isChecked(),
                                           rotateLineEdit->text().toDouble() );
    sharedInfo->setQuality(qualitySpinBox->value());
    sharedInfo->setDestPrefix(destPrefixEdit->text());
    sharedInfo->setDestSuffix(destSuffixEdit->text());
    sharedInfo->setDestFolder(destFolder);
    sharedInfo->setOverwriteAll(false);

    //Gives a image to each thread convert
    for(int i = 0; i < nt; i++) {
        convertThreads[i]->setAcceptWork( true );
        item = itemsToConvert[convertedImages];
        convertThreads[i]->convertImage(item->text(NameColumn),
                                        item->text(ExtColumn),
                                        item->text(PathColumn));
        convertedImages++;
    }
}

void ConvertDialog::showSelectionDialog() {
    Selection selection(this);
    QAction *action = static_cast<QAction*> (sender());
    if (action == actionSelect)
        qDebug() << "Selected items:" << selection.selectItems();
    else if (actionImport_files)
        qDebug() << "Imported files:" << selection.importFiles();
}

/** Rotate checkbox slot.
  * Disables/enables rotation angle line edit.
  * \param status Status of the checkbox.
  */
void ConvertDialog::verifyRotate(int status) {

    if (status == Qt::Checked ) {
        rotateLineEdit->setEnabled(TRUE);
    }
    else {
        rotateLineEdit->setEnabled(FALSE);
    }

}

/** Shows window containing information about SIR. */
void ConvertDialog::about() {
    AboutDialog *about = new AboutDialog();
    about->setVersion(tr("Version") + " " + VERSION);
    about->exec();
    delete about;
}

/** Writes window state and position and show options dialog. */
void ConvertDialog::setOptions() {
    writeWindowProperties();
    filesTreeWidget->header()->saveSettings();
    OptionsDialog *options = new OptionsDialog(this);
    options->exec();
    delete options;
}

/** Reads settings and sets up window state, position and convertion preferences. */
void ConvertDialog::loadSettings() {
    // load tree widget settings
    filesTreeWidget->header()->loadSettings();

    Settings &s = Settings::instance();
    // main window
    if (                                s.mainWindow.maximized)
        showMaximized();
    move(                               s.mainWindow.possition);
    resize(                             s.mainWindow.size);
    horizontalSplitter->restoreState(   s.mainWindow.horizontalSplitter);
    verticalSplitter->restoreState(     s.mainWindow.verticalSplitter);
    // settings
    destFileEdit->setText(                      s.settings.targetFolder);
    targetFormatComboBox->setCurrentIndex(
                targetFormatComboBox->findText( s.settings.targetFormat));
    destPrefixEdit->setText(                    s.settings.targetPrefix);
    destSuffixEdit->setText(                    s.settings.targetSuffix);
    int quality =                               s.settings.quality;
    qualitySpinBox->setValue(quality);
    qualitySlider->setValue(quality);
    numThreads =                                s.settings.cores;
    if (numThreads == 0)
        numThreads = GeneralGroupBox::detectCoresCount();
    QString selectedTranslationFile =
            QCoreApplication::applicationDirPath() + "/../share/sir/translations/";
    selectedTranslationFile +=                  s.settings.languageFileName;
    QString qtTranslationFile = "qt_" +
            selectedTranslationFile.split('_').at(1).split('.').first();
    qtTranslator->load(qtTranslationFile,
                       QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    appTranslator->load(selectedTranslationFile);
    retranslateStrings();
    alreadySent =                               s.settings.alreadySent;
    dateFormat =                                s.settings.dateDisplayFormat;
    timeFormat =                                s.settings.timeDisplayFormat;
    dateTimeFormat = dateFormat + ' ' + timeFormat;
    // size
    int sizeUnitIndex =                 s.size.sizeUnit;
    setSizeUnit(sizeUnitIndex);
    sizeUnitComboBox->setCurrentIndex(sizeUnitIndex);
    fileSizeSpinBox->setValue(          s.size.fileSizeValue);
    fileSizeComboBox->setCurrentIndex(  s.size.fileSizeUnit);
    maintainCheckBox->setChecked(       s.size.keepAspectRatio);
    if (sizeUnitComboBox->currentIndex() == 1) {
        sizeWidth =                     s.size.widthPx;
        sizeHeight =                    s.size.heightPx;
        widthDoubleSpinBox->setValue(sizeWidth);
        widthDoubleSpinBox->setValue(   s.size.widthPercent);
        heightDoubleSpinBox->setValue(sizeHeight);
        heightDoubleSpinBox->setValue(  s.size.heightPercent);
    }
    else if (sizeUnitComboBox->currentIndex() == 0) {
        sizeWidth =                     s.size.widthPercent;
        sizeHeight =                    s.size.heightPercent;
        widthDoubleSpinBox->setValue(sizeWidth);
        widthDoubleSpinBox->setValue(   s.size.widthPx);
        heightDoubleSpinBox->setValue(sizeHeight);
        heightDoubleSpinBox->setValue(  s.size.heightPx);
    }
    // raw
    rawEnabled = s.raw.enabled;
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
#ifdef SIR_METADATA_SUPPORT
    // metadata
    using namespace MetadataUtils;
    bool metadataEnabled =                  s.metadata.enabled;
    sharedInfo->setMetadataEnabled(metadataEnabled);
    bool saveMetadata =                     s.metadata.saveMetadata;
    sharedInfo->setSaveMetadata(saveMetadata);
    if (saveMetadata) {
        sharedInfo->setRealRotate(       s.metadata.realRotate);
        sharedInfo->setUpdateThumbnail(  s.metadata.updateThumbnail);
        sharedInfo->setRotateThumbnail(  s.metadata.rotateThumbnail);
    }
    else
        sharedInfo->setRealRotate(true);
    // exif
    bool exifOverwrite;
    exifOverwrite =                         s.exif.artistOverwrite;
    Exif::setArtistOverwrite(exifOverwrite);
    if (exifOverwrite)
        Exif::setArtistString(String(       s.exif.artistMap.keys().first()));

    exifOverwrite =                         s.exif.copyrightOverwrite;
    Exif::setCopyrightOverwrite(exifOverwrite);
    if (exifOverwrite)
        Exif::setCopyrightString(String(    s.exif.copyrightMap.keys().first()));

    exifOverwrite =                         s.exif.userCommentOverwrite;
    Exif::setUserCommentOverwrite(exifOverwrite);
    if (exifOverwrite)
        Exif::setUserCommentString(String(  s.exif.userCommentMap.keys().first()));
#endif // SIR_METADATA_SUPPORT
}

/** Save new window state and size in private fields.
  *
  * This is overloaded QWidget's method.
  */
void ConvertDialog::changeEvent(QEvent *e) {

    if (e->type()!=QEvent::WindowStateChange)
        return;

    QWindowStateChangeEvent *event = (QWindowStateChangeEvent*)(e);
    if (event->oldState()==Qt::WindowNoState) {
        windowSize = this->size();
        windowPossition = this->pos();
    }
}

/** Set converting status of image.
  * \param imageData List of strings containing path, image name and file extension.
  * \param status Status message.
  * \param statusNum Status code defined in src/defines.h.
  */
void ConvertDialog::setImageStatus(const QStringList& imageData,
                                   const QString& status, int statusNum) {
    if(statusNum != CONVERTING) {
        //We don't want to update the status bar if the statusValue is
        //CONVERTING
        convertProgressBar->setValue(convertProgressBar->value()+1);
    }
    int count = filesTreeWidget->topLevelItemCount();
    QString fileName;
    for (int i = 0; i < count; i++)
    {
        QTreeWidgetItem *item = filesTreeWidget->topLevelItem(i);
        if(item->text(NameColumn) == imageData.at(0)
                && item->text(ExtColumn) == imageData.at(1)
                && item->text(PathColumn) == imageData.at(2)) {
            item->setText(StatusColumn, status);
            fileName = item->text(PathColumn) + QDir::separator()
                     + item->text(NameColumn) + '.' + item->text(ExtColumn);
            filesTreeWidget->statusList->insert(fileName,statusNum);
            break;
        }
    }
    if(convertProgressBar->value() == convertProgressBar->maximum())
        updateInterface();
}

/** Ask for users agreement of typed action on file.
  * \param targetFile Asking file path.
  * \param tid Worker thread ID.
  * \param whatToDo Action on target file. Support for \em overwrite and \em enlarge only.
  */
void ConvertDialog::query(const QString& targetFile, Question whatToDo) {
    switch (whatToDo) {
    case Enlarge:
        if (sharedInfo->noEnlargeAll)
            sharedInfo->enlargeResult = QMessageBox::NoToAll;
        else if (sharedInfo->abort)
            sharedInfo->enlargeResult = QMessageBox::Cancel;
        else if (!sharedInfo->enlargeAll) {
            int result = MessageBox::question(
                        this,
                        tr("Enlarge File? - SIR"),
                        tr("A file called %1 is smaller than the requested size. "
                           "Enlargement can cause deterioration of picture quality. "
                           "Do you want enlarge it?").arg(targetFile) );
            if (result == MessageBox::YesToAll)
                sharedInfo->enlargeAll = true;
            else if (result == MessageBox::NoToAll)
                sharedInfo->noEnlargeAll = true;
            else if (result == MessageBox::Cancel)
                sharedInfo->abort = true;
            sharedInfo->enlargeResult = result;
        }
        else
            sharedInfo->enlargeResult = QMessageBox::YesToAll;
        break;
    case Overwrite:
        if (sharedInfo->noOverwriteAll)
            sharedInfo->overwriteResult = QMessageBox::NoToAll;
        else if(sharedInfo->abort)
            sharedInfo->overwriteResult = QMessageBox::Cancel;
        else if(!sharedInfo->overwriteAll) {
            int result = MessageBox::question(
                             this,
                             tr("Overwrite File? -- SIR"),
                             tr("A file called %1 already exists."
                                "Do you want to overwrite it?").arg(targetFile) );
            if (result == QMessageBox::YesToAll)
                sharedInfo->overwriteAll = true;
            else if (result == QMessageBox::NoToAll)
                sharedInfo->noOverwriteAll = true;
            else if (result == QMessageBox::Cancel)
                sharedInfo->abort = true;
            sharedInfo->overwriteResult = result;
        }
        else
            sharedInfo->overwriteResult = QMessageBox::YesToAll;
        break;
    default:
        break;
    }
}

/** Retranslates GUI. */
void ConvertDialog::retranslateStrings() {
    int sizeUnitIndex = sizeUnitComboBox->currentIndex();
    int fileSizeIndex = fileSizeComboBox->currentIndex();
    retranslateUi(this);
    filesTreeWidget->retranslateStrings();
    // restoring nulled indexes
    sizeUnitComboBox->setCurrentIndex(sizeUnitIndex);
    fileSizeComboBox->setCurrentIndex(fileSizeIndex);
}

/** Returns file size string with \e KiB or \e MiB suffix (depending
  * fileSizeComboBox current text).
  * \param size File size in bytes.
  */
QString ConvertDialog::fileSizeString(qint64 size) {
    double fileSize_M_B = size
            / pow(1024., this->fileSizeComboBox->currentIndex()+1.);
    QString fileSizeString = QString::number(fileSize_M_B, 'f', 2)
            + ' ' + this->fileSizeComboBox->currentText();
    return fileSizeString;
}

/** Cancels converting if converting runs; otherwise close window. */
void ConvertDialog::closeOrCancel() {
    if (converting) {
        stopConvertThreads();
        updateInterface();
        setCanceled();
    }
    else
        close();
}

/** Terminates all worker threads. */
void ConvertDialog::stopConvertThreads() {
    for (int i=0; i<convertThreads.length(); i++)
        convertThreads[i]->terminate();
}

/** Updates user interface after convering. */
void ConvertDialog::updateInterface() {
    converting = false;
    convertSelectedButton->setEnabled(true);
    convertButton->setEnabled(true);
    filesTreeWidget->resizeColumnsToContents();
    setCursor(Qt::ArrowCursor);
    quitButton->setText(tr("Quit"));
}

/** Sets image status to \em Cancelled if the image isn't converted yet. */
void ConvertDialog::setCanceled() {
    int count = filesTreeWidget->topLevelItemCount();
    QTreeWidgetItem *item = new QTreeWidgetItem();
    QString status = tr("Cancelled");
    QString converted = tr("Converted");
    for(int i = 0; i < count; i++) {
        item = filesTreeWidget->topLevelItem(i);
        if (item->text(StatusColumn) != converted)
            item->setText(StatusColumn, status);
    }
}

/** Shows size values corresponding index of size unit combo box. */
void ConvertDialog::setSizeUnit(int index) {
    if (index < 0)
        return;
    static int lastIndex = index;
    static int lastIndexPxPercent = index+1;
    static bool keepAspectRatio = false;
    if (index == 2) { // bytes
        geometryWidget->hide();
        fileSizeWidget->show();
        keepAspectRatio = maintainCheckBox->isChecked();
        maintainCheckBox->setChecked(true);
        maintainCheckBox->setEnabled(false);
    }
    else { // px or %
        fileSizeWidget->hide();
        geometryWidget->show();
        disconnectSizeLinesEdit();
        if (lastIndexPxPercent != index) {
            float tmp = sizeWidth;
            sizeWidth = widthDoubleSpinBox->value();
            widthDoubleSpinBox->setValue(tmp);
            tmp = sizeHeight;
            sizeHeight = heightDoubleSpinBox->value();
            heightDoubleSpinBox->setValue(tmp);
            lastIndexPxPercent = index;
            QString suffix;
            int decimals;
            double max;
            if (index == 0) { // px
                suffix = " px";
                decimals = 0;
                max = 100000.;
            }
            else { // %
                suffix = " %";
                decimals = 2;
                max = 10000.;
            }
            widthDoubleSpinBox->setSuffix(suffix);
            widthDoubleSpinBox->setDecimals(decimals);
            widthDoubleSpinBox->setMaximum(max);
            heightDoubleSpinBox->setSuffix(suffix);
            heightDoubleSpinBox->setDecimals(decimals);
            heightDoubleSpinBox->setMaximum(max);
        }
        if (lastIndex == 2) {
            maintainCheckBox->setEnabled(true);
            maintainCheckBox->setChecked(keepAspectRatio);
        }
        if (maintainCheckBox->isChecked() && index == 1) { // %
            heightDoubleSpinBox->setValue(widthDoubleSpinBox->value());
            connectSizeLinesEdit();
        }
    }
    lastIndex = index;
}

/** If desired size unit is percent and it keeps aspect ratio this function will
  * be change width or heigth percent value following the user change in
  * adjacent spin box. Otherwise do nothing.
  */
void ConvertDialog::sizeChanged(double value) {
    if (sizeUnitComboBox->currentIndex() != 1 && !maintainCheckBox->isChecked())
        return;
    // size unit is % and maintainCheckBox is checked
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(sender());
    if (spinBox == widthDoubleSpinBox)
        heightDoubleSpinBox->setValue(value);
    else if (spinBox == heightDoubleSpinBox)
        widthDoubleSpinBox->setValue(value);
}
