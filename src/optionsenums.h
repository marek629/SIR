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

#ifndef OPTIONSENUM_H
#define OPTIONSENUM_H

//! Options dialog namespace.
namespace Options {
/** Categories indexes on list widgets model and group boxes pointer array. */
enum Categories {
    General,
    FileList,
#ifdef SIR_METADATA_SUPPORT
    Metadata,
    FileDetails,
#endif // SIR_METADATA_SUPPORT
    Selection,
    Raw
};
}

#ifdef SIR_METADATA_SUPPORT
/** \brief File details options flags namespace.
  *
  * Flags are used to store settings whitch metadata print in file details view.
  * This namespace is available only if SIR_METADATA_SUPPORT is defined.
  * \sa ConvertDialog::showDetails()
  */
namespace DetailsOptions {
/** Corresponding picture section in MetadataDialog's tool box. */
enum ExifPicture {
    ExifVersion             = 0x01,
    ProcessingSoftware      = 0x02,
    GeneratedDateAndTime    = 0x04,
    DigitizedDateAndTime    = 0x08,
    Orientation             = 0x10
};
/** Corresponding photo section in MetadataDialog's tool box. */
enum ExifPhoto {
    FocalLenght         = 0x001,
    Aperture            = 0x002,
    ShutterSpeed        = 0x004,
    IsoSpeed            = 0x008,
    ExposureTime        = 0x010,
    ExposureBias        = 0x020,
    ExposureProgram     = 0x040,
    LightMeteringMode   = 0x080,
    FlashMode           = 0x100
};
/** Corresponding camera section in MetadataDialog's tool box. */
enum ExifCamera {
    Manufacturer    = 0x1,
    Model           = 0x2
};
/** Corresponding author section in MetadataDialog's tool box. */
enum ExifAuthor {
    Artist      = 0x1,
    Copyright   = 0x2,
    UserComment = 0x4
};
/** Corresponding IPTC tab in MetadataDialog's tab widget. */
enum Iptc {
    ModelVersion    = 0x0001,
    DateCreated     = 0x0002,
    TimeCreated     = 0x0004,
    DigitizedDate   = 0x0008,
    DigitizedTime   = 0x0010,
    Byline          = 0x0020,
    CopyrightIptc   = 0x0040,
    ObjectName      = 0x0080,
    Keywords        = 0x0100,
    Caption         = 0x0200,
    CountryName     = 0x0400,
    City            = 0x0800,
    EditStatus      = 0x1000
};
}
#endif // SIR_METADATA_SUPPORT

//! TreeWidget options flags namespace.
namespace TreeWidgetOptions {
/** Columns of TreeWidget enumerator.\n
  * It allows to store settings whitch column should be visible.
  */
enum Columns {
    NameColumn      = 0x01,
    ExtColumn       = 0x02,
    PathColumn      = 0x04,
    ImageSizeColumn = 0x08,
    FileSizeColumn  = 0x10,
    StatusColumn    = 0x20
};
}

#endif // OPTIONSENUM_H
