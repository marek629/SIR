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

#ifndef EXIF_H
#define EXIF_H

#ifndef SIR_CMAKE
#define SIR_METADATA_SUPPORT
#endif // SIR_CMAKE

#include <QCoreApplication>
#include <QImage>
#include "sir_String.hpp"
#include "metadata/structs/MetadataStruct.hpp"

#ifdef SIR_METADATA_SUPPORT
//! The namespace of exiv2 library API.
namespace Exiv2 {
    class Exifdatum;
    class Metadatum;
}
#endif // SIR_METADATA_SUPPORT

namespace MetadataUtils {
//! Exif's orientation flip enumerator.
enum Flip {
    None = 0x0,
    Vertical = 0x1,
    Horizontal = 0x2,
    VerticalAndHorizontal = 0x3
};

#ifdef SIR_METADATA_SUPPORT
/** \brief Exif metadata storage and manipulation class.
  *
  * Exif class provide general manipulation Exif metadata methods like
  * getting information about user comments, orientation and flash.
  * This class are comlemented by another classes from MetadataUtils namespace,
  * check See also.
  * \sa ExifStruct Iptc Flash Flip String
  */
class Exif {
    Q_DECLARE_TR_FUNCTIONS(MetadataUtils::Exif)

public:
    Exif();
    bool isVersionKnown();
    void setVersion(const Exiv2::Metadatum &datum);
    const sir::String &getVersion();
    const sir::String &getVersion(const Exiv2::Metadatum &datum);
    static QString flashString(short);
    static short flashShort(const QString&);
    static void setArtistOverwrite(bool);
    static bool isArtistOverwrite();
    static void setArtistString(const sir::String&);
    static sir::String stringArtist();
    static void setCopyrightOverwrite(bool);
    static bool isCopyrightOverwrite();
    static void setCopyrightString(const sir::String&);
    static sir::String stringCopyright();
    static void setUserCommentOverwrite(bool);
    static bool isUserCommentOverwrite();
    static void setUserCommentString(const sir::String&);
    static sir::String stringUserComment();
    static char getOrientation(short rotation, int flipValue = None);
    static short rotationAngle(char orientation);
    static short rotationAngle(char orientation, int *flip);
    static Flip flipValue(char orientation);
    static sir::String orientationString(char orientation);
    static sir::String expProgramString(uchar programId);
    static sir::String meteringModeString(short modeId);
    static float floatRational(Exiv2::Exifdatum &datum);
    static Exiv2::Rational rational(Exiv2::Exifdatum &datum);
    static long getLong(Exiv2::Exifdatum &datum);
    static const QString dateTimeFormat;

private:
    bool versionKnown;
    sir::String version;
    static bool artistOverwrite;
    static sir::String artistString;
    static bool copyrightOverwrite;
    static sir::String copyrightString;
    static bool userCommentOverwrite;
    static sir::String userCommentString;
};

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
    QString accept(RichTextVisitor *visitor);
    void reset();
    void clear();
    ExifStruct copy();
};

/** \brief Stores flash mode strings coresponding with \e Exif.Photo.Flash
  * metadata field.
  * \sa ExifStruct::flashMode
  */
class Flash {
    Q_DECLARE_TR_FUNCTIONS(MetadataUtils::Flash)

public:
    /** Returns translated string containing information about the lack of flash
      * function.
      */
    static QString noFlashFunction() { return tr("No flash function."); }
    /** Returns translated string containing information about flash fired.
      * \sa noFired
      */
    static QString fired() { return tr("Flash fired"); }
    /** Returns translated string containing information about flash didn't fire.
      * \sa fired
      */
    static QString noFired() { return tr("Flash didn't fire"); }
    /** Returns translated string containing information about flash fired in
      * compulsory mode.
      */
    static QString compulsoryMode() { return tr(", compulsory flash mode"); }
    /** Returns translated string containing information about flash fired in
      * auto mode.
      */
    static QString autoMode() { return tr(", auto mode"); }
    /** Returns translated string containing information about flash fired in
      * red-eye reduction mode. */
    static QString redEyeReduction() { return tr(", red-eye reduction"); }
    /** Returns translated string containing information about detection of
      * returning strobe ligth.
      * \sa strobeReturnNotDetected
      */
    static QString strobeReturnDetected() {
        return tr(", strobe return light detected"); }
    /** Returns translated string containing information about failed to
      * detection of returning strobe ligth.
      * \sa strobeReturnDetected
      */
    static QString strobeReturnNotDetected() {
        return tr(", strobe return light not detected"); }
};
#endif // SIR_METADATA_SUPPORT
}

#endif // EXIF_H
