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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Program URL: http://sir.projet-libre.org/
 */

#include <exiv2/metadatum.hpp>
#include "exif.h"

bool MetadataUtils::Exif::artistOverwrite = false;
MetadataUtils::String MetadataUtils::Exif::artistString = "";
bool MetadataUtils::Exif::copyrightOverwrite = false;
MetadataUtils::String MetadataUtils::Exif::copyrightString = "";
bool MetadataUtils::Exif::userCommentOverwrite = false;
MetadataUtils::String MetadataUtils::Exif::userCommentString = "";

/** Default constructor. */
MetadataUtils::Exif::Exif() {
    versionKnown = false;
}

/** Returns string containing information about flash mode.
  * \param num Encoded value of \a Exif.Photo.Flash.
  * \sa Flash flashShort
  */
QString MetadataUtils::Exif::flashString(short num) {
    if (num == -1)
        return QObject::tr("Unknown");
    QString result;
    QString bin = QString::number(num,2);
    // invert bin
    short binHalfLength = bin.length() / 2;
    for (short i=0, j=bin.length()-1; i<binHalfLength; i++, j--) {
        QChar temp = bin[j];
        bin[j] = bin[i];
        bin[i] = temp;
    }
    // copmlete with 0 up to target bin string length
    short toAppend = 7 - bin.length();
    for (short i=0; i<toAppend; i++)
        bin.append('0');

    // bit 5 indicating the presence of a flash function
    if ( bin[5] == '1' )
        return MetadataUtils::Flash::noFlashFunction();

    // bit 0 indicating whether the flash fired
    if ( bin[0] == '1' )
        result = MetadataUtils::Flash::fired();
    else
        result = MetadataUtils::Flash::noFired();

    // bits 3 and 4 indicating the camera's flash mode
    if ( bin[3]=='1' && bin[4]=='1' )
        result.append(MetadataUtils::Flash::autoMode());
    else if ( (bin[3]=='0' && bin[4]=='1') || (bin[3]=='1' && bin[4]=='0') )
        result.append(MetadataUtils::Flash::compulsoryMode());

    // bits 1 and 2 indicating the status of returned light
    if ( bin[1] == '1' ) {
        if ( bin[2] == '1' )
            result.append(MetadataUtils::Flash::strobeReturnDetected());
        else
            result.append(MetadataUtils::Flash::strobeReturnNotDetected());
    }

    // bit 6 indicating the camera's red-eye mode
    if ( bin[6] == '1' )
        result.append(MetadataUtils::Flash::redEyeReduction());

    result.append('.');
    return result;
}

/** Returns flash mode \b str string converted to encoded \a Exif.Photo.Flash value.
  \sa Flash flashString
  */
short MetadataUtils::Exif::flashShort(const QString &str) {
    // bit 5 indicating the presence of a flash function
    if ( str == MetadataUtils::Flash::noFlashFunction() )
        return 32; // 2^5

    short result = 0;

    // bit 0 indicating whether the flash fired
    if (str.contains(MetadataUtils::Flash::fired()))
        result += 1; // 2^0

    // bits 1 and 2 indicating the status of returned light
    if (str.contains(MetadataUtils::Flash::strobeReturnDetected()))
        result += 6; // 2^1 + 2^2
    else if (str.contains(MetadataUtils::Flash::strobeReturnNotDetected()))
        result += 2; // 2^1

    // bits 3 and 4 indicating the camera's flash mode
    if (str.contains(MetadataUtils::Flash::autoMode()))
        result += 24; // 2^3 + 2^4
    else if (str.contains(MetadataUtils::Flash::compulsoryMode()))
        result += 16; // 2^4

    // bit 6 indicating the camera's red-eye mode
    if (str.contains(MetadataUtils::Flash::redEyeReduction()))
        result += 64; // 2^6

    return result;
}

/** Returns true if Exif version is known; otherwise returns false.
  * \sa setVersion getVersion
  */
bool MetadataUtils::Exif::isVersionKnown() {
    return versionKnown;
}

