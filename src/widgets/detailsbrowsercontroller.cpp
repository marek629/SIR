/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2014  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://marek629.github.io/sir/
 */

#include "detailsbrowsercontroller.h"
#include "detailsbrowserview.h"
#include "convertdialog.h"
#include "../optionsenums.h"
#include "../convertshareddata.h"
#include "../detailsthumbnail.h"

using namespace sir;

// this file specifed variables
// HTML constants
const QString htmlOrigin = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" "
        "\"http://www.w3.org/TR/REC-html40/strict.dtd\">"
        "<html><head><meta name=\"qrichtext\" content=\"1\" />"
        "<style type=\"text/css\">p, li { white-space: pre-wrap; }</style>"
        "</head><body style=\" font-family:'Sans Serif';"
        "font-size:9pt; font-weight:400; font-style:normal;\">";
const QString htmlEnd = "</body></html>";
const QString htmlBr = "<br />";
const QString htmlHr = "<hr />";

DetailsBrowserController::DetailsBrowserController(TreeWidget *model,
                                                   DetailsBrowserView *view,
                                                   QObject *parent)
    : QObject(parent) {
    this->model = model;
    connect(this->model, SIGNAL(itemSelectionChanged()), this, SLOT(showDetails()));

    this->view = view;
    this->view->setController(this);
    this->view->setReadOnly(true);

    convertDialog = (ConvertDialog*)this->view->window();

    loadSettings();
}

/** Adds detailed information (including thumbnail) about \a item.
  * \param item Files tree widget item.
  * \param index Serial number of thumbnail for \a item.
  * \sa addMetadataToContent()
  */
void DetailsBrowserController::addItem(QTreeWidgetItem *item, int index) {
    DetailsThumbnail thumb(item, index, view->usableWidth());

    htmlContent += "<center><img src=\"" + thumb.filePath() + "\"";
    if (thumb.size().width() > view->usableWidth())
        htmlContent += " width=\"" + QString::number(view->usableWidth()) + "\"";
    htmlContent += "/></center>" + htmlBr;

    htmlContent += thumb.sourceFilePath() + htmlBr;

    if (thumb.isRenderedFromSVG())
        htmlContent += tr("Default image size: ");
    else
        htmlContent += tr("Image size: ");
    QSize imageSize = thumb.sourceImageSize();
    htmlContent += QString::number(imageSize.width()) + "x"
            + QString::number(imageSize.height()) + " px" + htmlBr;

    htmlContent += tr("File size: ");
    htmlContent += convertDialog->fileSizeString(thumb.sourceFileSize()) + htmlBr;

#ifdef SIR_METADATA_SUPPORT
    // TODO: exifStruct and iptcStruct should be parameters of methods - not class fields
    exifStruct = thumb.exifStructPtr();
    iptcStruct = thumb.iptcStructPtr();

    if (thumb.isReadFromMetadataThumbnail())
        htmlContent += addMetadataToContent();
#endif // SIR_METADATA_SUPPORT
}

/** Shows file details or few files summary.\n
  * This function is called when selection items changed in tree view list.\n
  * When horizontal spliters widgets containing this details widget width equal
  * 0 this function will do nothing.
  * \sa addItem()
  */
void DetailsBrowserController::showDetails() {
    if (convertDialog->horizontalSplitter->widget(1)->width() == 0)
        return;

    view->clear();
    htmlContent.clear();

    selectedFiles = convertDialog->filesTreeWidget->selectedItems();

    if (selectedFiles.length() <= 0) {
        view->setText(tr("Select image to show this one details."));
        return;
    }

    if (selectedFiles.length() == 1) {
        addItem(selectedFiles.first());
    }
    else { // many files summary
        int i = 0;
        int lastItemIndex = selectedFiles.length() - 1;
        foreach (QTreeWidgetItem *item, selectedFiles) {
            addItem(item, i);
            if (i < lastItemIndex)
                htmlContent += htmlHr;
            i++;
        }
    }

    view->setHtml(htmlOrigin + htmlContent + htmlEnd);
}

