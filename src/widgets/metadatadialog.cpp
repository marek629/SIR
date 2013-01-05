/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
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

#include <QLineEdit>
#include <QMessageBox>
#include <exiv2/exif.hpp>
#include "metadatadialog.h"
#include "metadatautils.h"
#include "settings.h"

using namespace MetadataUtils;

 /** Default constructor.\n
   * Sets up window and reads metadata from file within \a currentImage index
   * into \a images list.
   */
MetadataDialog::MetadataDialog(QWidget *parent, QStringList *images,
                               int currentImage) : QDialog(parent) {
    setupUi(this);
    this->images = images;
    this->currentImage = currentImage;
    this->imagePath = this->images->at(this->currentImage);
    metadata = new Metadata;

    if (currentImage == 0)
        previousButton->setEnabled(false);
    if (currentImage == images->length()-1)
        nextButton->setEnabled(false);
    filePathLabel->setText(imagePath);

    setupInputWidgets();
    readFile();
    setupValues();
    createConnections();
}

/** Destructor.
  * \note Deallocates \a images list typed in \ref MetadataDialog() "constructor".
  */
MetadataDialog::~MetadataDialog() {
    // export history comboboxes to settings
    saveHistory();
    // deallocate memory
    delete metadata;
    delete images;
}

/** Resets metadata structs to default values. */
void MetadataDialog::resetStructs() {
    exifStruct->reset();
    iptcStruct->reset();
}

/** Sets up items and properties of all user input widgets, including text edit
  * history import from settings.
  */
void MetadataDialog::setupInputWidgets() {
    QString noDataString = String::noData();
    // special value text of date/time editors
    exifOriginalDateEdit->setSpecialValueText(noDataString);
    exifOriginalTimeEdit->setSpecialValueText(noDataString);
    exifDigitizedDateEdit->setSpecialValueText(noDataString);
    exifDigitizedTimeEdit->setSpecialValueText(noDataString);
    iptcCreatedDateEdit->setSpecialValueText(noDataString);
    iptcCreatedTimeEdit->setSpecialValueText(noDataString);
    iptcDigitizedDateEdit->setSpecialValueText(noDataString);
    iptcDigitizedTimeEdit->setSpecialValueText(noDataString);
    // spin boxes
    exifFocalLengthSpinBox->setSpecialValueText(noDataString);
    exifExpBiasSpinBox->setSpecialValueText(noDataString);
    exifApertureSpinBox->setSpecialValueText(noDataString);
    exifIsoSpeedSpinBox->setSpecialValueText(noDataString);
    // time combo boxes
    exifExpTimeComboBox->insertItem(0, noDataString);
    exifShutterTimeComboBox->insertItem(0, noDataString);
    // setup Exif orientation combo box
    for (char i=1; i<=8; i++)
        exifOrientationComboBox->addItem(Exif::orientationString(i));
    // setup Exif exposure program combo box
    for (uchar i=0; i<=8; i++)
        exifExpProgramComboBox->addItem(Exif::expProgramString(i));
    // setup Exif light metering mode combo box
    for (short i=0; i<8; i++)
        exifLightMeteringModeComboBox->addItem(Exif::meteringModeString(i));
    // flashmode combobox
    exifFlashModeComboBox->addItem(Exif::flashString(-1));
    exifFlashModeComboBox->addItem(Exif::flashString(0));
    exifFlashModeComboBox->addItem(Exif::flashString(1));
    exifFlashModeComboBox->addItem(Exif::flashString(7));
    exifFlashModeComboBox->addItem(Exif::flashString(9));
    exifFlashModeComboBox->addItem(Exif::flashString(0x18));
    exifFlashModeComboBox->addItem(Exif::flashString(0x19));
    exifFlashModeComboBox->addItem(Exif::flashString(0x1d));
    exifFlashModeComboBox->addItem(Exif::flashString(0x1f));
    exifFlashModeComboBox->addItem(Exif::flashString(0x41));
    exifFlashModeComboBox->addItem(Exif::flashString(0x45));
    exifFlashModeComboBox->addItem(Exif::flashString(0x4d));
    exifFlashModeComboBox->addItem(Exif::flashString(0x47));
    // load settings
    Settings *s = Settings::instance();
    QString dateFormat  = s->settings.dateDisplayFormat;
    QString timeFormat  = s->settings.timeDisplayFormat;
    // max history count for HistoryComboBox's import functions
    int maxHistoryCount = s->settings.maxHistoryCount;
    // display format of date/time editors
    exifOriginalDateEdit->setDisplayFormat(dateFormat);
    exifOriginalTimeEdit->setDisplayFormat(timeFormat);
    exifDigitizedDateEdit->setDisplayFormat(dateFormat);
    exifDigitizedTimeEdit->setDisplayFormat(timeFormat);
    iptcCreatedDateEdit->setDisplayFormat(dateFormat);
    iptcCreatedTimeEdit->setDisplayFormat(timeFormat);
    iptcDigitizedDateEdit->setDisplayFormat(dateFormat);
    iptcDigitizedTimeEdit->setDisplayFormat(timeFormat);
    // Exif tab
    // Camera toolbox
    exifManufacturerComboBox->importHistory(s->exif.cameraManufacturerMap,
                                            s->exif.cameraManufacturerList,
                                            maxHistoryCount);
    exifManufacturerComboBox->setCurrentIndex(-1);
    exifModelComboBox->importHistory(       s->exif.cameraModelMap,
                                            s->exif.cameraModelList, maxHistoryCount);
    exifModelComboBox->setCurrentIndex(-1);
    // Author toolbox
    exifArtistComboBox->importHistory(      s->exif.artistMap,
                                            s->exif.artistList, maxHistoryCount);
    exifArtistComboBox->setCurrentIndex(-1);
    exifCopyrightComboBox->importHistory(   s->exif.copyrightMap,
                                            s->exif.copyrightList, maxHistoryCount);
    exifCopyrightComboBox->setCurrentIndex(-1);
    exifUserCommentComboBox->importHistory( s->exif.userCommentMap,
                                            s->exif.userCommentList, maxHistoryCount);
    exifUserCommentComboBox->setCurrentIndex(-1);
    // IPTC tab
    iptcEditStatusComboBox->importHistory(  s->iptc.editStatusMap,
                                            s->iptc.editStatusList, maxHistoryCount);
    iptcEditStatusComboBox->setCurrentIndex(-1);
}

