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

#include <QDir>
#include <QImageWriter>
#include <QCompleter>
#include <QDirModel>
#include <QLocale>
#include <QMap>
#include <QIcon>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include "optionsdialog.h"
#include "languageutils.h"
#include "convertdialog.h"

quint8 OptionsDialog::maxCoresCount = 50;

/** Default constructor.\n
  * Sets up window and read settings.
  */
OptionsDialog::OptionsDialog( QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {

    setupUi(this);

    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList list;

    foreach(QByteArray format, imageFormats) {
        list.append(*new QString(format));
    }

    languages = new LanguageUtils();

    fileToNiceName = new QMap<QString, QString>();

    targetFormatComboBox->insertItems(0,list);

    // copying items of combo boxes from parent ConvertDialog window
    ConvertDialog* mainWindow = (ConvertDialog*)parent;
    int size = mainWindow->sizeUnitComboBox->count();
    for (int i=0; i<size; i++)
        sizeUnitComboBox->addItem(mainWindow->sizeUnitComboBox->itemText(i));
    size = mainWindow->fileSizeComboBox->count();
    for (int i=0; i<size; i++)
        fileSizeComboBox->addItem(mainWindow->fileSizeComboBox->itemText(i));

    QCompleter *completer = new QCompleter(this);
    QDirModel *dir = new QDirModel(completer);
    dir->setFilter(QDir::AllDirs);
    completer->setModel(dir);
    targetFolderLineEdit->setCompleter(completer);

    QStringList wordList;
    wordList << "web" << "thumb" << "thumbnail" << "resized" << "new";
    QCompleter *completer2 = new QCompleter(wordList, this);
    completer2->setCaseSensitivity(Qt::CaseInsensitive);
    targetPrefixLineEdit->setCompleter(completer2);

    coresSpinBox->setMaximum(maxCoresCount);

    createLanguageMenu();
    createConnections();
    setupWindow();
    loadSettings();

    delete dir;
    delete completer;
    delete completer2;
}

/** Destructor. */
OptionsDialog::~OptionsDialog() {
    delete languages;
    delete validator;
    delete fileToNiceName;
    delete groupBoxes;
}

/** Connects signals to slots. */
void OptionsDialog::createConnections() {
    // general
    connect(listWidget, SIGNAL(currentRowChanged(int)),
            this, SLOT(categoryChanged(int)));
    connect(browsePushButton, SIGNAL(clicked()),
            this, SLOT(browseDestination()));
    connect(coresCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(respondCoresSpinBox(bool)));

    // metadata
    connect(metadataCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(enableMetadata(bool)));
    connect(saveMetadataCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(saveMetadata(bool)));
    connect(thumbUpdateCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(updateThumbnail(bool)));
    connect(exifArtistCheckBox, SIGNAL(toggled(bool)),
            exifArtistComboBox, SLOT(setEnabled(bool)));
    connect(exifCopyrightCheckBox, SIGNAL(toggled(bool)),
            exifCopyrightComboBox, SLOT(setEnabled(bool)));
    connect(exifUserCommentCheckBox, SIGNAL(toggled(bool)),
            exifUserCommentComboBox, SLOT(setEnabled(bool)));

    // raw
    connect(dcrawPushButton, SIGNAL(clicked()), this, SLOT(browseDcraw()));
    connect(rawCheckBox, SIGNAL(stateChanged(int)), SLOT(setRawStatus(int)));

    // ok button
    connect(okButton,SIGNAL(clicked()), this, SLOT(saveSettings()));
}

/** Sets validator objects for same line edits widgets, splitter sizes and
  * groupBoxes array of pointer for changing categories view.
  * \sa categoryChanged
  */
void OptionsDialog::setupWindow() {
    // validate only ASCII characters
    validator = new QRegExpValidator(QRegExp("^[\\00-\\0177]+"),this);
    exifArtistComboBox->setValidator(validator);
    exifCopyrightComboBox->setValidator(validator);

    QList <int> sizes;
    sizes.append(120);
    sizes.append(500);
    splitter->setSizes(sizes);

    currentListItem = 0;
    listWidget->setCurrentRow(currentListItem);

    int categoriesCount = listWidget->count();
    groupBoxes = new QGroupBox* [categoriesCount];
    groupBoxes[0] = generalBox;
    groupBoxes[1] = metadataBox;
    groupBoxes[2] = detailsBox;
    groupBoxes[3] = selectionBox;
    groupBoxes[4] = rawBox;
    for (int i=0; i<categoriesCount; i++)
        groupBoxes[i]->hide();
    groupBoxes[currentListItem]->show();
}

/** Hides last choosed category (group box) and shows just selected category.
  * \sa setupWindow
  */
void OptionsDialog::categoryChanged(int current) {
    if (current == currentListItem)
        return;
    groupBoxes[currentListItem]->hide();
    groupBoxes[current]->show();
    currentListItem = current;
}

void OptionsDialog::browseDestination() {

    QString initialName = targetFolderLineEdit->text();

    if (initialName.isEmpty())
        initialName = QDir::homePath();


    QString fileName = QFileDialog::getExistingDirectory(
            this,
            tr("Choose a directory"),
            initialName,
            QFileDialog::ShowDirsOnly );

    fileName = QDir::convertSeparators(fileName);

    if (!fileName.isEmpty()) {
        targetFolderLineEdit->setText(fileName);
    }
}

bool OptionsDialog::checkDcrawPath(QString fileName) {
    if (!fileName.isEmpty()) {
        QFile dcraw(fileName);
        //Check if the file exists
        if(!dcraw.exists()) {
            QMessageBox::warning(
                    this, "SIR",
                    tr("dcraw executable not found. "
                       "RAW support will not be enabled!")
            );
            return false;
        }
        else {
            //is executable??
            if(dcraw.permissions().testFlag(QFile::ExeOwner)) {
                //the file is executable
                return true;
            }
            else {
                //Not executable
                QMessageBox::warning(
                        this, "SIR",
                        tr("The chosen file is not executable. "
                           "RAW support will not be enabled!" )
                );
                return false;

            }
        }
    }
    else {
        QMessageBox::warning(
                        this, "SIR",
                        tr("No dcraw executable chosen. "
                           "RAW support will not be enabled!" )
                        );
        return false;
    }
}

void OptionsDialog::browseDcraw() {

    QString initialName = targetFolderLineEdit->text();

    if (initialName.isEmpty())
        initialName = QDir::homePath();

       QString fileName = QFileDialog::getOpenFileName(
                            this,
                            tr("Select dcraw executable"),
                            initialName
                        );

    fileName = QDir::convertSeparators(fileName);
    dcrawLineEdit->setText(fileName);

}

/** Writes settings based values of the user input wigets.
  * \sa readSettings
  */
void OptionsDialog::saveSettings() {
    Settings &s = Settings::instance();
    // general settings
    s.settings.targetFolder         = targetFolderLineEdit->text();
    s.settings.targetFormat         = targetFormatComboBox->currentIndex();
    s.settings.targetPrefix         = targetPrefixLineEdit->text();
    s.settings.targetSuffix         = targetSuffixLineEdit->text();
    s.settings.quality              = qualitySpinBox->value();
    s.settings.languageNiceName     = languagesComboBox->currentText();
    s.settings.languageFileName     = fileToNiceName->value(
                                            languagesComboBox->currentText() );
    s.settings.dateDisplayFormat    = dateDisplayFormatLineEdit->text();
    s.settings.timeDisplayFormat    = timeDisplayFormatLineEdit->text();
    if (coresCheckBox->isChecked())
        s.settings.cores            = 0;
    else
        s.settings.cores            = coresSpinBox->value();
    s.settings.maxHistoryCount      = historySpinBox->value();
    // size
    s.size.widthPx          = widthPxSpinBox->value();
    s.size.widthPercent     = widthPercentDoubleSpinBox->value();
    s.size.heightPx         = heightPxSpinBox->value();
    s.size.heightPercent    = heightPercentDoubleSpinBox->value();
    s.size.fileSizeValue    = fileSizeSpinBox->value();
    s.size.fileSizeUnit     = fileSizeComboBox->currentIndex();
    s.size.sizeUnit         = sizeUnitComboBox->currentIndex();
    // metadata (general part)
    s.metadata.enabled          = metadataCheckBox->isChecked();
    s.metadata.saveMetadata     = saveMetadataCheckBox->isChecked();
    s.metadata.realRotate       = rotateRadioButton->isChecked();
    s.metadata.updateThumbnail  = thumbUpdateCheckBox->isChecked();
    s.metadata.rotateThumbnail  = thumbRotateCheckBox->isChecked();
    // metadata (Exif part)
    HistoryMap  historyMap;
    HistoryList historyList;
    s.exif.artistOverwrite      = exifArtistCheckBox->isChecked();
    exifArtistComboBox->exportHistory(&historyMap,&historyList,maxHistoryCount);
    s.exif.artistMap            = historyMap;
    s.exif.artistList           = historyList;
    s.exif.copyrightOverwrite   = exifCopyrightCheckBox->isChecked();
    exifCopyrightComboBox->exportHistory(&historyMap,&historyList,maxHistoryCount);
    s.exif.copyrightMap         = historyMap;
    s.exif.copyrightList        = historyList;
    s.exif.userCommentOverwrite = exifUserCommentCheckBox->isChecked();
    exifUserCommentComboBox->exportHistory(&historyMap,&historyList,maxHistoryCount);
    s.exif.userCommentMap       = historyMap;
    s.exif.userCommentList      = historyList;
    // details
    int hex = 0;
    if (exifVersionCheckBox->isChecked())
        hex |= DetailsOptions::ExifVersion;
    if (exifSoftCheckBox->isChecked())
        hex |= DetailsOptions::ProcessingSoftware;
    if (exifOrientationCheckBox->isChecked())
        hex |= DetailsOptions::Orientation;
    if (exifGeneratedTimeCheckBox->isChecked())
        hex |= DetailsOptions::GeneratedDateAndTime;
    if (exifDigitizedTimeCheckBox->isChecked())
        hex |= DetailsOptions::DigitizedDateAndTime;
    s.details.exifImage     = hex;
    hex = 0;
    if (exifFocalLengthCheckBox->isChecked())
        hex |= DetailsOptions::FocalLenght;
    if (exifApertureCheckBox->isChecked())
        hex |= DetailsOptions::Aperture;
    if (exifExpTimeCheckBox->isChecked())
        hex |= DetailsOptions::ExposureTime;
    if (exifShutterSpeedCheckBox->isChecked())
        hex |= DetailsOptions::ShutterSpeed;
    if (exifExpBiasCheckBox->isChecked())
        hex |= DetailsOptions::ExposureBias;
    if (exifIsoCheckBox->isChecked())
        hex |= DetailsOptions::IsoSpeed;
    if (exifExpProgramCheckBox->isChecked())
        hex |= DetailsOptions::ExposureProgram;
    if (exifMeteringCheckBox->isChecked())
        hex |= DetailsOptions::LightMeteringMode;
    if (exifFlashCheckBox->isChecked())
        hex |= DetailsOptions::FlashMode;
    s.details.exifPhoto     = hex;
    hex = 0;
    if (exifManufacturerCheckBox->isChecked())
        hex |= DetailsOptions::Manufacturer;
    if (exifModelCheckBox->isChecked())
        hex |= DetailsOptions::Model;
    s.details.exifCamera    = hex;
    hex = 0;
    if (exifArtistCheckBox->isChecked())
        hex |= DetailsOptions::Artist;
    if (exifCopyrightCheckBox_D->isChecked())
        hex |= DetailsOptions::Copyright;
    if (exifUserCommentCheckBox_D->isChecked())
        hex |= DetailsOptions::UserComment;
    s.details.exifAuthor    = hex;
    hex = 0;
    if (iptcVersionCheckBox->isChecked())
        hex |= DetailsOptions::ModelVersion;
    if (iptcBylineCheckBox->isChecked())
        hex |= DetailsOptions::Byline;
    if (iptcCopyrightCheckBox->isChecked())
        hex |= DetailsOptions::CopyrightIptc;
    if (iptcObjectNameCheckBox->isChecked())
        hex |= DetailsOptions::ObjectName;
    if (iptcKeywordsCheckBox->isChecked())
        hex |= DetailsOptions::Keywords;
    if (iptcCaptionCheckBox->isChecked())
        hex |= DetailsOptions::Caption;
    if (iptcCountryNameCheckBox->isChecked())
        hex |= DetailsOptions::CountryName;
    if (iptcCityCheckBox->isChecked())
        hex |= DetailsOptions::City;
    if (iptcEditStatusCheckBox->isChecked())
        hex |= DetailsOptions::EditStatus;
    if (iptcCreatedDateCheckBox->isChecked())
        hex |= DetailsOptions::DateCreated;
    if (iptcCreatedTimeCheckBox->isChecked())
        hex |= DetailsOptions::TimeCreated;
    if (iptcDigitizedDateCheckBox->isChecked())
        hex |= DetailsOptions::DigitizedDate;
    if (iptcDigitizedTimeCheckBox->isChecked())
        hex |= DetailsOptions::DigitizedTime;
    s.details.iptc          = hex;
    // selection
    s.selection.clearSelection      = clearSelectionCheckBox->isChecked();
    s.selection.subdirs             = importSubdirsCheckBox->isChecked();
    s.selection.selectImported      = selectImportedCheckBox->isChecked();
    s.selection.fileSizeSymbol      = sFileSizeLineEdit->text();
    s.selection.imageWidthSymbol    = sImgWidthLineEdit->text();
    s.selection.imageHeightSymbol   = sImgHeightLineEdit->text();
    // raw
    bool dcrawOk = false;
    bool firstState = rawCheckBox->isChecked();
    //check dcraw executable
    if(rawCheckBox->isChecked()) {
        if((dcrawOk = checkDcrawPath(dcrawLineEdit->text()))) {
            s.raw.enabled       = true;
            s.raw.dcrawPath     = dcrawLineEdit->text();
            s.raw.dcrawOptions  = dcrawOptions->text();
        }
        else {
            s.raw.enabled       = false;
            rawCheckBox->setChecked(false);
            s.raw.dcrawPath     = dcrawLineEdit->text();
            s.raw.dcrawOptions  = dcrawOptions->text();
            setRawStatus(false);
        }
    }
    else {
        s.raw.enabled           = false;
        s.raw.dcrawPath         = dcrawLineEdit->text();
        s.raw.dcrawOptions      = dcrawOptions->text();
    }
    if(dcrawOk || !firstState) {
        emit ok();
        this->close();
    }
}

/** Reads settings and sets value of the user input widgets.
  * \sa writeSettings
  */
void OptionsDialog::loadSettings() {
    Settings &s = Settings::instance();
    // general settings
    targetFolderLineEdit->setText(              s.settings.targetFolder);
    targetFormatComboBox->setCurrentIndex(
                targetFormatComboBox->findText( s.settings.targetFormat));
    targetPrefixLineEdit->setText(              s.settings.targetPrefix);
    targetSuffixLineEdit->setText(              s.settings.targetSuffix);
    qualitySpinBox->setValue(                   s.settings.quality);
    languagesComboBox->setCurrentIndex(
                languagesComboBox->findText(    s.settings.languageNiceName,
                                            Qt::MatchExactly) );
    coresCount =                                s.settings.cores;
    if (coresCount == 0) {
        coresCheckBox->setChecked(true);
        respondCoresSpinBox(true);
    }
    else {
        coresCheckBox->setChecked(false);
        respondCoresSpinBox(false);
    }
    dateDisplayFormatLineEdit->setText(         s.settings.dateDisplayFormat);
    timeDisplayFormatLineEdit->setText(         s.settings.timeDisplayFormat);
    maxHistoryCount =                           s.settings.maxHistoryCount;
    historySpinBox->setValue(maxHistoryCount);
    // size
    widthPxSpinBox->setValue(               s.size.widthPx);
    widthPercentDoubleSpinBox->setValue(    s.size.widthPercent);
    heightPxSpinBox->setValue(              s.size.heightPx);
    heightPercentDoubleSpinBox->setValue(   s.size.heightPercent);
    fileSizeSpinBox->setValue(              s.size.fileSizeValue);
    fileSizeComboBox->setCurrentIndex(      s.size.fileSizeUnit);
    sizeUnitComboBox->setCurrentIndex(      s.size.sizeUnit);
    // metadata (general part)
    metadataCheckBox->setChecked(           s.metadata.enabled);
    if (!metadataCheckBox->isChecked()) {
        saveMetadataCheckBox->setEnabled(false);
        thumbUpdateCheckBox->setEnabled(false);
        thumbRotateCheckBox->setEnabled(false);
    }
    else
        saveMetadataCheckBox->setChecked(   s.metadata.saveMetadata);
    if (saveMetadataCheckBox->isChecked()) {
        thumbUpdateCheckBox->setChecked(    s.metadata.updateThumbnail);
        if (thumbUpdateCheckBox->isChecked())
            thumbRotateCheckBox->setChecked(s.metadata.rotateThumbnail);
        rotateRadioButton->setChecked(      s.metadata.realRotate);
    }
    else {
        thumbUpdateCheckBox->setEnabled(false);
        thumbRotateCheckBox->setEnabled(false);
        rotateRadioButton->setChecked(true);
    }
    // metadata (Exif part)
    bool exifOverwrite;
    HistoryMap exifDefaultMap;
    exifOverwrite =                         s.exif.artistOverwrite;
    exifArtistCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.insert(tr("Camera owner: ; Photographer: "),false);
    exifArtistComboBox->importHistory(      s.exif.artistMap,
                                            s.exif.artistList, maxHistoryCount);
    exifArtistComboBox->setEnabled(exifOverwrite);
    exifOverwrite =                         s.exif.copyrightOverwrite;
    exifCopyrightCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.clear();
    exifDefaultMap.insert(tr("Copyright owner"),false);
    exifCopyrightComboBox->importHistory(   s.exif.copyrightMap,
                                            s.exif.copyrightList, maxHistoryCount);
    exifCopyrightComboBox->setEnabled(exifOverwrite);

    exifOverwrite =                         s.exif.userCommentOverwrite;
    exifUserCommentCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.clear();
    exifDefaultMap.insert(
                tr("This picture was edited with Simple Image Resizer"),false);
    exifUserCommentComboBox->importHistory( s.exif.userCommentMap,
                                            s.exif.userCommentList, maxHistoryCount);
    exifUserCommentComboBox->setEnabled(exifOverwrite);
    // details
    int hex;
    // exif
    hex = s.details.exifImage;
    exifVersionCheckBox->setChecked(hex & DetailsOptions::ExifVersion);
    exifSoftCheckBox->setChecked(hex & DetailsOptions::ProcessingSoftware);
    exifOrientationCheckBox->setChecked(hex & DetailsOptions::Orientation);
    exifGeneratedTimeCheckBox->setChecked(hex & DetailsOptions::GeneratedDateAndTime);
    exifDigitizedTimeCheckBox->setChecked(hex & DetailsOptions::DigitizedDateAndTime);
    hex = s.details.exifPhoto;
    exifFocalLengthCheckBox->setChecked(hex & DetailsOptions::FocalLenght);
    exifApertureCheckBox->setChecked(hex & DetailsOptions::Aperture);
    exifIsoCheckBox->setChecked(hex & DetailsOptions::IsoSpeed);
    exifShutterSpeedCheckBox->setChecked(hex & DetailsOptions::ShutterSpeed);
    exifExpTimeCheckBox->setChecked(hex & DetailsOptions::ExposureTime);
    exifExpBiasCheckBox->setChecked(hex & DetailsOptions::ExposureBias);
    exifExpProgramCheckBox->setChecked(hex & DetailsOptions::ExposureProgram);
    exifMeteringCheckBox->setChecked(hex & DetailsOptions::LightMeteringMode);
    exifFlashCheckBox->setChecked(hex & DetailsOptions::FlashMode);
    hex = s.details.exifCamera;
    exifManufacturerCheckBox->setChecked(hex & DetailsOptions::Manufacturer);
    exifModelCheckBox->setChecked(hex & DetailsOptions::Model);
    hex = s.details.exifAuthor;
    exifArtistCheckBox_D->setChecked(hex & DetailsOptions::Artist);
    exifCopyrightCheckBox_D->setChecked(hex & DetailsOptions::Copyright);
    exifUserCommentCheckBox_D->setChecked(hex & DetailsOptions::UserComment);
    // iptc
    hex = s.details.iptc;
    iptcVersionCheckBox->setChecked(hex & DetailsOptions::ModelVersion);
    iptcBylineCheckBox->setChecked(hex & DetailsOptions::Byline);
    iptcCopyrightCheckBox->setChecked(hex & DetailsOptions::CopyrightIptc);
    iptcObjectNameCheckBox->setChecked(hex & DetailsOptions::ObjectName);
    iptcKeywordsCheckBox->setChecked(hex & DetailsOptions::Keywords);
    iptcCaptionCheckBox->setChecked(hex & DetailsOptions::Caption);
    iptcCountryNameCheckBox->setChecked(hex & DetailsOptions::CountryName);
    iptcCityCheckBox->setChecked(hex & DetailsOptions::City);
    iptcEditStatusCheckBox->setChecked(hex & DetailsOptions::EditStatus);
    iptcCreatedDateCheckBox->setChecked(hex & DetailsOptions::DateCreated);
    iptcCreatedTimeCheckBox->setChecked(hex & DetailsOptions::TimeCreated);
    iptcDigitizedDateCheckBox->setChecked(hex & DetailsOptions::DigitizedDate);
    iptcDigitizedTimeCheckBox->setChecked(hex & DetailsOptions::DigitizedTime);
    // selection
    clearSelectionCheckBox->setChecked( s.selection.clearSelection);
    importSubdirsCheckBox->setChecked(  s.selection.subdirs);
    selectImportedCheckBox->setChecked( s.selection.selectImported);
    sFileSizeLineEdit->setText(         s.selection.fileSizeSymbol);
    sImgWidthLineEdit->setText(         s.selection.imageWidthSymbol);
    sImgHeightLineEdit->setText(        s.selection.imageHeightSymbol);
    // raw
    int state =             s.raw.enabled;
    rawCheckBox->setChecked(state);
    setRawStatus(state);
    dcrawLineEdit->setText( s.raw.dcrawPath);
    dcrawOptions->setText(  s.raw.dcrawOptions);
}

void OptionsDialog::createLanguageMenu() {

    QDir dir(":translations/");
    QStringList filter;
    filter << "sir_*.qm";
    QStringList fileNames = dir.entryList(filter);

    QString languageName;
    QIcon * countryFlag;
    LanguageInfo info;
    info = languages->getLanguageInfo("sir_en_US.qm");

    languageName = info.niceName;
    QString trlangName = tr(languageName.toStdString().c_str());

    countryFlag = new QIcon(":images/" + info.flagFile);

    languagesComboBox->insertItem(0, *(countryFlag), trlangName);

    fileToNiceName->insert(trlangName, "sir_en_US.qm");

    for (int i = 0; i < (int)fileNames.size(); ++i) {
        info = languages->getLanguageInfo(fileNames[i]);
        languageName = info.niceName;

        trlangName = tr(languageName.toStdString().c_str());
        countryFlag = new QIcon(":images/" + info.flagFile);

        languagesComboBox->insertItem(i+1, *(countryFlag), trlangName);

        fileToNiceName->insert(trlangName, fileNames[i]);
    }

}

void OptionsDialog::setRawStatus(int state) {
    dcrawLineEdit->setEnabled(state);
    dcrawPushButton->setEnabled(state);
    dcrawOptions->setEnabled(state);
}

void OptionsDialog::respondCoresSpinBox(bool checked) {
    if (checked) {
        coresCount = detectCoresCount();
        coresSpinBox->setValue(coresCount);
        coresSpinBox->setReadOnly(true);
    }
    else {
        coresSpinBox->setValue(coresCount);
        coresSpinBox->setReadOnly(false);
    }
}

void OptionsDialog::enableMetadata(bool checked) {
    if (checked) {
        saveMetadataCheckBox->setEnabled(true);
        exifArtistCheckBox->setEnabled(true);
        exifCopyrightCheckBox->setEnabled(true);
        exifUserCommentCheckBox->setEnabled(true);
    }
    else {
        saveMetadataCheckBox->setChecked(false);
        saveMetadataCheckBox->setEnabled(false);
        exifArtistCheckBox->setChecked(false);
        exifArtistCheckBox->setEnabled(false);
        exifCopyrightCheckBox->setChecked(false);
        exifCopyrightCheckBox->setEnabled(false);
        exifUserCommentCheckBox->setChecked(false);
        exifUserCommentCheckBox->setEnabled(false);
    }
}

/** \em "Save metadata" checkbox slot. Enables or disables and (un)check
  * checkboxes resposibled saving metadata basing \a save value.
  */
void OptionsDialog::saveMetadata(bool save) {
    thumbUpdateCheckBox->setChecked(save);
    thumbUpdateCheckBox->setEnabled(save);
    thumbRotateCheckBox->setEnabled(save);
    exifOrientationRadioButton->setEnabled(save);
    exifArtistCheckBox->setEnabled(save);
    exifCopyrightCheckBox->setEnabled(save);
    exifUserCommentCheckBox->setEnabled(save);
    if (save)
        exifOrientationRadioButton->setChecked(true);
    else {
        rotateRadioButton->setChecked(true);
    }
}

void OptionsDialog::updateThumbnail(bool update) {
    if (!update)
        thumbRotateCheckBox->setChecked(false);
}

/** Returns CPU cores count. */
quint8 OptionsDialog::detectCoresCount() {
    int cores = QThread::idealThreadCount();
    if (cores == -1) {
        qWarning("OptionsDialog: cores count detect failed");
        return 1;
    }
    else if (cores > maxCoresCount)
        return maxCoresCount;
    else
        return (quint8)cores;
}
