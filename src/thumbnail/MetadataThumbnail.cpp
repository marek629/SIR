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

#include "thumbnail/MetadataThumbnail.hpp"

#include "metadata/Metadata.hpp"
#include "Settings.hpp"


MetadataThumbnail::MetadataThumbnail() {}

MetadataThumbnail::MetadataThumbnail(const QString &imagePath,
                                     const QString &thumbPath)
{
    this->imagePath = imagePath;
    this->thumbPath = thumbPath;
}

bool MetadataThumbnail::writeThumbnail(bool isMetadataEnabled)
{
    isThumbnailSavedSuccessfully = false;

#ifdef SIR_METADATA_SUPPORT
    if (isMetadataEnabled) {
        MetadataUtils::Metadata metadata;
        if (metadata.read(imagePath, true)) {
            exifStruct_ = metadata.exifStruct()->copy();
            iptcStruct_ = metadata.iptcStruct()->copy();
            Exiv2::Image::AutoPtr image = metadata.imageAutoPtr();
            imageSize = QSize(image->pixelWidth(), image->pixelHeight());
            Exiv2::PreviewManager previewManager (*image);
            Exiv2::PreviewPropertiesList previewList = previewManager.
                    getPreviewProperties();
            if (previewList.empty()) {
                return isThumbnailSavedSuccessfully;
            }
            else { // read from metadata thumnail
                Exiv2::PreviewImage preview = previewManager.getPreviewImage(
                            previewList[0]);
                preview.writeFile(thumbPath.toStdString());
                thumbPath += preview.extension().c_str();
                thumbSize.setWidth(preview.width());
                thumbSize.setHeight(preview.height());
            }
            isThumbnailSavedSuccessfully = true;
            return isThumbnailSavedSuccessfully;
        }
        else {
            return isThumbnailSavedSuccessfully;
        }
    }
#endif // SIR_METADATA_SUPPORT

    return isThumbnailSavedSuccessfully;
}

bool MetadataThumbnail::isThumbnailSaved() const
{
    return isThumbnailSavedSuccessfully;
}

#ifdef SIR_METADATA_SUPPORT
MetadataUtils::ExifStruct *MetadataThumbnail::exifStruct()
{
    return &exifStruct_;
}

MetadataUtils::IptcStruct *MetadataThumbnail::iptcStruct()
{
    return &iptcStruct_;
}
#endif // SIR_METADATA_SUPPORT

QSize MetadataThumbnail::size() const
{
    return thumbSize;
}

QString MetadataThumbnail::filePath() const
{
    return thumbPath;
}

QSize MetadataThumbnail::sourceImageSize() const
{
    return imageSize;
}
