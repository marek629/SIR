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

#include <QScrollBar>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QPainter>

#include "detailsbrowsercontroller.h"
#include "detailsbrowserview.h"
#include "convertdialog.h"
#include "../optionsenums.h"
#include "../convertshareddata.h"

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
    QSize imageSize;
    bool isSvg = false;

#ifdef SIR_METADATA_SUPPORT
    Settings *s = Settings::instance();
    bool metadataEnabled(s->metadata.enabled);
    MetadataUtils::Metadata metadata;
    exifStruct = 0;
    iptcStruct = 0;
#endif // SIR_METADATA_SUPPORT

    QString ext = item->text(ExtColumn);

    String imagePath = item->text(PathColumn) + QDir::separator()
            + item->text(NameColumn) + '.' + ext;

    QString thumbPath = QDir::tempPath() + QDir::separator() + "sir_thumb_"
            + QString::number(index);
    QSize thumbSize;

    ext = ext.toUpper();

    // thumbnail generation
    if (ext != "SVG" && ext != "SVGZ") {
#ifdef SIR_METADATA_SUPPORT
        bool fromData(!s->metadata.enabled);
        if (!fromData) {
            if (!metadata.read(imagePath, true))
                fromData = true;
            else {
                exifStruct = metadata.exifStruct();
                iptcStruct = metadata.iptcStruct();
                Exiv2::Image::AutoPtr image = metadata.imageAutoPtr();
                imageSize = QSize(image->pixelWidth(), image->pixelHeight());
                Exiv2::PreviewManager previewManager (*image);
                Exiv2::PreviewPropertiesList previewList = previewManager.
                        getPreviewProperties();
                if (!previewList.empty()) { // read from metadata thumnail
                    Exiv2::PreviewImage preview = previewManager.getPreviewImage(
                                previewList[0]);
                    preview.writeFile(thumbPath.toStdString());
                    thumbPath += preview.extension().c_str();
                    thumbSize.setWidth(preview.width());
                    thumbSize.setHeight(preview.height());
                }
                else
                    fromData = true;
            }
        }
#else
        bool fromData(true);
#endif // SIR_METADATA_SUPPORT
        if (fromData) { // generate from image data
            QImage img(imagePath);
            if (!imageSize.isValid())
                imageSize = img.size();
            thumbPath += ".tif";
            QImage thumbnail;
            if (img.width() > view->usableWidth())
                thumbnail = img.scaledToWidth(view->usableWidth(),
                                              Qt::SmoothTransformation);
            else
                thumbnail = img;
            thumbnail.save(thumbPath, "TIFF");
            thumbSize = thumbnail.size();
        }
    }
    else { // render from SVG file
        isSvg = true;
#ifdef SIR_METADATA_SUPPORT
        metadataEnabled = false;
#endif // SIR_METADATA_SUPPORT
        QGraphicsSvgItem svg(imagePath);
        QSvgRenderer *renderer = svg.renderer();
        QSize size = renderer->defaultSize();
        imageSize = size;
        double sizeRatio = (double) view->usableWidth() / size.width();
        size *= sizeRatio;
        QImage thumbnail (size, QImage::Format_ARGB32);
        thumbnail.fill(Qt::transparent);
        QPainter painter (&thumbnail);
        renderer->render(&painter);
        thumbPath += ".tif";
        thumbnail.save(thumbPath, "TIFF");
    }

    htmlContent += "<center><img src=\"" + thumbPath + "\"";
    if (thumbSize.width() > view->usableWidth())
        htmlContent += " width=\"" + QString::number(view->usableWidth()) + "\"";
    htmlContent += "/></center>" + htmlBr;

    htmlContent += imagePath + htmlBr;
    if (isSvg)
        htmlContent += tr("Default image size: ");
    else
        htmlContent += tr("Image size: ");
    htmlContent += QString::number(imageSize.width()) + "x"
            + QString::number(imageSize.height()) + " px" + htmlBr;

    QFileInfo info(imagePath);
    htmlContent += tr("File size: ");
    htmlContent += convertDialog->fileSizeString(info.size()) + htmlBr;

#ifdef SIR_METADATA_SUPPORT
    if (metadataEnabled)
        addMetadataToContent();
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
/** Appends metadata information string into htmlContent string using data
  * stored in exifStruct and iptcStruct structs and exifPhoto, exifImage,
  * exifAuthor, exifCamera and iptcPrint enumeration fields.\n
  * This function is available if SIR_METADATA_SUPPORT is defined.
  * \sa DetailsOptions addItem()
  */
