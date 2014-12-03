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
#endif // SIR_METADATA_SUPPORT
}

#endif // EXIF_H
