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

#ifndef DETAILSTHUMBNAIL_H
#define DETAILSTHUMBNAIL_H

#include "metadata/Exif.hpp"
#include "metadata/Iptc.hpp"
#include "metadata/structs/ExifStruct.hpp"
#include "metadata/structs/IptcStruct.hpp"

#include <QSize>
#include <QString>

class FileInfo;
class Settings;


/*! Class used in DetailsBrowserController for generate and write image thumbnail. */
class DetailsThumbnail {
public:
    DetailsThumbnail(Settings *settings);

    bool isRenderedFromSVG() const;
    QSize size() const;
    QString filePath() const;
    QSize sourceImageSize() const;
    QString sourceFilePath() const;
    qint64 sourceFileSize() const;

    void writeThumbnail(const FileInfo &fileInfo, int index, int maxWidth);

#ifdef SIR_METADATA_SUPPORT
    bool isReadFromMetadataThumbnail() const;
    MetadataUtils::ExifStruct *exifStruct();
    MetadataUtils::IptcStruct *iptcStruct();
#endif // SIR_METADATA_SUPPORT

private:
    bool isSvg;
    QString thumbPath;
    QString imagePath;
    QSize imageSize;
    QSize thumbSize;

    const char *thumbnailFileExtension = ".jpg";
    const char *thumbnailFileFormat = "JPEG";

#ifdef SIR_METADATA_SUPPORT
    bool metadataEnabled;
    MetadataUtils::ExifStruct exifStruct_;
    MetadataUtils::IptcStruct iptcStruct_;
#endif // SIR_METADATA_SUPPORT

    bool writeThumbnailFromMetadata();
    void writeThumbnailFromImageData(int maxWidth);
    void writeThumbnailFromSVG(int maxWidth);
};

#endif // DETAILSTHUMBNAIL_H