/** Sets version string and version knowlege status basing on datum.
  * \sa getVersion isVersionKnown
  */
void MetadataUtils::Exif::setVersion(const Exiv2::Metadatum &datum) {
    int size = datum.size();
    if (size < 4) {
        version = MetadataUtils::String::noData();
        versionKnown = false;
        return;
    }
    versionKnown = true;
    char ch[size];
    for (int i=0; i<size; i++)
        ch[i] = datum.toLong(i);
    version = QString(QByteArray(ch,size));
    version.insert(size-2,'.');
    if (version[size-1] == '0')
        version.remove(size-1,1);
    if (version[0] == '0')
        version.remove(0,1);
}

/** Returns version string.
  * \sa setVersion isVersionKnown
  */
const MetadataUtils::String & MetadataUtils::Exif::getVersion() {
    return version;
}

/** Sets version string and version knowlege status basing on datum and
  * returns version string.
  * \sa setVersion isVersionKnown
  */
const MetadataUtils::String & MetadataUtils::Exif::getVersion(
        const Exiv2::Metadatum &datum) {
    setVersion(datum);
    return version;
}

/** Returns true if \a Exif.Image.Artist field should be overwrite;
  * otherwise returns false.
  * \sa setArtistOverwrite stringArtist setArtistString
  */
bool MetadataUtils::Exif::isArtistOverwrite() {
    return artistOverwrite;
}

/** Sets value of \a Exif.Image.Artist overwrite status.
  * \sa isArtistOverwrite stringArtist setArtistString
  */
void MetadataUtils::Exif::setArtistOverwrite(bool v) {
    artistOverwrite = v;
}

/** Returns string with \a Exif.Image.Artist field should be saved.
  * \sa setArtistString isArtistOverwrite setArtistOverwrite
  */
MetadataUtils::String MetadataUtils::Exif::stringArtist() {
    return artistString;
}

/** Sets string with \a Exif.Image.Artist field should be saved.
  * \sa stringArtist isArtistOverwrite setArtistOverwrite
  */
void MetadataUtils::Exif::setArtistString(const MetadataUtils::String &v) {
    artistString = v;
}

/** Returns true if \a Exif.Image.Copyright field should be overwrite;
  * otherwise returns false.
  * \sa setCopyrightOverwrite stringCopyright setCopyrightString
  */
bool MetadataUtils::Exif::isCopyrightOverwrite() {
    return copyrightOverwrite;
}

/** Sets value of \a Exif.Image.Copyright overwrite status.
  * \sa isCopyrightOverwrite stringCopyright setCopyrightString
  */
void MetadataUtils::Exif::setCopyrightOverwrite(bool v) {
    copyrightOverwrite = v;
}

/** Returns string with \a Exif.Image.Copyright field should be saved.
  * \sa setCopyrightString isCopyrightOverwrite setCopyrightOverwrite
  */
MetadataUtils::String MetadataUtils::Exif::stringCopyright() {
    return copyrightString;
}

/** Sets string with \a Exif.Image.Copyright field should be saved.
  * \sa stringCopyright isCopyrightOverwrite setCopyrightOverwrite
  */
void MetadataUtils::Exif::setCopyrightString(const MetadataUtils::String &v) {
    copyrightString = v;
}

/** Returns true if \a Exif.Photo.UserComment field should be overwrite;
  * otherwise returns false.
  * \sa setUserCommentOverwrite stringUserComment setUserCommentString
  */
bool MetadataUtils::Exif::isUserCommentOverwrite() {
    return userCommentOverwrite;
}

/** Sets value of \a Exif.Photo.UserComment overwrite status.
  * \sa isUserCommentOverwrite stringUserComment setUserCommentString
  */
void MetadataUtils::Exif::setUserCommentOverwrite(bool v) {
    userCommentOverwrite = v;
}

/** Returns string with \a Exif.Photo.UserComment field should be saved.
  * \sa setUserCommentString isUserCommentOverwrite setUserCommentOverwrite
  */