/** Read current image file and shows information about issues if exist.\n
  * This function will call resetStructs() function if error cought.
  */
void MetadataDialog::readFile() {
    bool readSuccess = metadata->read(imagePath,true);
    QString errorTitle = tr("Metadata error");
    exifStruct = metadata->exifStruct();
    iptcStruct = metadata->iptcStruct();
    if (!readSuccess) {
        Error *e = metadata->lastError();
        QString errorMessage = e->message();
        errorMessage += tr("\nError code: %1\nError message: %2").
                arg(e->code()).arg(e->what());
        QMessageBox::critical(this, errorTitle, errorMessage);
        resetStructs();
        tabWidget->setCurrentWidget(iptcTab);
        return;
    }
    Exiv2::Image::AutoPtr image = metadata->imageAutoPtr();
    if (image.get() == 0) {
        QMessageBox::critical(this, errorTitle,
                              tr("Unexpected metadata read error occured."));
        return;
    }
    QString message;
    if (image->exifData().empty() && image->iptcData().empty()) {
        tabWidget->setCurrentWidget(iptcTab);
        message = tr("No Exif and IPTC metadata.");
    }
    else if (image->exifData().empty()) {
        tabWidget->setCurrentWidget(iptcTab);
        message = tr("No Exif metadata.");
    }
    else if (image->iptcData().empty()) {
        tabWidget->setCurrentWidget(exifTab);
        message = tr("No IPTC metadata.");
    }
    if (!message.isNull())
        QMessageBox::warning(this, tr("Metadata warning"), message);
}