/** Loads settings.
  * \note If SIR_METADATA_SUPPORT isn't defined this function has no effect.
  */
void DetailsBrowserController::loadSettings() {
#ifdef SIR_METADATA_SUPPORT
    Settings *s = Settings::instance();
    if (s->metadata.enabled) {
        // details
        exifAuthor  = s->details.exifAuthor;
        exifCamera  = s->details.exifCamera;
        exifPhoto   = s->details.exifPhoto;
        exifImage   = s->details.exifImage;
        iptcPrint   = s->details.iptc;
    }
    else {
        exifAuthor = 0;
        exifCamera = 0;
        exifPhoto = 0;
        exifImage = 0;
        iptcPrint = 0;
    }
#endif // SIR_METADATA_SUPPORT
}

#ifdef SIR_METADATA_SUPPORT
/** Returns metadata information string using data stored in
  * exifStruct and iptcStruct structs and exifPhoto, exifImage,
  * exifAuthor, exifCamera and iptcPrint enumeration fields.
  *
  * This function is available if SIR_METADATA_SUPPORT is defined.
  *
  * \sa DetailsOptions addItem()
  */
QString DetailsBrowserController::addMetadataToContent() {
    QString content;

    content += exifContent();
    content += iptcContent();

    return content;
}

QString DetailsBrowserController::exifContent() {
    if (exifStruct->version == String::noData())
        return QString();

    QString content;

    if (exifPhoto != 0 || exifImage != 0 || exifAuthor != 0 || exifCamera != 0)
        content += htmlBr;

    content += exifImageContent();
    content += exifPhotoContent();
    content += exifCameraContent();
    content += exifAuthorContent();

    return content;
}

QString DetailsBrowserController::exifImageContent() {
    QString content;
    const ConvertSharedData &csd = convertDialog->convertSharedData();

    if (exifImage & DetailsOptions::ExifVersion)
        content += tr("Exif Version") + ": " + exifStruct->version + htmlBr;
    if (exifImage & DetailsOptions::ProcessingSoftware)
        content += tr("Processing Software") + ": " +
                exifStruct->processingSoftware + htmlBr;

    if (exifImage & DetailsOptions::Orientation)
        content += tr("Orientation") + ": " +
                MetadataUtils::Exif::orientationString(
                    exifStruct->orientation) + htmlBr;

    if (exifImage & DetailsOptions::GeneratedDateAndTime)
        content += tr("Generated Date and Time") + ": " +
                String::fromDateTimeString(
                    exifStruct->originalDate,
                    MetadataUtils::Exif::dateTimeFormat,
                    csd.dateTimeFormat) + htmlBr;
    if (exifImage & DetailsOptions::DigitizedDateAndTime)
        content += tr("Digitized Date and Time") + ": " +
                String::fromDateTimeString(
                    exifStruct->digitizedDate,
                    MetadataUtils::Exif::dateTimeFormat,
                    csd.dateTimeFormat) + htmlBr;

    return content;
}

QString DetailsBrowserController::exifPhotoContent() {
    QString content;

    if (exifPhoto & DetailsOptions::FocalLenght)
        content += tr("Focal lenght") + ": " +
                QString::number(exifStruct->focalLength,'f',1) + " mm" + htmlBr;
    if (exifPhoto & DetailsOptions::Aperture)
        content += tr("Aperture") + ": F" +
                QString::number(exifStruct->aperture,'f',1) + htmlBr;

    if (exifPhoto & DetailsOptions::ExposureTime)
        content += tr("Exposure time") + ": " + exifStruct->expTime + htmlBr;
    if (exifPhoto & DetailsOptions::ShutterSpeed)
        content += tr("Shutter Speed") + ": " + exifStruct->shutterSpeed + htmlBr;

    if (exifPhoto & DetailsOptions::ExposureBias)
        content += tr("Exposure bias") + ": " +
                QString::number(exifStruct->expBias,'f',1) + "EV" + htmlBr;
    if (exifPhoto & DetailsOptions::IsoSpeed)
        content += tr("ISO Speed") + ": " +
                QString::number(exifStruct->isoSpeed) + htmlBr;

    if (exifPhoto & DetailsOptions::ExposureProgram)
        content += tr("Exposure program") + ": " +
                MetadataUtils::Exif::expProgramString(
                    exifStruct->expProgram) + htmlBr;
    if (exifPhoto & DetailsOptions::LightMeteringMode)
        content += tr("Light metering mode") + ": " +
                MetadataUtils::Exif::meteringModeString(
                    exifStruct->meteringMode) + htmlBr;

    if (exifPhoto & DetailsOptions::FlashMode)
        content += tr("Flash mode") + ": " +
                MetadataUtils::Exif::flashString(exifStruct->flashMode) + htmlBr;

    return content;
}