MetadataUtils::String MetadataUtils::Exif::stringUserComment() {
    return userCommentString;
}

/** Sets string with \a Exif.Photo.UserComment field should be saved.
  * \sa stringUserComment isUserCommentOverwrite setUserCommentOverwrite
  */
void MetadataUtils::Exif::setUserCommentString(const MetadataUtils::String &v) {
    userCommentString = v;
}

/** Returns \a Exif.Image.Orientation code.
  * \param rotation Rotation angle in degrees (-90, 0, 90, 180 and 270 is supported).
  * \param flip Value of enumeration Flip.
  * \return -1 if \b rotation value is unexpected\n
  *         -2 if \b flip value is unexpected\n
  *         otherwise returing value is in range 1 to 8
  * \sa orientationString rotationAngle flipValue Flip
  */
char MetadataUtils::Exif::getOrientation(short rotation, int flip) {
    using namespace MetadataUtils;
    switch (rotation) {
    case 0:
        if (flip == None)
            return 1;
        else if (flip == Horizontal)
            return 2;
        else if (flip == Vertical)
            return 4;
        break;
    case 180:
        if (flip == None)
            return 3;
        break;
    case 90:
        if (flip == None)
            return 6;
        else if (flip == Horizontal)
            return 5;
        else if (flip == Vertical)
            return 7;
        break;
    case -90:
    case 270:
        if (flip == None)
            return 8;
        else if (flip == Horizontal)
            return 7;
        else if (flip == Vertical)
            return 5;
        break;
    default:
        return -1;
        break;
    }
    return -2;
}

/** Returs translated string coresponding \b orientation code.
  * \param orientation Orientation code in range 1 to 8. If it isn't in range
  *        this function will return \em "No rotation" text.
  * \sa getOrientation expProgramString meteringModeString
  */
MetadataUtils::String MetadataUtils::Exif::orientationString(char orientation) {
    switch (orientation) {
    case 1: return tr("No rotation");
    case 2: return tr("No rotation, flip verticaly");
    case 3: return tr("Rotation 180°");
    case 4: return tr("Rotation 180°, flip verticaly");
    case 5: return tr("Clockwise rotation, flip horizontaly");
    case 6: return tr("Clockwise rotation");
    case 7: return tr("Counterclockwise rotation, flip horizontaly");
    case 8: return tr("Counterclockwise rotation");
    default: return tr("No rotation");
    }
}

/** Returs translated string coresponding \b programId code.
  * \param orientation Orientation code in range 0 to 8. If it isn't in range
  *        this function will return \em "Not defined" text.
  * \sa orientationString meteringModeString
  */
MetadataUtils::String MetadataUtils::Exif::expProgramString(uchar programId) {
    switch (programId) {
    case 0: return tr("Not defined");
    case 1: return tr("Manual");
    case 2: return tr("Auto");
    case 3: return tr("Aperture priority");
    case 4: return tr("Shutter priority");
    case 5: return tr("Creative program (biased toward depth of field)");
    case 6: return tr("Action program (biased toward fast shutter speed)");
    case 7: return tr("Portrait mode (for closeup photos with the background out of focus)");
    case 8: return tr("Landscape mode (for landscape photos with the background in focus)");
    default: return tr("Not defined");
    }
}

/** Returs translated string coresponding \b modeId code.
  * \param orientation Orientation code in range 0 to 6. If it isn't in range
  *        this function will return \em "Other" text.
  * \sa orientationString expProgramString
  */
MetadataUtils::String MetadataUtils::Exif::meteringModeString(short modeId) {
    switch (modeId) {
    case 0: return tr("Unknown");
    case 1: return tr("Average");
    case 2: return tr("Center weighted average");
    case 3: return tr("Spot");
    case 4: return tr("Multi spot");
    case 5: return tr("Pattern");
    case 6: return tr("Partial");
    default: return tr("Other");
    }
}

