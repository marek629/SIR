#include "metadatadialog.h"
#include "metadatautils.h"
#include <QLineEdit>
#include <QSettings>

MetadataDialog::MetadataDialog(QWidget *parent, QStringList *images,
                               int currentImage) :
    QDialog(parent)
{
    setupUi(this);
    this->images = images;
    this->currentImage = currentImage;
    this->imagePath = this->images->at(this->currentImage);
    metadata = new MetadataUtils::Metadata();
    metadata->read(imagePath);
    setupValues();

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

MetadataDialog::~MetadataDialog()
{
    delete metadata;
    delete images;
}

void MetadataDialog::setupValues()
{
    MetadataUtils::ExifStruct *exifStruct = metadata->ptrExifStruct();
    QSettings settings("SIR");
    settings.beginGroup("Exif");
    int maxHistoryCount = settings.value("maxHistoryCount", 5).toInt();

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

    exifThumbnailLabel->setPixmap( exifStruct->thumbnailPixmap );
    exifThumbnailWidthLabel->setText( exifStruct->thumbnailWidth );
    exifThumbnailHeightLabel->setText( exifStruct->thumbnailHeight );

    exifFocalLengthSpinBox->setSpecialValueText(MetadataUtils::Metadata::noData());
    exifFocalLengthSpinBox->setValue( exifStruct->focalLength );
    exifExpTimeComboBox->insertItem(0,MetadataUtils::Metadata::noData());
    if (exifStruct->expTime.isEmpty())
        exifExpTimeComboBox->setCurrentIndex(0);
    else
    {
        int idx = exifExpTimeComboBox->findText( exifStruct->expTime );
        if (idx != -1)
            exifExpTimeComboBox->setCurrentIndex( idx );
        else
        {
            exifExpTimeComboBox->addItem( exifStruct->expTime );
            // TODO: add item to right index
        }
    }
    exifExpBiasSpinBox->setSpecialValueText(MetadataUtils::Metadata::noData());
    exifExpBiasSpinBox->setValue( exifStruct->expBias );
    exifApertureSpinBox->setSpecialValueText(MetadataUtils::Metadata::noData());
    exifApertureSpinBox->setValue( exifStruct->aperture );
    exifShutterSpeedComboBox->insertItem(0,MetadataUtils::Metadata::noData());
    if (exifStruct->shutterSpeed.isEmpty())
        exifShutterSpeedComboBox->setCurrentIndex(0);
    else
    {
        int idx = exifShutterSpeedComboBox->findText( exifStruct->shutterSpeed );
        if (idx != -1)
            exifShutterSpeedComboBox->setCurrentIndex( idx );
        else
        {
            exifExpTimeComboBox->addItem( exifStruct->expTime );
            // TODO: add item to right index
        }
    }
    exifIsoSpeedSpinBox->setSpecialValueText(MetadataUtils::Metadata::noData());
    exifIsoSpeedSpinBox->setValue( exifStruct->isoSpeed );
    exifExpProgramComboBox->setCurrentIndex( exifStruct->expProgram );
    exifLightMeteringModeComboBox->setCurrentIndex( exifStruct->meteringMode );

    exifManufacturerComboBox->lineEdit()->setText( exifStruct->cameraManufacturer );
    exifModelComboBox->lineEdit()->setText( exifStruct->cameraModel );

    settings.endGroup();
}
