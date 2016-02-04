/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2016  Marek Jędryka   <jedryka89@gmail.com>
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

#include "DetailsThumbnail.hpp"

#include "optionsenums.h"
#include "Settings.hpp"
#include "file/FileInfo.hpp"
#include "widgets/ConvertDialog.hpp"

#include <QTreeWidgetItem>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QPainter>
#include <QDir>

using namespace sir;


DetailsThumbnail::DetailsThumbnail(Settings *settings)
{
    isSvg = false;
}

bool DetailsThumbnail::isRenderedFromSVG() const {
    return isSvg;
}

QSize DetailsThumbnail::size() const {
    return thumbSize;
}

QString DetailsThumbnail::filePath() const {
    return thumbPath;
}

QSize DetailsThumbnail::sourceImageSize() const {
    return imageSize;
}

QString DetailsThumbnail::sourceFilePath() const {
    return imagePath;
}

qint64 DetailsThumbnail::sourceFileSize() const {
    QFileInfo info(imagePath);
    return info.size();
}

#ifdef SIR_METADATA_SUPPORT
bool DetailsThumbnail::isReadFromMetadataThumbnail() const
{
    return metadataThumbnail.isThumbnailSaved();
}

MetadataUtils::ExifStruct *DetailsThumbnail::exifStruct()
{
    return metadataThumbnail.exifStruct();
}

MetadataUtils::IptcStruct *DetailsThumbnail::iptcStruct()
{
    return metadataThumbnail.iptcStruct();
}
#endif // SIR_METADATA_SUPPORT

bool DetailsThumbnail::writeThumbnailFromMetadata()
{
    metadataThumbnail = MetadataThumbnail(imagePath, thumbPath);
#ifdef SIR_METADATA_SUPPORT
    Settings *settings = Settings::instance();
    bool isMetadataEnabled = settings->metadata.enabled;
#else
    bool isMetadataEnabled = false;
#endif // SIR_METADATA_SUPPORT
    return metadataThumbnail.writeThumbnail(isMetadataEnabled);
}

void DetailsThumbnail::writeThumbnail(const FileInfo &fileInfo, int index,
                                      int maxWidth)
{
    QString ext = fileInfo.extension();

    imagePath = fileInfo.fullPath();

    thumbPath = QDir::tempPath() + QDir::separator() + "sir_thumb_"
            + QString::number(index);

    ext = ext.toUpper();

    // thumbnail generation
    if (ext != "SVG" && ext != "SVGZ") {
        bool isThumbSaved = writeThumbnailFromMetadata();
        if (isThumbSaved) {
            imageSize = metadataThumbnail.sourceImageSize();
            thumbPath = metadataThumbnail.filePath();
            thumbSize = metadataThumbnail.size();
        }
        else {
            writeThumbnailFromImageData(maxWidth);
        }
    }
    else { // render from SVG file
        isSvg = true;
        writeThumbnailFromSVG(maxWidth);
    }
}

void DetailsThumbnail::writeThumbnailFromImageData(int maxWidth)
{
    QImage img(imagePath);

    if (!imageSize.isValid())
        imageSize = img.size();
    thumbPath += thumbnailFileExtension;

    QImage thumbnail;
    if (img.width() > maxWidth)
        thumbnail = img.scaledToWidth(maxWidth, Qt::SmoothTransformation);
    else
        thumbnail = img;
    thumbnail.save(thumbPath, thumbnailFileFormat);

    thumbSize = thumbnail.size();
}

void DetailsThumbnail::writeThumbnailFromSVG(int maxWidth)
{
    QGraphicsSvgItem svg(imagePath);
    QSvgRenderer *renderer = svg.renderer();
    QSize size = renderer->defaultSize();
    imageSize = size;

    double sizeRatio = (double) maxWidth / size.width();
    size *= sizeRatio;

    QImage thumbnail (size, QImage::Format_ARGB32);
    thumbnail.fill(Qt::transparent);
    QPainter painter (&thumbnail);
    renderer->render(&painter);

    thumbPath += thumbnailFileExtension;
    thumbnail.save(thumbPath, thumbnailFileFormat);
}