QString DetailsBrowserController::exifCameraContent() {
    QString content;

    if (exifCamera & DetailsOptions::Manufacturer)
        content += tr("Camera manufacturer: ") +
                exifStruct->cameraManufacturer + htmlBr;
    if (exifCamera & DetailsOptions::Model)
        content += tr("Camera model: ") + exifStruct->cameraModel + htmlBr;

    return content;
}

QString DetailsBrowserController::exifAuthorContent() {
    QString content;

    if (exifAuthor & DetailsOptions::Artist)
        content += tr("Artist") + ": " + exifStruct->artist + htmlBr;
    if (exifAuthor & DetailsOptions::Copyright)
        content += tr("Copyright") + ": " + exifStruct->copyright + htmlBr;
    if (exifAuthor & DetailsOptions::UserComment)
        content += tr("User Comment") + ": " + exifStruct->userComment + htmlBr;

    return content;
}

QString DetailsBrowserController::iptcContent() {
    if (!MetadataUtils::Iptc::isVersionKnown())
        return QString();

    QString content;
    const ConvertSharedData &csd = convertDialog->convertSharedData();

    if (iptcPrint & DetailsOptions::ModelVersion)
        content += tr("Model version") + ": " +
                iptcStruct->modelVersion + htmlBr;

    if (iptcPrint & DetailsOptions::DateCreated)
        content += tr("Created date") + ": " +
                iptcStruct->dateCreated.toString(csd.dateFormat) + htmlBr;
    if (iptcPrint & DetailsOptions::TimeCreated)
        content += tr("Created time") + ": " +
                iptcStruct->timeCreated.toString(csd.timeFormat) + htmlBr;

    if (iptcPrint & DetailsOptions::DigitizedDate)
        content += tr("Digitized date") + ": " +
                iptcStruct->digitizationDate.toString(csd.dateFormat) + htmlBr;
    if (iptcPrint & DetailsOptions::DigitizedTime)
        content += tr("Digitized time") + ": " +
                iptcStruct->digitizationTime.toString(csd.timeFormat) + htmlBr;

    if (iptcPrint & DetailsOptions::Byline)
        content += tr("Author") + ": " + iptcStruct->byline + htmlBr;
    if (iptcPrint & DetailsOptions::CopyrightIptc)
        content += tr("Copyright") + ": " + iptcStruct->copyright + htmlBr;

    if (iptcPrint & DetailsOptions::ObjectName)
        content += tr("Object name") + ": " + iptcStruct->objectName + htmlBr;
    if (iptcPrint & DetailsOptions::Keywords)
        content += tr("Keywords") + ": " + iptcStruct->keywords + htmlBr;
    if (iptcPrint & DetailsOptions::Caption)
        content += tr("Description") + ": " + iptcStruct->caption + htmlBr;

    if (iptcPrint & DetailsOptions::CountryName)
        content += tr("Country") + ": " + iptcStruct->countryName + htmlBr;
    if (iptcPrint & DetailsOptions::City)
        content += tr("City") + ": " + iptcStruct->city + htmlBr;

    if (iptcPrint & DetailsOptions::EditStatus)
        content += tr("Edit status") + ": " + iptcStruct->editStatus + htmlBr;

    return content;
}
#endif // SIR_METADATA_SUPPORT