void DetailsBrowserController::addMetadataToContent() {
    const ConvertSharedData &csd = convertDialog->convertSharedData();

    if (exifStruct->version != String::noData()) {
        if (exifPhoto != 0 || exifImage != 0 || exifAuthor != 0 || exifCamera != 0)
            htmlContent += htmlBr;
        // exif image
        if (exifImage & DetailsOptions::ExifVersion)
            htmlContent += tr("Exif Version") + ": " + exifStruct->version
                    + htmlBr;
        if (exifImage & DetailsOptions::ProcessingSoftware)
            htmlContent += tr("Processing Software") + ": " +
                    exifStruct->processingSoftware + htmlBr;
        if (exifImage & DetailsOptions::Orientation)
            htmlContent += tr("Orientation") + ": " +
                    MetadataUtils::Exif::orientationString(
                        exifStruct->orientation) + htmlBr;
        if (exifImage & DetailsOptions::GeneratedDateAndTime)
            htmlContent += tr("Generated Date and Time") + ": " +
                    String::fromDateTimeString(
                        exifStruct->originalDate,
                        MetadataUtils::Exif::dateTimeFormat,
                        csd.dateTimeFormat) + htmlBr;
        if (exifImage & DetailsOptions::DigitizedDateAndTime)
            htmlContent += tr("Digitized Date and Time") + ": " +
                    String::fromDateTimeString(
                        exifStruct->digitizedDate,
                        MetadataUtils::Exif::dateTimeFormat,
                        csd.dateTimeFormat) + htmlBr;
        // exif photo
        if (exifPhoto & DetailsOptions::FocalLenght)
            htmlContent += tr("Focal lenght") + ": " +
                    QString::number(exifStruct->focalLength,'f',1) + " mm"
                    + htmlBr;
        if (exifPhoto & DetailsOptions::Aperture)
            htmlContent += tr("Aperture") + ": F" +
                    QString::number(exifStruct->aperture,'f',1) + htmlBr;
        if (exifPhoto & DetailsOptions::ExposureTime)
            htmlContent += tr("Exposure time") + ": " + exifStruct->expTime
                    + htmlBr;
        if (exifPhoto & DetailsOptions::ShutterSpeed)
            htmlContent += tr("Shutter Speed") + ": " +
                    exifStruct->shutterSpeed + htmlBr;
        if (exifPhoto & DetailsOptions::ExposureBias)
            htmlContent += tr("Exposure bias") + ": " +
                    QString::number(exifStruct->expBias,'f',1) + "EV" + htmlBr;
        if (exifPhoto & DetailsOptions::IsoSpeed)
            htmlContent += tr("ISO Speed") + ": " +
                    QString::number(exifStruct->isoSpeed) + htmlBr;
        if (exifPhoto & DetailsOptions::ExposureProgram)
            htmlContent += tr("Exposure program") + ": " +
                    MetadataUtils::Exif::expProgramString(
                        exifStruct->expProgram) + htmlBr;
        if (exifPhoto & DetailsOptions::LightMeteringMode)
            htmlContent += tr("Light metering mode") + ": " +
                    MetadataUtils::Exif::meteringModeString(
                        exifStruct->meteringMode) + htmlBr;
        if (exifPhoto & DetailsOptions::FlashMode)
            htmlContent += tr("Flash mode") + ": " +
                    MetadataUtils::Exif::flashString(exifStruct->flashMode)
                    + htmlBr;
        // exif camera
        if (exifCamera & DetailsOptions::Manufacturer)
            htmlContent += tr("Camera manufacturer: ") +
                    exifStruct->cameraManufacturer + htmlBr;
        if (exifCamera & DetailsOptions::Model)
            htmlContent += tr("Camera model: ") + exifStruct->cameraModel
                    + htmlBr;
        // exif author
        if (exifAuthor & DetailsOptions::Artist)
            htmlContent += tr("Artist") + ": " + exifStruct->artist + htmlBr;
        if (exifAuthor & DetailsOptions::Copyright)
            htmlContent += tr("Copyright") + ": " + exifStruct->copyright
                    + htmlBr;
        if (exifAuthor & DetailsOptions::UserComment)
            htmlContent += tr("User Comment") + ": " + exifStruct->userComment
                    + htmlBr;
    }

    htmlContent += iptcContent();
}

QString DetailsBrowserController::iptcContent() {
    if (MetadataUtils::Iptc::isVersionKnown())
        return "";

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
