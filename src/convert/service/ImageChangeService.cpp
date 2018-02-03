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

#include "convert/service/ImageChangeService.hpp"

#include "metadata/MetadataUtils.hpp"


ImageChangeService::ImageChangeService(const QImage &image)
{
    changingImage = image;
}

QImage ImageChangeService::image() const
{
    return changingImage;
}

double ImageChangeService::rotateImage(TargetImage *targetImage)
{
    int alpha = (int)targetImage->rotationAngle();
    bool metadataSaved = false;
    if (targetImage->isSaveExifOrientationAllowed()) {
        metadataSaved = saveExifOrientation(&alpha, *targetImage);
    }
    if (!metadataSaved || targetImage->isRotateImageAllowed()) {
        if (alpha%90 == 0 && alpha%180 != 0) {
            // flip dimension variables
            QSize size = targetImage->size();
            size.transpose();
            targetImage->setSize(size);
        }
        this->changingImage = this->changingImage.transformed(
                    this->getTransformationMatrix(targetImage),
                    Qt::SmoothTransformation);
    }
    return targetImage->rotationAngle();
}

/**
 * @brief Don't rotate but save orientation tag in Exif metadata only.
 * @param alpha Working rotation angle
 * @param targetImage Target image model
 * @return Success status
 */
bool ImageChangeService::saveExifOrientation(int *alpha, const TargetImage &targetImage)
{
    bool result = false;

#ifdef SIR_METADATA_SUPPORT
    int flip;
    *alpha += MetadataUtils::Exif::rotationAngle(
                metadata.exifStruct()->orientation, &flip );
    if (*alpha == -360) {
        *alpha = 0;
        flip = MetadataUtils::None;
    }

    flip ^= targetImage.flip();

    // normalization of values alpha and flip
    if (*alpha == -270) {
        *alpha = 90;
    } else if (*alpha == 270) {
        *alpha = -90;
    } else if (*alpha == -180) {
        *alpha = 180;
    }
    if (flip != MetadataUtils::None && (int)targetImage.rotationAngle()%180 != 0) {
        flip ^= MetadataUtils::VerticalAndHorizontal;
    }

    char orientation = MetadataUtils::Exif::getOrientation(*alpha, flip);
    if (orientation < 1) {
        metadata.setExifDatum("Exif.Image.Orientation", 1);
        metadata.exifStruct()->orientation = 1;
        result = false;
    } else {
        metadata.setExifDatum("Exif.Image.Orientation", orientation);
        metadata.exifStruct()->orientation = orientation;
        result = true;
    }
#endif // SIR_METADATA_SUPPORT
    return result;
}

QTransform ImageChangeService::getTransformationMatrix(TargetImage *targetImage)
{
    QTransform transform;

#ifdef SIR_METADATA_SUPPORT
    if (targetImage->isSaveMetadataAllowed()) {
        metadata.setExifDatum("Exif.Image.Orientation",1);
        int flip;
        targetImage->incrementRotationAngle(
                    MetadataUtils::Exif::rotationAngle(
                        metadata.exifStruct()->orientation, &flip)
                    );
        if (flip == MetadataUtils::Vertical)
            transform.scale(1.0,-1.0);
        else if (flip == MetadataUtils::Horizontal)
            transform.scale(-1.0,1.0);
        else if (flip == MetadataUtils::VerticalAndHorizontal)
            targetImage->incrementRotationAngle(360);
    }
#endif // SIR_METADATA_SUPPORT

    transform.rotate(targetImage->rotationAngle());
    return transform;
}