void MetadataDialog::setupValues() {
    // Exif tab
    // Image toolbox
    exifVersionLabel->setText( exifStruct->version );
    exifProcessingSoftLabel->setText( exifStruct->processingSoftware );
    exifWidthLabel->setText( exifStruct->imageWidth );
    exifHeightLabel->setText( exifStruct->imageHeight );
    exifOrientationComboBox->setCurrentIndex( exifStruct->orientation - 1 );
    QDateTime dt = QDateTime::fromString(exifStruct->originalDate, Exif::dateTimeFormat);
    QDate minDate(1,1,1);
    if (!dt.date().isValid())
        dt.setDate(minDate);
    exifOriginalDateEdit->setDate(dt.date());
    exifOriginalTimeEdit->setTime(dt.time());
    dt = QDateTime::fromString(exifStruct->digitizedDate, Exif::dateTimeFormat);
    if (!dt.date().isValid())
        dt.setDate(minDate);
    exifDigitizedDateEdit->setDate(dt.date());
    exifDigitizedTimeEdit->setTime(dt.time());
    // Thumbnail toolbox
    exifThumbnailLabel->setPixmap(QPixmap::fromImage(exifStruct->thumbnailImage));
    exifThumbnailWidthLabel->setText( exifStruct->thumbnailWidth );
    exifThumbnailHeightLabel->setText( exifStruct->thumbnailHeight );
    // Photo toolbox
    exifFocalLengthSpinBox->setValue( exifStruct->focalLength );
    if (exifStruct->expTime.isEmpty() || exifStruct->expTime == "1/-1 s")
        exifExpTimeComboBox->setCurrentIndex(0);
    else
        exifExpTimeComboBox->addItem( exifStruct->expTime );
    if(exifStruct->shutterSpeed.isEmpty())
        exifShutterTimeComboBox->setCurrentIndex(0);
    else
        exifShutterTimeComboBox->addItem(exifStruct->shutterSpeed);
    exifExpBiasSpinBox->setValue( exifStruct->expBias );
    exifApertureSpinBox->setValue( exifStruct->aperture );
    exifIsoSpeedSpinBox->setValue( exifStruct->isoSpeed );
    exifExpProgramComboBox->setCurrentIndex( exifStruct->expProgram );
    exifLightMeteringModeComboBox->setCurrentIndex( exifStruct->meteringMode );
    exifFlashModeComboBox->setCurrentIndex(
                exifFlashModeComboBox->findText( Exif::flashString(
                                                     exifStruct->flashMode) ));
    // Camera toolbox
    exifManufacturerComboBox->lineEdit()->setText( exifStruct->cameraManufacturer );
    exifModelComboBox->lineEdit()->setText( exifStruct->cameraModel );
    // Author toolbox
    exifArtistComboBox->lineEdit()->setText( exifStruct->artist );
    exifCopyrightComboBox->lineEdit()->setText( exifStruct->copyright );
    exifUserCommentComboBox->lineEdit()->setText( exifStruct->userComment );

    // IPTC tab
    iptcVersionLabel->setText(iptcStruct->modelVersion);
    iptcCreatedDateEdit->setDate(iptcStruct->dateCreated);
    iptcCreatedTimeEdit->setTime(iptcStruct->timeCreated);
    iptcDigitizedDateEdit->setDate(iptcStruct->digitizationDate);
    iptcDigitizedTimeEdit->setTime(iptcStruct->digitizationTime);
    iptcByLineEdit->setText(iptcStruct->byline);
    iptcCopyrightLineEdit->setText(iptcStruct->copyright);
    iptcObjectNameLineEdit->setText(iptcStruct->objectName);
    iptcKeywordsLineEdit->setText(iptcStruct->keywords);
    iptcDescriptionTextEdit->setPlainText(iptcStruct->caption);
    iptcCountryLineEdit->setText(iptcStruct->countryName);
    iptcCityLineEdit->setText(iptcStruct->city);
    iptcEditStatusComboBox->lineEdit()->setText(iptcStruct->editStatus);
}

/** Connects this dialog widgets signals corresponding slots.
  * \sa MetadataDialog()
  */
