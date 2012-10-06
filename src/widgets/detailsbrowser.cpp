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

#include <QScrollBar>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QPainter>
#include "detailsbrowser.h"
#include "convertdialog.h"
#include "optionsenums.h"

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
// settings object reference
Settings &settings = Settings::instance();

DetailsBrowser::DetailsBrowser(QWidget *parent) : QTextEdit(parent) {
    setReadOnly(true);
    convertDialog = (ConvertDialog*)parent->window();
    loadSettings();
}

/** Shows file details or few files summary.\n
  * This function is called when selection items changed in tree view list.\n
  * When horizontal spliters widgets containing this details widget width equal
  * 0 this function will do nothing.
  * \sa addItem()
  */
void DetailsBrowser::showDetails() {
    if (convertDialog->horizontalSplitter->widget(1)->width() == 0)
        return;
    this->clear();
    htmlContent.clear();
    selectedFiles = convertDialog->filesTreeWidget->selectedItems();
    if (selectedFiles.length() <= 0) {
        this->setText(tr("Select image to show this one details."));
        return;
    }

    if (selectedFiles.length() == 1) {
        addItem(selectedFiles.first());
    }
    else { // many files summary
        int i = 0;
        int lastItemIndex = selectedFiles.length() - 1;
        foreach (QTreeWidgetItem *item, selectedFiles) {
            addItem(item,i);
            if (i < lastItemIndex)
                htmlContent += htmlHr;
            i++;
        }
    }
    this->setHtml(htmlOrigin + htmlContent + htmlEnd);
}

/** Adds detailed information (including thumbnail) about \a item.
  * \param item Files tree widget item.
  * \param index Serial number of thumbnail for \a item.
  * \sa addMetadataToContent()
  */
void DetailsBrowser::addItem(QTreeWidgetItem *item, int index) {
    QSize imageSize;
    bool isSvg = false;
#ifdef SIR_METADATA_SUPPORT
    bool metadataEnabled(settings.metadata.enabled);
    MetadataUtils::Metadata metadata;
    exifStruct = 0;
    iptcStruct = 0;
#endif // SIR_METADATA_SUPPORT
    QString ext = item->text(ExtColumn);
    MetadataUtils::String imagePath = item->text(PathColumn) + QDir::separator()
            + item->text(NameColumn) + '.' + ext;
    QString thumbPath = QDir::tempPath() + QDir::separator() + "sir_thumb_"
            + QString::number(index);
    ext = ext.toUpper();
    // thumbnail generation
    if (ext != "SVG" && ext != "SVGZ") {
#ifdef SIR_METADATA_SUPPORT
        bool fromData(!settings.metadata.enabled);
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
            if (img.width() > usableWidth_)
                thumbnail = img.scaledToWidth(usableWidth_,
                                              Qt::SmoothTransformation);
            else
                thumbnail = img;
            thumbnail.save(thumbPath, "TIFF");
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
        double sizeRatio = (double) usableWidth_ / size.width();
        size *= sizeRatio;
        QImage thumbnail (size, QImage::Format_ARGB32);
        thumbnail.fill(Qt::transparent);
        QPainter painter (&thumbnail);
        renderer->render(&painter);
        thumbPath += ".tif";
        thumbnail.save(thumbPath, "TIFF");
    }
    htmlContent += "<center><img src=\"" + thumbPath + "\" /></center>" + htmlBr;
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

/** Loads settings.
  * \note If SIR_METADATA_SUPPORT isn't defined this function has no effect.
  */
void DetailsBrowser::loadSettings() {
#ifdef SIR_METADATA_SUPPORT
    if (settings.metadata.enabled) {
        // details
        exifAuthor  = settings.details.exifAuthor;
        exifCamera  = settings.details.exifCamera;
        exifPhoto   = settings.details.exifPhoto;
        exifImage   = settings.details.exifImage;
        iptcPrint   = settings.details.iptc;
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
void DetailsBrowser::addMetadataToContent() {
    if (exifStruct->version != MetadataUtils::String::noData()) {
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
                    MetadataUtils::String::fromDateTimeString(
                        exifStruct->originalDate,
                        MetadataUtils::Exif::dateTimeFormat,
                        convertDialog->dateTimeFormat) + htmlBr;
        if (exifImage & DetailsOptions::DigitizedDateAndTime)
            htmlContent += tr("Digitized Date and Time") + ": " +
                    MetadataUtils::String::fromDateTimeString(
                        exifStruct->digitizedDate,
                        MetadataUtils::Exif::dateTimeFormat,
                        convertDialog->dateTimeFormat) + htmlBr;
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
    if (MetadataUtils::Iptc::isVersionKnown()) {
        if (iptcPrint & DetailsOptions::ModelVersion)
            htmlContent += tr("Model version") + ": " +
                    iptcStruct->modelVersion + htmlBr;
        if (iptcPrint & DetailsOptions::DateCreated)
            htmlContent += tr("Created date") + ": " +
                    iptcStruct->dateCreated.toString(convertDialog->dateFormat) + htmlBr;
        if (iptcPrint & DetailsOptions::TimeCreated)
            htmlContent += tr("Created time") + ": " +
                    iptcStruct->timeCreated.toString(convertDialog->timeFormat) + htmlBr;
        if (iptcPrint & DetailsOptions::DigitizedDate)
            htmlContent += tr("Digitized date") + ": " +
                    iptcStruct->digitizationDate.toString(convertDialog->dateFormat)
                    + htmlBr;
        if (iptcPrint & DetailsOptions::DigitizedTime)
            htmlContent += tr("Digitized time") + ": " +
                    iptcStruct->digitizationTime.toString(convertDialog->timeFormat)
                    + htmlBr;
        if (iptcPrint & DetailsOptions::Byline)
            htmlContent += tr("Author") + ": " + iptcStruct->byline + htmlBr;
        if (iptcPrint & DetailsOptions::CopyrightIptc)
            htmlContent += tr("Copyright") + ": " + iptcStruct->copyright
                    + htmlBr;
        if (iptcPrint & DetailsOptions::ObjectName)
            htmlContent += tr("Object name") + ": " + iptcStruct->objectName
                    + htmlBr;
        if (iptcPrint & DetailsOptions::Keywords)
            htmlContent += tr("Keywords") + ": " + iptcStruct->keywords
                    + htmlBr;
        if (iptcPrint & DetailsOptions::Caption)
            htmlContent += tr("Description") + ": " + iptcStruct->caption
                    + htmlBr;
        if (iptcPrint & DetailsOptions::CountryName)
            htmlContent += tr("Country") + ": " + iptcStruct->countryName
                    + htmlBr;
        if (iptcPrint & DetailsOptions::City)
            htmlContent += tr("City") + ": " + iptcStruct->city + htmlBr;
        if (iptcPrint & DetailsOptions::EditStatus)
            htmlContent += tr("Edit status") + ": " + iptcStruct->editStatus
                    + htmlBr;
    }
}
#endif // SIR_METADATA_SUPPORT

/** Calls QTextEdit::resizeEvent() and changes usableWidth property. */
void DetailsBrowser::resizeEvent(QResizeEvent *e) {
    QTextEdit::resizeEvent(e);
    // vertical scroll bars width (16 px) + horizontal margins (16 px) = 32 px
    usableWidth_ = this->width() - 32;
}
