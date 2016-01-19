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

#ifndef EXIFSTRUCT_HPP
#define EXIFSTRUCT_HPP

#include "metadata/structs/MetadataStruct.hpp"
#include "sir_String.hpp"
#include <QImage>

namespace MetadataUtils {
//! Stores \a Exif metadata user readable.
struct ExifStruct : public MetadataStruct {
    // Image section
    sir::String version; /**< Contains information about version of Exiv library. */
    sir::String processingSoftware;  /**< Contains information about processing software. */
    sir::String imageWidth; /**< Contains image width with " px" suffix. */
    sir::String imageHeight;  /**< Contains image height with " px" suffix. */
    char orientation; /**< Information about orientation of image. */
    sir::String originalDate; /**< Creation date and time. */
    sir::String digitizedDate; /**< Digitalization date and time. */
    // Thumbnail section
    QImage thumbnailImage; /**< Contains photo thumbnail. */
    sir::String thumbnailWidth; /**< Contains thumbnail width with " px" suffix. */
    sir::String thumbnailHeight;  /**< Contains thumbnail height with " px" suffix. */
    // Photo section
    float focalLength; /**< Contains Focal Length value in milimeters. */
    float aperture; /**< Contains Aperture value. */
    int isoSpeed; /**< Contains ISO Speed value. */
    sir::String shutterSpeed; /**< Contains Shutter Speed. */
    sir::String expTime;  /**< Contains Exposure time. */
    float expBias;  /**< Contains Exposure bias value in EV. */
    char expProgram; /**< Contains information about Exposure program. */
    short meteringMode; /**< Contains information about Light metering mode. */
    short flashMode; /**< Contains information about flash mode. \sa Flash */
    // Camera section
    sir::String cameraManufacturer;  /**< Contains information about Light metering mode. */
    sir::String cameraModel; /**< Contains information about Camera's model. */
    // Author section
    sir::String artist; /**< Contains information about autor. */
    sir::String copyright; /**< Contains copyright information. */
    sir::String userComment; /**< Contains user comment. */

    // Methods
    void accept(RichTextVisitor *visitor);
    void reset();
    void clear();
    ExifStruct copy();
};
}

#endif // EXIFSTRUCT_HPP
