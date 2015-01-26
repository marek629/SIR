/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2015  Marek Jędryka   <jedryka89@gmail.com>
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

#include "ExifStruct.hpp"

using namespace MetadataUtils;
using namespace sir;

QString ExifStruct::accept(RichTextVisitor *visitor) {
    return visitor->visit(this);
}

/** Sets default value of ExifStruct fields, mainly 0 and \em "no data" strings.
  * \sa clear
  */
void ExifStruct::reset() {
    // Image section
    version = String::noData();
    processingSoftware = String::noData();
    imageWidth = String::noData();
    imageHeight = String::noData();
    orientation = 1;
    originalDate = String::noData();
    digitizedDate = String::noData();
    // Thumbnail section
    thumbnailImage = QImage();
    thumbnailWidth = String::noData();
    thumbnailHeight = String::noData();
    // Photo section
    focalLength = 0.f;
    aperture = 0.f;
    isoSpeed = 0;
    expTime = String::noData();
    shutterSpeed = String::noData();
    expBias = 0.f;
    expProgram = 0;
    meteringMode = 0;
    flashMode = -1;
    // Camera section
    cameraManufacturer = String::noData();
    cameraModel = String::noData();
    // Author section
    artist = String::noData();
    copyright = String::noData();
    userComment = String::noData();
}

/** Sets null value of ExifStruct fields: for all numeric variables sets 0,
  * for complex variables (MetadataUtils::String, QImage) sets null objects.
  * \sa reset
  */
void ExifStruct::clear() {
    // Image section
    version = String();
    processingSoftware = String();
    imageWidth = String();
    imageHeight = String();
    orientation = 0;
    originalDate = String();
    digitizedDate = String();
    // Thumbnail section
    thumbnailImage = QImage();
    thumbnailWidth = String();
    thumbnailHeight = String();
    // Photo section
    focalLength = 0.f;
    aperture = 0.f;
    isoSpeed = 0;
    expTime = String();
    shutterSpeed = String();
    expBias = 0.f;
    expProgram = 0;
    meteringMode = 0;
    flashMode = 0;
    // Camera section
    cameraManufacturer = String();
    cameraModel = String();
    // Author section
    artist = String();
    copyright = String();
    userComment = String();
}

ExifStruct ExifStruct::copy() {
    ExifStruct copiedStruct;

    // Image section
    copiedStruct.version = version;
    copiedStruct.processingSoftware = processingSoftware;
    copiedStruct.imageWidth = imageWidth;
    copiedStruct.imageHeight = imageHeight;
    copiedStruct.orientation = orientation;
    copiedStruct.originalDate = originalDate;
    copiedStruct.digitizedDate = digitizedDate;

    // Thumbnail section
    copiedStruct.thumbnailImage = thumbnailImage;
    copiedStruct.thumbnailWidth = thumbnailWidth;
    copiedStruct.thumbnailHeight = thumbnailHeight;

    // Photo section
    copiedStruct.focalLength = focalLength;
    copiedStruct.aperture = aperture;
    copiedStruct.isoSpeed = isoSpeed;
    copiedStruct.expTime = expTime;
    copiedStruct.shutterSpeed = shutterSpeed;
    copiedStruct.expBias = expBias;
    copiedStruct.expProgram = expProgram;
    copiedStruct.meteringMode = meteringMode;
    copiedStruct.flashMode = flashMode;

    // Camera section
    copiedStruct.cameraManufacturer = cameraManufacturer;
    copiedStruct.cameraModel = cameraModel;

    // Author section
    copiedStruct.artist = artist;
    copiedStruct.copyright = copyright;
    copiedStruct.userComment = userComment;

    return copiedStruct;
}