/** Returns rotation angle in degrees basing on orientation code.
  * \param orientation Orientation code in range 1 to 8.
  * \return -360 if \b orientation value is unexpected\n
  *         otherwise returing value is -90, 0, 90 or 180
  * \sa flipValue getOrientation
  */
short MetadataUtils::Exif::rotationAngle(char orientation) {
    short result = 0;
    switch (orientation) {
    case 1:
        break;
    case 2:
        break;
    case 3:
        result = 180;
        break;
    case 4:
        break;
    case 5:
        result = 90;
        break;
    case 6:
        result = 90;
        break;
    case 7:
        result = -90;
        break;
    case 8:
        result = -90;
        break;
    default: // invalid orientation value
        result = -360;
        break;
    }
    return result;
}

/** Returns rotation angle in degrees and sets value of flip basing on orientation code.
  * \param orientation Orientation code in range 1 to 8.
  * \param flip Pointer to Flip enumerated value.
  * \return -360 if \b orientation value is unexpected\n
  *         otherwise returing value is -90, 0, 90 or 180
  * \sa rotationAngle getOrientation Flip
  */
short MetadataUtils::Exif::rotationAngle(char orientation, int *flip) {
    using namespace MetadataUtils;
    *flip = None;
    short result = 0;
    switch (orientation) {
    case 1:
        break;
    case 2:
        *flip = Horizontal;
        break;
    case 3:
        result = 180;
        break;
    case 4:
        *flip = Vertical;
        break;
    case 5:
        result = 90;
        *flip = Horizontal;
        break;
    case 6:
        result = 90;
        break;
    case 7:
        result = -90;
        *flip = Horizontal;
        break;
    case 8:
        result = -90;
        break;
    default: // invalid orientation value
        result = -360;
        *flip = VerticalAndHorizontal;
        break;
    }
    return result;
}

/** Returns Flip enumerated value basing on orientation code.
  * \param orientation Orientation code in range 1 to 8.
  * \return \a VerticalAndHorizontal if \b orientation value is unexpected\n
  *         otherwise returing value is \a None, \a Vertical or \a Horizontal
  * \sa rotationAngle getOrientation
  */
MetadataUtils::Flip MetadataUtils::Exif::flipValue(char orientation) {
    using namespace MetadataUtils;
    Flip result = None;
    switch (orientation) {
    case 1:
        break;
    case 2:
        result = Horizontal;
        break;
    case 3:
        break;
    case 4:
        result = Vertical;
        break;
    case 5:
        result = Horizontal;
        break;
    case 6:
        break;
    case 7:
        result = Horizontal;
        break;
    case 8:
        break;
    default: // invalid orientation value
        result = VerticalAndHorizontal;
        break;
    }
    return result;
}

/** Sets default value of ExifStruct fields, mainly 0 and \em no \em data strings.
  */
void MetadataUtils::ExifStruct::reset() {
    // Image section
    version = MetadataUtils::String::noData();
    processingSoftware = MetadataUtils::String::noData();
    imageWidth = MetadataUtils::String::noData();
    imageHeight = MetadataUtils::String::noData();
    orientation = 1;
    originalDate = MetadataUtils::String::noData();
    digitizedDate = MetadataUtils::String::noData();
    // Thumbnail section
    thumbnailImage = QImage();
    thumbnailWidth = MetadataUtils::String::noData();
    thumbnailHeight = MetadataUtils::String::noData();
    // Photo section
    focalLength = 0.0;
    aperture = 0.0;
    isoSpeed = 0;
    expTime = MetadataUtils::String::noData();
    shutterSpeed = MetadataUtils::String::noData();
    expBias = 0.0;
    expProgram = 0;
    meteringMode = 0;
    flashMode = -1;
    // Camera section
    cameraManufacturer = MetadataUtils::String::noData();
    cameraModel = MetadataUtils::String::noData();
    // Author section
    artist = MetadataUtils::String::noData();
    copyright = MetadataUtils::String::noData();
    userComment = MetadataUtils::String::noData();
}
