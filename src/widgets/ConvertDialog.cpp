/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2015  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://marek629.github.io/SIR/
 */

#include "widgets/ConvertDialog.hpp"

#include "CommandLineAssistant.hpp"
#include "ConvertSharedData.hpp"
#include "EffectsCollector.hpp"
#include "LanguageUtils.hpp"
#include "metadata/MetadataUtils.hpp"
#include "NetworkUtils.hpp"
#include "Selection.hpp"
#include "Session.hpp"
#include "SharedInformationBuilder.hpp"
#include "Version.hpp"
#include "widgets/AboutDialog.hpp"
#include "widgets/DetailsBrowserController.hpp"
#include "widgets/MessageBox.hpp"
#include "widgets/TreeWidget.hpp"
#include "widgets/OptionsDialog.hpp"
#include "widgets/options/GeneralGroupBoxController.hpp"

#include <QCompleter>
#include <QDebug>
#include <QDesktopServices>
#include <QDirModel>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QLibraryInfo>
#include <QTranslator>
#include <QUrl>
#include <QWindowStateChangeEvent>

#include <cmath>


using namespace sir;

/** Default constuctor.
  *
  * Sets up window with saved settings like window state, position
  * and convertion preferences.
  * \param parent       Pointer to parent object.
  * \param args         String containing aplication argv tables records
  *                     separated by ** (double star-sign).
  * \param cmdAssistant Command line assistant object.
  * \sa init()
  */
ConvertDialog::ConvertDialog(QWidget *parent, const QStringList &args,
                             CommandLineAssistant *cmdAssistant)
    : QMainWindow(parent) {
    csd = ConvertSharedData::instance();

    setupUi(this);

    this->args = args;
    net = NULL;
    sharedInfo = ConvertThread::sharedInfo();
    effectsDir = QDir::home();
    sessionDir = QDir::home();

    init();

    session = new Session(this);

    cmdAssistant->setTreeWidget(filesTreeWidget);
    if (!cmdAssistant->sessionFile().isEmpty())
        session->restore(cmdAssistant->sessionFile());

    effectsCollector = new EffectsCollector(this);

    detailsBrowserController = new DetailsBrowserController(filesTreeWidget,
                                                            detailsBrowser,
                                                            this);
}

/** Destructor.\n
  * Writes window state and position, dealocates dynamic allocated memory
  * and clears temporary directory.
  * \sa writeWindowProperties() clearTempDir()
  */
ConvertDialog::~ConvertDialog() {
    writeWindowProperties();
    if(net)
        delete net;
    clearTempDir();
    delete session;
    delete effectsCollector;
    delete detailsBrowserController;
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
    // and actions
    connect(actionAdd_File, SIGNAL(triggered()), filesTreeWidget, SLOT(addFile()));
    connect(actionAdd_Dir, SIGNAL(triggered()), filesTreeWidget, SLOT(addDir()));
    connect(actionSelect, SIGNAL(triggered()), SLOT(showSelectionDialog()));
    connect(actionImport_files, SIGNAL(triggered()), SLOT(showSelectionDialog()));
    connect(actionRemoveAll, SIGNAL(triggered()),
            filesTreeWidget, SLOT(removeAll()));

    // status bar
    connect(filesTreeWidget, SIGNAL(loadingFilesStart(int)), statusWidget, SLOT(onFilesLoadingStart(int)));
    connect(filesTreeWidget, SIGNAL(loadingFilesTick(int)), statusWidget, SLOT(onFilesLoadingTick(int)));
    connect(filesTreeWidget, SIGNAL(loadingFilesStop()), statusWidget, SLOT(onFilesLoadingStop()));
    connect(this, SIGNAL(convertStart(int)), statusWidget, SLOT(onConvetionStart(int)));
    connect(this, SIGNAL(convertTick(int)), statusWidget, SLOT(onConvetionTick(int)));
    connect(this, SIGNAL(convertStop()), statusWidget, SLOT(onConvetionStop()));

    // menu actions
    connect(actionExit, SIGNAL(triggered()), SLOT(close()));
    connect(actionAbout_Qt, SIGNAL(triggered()),qApp, SLOT(aboutQt()));
    connect(actionAbout_Sir, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionOptions, SIGNAL(triggered()), this, SLOT(setOptions()));
    connect(actionCheckforUpdates, SIGNAL(triggered()), SLOT(checkUpdates()));
    connect(actionRestoreSession, SIGNAL(triggered()), SLOT(restoreSession()));
    connect(actionSaveSession, SIGNAL(triggered()), SLOT(saveSession()));
    connect(actionRestoreEffects, SIGNAL(triggered()), SLOT(restoreEffects()));
    connect(actionSaveEffects, SIGNAL(triggered()), SLOT(saveEffects()));
    connect(actionSendInstall, SIGNAL(triggered()), SLOT(sendInstall()));

    // browse button
    connect(browseDestButton, SIGNAL(clicked()), SLOT(browseDestination()));

    // convert... & stop/exit buttons
    connect(convertButton, SIGNAL(clicked()), this, SLOT(convertAll()));
    connect(convertSelectedButton, SIGNAL(clicked()), SLOT(convertSelected()));
    connect(quitButton, SIGNAL(clicked()), SLOT(closeOrCancel()));
}