void MetadataDialog::createConnections() {
    // push buttons
    connect(previousButton, SIGNAL(clicked()), this, SLOT(previousImage()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(nextImage()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteMetadata()));

    // date time editors
    // original (created) date time
    connect(exifOriginalDateEdit, SIGNAL(dateChanged(QDate)),
            iptcCreatedDateEdit, SLOT(setDate(QDate)));
    connect(exifOriginalTimeEdit, SIGNAL(timeChanged(QTime)),
            iptcCreatedTimeEdit, SLOT(setTime(QTime)));
    connect(iptcCreatedDateEdit, SIGNAL(dateChanged(QDate)),
            exifOriginalDateEdit, SLOT(setDate(QDate)));
    connect(iptcCreatedTimeEdit, SIGNAL(timeChanged(QTime)),
            exifOriginalTimeEdit, SLOT(setTime(QTime)));
    // digitized date time
    connect(exifDigitizedDateEdit, SIGNAL(dateChanged(QDate)),
            iptcDigitizedDateEdit, SLOT(setDate(QDate)));
    connect(exifDigitizedTimeEdit, SIGNAL(timeChanged(QTime)),
            iptcDigitizedTimeEdit, SLOT(setTime(QTime)));
    connect(iptcDigitizedDateEdit, SIGNAL(dateChanged(QDate)),
            exifDigitizedDateEdit, SLOT(setDate(QDate)));
    connect(iptcDigitizedTimeEdit, SIGNAL(timeChanged(QTime)),
            exifDigitizedTimeEdit, SLOT(setTime(QTime)));
}

/** Saves text edit history to settings.
  * \sa HistoryComboBox HistoryComboBox::exportHistory
  */
void MetadataDialog::saveHistory() {
    HistoryMap  map;
    HistoryList list;
    Settings *s = Settings::instance();
    // general settings
    int maxHistoryCount = s->settings.maxHistoryCount;
    // Exif tab
    // Camera toolbox
    exifManufacturerComboBox->exportHistory(&map, &list, maxHistoryCount);
    s->exif.cameraManufacturerMap    = map;
    s->exif.cameraManufacturerList   = list;
    exifModelComboBox->exportHistory(&map, &list, maxHistoryCount);
    s->exif.cameraModelMap           = map;
    s->exif.cameraModelList          = list;
    // Artist toolbox
    exifArtistComboBox->exportHistory(&map, &list, maxHistoryCount);
    s->exif.artistMap                = map;
    s->exif.artistList               = list;
    exifCopyrightComboBox->exportHistory(&map, &list, maxHistoryCount);
    s->exif.copyrightMap             = map;
    s->exif.copyrightList            = list;
    exifUserCommentComboBox->exportHistory(&map, &list, maxHistoryCount);
    s->exif.userCommentMap           = map;
    s->exif.userCommentList          = list;
    // IPTC tab
    iptcEditStatusComboBox->exportHistory(&map, &list, maxHistoryCount);
    s->iptc.editStatusMap    = map;
    s->iptc.editStatusList   = list;
}

void MetadataDialog::previousImage() {
    currentImage--;
    if (currentImage == 0)
        previousButton->setEnabled(false);
    if (!nextButton->isEnabled() && currentImage < images->length()-1)
        nextButton->setEnabled(true);
    imagePath = images->at(currentImage);
    readFile();
    setupValues();
    filePathLabel->setText(imagePath);
}

void MetadataDialog::nextImage() {
    currentImage++;
    if (currentImage == images->length()-1)
        nextButton->setEnabled(false);
    if (!previousButton->isEnabled())
        previousButton->setEnabled(true);
    imagePath = images->at(currentImage);
    readFile();
    setupValues();
    filePathLabel->setText(imagePath);
}

void MetadataDialog::saveChanges() {
    // Exif tab
    // Image toolbox
    exifStruct->orientation = exifOrientationComboBox->currentIndex();
    QDateTime dt;
    dt.setDate(exifOriginalDateEdit->date());
    dt.setTime(exifOriginalTimeEdit->time());
    exifStruct->originalDate = dt.toString(Exif::dateTimeFormat);
    dt.setDate(exifDigitizedDateEdit->date());
    dt.setTime(exifDigitizedTimeEdit->time());
    exifStruct->digitizedDate = dt.toString(Exif::dateTimeFormat);
    // Photo toolbox
    exifStruct->focalLength = exifFocalLengthSpinBox->value();
    exifStruct->expTime = exifExpTimeComboBox->currentText();
    exifStruct->expBias = exifExpBiasSpinBox->value();
    exifStruct->aperture = exifApertureSpinBox->value();
    exifStruct->shutterSpeed = exifShutterTimeComboBox->currentText();
    exifStruct->isoSpeed = exifIsoSpeedSpinBox->value();
    exifStruct->expProgram = exifExpProgramComboBox->currentIndex();
    exifStruct->meteringMode = exifLightMeteringModeComboBox->currentIndex();
    exifStruct->flashMode = Exif::flashShort(exifFlashModeComboBox->currentText());
    // Camera toolbox
    exifStruct->cameraManufacturer = exifManufacturerComboBox->lineEdit()->text();
    exifStruct->cameraModel = exifModelComboBox->lineEdit()->text();
    // Author toolbox
    exifStruct->artist = exifArtistComboBox->lineEdit()->text();
    exifStruct->copyright = exifCopyrightComboBox->lineEdit()->text();
    exifStruct->userComment = exifUserCommentComboBox->lineEdit()->text();

    // IPTC tab
    iptcStruct->byline = iptcByLineEdit->text();
    iptcStruct->copyright = iptcCopyrightLineEdit->text();
    iptcStruct->objectName = iptcObjectNameLineEdit->text();
    iptcStruct->keywords = iptcKeywordsLineEdit->text();
    iptcStruct->caption = iptcDescriptionTextEdit->toPlainText();
    iptcStruct->countryName = iptcCountryLineEdit->text();
    iptcStruct->city = iptcCityLineEdit->text();
    iptcStruct->editStatus = iptcEditStatusComboBox->lineEdit()->text();
    /* If any sting in iptcStruct isn't empty save date and time values that are
     * the same as Exif date time strings. */
    if (!iptcStruct->byline.isEmpty() || !iptcStruct->copyright.isEmpty()
            || !iptcStruct->objectName.isEmpty() || !iptcStruct->keywords.isEmpty()
            || !iptcStruct->caption.isEmpty() || !iptcStruct->countryName.isEmpty()
            || !iptcStruct->city.isEmpty() || !iptcStruct->editStatus.isEmpty()) {
        if (iptcCreatedDateEdit->date() == iptcCreatedDateEdit->minimumDate())
            iptcStruct->dateCreated = QDate();
        else
            iptcStruct->dateCreated = iptcCreatedDateEdit->date();
        if (iptcCreatedTimeEdit->time() == iptcCreatedTimeEdit->minimumTime())
            iptcStruct->timeCreated = QTime();
        else
            iptcStruct->timeCreated = iptcCreatedTimeEdit->time();
        if (iptcDigitizedDateEdit->date() == iptcDigitizedDateEdit->minimumDate())
            iptcStruct->digitizationDate = QDate();
        else
            iptcStruct->digitizationDate = iptcDigitizedDateEdit->date();
        if (iptcDigitizedTimeEdit->time() == iptcDigitizedTimeEdit->minimumTime())
            iptcStruct->digitizationTime = QTime();
        else
            iptcStruct->digitizationTime = iptcDigitizedTimeEdit->time();
    }

    // saving
    metadata->setExifData();
    metadata->setIptcData();
    metadata->write(imagePath);
    this->close();
}

void MetadataDialog::deleteMetadata() {
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Delete metadata"));
    msgBox.setText(tr("Do you really want to delete metadata from this image?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() == QMessageBox::Yes) {
        metadata->clearMetadata();
        metadata->write(imagePath);
        resetStructs();
        setupValues();
        this->close();
    }
}
