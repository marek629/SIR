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

#include "metadata/visitors/ExifRichTextVisitor.hpp"
#include "sir_String.hpp"
#include "optionsenums.h"

using namespace MetadataUtils;
using namespace sir;

ExifRichTextVisitor::ExifRichTextVisitor(int exifAuthor, int exifCamera,
                                         int exifImage, int exifPhoto) {
    this->exifAuthor = exifAuthor;
    this->exifCamera = exifCamera;
    this->exifImage = exifImage;
    this->exifPhoto = exifPhoto;
}

QString ExifRichTextVisitor::visit(const MetadataStruct &metadataStruct) {
    const ExifStruct &exifStruct = (const ExifStruct &) metadataStruct;
    if (exifStruct.version == String::noData())
        return QString();

    QString content;

    if (exifPhoto != 0 || exifImage != 0 || exifAuthor != 0 || exifCamera != 0)
        content += htmlBr;

    content += imageContent(exifStruct);
    content += photoContent(exifStruct);
    content += cameraContent(exifStruct);
    content += authorContent(exifStruct);

    return content;
}

QString ExifRichTextVisitor::authorContent(const ExifStruct &exifStruct) {
    QString content;

    if (exifAuthor & DetailsOptions::Artist)
        content += tr("Artist") + ": " + exifStruct.artist + htmlBr;
    if (exifAuthor & DetailsOptions::Copyright)
        content += tr("Copyright") + ": " + exifStruct.copyright + htmlBr;
    if (exifAuthor & DetailsOptions::UserComment)
        content += tr("User Comment") + ": " + exifStruct.userComment + htmlBr;

    return content;
}

QString ExifRichTextVisitor::cameraContent(const ExifStruct &exifStruct) {
    QString content;

    if (exifCamera & DetailsOptions::Manufacturer)
        content += tr("Camera manufacturer: ") +
                exifStruct.cameraManufacturer + htmlBr;
    if (exifCamera & DetailsOptions::Model)
        content += tr("Camera model: ") + exifStruct.cameraModel + htmlBr;

    return content;
}

QString ExifRichTextVisitor::imageContent(const ExifStruct &exifStruct) {
    QString content;
    const ConvertSharedData &csd = convertDialog->convertSharedData();

    if (exifImage & DetailsOptions::ExifVersion)
        content += tr("Exif Version") + ": " + exifStruct.version + htmlBr;
    if (exifImage & DetailsOptions::ProcessingSoftware)
        content += tr("Processing Software") + ": " +
                exifStruct.processingSoftware + htmlBr;

    if (exifImage & DetailsOptions::Orientation)
        content += tr("Orientation") + ": " +
                MetadataUtils::Exif::orientationString(
                    exifStruct.orientation) + htmlBr;

    if (exifImage & DetailsOptions::GeneratedDateAndTime)
        content += tr("Generated Date and Time") + ": " +
                String::fromDateTimeString(
                    exifStruct.originalDate,
                    MetadataUtils::Exif::dateTimeFormat,
                    csd.dateTimeFormat) + htmlBr;
    if (exifImage & DetailsOptions::DigitizedDateAndTime)
        content += tr("Digitized Date and Time") + ": " +
                String::fromDateTimeString(
                    exifStruct.digitizedDate,
                    MetadataUtils::Exif::dateTimeFormat,
                    csd.dateTimeFormat) + htmlBr;

    return content;
}

QString ExifRichTextVisitor::photoContent(const ExifStruct &exifStruct) {
    QString content;

    if (exifPhoto & DetailsOptions::FocalLenght)
        content += tr("Focal lenght") + ": " +
                QString::number(exifStruct.focalLength,'f',1) + " mm" + htmlBr;
    if (exifPhoto & DetailsOptions::Aperture) {
        content += tr("Aperture") + ": ";
        if (exifStruct.aperture == -1)
            content += String::noData();
        else
            content += "F" + QString::number(exifStruct.aperture,'f',1);
        content += htmlBr;
    }

    if (exifPhoto & DetailsOptions::ExposureTime) {
        content += tr("Exposure time") + ": ";
        if (exifStruct.expTime == "1/-1 s")
            content += String::noData();
        else
            content += exifStruct.expTime;
        content += htmlBr;
    }
    if (exifPhoto & DetailsOptions::ShutterSpeed)
        content += tr("Shutter Speed") + ": " + exifStruct.shutterSpeed + htmlBr;

    if (exifPhoto & DetailsOptions::ExposureBias) {
        content += tr("Exposure bias") + ": ";
        if (exifStruct.expBias == -101)
            content += String::noData();
        else
            content += QString::number(exifStruct.expBias,'f',1) + "EV";
        content += htmlBr;
    }
    if (exifPhoto & DetailsOptions::IsoSpeed)
        content += tr("ISO Speed") + ": " +
                QString::number(exifStruct.isoSpeed) + htmlBr;

    if (exifPhoto & DetailsOptions::ExposureProgram)
        content += tr("Exposure program") + ": " +
                MetadataUtils::Exif::expProgramString(
                    exifStruct.expProgram) + htmlBr;
    if (exifPhoto & DetailsOptions::LightMeteringMode)
        content += tr("Light metering mode") + ": " +
                MetadataUtils::Exif::meteringModeString(
                    exifStruct.meteringMode) + htmlBr;

    if (exifPhoto & DetailsOptions::FlashMode)
        content += tr("Flash mode") + ": " +
                MetadataUtils::Exif::flashString(exifStruct.flashMode) + htmlBr;

    return content;
}