/** Sets icons from system theme. */
void ConvertDialog::setIcons() {
    const QIcon appExit = QIcon::fromTheme("application-exit",
                                           QIcon(":/images/exit.png") );
    const QIcon helpAbout = QIcon::fromTheme("help-about");
    const QIcon listAdd = QIcon::fromTheme("list-add");

    convertButton->setIcon(QIcon::fromTheme("system-run"));
    quitButton->setIcon(appExit);
    actionExit->setIcon(appExit);
    actionOptions->setIcon(QIcon::fromTheme("preferences-desktop"));
    actionAbout_Sir->setIcon(helpAbout);
    actionAbout_Qt->setIcon(helpAbout);
    actionAdd_File->setIcon(listAdd);
    actionAdd_Dir->setIcon(listAdd);
    actionCheckforUpdates->setIcon(QIcon::fromTheme("system-software-update"));
    actionSendInstall->setIcon(QIcon::fromTheme("network-transmit"));
    actionRemoveAll->setIcon(QIcon::fromTheme("edit-clear"));
    actionSelect->setIcon(QIcon::fromTheme("edit-select-all"));
    actionImport_files->setIcon(QIcon::fromTheme("edit-find"));
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
        Settings::instance()->settings.alreadySent = alreadySent;
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
    else if (result->contains("no_new_version")) {
        QMessageBox::information(this, tr("SIR Updates"),
                                 tr("You have the lastest version of SIR!"));
    }
    else if (result->contains("new_version")) {
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
            QUrl sirUrl("http://marek629.github.io/SIR/#download");
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
                SIGNAL(question(const QString &, int)),
                SLOT(query(const QString &, int)),
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
    csd->fileFilters = "*.";
    csd->fileFilters.append(list.join(" *.").toUpper());

    loadSettings();

    if (!args.isEmpty())
        filesTreeWidget->initList(args);

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
    converting = false;

    createConnections();
    setIcons();
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

    fileName = QDir::toNativeSeparators(fileName);

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
        emit convertTick(convertedImages);
    }
    else {
        convertThreads[threadNum]->setAcceptWork(false);
        emit convertStop();
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
void ConvertDialog::convert()
{
    resetAnswers();
    bool hasWidth = false;
    bool hasHeight = false;

    int w = 0;
    int h = 0;

    QDir destFolder(destFileEdit->text());

    if (sizeScrollArea->widthDoubleSpinBox->value()
            != sizeScrollArea->widthDoubleSpinBox->minimum()) {
        w = sizeScrollArea->widthDoubleSpinBox->value();
        hasWidth = true;
    }
    if (sizeScrollArea->heightDoubleSpinBox->value()
            != sizeScrollArea->heightDoubleSpinBox->minimum()) {
        h = sizeScrollArea->heightDoubleSpinBox->value();
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

    emit convertStart(numImages);

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

    SharedInformation shared = SharedInformation(*sharedInfo);

    if (sizeScrollArea->sizeUnitComboBox->currentIndex() == 2) {
        int multiplier = 1024;
        if (sizeScrollArea->fileSizeComboBox->currentIndex() == 1) {
            // MiB
            multiplier *= 1024;
        }
        int size = sizeScrollArea->fileSizeSpinBox->value() * multiplier;
        shared.setDesiredSize(size);
    } else {
        bool isPercent = (sizeScrollArea->sizeUnitComboBox->currentIndex() == 1);
        shared.setDesiredSize(w, h, isPercent, hasWidth, hasHeight,
                                   sizeScrollArea->maintainCheckBox->isChecked());
    }
    QString desiredFormat = targetFormatComboBox->currentText().toLower();
    shared.setDesiredFormat(desiredFormat);
    shared.setDesiredFlip(optionsScrollArea->flipComboBox->currentIndex());
    shared.setDesiredRotation(optionsScrollArea->rotateCheckBox->isChecked(),
                              optionsScrollArea->rotateLineEdit->text().toDouble());
    shared.setQuality(optionsScrollArea->qualitySpinBox->value());
    shared.setDestPrefix(destPrefixEdit->text());
    shared.setDestSuffix(destSuffixEdit->text());
    shared.setDestFolder(destFolder);
    shared.setOverwriteAll(false);

    // backgroud color
    if (optionsScrollArea->backgroundColorCheckBox->isChecked()) {
        shared.backgroundColor = optionsScrollArea->backgroundColorFrame->color();
    } else {
        shared.backgroundColor = QColor();
    }

    SharedInformationBuilder sharedInfoBuilder = SharedInformationBuilder(shared);
    SharedInformationBuilder *sharedInfoConfigurator = &sharedInfoBuilder;
    shared = SharedInformation(*sharedInfoConfigurator
            ->withEffects(effectsScrollArea)
            ->withSVG(svgScrollArea)
            ->withRaw(rawTabWidget)
            ->build());

    if (shared.effectsConfiguration().getImageLoadError()) {
        QMessageBox::StandardButton answer =
                QMessageBox::warning(this, tr("Load image failed"),
                                     tr("Load image to add failed.\n"
                                        "Do you want continue anyway?"),
                                     QMessageBox::Ignore | QMessageBox::Abort,
                                     QMessageBox::Abort);
        if (answer != QMessageBox::Ignore)
            return;
    }

    ConvertThread::setSharedInfo(shared);
    sharedInfo = ConvertThread::sharedInfo();

    //Gives a image to each thread convert
    for(int i = 0; i < nt; i++) {
        convertThreads[i]->setAcceptWork( true );
        item = itemsToConvert[convertedImages];
        convertThreads[i]->convertImage(item->text(NameColumn),
                                        item->text(ExtColumn),
                                        item->text(PathColumn));
        convertedImages++;
    }
    emit convertTick(convertedImages);
}

/** Shows selection dialog.
  * \sa Selection::selectItems() Selection::selectFiles()
  */
void ConvertDialog::showSelectionDialog() {
    Selection selection(this);
    QAction *action = static_cast<QAction*> (sender());
    if (action == actionSelect)
        qDebug() << "Selected items:" << selection.selectItems();
    else if (actionImport_files)
        qDebug() << "Imported files:" << selection.importFiles();
}

/** Gets session file path from the user and restores session from the file.
  * \sa saveSession() Session
  */
void ConvertDialog::restoreSession() {
    QString path = QFileDialog::getOpenFileName(this, tr("Choose session file"),
                                                sessionDir.path(),
                                                tr("XML Files") + " (*.xml)");
    if (path.isEmpty())
        return;

    QFileInfo info(path);
    sessionDir = info.absoluteDir();
    session->restore(path);
}

/** Gets session file path from the user and saves session to the file.
  * \sa restoreSession() Session
  */
void ConvertDialog::saveSession() {
    QString path = QFileDialog::getSaveFileName(this, tr("Choose session file"),
                                                sessionDir.path(),
                                                tr("XML Files") + " (*.xml)");
    if (path.isEmpty())
        return;

    QFileInfo info(path);
    sessionDir = info.absoluteDir();
    session->save(path);
}

/** Gets effects file path from the user and restores effects from the file.
  * \sa saveEffects() EffectsCollector
  */
void ConvertDialog::restoreEffects() {
    QString path = QFileDialog::getOpenFileName(this, tr("Choose effects file"),
                                                effectsDir.path(),
                                                tr("XML Files") + " (*.xml)");
    if (path.isEmpty())
        return;

    QFileInfo info(path);
    effectsDir = info.absoluteDir();
    effectsCollector->restore(path);
}

/** Gets effects file path from the user and saves effects to the file.
  * \sa restoreEffects() EffectsCollector
  */
void ConvertDialog::saveEffects() {
    QString path = QFileDialog::getSaveFileName(this, tr("Choose effects file"),
                                                effectsDir.path(),
                                                tr("XML Files") + " (*.xml)");
    if (path.isEmpty())
        return;

    QFileInfo info(path);
    effectsDir = info.absoluteDir();
    effectsCollector->save(path);
}

/** Shows window containing information about SIR. */
void ConvertDialog::about() {
    AboutDialog *about = new AboutDialog();
    about->setVersion(tr("Version") + " " + Version().version());
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

    Settings *s = Settings::instance();
    // main window
    if (                                s->mainWindow.maximized)
        showMaximized();
    move(                               s->mainWindow.possition);
    resize(                             s->mainWindow.size);
    horizontalSplitter->restoreState(   s->mainWindow.horizontalSplitter);
    verticalSplitter->restoreState(     s->mainWindow.verticalSplitter);
    // settings
    destFileEdit->setText(                      s->settings.targetFolder);
    targetFormatComboBox->setCurrentIndex(
                targetFormatComboBox->findText( s->settings.targetFormat));
    destPrefixEdit->setText(                    s->settings.targetPrefix);
    destSuffixEdit->setText(                    s->settings.targetSuffix);
    int quality =                               s->settings.quality;
    optionsScrollArea->qualitySpinBox->setValue(quality);
    optionsScrollArea->qualitySlider->setValue(quality);
    numThreads =                                s->settings.cores;
    if (numThreads == 0)
        numThreads = GeneralGroupBoxController::detectCoresCount();
    QString selectedTranslationFile =
            QCoreApplication::applicationDirPath() + "/../share/sir/translations/";
    selectedTranslationFile +=                  s->settings.languageFileName;
    QString qtTranslationFile = "qt_" +
            selectedTranslationFile.split('_').at(1).split('.').first();
    LanguageUtils *languages = LanguageUtils::instance();
    languages->qtTranslator->load(qtTranslationFile,
                                 QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    languages->appTranslator->load(selectedTranslationFile);
    retranslateStrings();
    alreadySent =                               s->settings.alreadySent;
    csd->dateFormat =                                s->settings.dateDisplayFormat;
    csd->timeFormat =                                s->settings.timeDisplayFormat;
    csd->dateTimeFormat = csd->dateFormat + ' ' + csd->timeFormat;
    // size
    int sizeUnitIndex =                 s->size.sizeUnit;
    sizeScrollArea->setSizeUnit(sizeUnitIndex);
    sizeScrollArea->sizeUnitComboBox->setCurrentIndex(sizeUnitIndex);
    sizeScrollArea->fileSizeSpinBox->setValue(          s->size.fileSizeValue);
    sizeScrollArea->fileSizeComboBox->setCurrentIndex(  s->size.fileSizeUnit);
    if (sizeScrollArea->sizeUnitComboBox->currentIndex() == 1) {
        csd->sizeWidth =                     s->size.widthPx;
        csd->sizeHeight =                    s->size.heightPx;
        sizeScrollArea->widthDoubleSpinBox->setValue(csd->sizeWidth);
        sizeScrollArea->widthDoubleSpinBox->setValue(   s->size.widthPercent);
        sizeScrollArea->heightDoubleSpinBox->setValue(csd->sizeHeight);
        sizeScrollArea->heightDoubleSpinBox->setValue(  s->size.heightPercent);
    }
    else if (sizeScrollArea->sizeUnitComboBox->currentIndex() == 0) {
        csd->sizeWidth =                     s->size.widthPercent;
        csd->sizeHeight =                    s->size.heightPercent;
        sizeScrollArea->widthDoubleSpinBox->setValue(csd->sizeWidth);
        sizeScrollArea->widthDoubleSpinBox->setValue(   s->size.widthPx);
        sizeScrollArea->heightDoubleSpinBox->setValue(csd->sizeHeight);
        sizeScrollArea->heightDoubleSpinBox->setValue(  s->size.heightPx);
    }
    sizeScrollArea->maintainCheckBox->setChecked(       s->size.keepAspectRatio);
#ifdef SIR_METADATA_SUPPORT
    // metadata
    using namespace MetadataUtils;
    bool metadataEnabled =               s->metadata.enabled;
    sharedInfo->setMetadataEnabled(metadataEnabled);
    bool saveMetadata =                  s->metadata.saveMetadata;
    sharedInfo->setSaveMetadata(saveMetadata);
    if (saveMetadata) {
        sharedInfo->setRealRotate(       s->metadata.realRotate);
        sharedInfo->setUpdateThumbnail(  s->metadata.updateThumbnail);
        sharedInfo->setRotateThumbnail(  s->metadata.rotateThumbnail);
    }
    else
        sharedInfo->setRealRotate(true);
    // exif
    bool exifOverwrite;
    exifOverwrite =                             s->exif.artistOverwrite;
    Exif::setArtistOverwrite(exifOverwrite);
    if (exifOverwrite)
        Exif::setArtistString(sir::String(      s->exif.artistMap.keys().first()));

    exifOverwrite =                             s->exif.copyrightOverwrite;
    Exif::setCopyrightOverwrite(exifOverwrite);
    if (exifOverwrite)
        Exif::setCopyrightString(sir::String(   s->exif.copyrightMap.keys().first()));

    exifOverwrite =                             s->exif.userCommentOverwrite;
    Exif::setUserCommentOverwrite(exifOverwrite);
    if (exifOverwrite)
        Exif::setUserCommentString(sir::String( s->exif.userCommentMap.keys().first()));
#endif // SIR_METADATA_SUPPORT
    // raw
    rawTabWidget->loadSettings(s->raw);
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
  * \param statusNum Status code.
  */
void ConvertDialog::setImageStatus(const QStringList& imageData,
                                   const QString& status, int statusNum) {
    if (statusNum != ConvertThread::Converting) {
        //We don't want to update the status bar if the statusValue is
        //CONVERTING
        convertProgressBar->setValue(convertProgressBar->value()+1);
    }

    int count = filesTreeWidget->topLevelItemCount();
    QString fileName;

    for (int i = 0; i < count; i++) {
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
    if (convertProgressBar->value() == convertProgressBar->maximum())
        updateInterface();
}

/** Ask for users agreement of typed action on file.
  * \param targetFile Asking file path.
  * \param tid Worker thread ID.
  * \param questionCode Action on target file. Support for \em overwrite and \em enlarge only.
  */
void ConvertDialog::query(const QString& targetFile, int questionCode) {
    ConvertThread::Question whatToDo = static_cast<ConvertThread::Question>(questionCode);
    switch (whatToDo) {
    case ConvertThread::Enlarge:
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
    case ConvertThread::Overwrite:
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
    retranslateUi(this);

    // retranslate children widgets
    filesTreeWidget->retranslateStrings();
    sizeScrollArea->retranslateStrings();
    optionsScrollArea->retranslateStrings();
    effectsScrollArea->retranslateStrings();
    svgScrollArea->retranslateStrings();

    statusWidget->retranslateStrings();
}

/** Returns file size string with \e KiB or \e MiB suffix (depending
  * fileSizeComboBox current text).
  * \param size File size in bytes.
  */
QString ConvertDialog::fileSizeString(qint64 size) {
    double fileSize_M_B = size
            / pow(1024., sizeScrollArea->fileSizeComboBox->currentIndex()+1.);
    QString fileSizeString = QString::number(fileSize_M_B, 'f', 2)
            + ' ' + sizeScrollArea->fileSizeComboBox->currentText();
    return fileSizeString;
}

/** Check SVG tab should disabled. If positive disables the tab. */
void ConvertDialog::checkSVGTab() {
    QStringList list;
    list << "svg" << "svgz";
    bool enable = filesTreeWidget->isColumnMember(ExtColumn, list, Qt::CaseInsensitive);
    tabWidget->setTabEnabled(3, enable);
}

const ConvertSharedData &ConvertDialog::convertSharedData() const {
    return *csd;
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
