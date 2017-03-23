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

#include "convert/service/ImageFileService.hpp"

#include "convert/model/WriteImageFormat.hpp"
#include "image/ImageWriter.hpp"


ImageFileService::ImageFileService(const QImage &image, const TargetImage &targetImage)
{
    this->image = image;
    this->targetImageModel = targetImage;
}

bool ImageFileService::writeImage(const QImage &image)
{
    QString targetFilePath = targetImageModel.filePath();
    ImageWriter writer(targetFilePath);
    WriteImageFormat format(targetFilePath.split('.').last());

    if (format.supportsQuality()) {
        writer.setQuality(targetImageModel.quality());
    }
    // TODO: use data from user forms input
    if (format.supportsProgressiveScanWrite()) {
        writer.enableProgressiveScanWrite();
    }
    if (format.supportsOptimizedWrite()) {
        writer.enableOptimizedWrite();
    }
    if (format.supportsCompression()) {
        writer.setCompression(0);
    }

    return writer.write(image);
}

void ImageFileService::updateThumbnail(int rotationAngle)
{
    if (!targetImageModel.isUpdateThumbnailAllowed()) {
        return;
    }

    MetadataUtils::ExifStruct *exifStruct = metadata.exifStruct();
    int w = exifStruct->thumbnailWidth.split(' ').first().toInt();
    int h = exifStruct->thumbnailHeight.split(' ').first().toInt();
    QImage tmpImg = image.scaled(w,h, Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation);
    bool specialRotate = targetImageModel.isRotateThumbnailAllowed() && rotationAngle%90 != 0;
    QImage *thumbnail = &exifStruct->thumbnailImage;
    if (specialRotate) {
        w = tmpImg.width();
        h = tmpImg.height();
        *thumbnail = tmpImg;
    } else {
        if (targetImageModel.backgroundColor().isValid()) {
            thumbnail->fill(targetImageModel.backgroundColor().rgb());
        } else {
            thumbnail->fill(Qt::black);
        }
        QPoint begin ( (w-tmpImg.width())/2, (h-tmpImg.height())/2 );
        for (int i=0, y=begin.y(); i<tmpImg.height(); i++, y++) {
            for (int j=0, x=begin.x(); j<tmpImg.width(); j++, x++) {
                thumbnail->setPixel(x, y, tmpImg.pixel(j,i));
            }
        }
    }

    // rotate thumbnail
    // TODO: check if set isRotateThumbnailAllowed value from shared.rotateThumbnail
    if (targetImageModel.isRotateThumbnailAllowed() && !specialRotate) {
        QTransform transform;
        int flip;
        transform.rotate(MetadataUtils::Exif::rotationAngle(
                             exifStruct->orientation, &flip));
        if (flip == MetadataUtils::Vertical) {
            transform.scale(1.0, -1.0);
        } else if (flip == MetadataUtils::Horizontal) {
            transform.scale(-1.0, 1.0);
        }
        *thumbnail = thumbnail->transformed(transform);
    }

    if (!metadata.setExifThumbnail(thumbnail, tid)) {
        printError();
    }
}

void ImageFileService::printError()
{
#ifdef SIR_METADATA_SUPPORT
    MetadataUtils::Error *error = metadata.lastError();
    qWarning() << "Metadata error within thread" << tid << '\n'
               << "    " << error->message() << '\n'
               << "    " << tr("Error code:") << error->code() << '\n'
               << "    " << error->what();
    delete error;
#endif // SIR_METADATA_SUPPORT
}
