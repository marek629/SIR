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

#include "convert/model/TargetImage.hpp"

#include <QFileInfo>


TargetImage::TargetImage() {}

TargetImage::TargetImage(const ImageFormat &imageFormat) : format(imageFormat) {}

ImageFormat TargetImage::imageFormat() const
{
    return format;
}

void TargetImage::setImageFormat(const ImageFormat &value)
{
    format = value;
}

bool TargetImage::isProgressiveWriteEnabled() const
{
    return progressiveWriteEnabled;
}

void TargetImage::setProgressiveWriteEnabled(bool value)
{
    progressiveWriteEnabled = value;
}

bool TargetImage::isOptimizedWriteEnabled() const
{
    return optimizedWriteEnabled;
}

void TargetImage::setOptimizedWriteEnabled(bool value)
{
    optimizedWriteEnabled = value;
}

int TargetImage::compression() const
{
    return compressionLevel;
}

void TargetImage::setCompression(int value)
{
    compressionLevel = value;
}

int TargetImage::quality() const
{
    return qualityLevel;
}

void TargetImage::setQuality(int value)
{
    qualityLevel = value;
}

unsigned int TargetImage::bytes() const
{
    return sizeBytes;
}

void TargetImage::setBytes(unsigned int value)
{
    // TODO: set value from shared.sizeBytes
    sizeBytes = value;
}

QSize TargetImage::size() const
{
    return sizePixels;
}

void TargetImage::setSize(const QSize &value)
{
    sizePixels = value;
}

int TargetImage::width() const
{
    return size().width();
}

int TargetImage::height() const
{
    return size().height();
}

bool TargetImage::hasWidth() const
{
    return width() > 0;
}

bool TargetImage::hasHeight() const
{
    return height() > 0;
}

QColor TargetImage::backgroundColor() const
{
    return bgColor;
}

void TargetImage::setBackgroundColor(const QColor &value)
{
    bgColor = value;
}

EffectsConfiguration TargetImage::effectsConfiguration() const
{
    return effectsConfig;
}

void TargetImage::setEffectsConfiguration(const EffectsConfiguration &value)
{
    effectsConfig = value;
}

double TargetImage::rotationAngle() const
{
    // TODO: set from shared.angle value
    return angle;
}

void TargetImage::setRotationAngle(double value)
{
    angle = value;
}

void TargetImage::incrementRotationAngle(double value)
{
    angle += value;
}

bool TargetImage::isUpdateThumbnailAllowed() const
{
    // TODO: set value from expression "saveMetadata && shared.updateThumbnail"
    return updateThumbnailAllowed;
}

void TargetImage::setUpdateThumbnailAllowed(bool value)
{
    updateThumbnailAllowed = value;
}

bool TargetImage::isRotateThumbnailAllowed() const
{
    return rotateThumbnailAllowed;
}

void TargetImage::setRotateThumbnailAllowed(bool value)
{
    rotateThumbnailAllowed = value;
}

bool TargetImage::isRotateImageAllowed() const
{
    return rotateImageAllowed;
}

void TargetImage::setRotateImageAllowed(bool value)
{
    rotateImageAllowed = value;
}

bool TargetImage::isRotateEnabled() const
{
    return rotateEnabled;
}

void TargetImage::setRotateEnabled(bool value)
{
    rotateEnabled = value;
}

int TargetImage::flip() const
{
    return flipCode;
}

void TargetImage::setFlip(int value)
{
    flipCode = value;
}

QString TargetImage::filePath() const
{
    return path;
}

void TargetImage::setFilePath(const QString &value)
{
    path = value;
}

QStringList TargetImage::imageDataStringList() const
{
    QFileInfo fileInfo(filePath());

    QStringList result;
    result << fileInfo.baseName() << fileInfo.suffix() << fileInfo.absolutePath();

    return result;
}

bool TargetImage::isSaveMetadataAllowed() const
{
#ifdef SIR_METADATA_SUPPORT
    return saveMetadataAllowed;
#else
    return false;
#endif // SIR_METADATA_SUPPORT
}

void TargetImage::setSaveMetadataAllowed(bool value)
{
#ifdef SIR_METADATA_SUPPORT
    saveMetadataAllowed = value;
#endif // SIR_METADATA_SUPPORT
}

bool TargetImage::isSaveExifOrientationAllowed() const
{
#ifdef SIR_METADATA_SUPPORT
    bool isAllowed = !this->isRotateImageAllowed();
    if (isAllowed && this->rotationAngle() % 90 != 0) {
        isAllowed = false;
    }
    return isSaveMetadataAllowed() && isAllowed;
#else
    return false;
#endif // SIR_METADATA_SUPPORT
}
