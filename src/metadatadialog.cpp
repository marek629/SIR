#include "metadatadialog.h"
#include "metadatautils.h"
#include <QLineEdit>
#include <QSettings>
#include <QMessageBox>
#include <exiv2/exif.hpp>
#include <QDebug>

MetadataDialog::MetadataDialog(QWidget *parent, QStringList *images,
                               int currentImage) : QDialog(parent) {
    setupUi(this);
    this->images = images;
    this->currentImage = currentImage;
    this->imagePath = this->images->at(this->currentImage);
    metadata = new MetadataUtils::Metadata;

    bool readSuccess = metadata->read(imagePath,true);
    exifStruct = metadata->exifStruct();

    if (!readSuccess) {
        QString errorTitle = tr("Metadata error");
        MetadataUtils::Error *e = metadata->lastError();
        QString errorMessage = e->message();
        errorMessage += tr("\nError code: %1\nError message: %2").
                arg(e->code()).arg(e->what());
        QMessageBox::critical(this, errorTitle, errorMessage);
        resetStructs();
    }
    setupValues();

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteMetadata()));
}

MetadataDialog::~MetadataDialog() {
    delete metadata;
    delete images;
}

void MetadataDialog::resetStructs() {
    exifStruct->reset();
}

void MetadataDialog::setupValues() {
    QSettings settings("SIR");
    settings.beginGroup("Exif");
    int maxHistoryCount = settings.value("maxHistoryCount", 5).toInt();

    // Exif tab
    // Image toolbox
    exifVersionLabel->setText( exifStruct->version );
    exifProcessingSoftLabel->setText( exifStruct->processingSoftware );
    exifWidthLabel->setText( exifStruct->imageWidth );
    exifHeightLabel->setText( exifStruct->imageHeight );
    exifOrientationComboBox->setCurrentIndex( exifStruct->orientation - 1 );
    exifOriginalDateComboBox->
                loadHistory( settings.value("originalDateMap").toMap(),
                             settings.value("originalDateList").toList(),
                             maxHistoryCount );
    exifOriginalDateComboBox->lineEdit()->setText( exifStruct->originalDate );
    exifDigitizedDateComboBox->
            loadHistory( settings.value("digitizedDateMap").toMap(),
                         settings.value("digitizedDateList").toList(),
                         maxHistoryCount );
    exifDigitizedDateComboBox->lineEdit()->setText( exifStruct->digitizedDate );

    // Thumbnail toolbox
    exifThumbnailLabel->setPixmap(QPixmap::fromImage(exifStruct->thumbnailImage));
    exifThumbnailWidthLabel->setText( exifStruct->thumbnailWidth );
    exifThumbnailHeightLabel->setText( exifStruct->thumbnailHeight );

    // Photo toolbox
    exifFocalLengthSpinBox->setSpecialValueText(MetadataUtils::String::noData());
    exifFocalLengthSpinBox->setValue( exifStruct->focalLength );
    exifExpTimeComboBox->insertItem(0,MetadataUtils::String::noData());
    if (exifStruct->expTime.isEmpty())
        exifExpTimeComboBox->setCurrentIndex(0);
    else
        exifExpTimeComboBox->addItem( exifStruct->expTime );
    exifShutterTimeComboBox->insertItem(0,MetadataUtils::String::noData());
    if(exifStruct->shutterSpeed.isEmpty())
        exifShutterTimeComboBox->setCurrentIndex(0);
    else
        exifShutterTimeComboBox->addItem(exifStruct->shutterSpeed);
    exifExpBiasSpinBox->setSpecialValueText(MetadataUtils::String::noData());
    exifExpBiasSpinBox->setValue( exifStruct->expBias );
    exifApertureSpinBox->setSpecialValueText(MetadataUtils::String::noData());
    exifApertureSpinBox->setValue( exifStruct->aperture );
    exifIsoSpeedSpinBox->setSpecialValueText(MetadataUtils::String::noData());
    exifIsoSpeedSpinBox->setValue( exifStruct->isoSpeed );
    exifExpProgramComboBox->setCurrentIndex( exifStruct->expProgram );
    exifLightMeteringModeComboBox->setCurrentIndex( exifStruct->meteringMode );
    // setup flashmode combobox
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(-1));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(0));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(1));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(7));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(9));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(0x18));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(0x19));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(0x1d));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(0x1f));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(0x41));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(0x45));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(0x4d));
    exifFlashModeComboBox->addItem(MetadataUtils::Exif::flashString(0x47));
    exifFlashModeComboBox->setCurrentIndex(
                exifFlashModeComboBox->findText( MetadataUtils::Exif::flashString(
                                                     exifStruct->flashMode) ));

    // Camera toolbox
    exifManufacturerComboBox->lineEdit()->setText( exifStruct->cameraManufacturer );
    exifModelComboBox->lineEdit()->setText( exifStruct->cameraModel );

    // Author toolbox
    exifArtistComboBox->loadHistory( settings.value("artistMap").toMap(),
                                     settings.value("artistList").toList(),
                                     maxHistoryCount );
    exifArtistComboBox->setCurrentIndex(-1);
    exifArtistComboBox->lineEdit()->setText( exifStruct->artist );
    exifCopyrightComboBox->loadHistory( settings.value("copyrightMap").toMap(),
                                        settings.value("copyrightList").toList(),
                                        maxHistoryCount );
    exifCopyrightComboBox->setCurrentIndex(-1);
    exifCopyrightComboBox->lineEdit()->setText( exifStruct->copyright );
    exifUserCommentComboBox->loadHistory( settings.value("userCommentMap").toMap(),
                                          settings.value("userCommentList").toList(),
                                          maxHistoryCount );
    exifUserCommentComboBox->setCurrentIndex(-1);
    exifUserCommentComboBox->lineEdit()->setText( exifStruct->userComment );

    settings.endGroup();
}

void MetadataDialog::saveChanges() {
    // Exif tab
    // Image toolbox
    exifStruct->orientation = exifOrientationComboBox->currentIndex();
    exifStruct->originalDate = exifOriginalDateComboBox->lineEdit()->text();
    exifStruct->digitizedDate = exifDigitizedDateComboBox->lineEdit()->text();
    // Photo toolbox
    exifStruct->focalLength = exifFocalLengthSpinBox->value();
    exifStruct->expTime = exifExpTimeComboBox->currentText();
    exifStruct->expBias = exifExpBiasSpinBox->value();
    exifStruct->aperture = exifApertureSpinBox->value();
    exifStruct->shutterSpeed = exifShutterTimeComboBox->currentText();
    exifStruct->isoSpeed = exifIsoSpeedSpinBox->value();
    exifStruct->expProgram = exifExpProgramComboBox->currentIndex();
    exifStruct->meteringMode = exifLightMeteringModeComboBox->currentIndex();
    exifStruct->flashMode = MetadataUtils::Exif::flashShort(
                exifFlashModeComboBox->currentText() );
    // Camera toolbox
    exifStruct->cameraManufacturer = exifManufacturerComboBox->lineEdit()->text();
    exifStruct->cameraModel = exifModelComboBox->lineEdit()->text();
    // Author toolbox
    exifStruct->artist = exifArtistComboBox->lineEdit()->text();
    exifStruct->copyright = exifCopyrightComboBox->lineEdit()->text();
    exifStruct->userComment = exifUserCommentComboBox->lineEdit()->text();

    // saving
    metadata->setExifData();
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
