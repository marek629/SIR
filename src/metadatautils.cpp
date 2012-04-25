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

#include "metadatautils.h"
#include "defines.h"
#include <QStringList>
#include <QDir>
#include <cmath>

bool MetadataUtils::Metadata::enabled = false;
bool MetadataUtils::Metadata::save = false;
QStringList MetadataUtils::Metadata::saveMetadataFormats =
        QStringList() << "jpeg" << "jpg" << "png" << "tif" << "tiff";

/** Default constructor. */
MetadataUtils::Metadata::Metadata() {}

MetadataUtils::Metadata::~Metadata() {
    close();
}

/** Reads metadata from \b path file path and optionaly setup ExifStruct if
  * \b setupStructs is true, and returns read success value.\n
  * If exception catched this error data will be saved into lastError_ field.
  * \sa lastError
  */
bool MetadataUtils::Metadata::read(const MetadataUtils::String& path, bool setupStructs) {
    close();
    try {
        std::string filePath = path.toNativeStdString();
        image = Exiv2::ImageFactory::open(filePath);
        image->readMetadata();

        exifData = image->exifData();
        exif.setVersion(exifData["Exif.Photo.ExifVersion"]);
        if (setupStructs)
            setExifStruct();

        iptcData = image->iptcData();
#ifdef EXV_HAVE_XMP_TOOLKIT
        xmpData = image->xmpData();
#endif // EXV_HAVE_XMP_TOOLKIT
        return true;
    }
    catch (Exiv2::Error &e) {
        lastError_.setMessage(tr("Error open file %1").arg(path.toQString()));
        lastError_.copy(e);
        return false;
    }
}

/** This is overloaded function. */
bool MetadataUtils::Metadata::read(const QString &path, bool setupStructs) {
    return read((const MetadataUtils::String&)path,setupStructs);
}

/** Writes metadata about file corresponding with \b path file path and
  * \b qImage image, and returns read success value.\n
  * If exception catched this error data will be saved into lastError_ field.
  * \sa lastError
  */
bool MetadataUtils::Metadata::write(const MetadataUtils::String& path, const QImage& qImage) {
    close();
    try {
        std::string filePath = path.toNativeStdString();
        image = Exiv2::ImageFactory::open(filePath);
        setData(qImage);
        image->writeMetadata();
        return true;
    }
    catch (Exiv2::Error &e) {
        lastError_.setMessage(tr("Error write file %1").arg( path.toQString() ) );
        lastError_.copy(e);
        return false;
    }
}

/** This is overloaded function. */
bool MetadataUtils::Metadata::write(const QString &path, const QImage &image) {
    return write((const MetadataUtils::String&)path, image);
}

/** Closes last file opened to read or write.
  * \sa open write
  */
void MetadataUtils::Metadata::close() {
    image.reset();
}

/** Clears metadata. */
void MetadataUtils::Metadata::clearMetadata() {
    exifData.clear();
    iptcData.clear();
#ifdef EXV_HAVE_XMP_TOOLKIT
    xmpData.clear();
#endif // EXV_HAVE_XMP_TOOLKIT
}

void MetadataUtils::Metadata::setData(const QImage& qImage) {
    if (!exifData.empty()) {
        if (!exif.isVersionKnown()) {
            Exiv2::byte version[4] = { 48, 50, 50, 48 };
            Exiv2::DataValue value(version,4);
            exifData["Exif.Photo.ExifVersion"] = value;
        }
        exifData["Exif.Image.ProcessingSoftware"] = "Simple Image Resizer " VERSION;
        if (!qImage.isNull()) {
            exifData["Exif.Image.ImageWidth"] = qImage.width();
            exifData["Exif.Image.ImageLength"] = qImage.height();
        }
        if (MetadataUtils::Exif::isArtistOverwrite())
            exifData["Exif.Image.Artist"] =
                    MetadataUtils::Exif::stringArtist().toStdString();
        if (MetadataUtils::Exif::isCopyrightOverwrite())
            exifData["Exif.Image.Copyright"] =
                    MetadataUtils::Exif::stringCopyright().toStdString();
        if (MetadataUtils::Exif::isUserCommentOverwrite())
            exifData["Exif.Photo.UserComment"] =
                    MetadataUtils::Exif::stringUserComment().toNativeStdString();
    }
    image->setExifData(exifData);
    image->setIptcData(iptcData);
#ifdef EXV_HAVE_XMP_TOOLKIT
    image->setXmpData(xmpData);
#endif // EXV_HAVE_XMP_TOOLKIT
}

/** Sets \a Exif metadata based on exifStruct_ data.
  * \sa ExifStruct setExifStruct
  */
void MetadataUtils::Metadata::setExifData() {
    // Image section
    exifData["Exif.Image.Orientation"] = exifStruct_.orientation;
    setExifDatum("Exif.Photo.DateTimeOriginal","Exif.Image.DateTimeOriginal",
                 exifStruct_.originalDate.toStdString() );
    exifData["Exif.Photo.DateTimeDigitized"] = exifStruct_.digitizedDate.toStdString();

    // Photo section
    Exiv2::Rational rational;
    rational = simpleRational( exifStruct_.focalLength * 10 );
    exifData["Exif.Photo.FocalLength"] = rational;
    rational = simpleRational( exifStruct_.aperture * 10 );
    setExifDatum("Exif.Photo.ApertureValue","Exif.Photo.FNumber",rational);
    MetadataUtils::String mString (exifStruct_.expTime);
    rational = mString.toRational();
    setExifDatum("Exif.Image.ExposureTime","Exif.Photo.ExposureTime",rational);
    rational = simpleRational( exifStruct_.expBias * 10 );
    setExifDatum("Exif.Image.ExposureBiasValue","Exif.Photo.ExposureBiasValue",
                 rational);
    setExifDatum("Exif.Image.ShutterSpeedValue","Exif.Photo.ShutterSpeedValue",
                 exifStruct_.shutterSpeed.toRationalPower());
    setExifDatum("Exif.Image.ISOSpeedRatings","Exif.Photo.ISOSpeedRatings",
                 exifStruct_.isoSpeed);
    setExifDatum("Exif.Image.Flash","Exif.Photo.Flash",exifStruct_.flashMode);
    setExifDatum("Exif.Image.ExposureProgram","Exif.Photo.ExposureProgram",
                 exifStruct_.expProgram);
    setExifDatum("Exif.Image.MeteringMode","Exif.Photo.MeteringMode",
                 exifStruct_.meteringMode);

    // Camera section
    exifData["Exif.Image.Make"] = exifStruct_.cameraManufacturer.toStdString();
    exifData["Exif.Image.Model"] = exifStruct_.cameraModel.toStdString();

    // Author section
    exifData["Exif.Image.Artist"] = exifStruct_.artist.toStdString();
    exifData["Exif.Image.Copyright"] = exifStruct_.copyright.toStdString();
    exifData["Exif.Photo.UserComment"] = exifStruct_.userComment.toNativeStdString();
}

/** Loads \a exifStruct_ data from \a Exif metadata.
  * \sa ExifStruct setExifData
  */
void MetadataUtils::Metadata::setExifStruct() {
    // Image section
    exifStruct_.version = exif.getVersion();
    exifStruct_.processingSoftware = MetadataUtils::String::fromStdString(
                exifData["Exif.Image.ProcessingSoftware"].toString() );
    if (exifStruct_.processingSoftware.isEmpty())
        exifStruct_.processingSoftware = MetadataUtils::String::noData();
    exifStruct_.imageWidth = QString::number(
                exifData["Exif.Image.ImageWidth"].toLong() );
    exifStruct_.imageWidth.appendUnit(" px");
    exifStruct_.imageHeight = QString::number(
                exifData["Exif.Image.ImageLength"].toLong() );
    exifStruct_.imageHeight.appendUnit(" px");
    exifStruct_.orientation = exifData["Exif.Image.Orientation"].toLong();
    exifStruct_.originalDate = QString::fromStdString(
                exifData["Exif.Photo.DateTimeOriginal"].toString() );
    if (exifStruct_.originalDate.isEmpty())
        exifStruct_.originalDate = QString::fromStdString(
                    exifData["Exif.Image.DateTimeOriginal"].toString() );
    exifStruct_.digitizedDate = QString::fromStdString(
                exifData["Exif.Photo.DateTimeDigitized"].toString() );
    if (exifStruct_.digitizedDate.isEmpty())
        exifStruct_.digitizedDate = QString::fromStdString(
                    exifData["Exif.Image.DateTimeDigitized"].toString() );

    // Thumbnail section
    Exiv2::PreviewManager previewManager(*image);
    Exiv2::PreviewImage thumbnail = previewManager.getPreviewImage(
                previewManager.getPreviewProperties()[0] );
    exifStruct_.thumbnailImage = QImage(thumbnail.width(), thumbnail.height(),
                                        QImage::Format_ARGB32);
    exifStruct_.thumbnailImage.loadFromData(thumbnail.pData(), thumbnail.size());
    exifStruct_.thumbnailWidth = MetadataUtils::String::number( thumbnail.width() );
    exifStruct_.thumbnailWidth.appendUnit(" px");
    exifStruct_.thumbnailHeight = MetadataUtils::String::number( thumbnail.height() );
    exifStruct_.thumbnailHeight.appendUnit(" px");

    // Photo section
    Exiv2::Rational rational = exifData["Exif.Photo.FocalLength"].toRational();
    exifStruct_.focalLength = (float)rational.first / rational.second;

    rational = exifData["Exif.Photo.FNumber"].toRational();
    if (rational.first == -1 && rational.second == 1)
        rational = exifData["Exif.Photo.ApertureValue"].toRational();
    exifStruct_.aperture = (float)rational.first / rational.second;
    exifStruct_.expTime = timeString("Exif.Image.ExposureTime","Exif.Photo.ExposureTime");

    rational = exifData["Exif.Photo.ShutterSpeedValue"].toRational();
    bool isEmpty (rational.first == -1 && rational.second == 1);
    if (isEmpty) {
        rational = exifData["Exif.Image.ShutterSpeedValue"].toRational();
        isEmpty = (rational.first == -1 && rational.second == 1);
        if (isEmpty)
            exifStruct_.shutterSpeed = MetadataUtils::String::noData();
    }
    if (!isEmpty)
        exifStruct_.shutterSpeed = timeString(rational);
    exifStruct_.isoSpeed = exifData["Exif.Photo.ISOSpeedRatings"].toLong();
    if ( exifStruct_.isoSpeed == -1 )
        exifStruct_.isoSpeed = exifData["Exif.Image.ISOSpeedRatings"].toLong();
    rational = exifData["Exif.Photo.ExposureBiasValue"].toRational();
    exifStruct_.expBias = (float)rational.first / rational.second;
    exifStruct_.expProgram = exifData["Exif.Photo.ExposureProgram"].toLong();
    exifStruct_.meteringMode = exifData["Exif.Photo.MeteringMode"].toLong();
    if ( exifStruct_.meteringMode == -1 )
        exifStruct_.meteringMode = exifData["Exif.Image.MeteringMode"].toLong();
    if ( exifStruct_.meteringMode == 255 ) //when value equal 'other'
        exifStruct_.meteringMode = 7;
    exifStruct_.flashMode = exifData["Exif.Photo.Flash"].toLong();
    if ( exifStruct_.flashMode == -1 )
        exifStruct_.flashMode = exifData["Exif.Image.Flash"].toLong();

    // Camera section
    exifStruct_.cameraManufacturer = QString::fromStdString(
                exifData["Exif.Image.Make"].toString() );
    exifStruct_.cameraModel = QString::fromStdString(
                exifData["Exif.Image.Model"].toString() );

    // Author section
    exifStruct_.artist = QString::fromStdString(
                exifData["Exif.Image.Artist"].toString() );
    if (exifStruct_.artist.isEmpty())
        exifStruct_.artist = MetadataUtils::String::noData();
    exifStruct_.copyright = QString::fromStdString(
                exifData["Exif.Image.Copyright"].toString() );
    if (exifStruct_.copyright.isEmpty())
        exifStruct_.copyright = MetadataUtils::String::noData();
    exifStruct_.userComment = QString::fromStdString(
                exifData["Exif.Photo.UserComment"].toString() );
    if (exifStruct_.userComment.isEmpty())
        exifStruct_.userComment = MetadataUtils::String::noData();
}

/** This is overloaded function useful for getting exposure time.\n
  * Returns valid fractional time string suffixed by /em " s" based on metadata
  * corresponding with \b key1 and \b key2 keys. If \b key1 is empty this function
  * returns empty string.\n
  * If \b key1 value isn't defined will be read \b key2 value. Otherwise will be
  * returned string based on \b key1.
  */
QString MetadataUtils::Metadata::timeString(const std::string &key1, const std::string &key2) {
    QString result;
    if (key1.empty())
        return result;
    Exiv2::Exifdatum& tag = exifData[key1];
    Exiv2::Rational rational;
    if (tag.size() > 0)
        rational = tag.toRational();
    else {
        rational.first = -1;
        rational.second = 1;
    }
    result = timeString(&rational,key2);
    return result;
}

/** This is overloaded function useful for getting shutter speed.\n
  * Returns valid fractional time string based on \b rationalPower power of 0.5.
  */
QString MetadataUtils::Metadata::timeString(const Exiv2::Rational &rationalPower) {
    QString result;
    Exiv2::Rational rational = rationalPower;
    if (rationalPower.first == 0)
        result = "1 s";
    else if (abs(rationalPower.first) < abs(2*rationalPower.second)) {
        // decimal stores point moved 3 digits to right and rounded to nearest integer
        int decimal = pow(0.5,(double)rationalPower.first/rationalPower.second) * 1000 + 0.5;
        if (decimal > 1000) {
            double d = decimal * 0.001;
            int i = d;
            result = QString::number(i + (int)(d-i+0.05));
        }
        rational.first = decimal % 1000;
        rational.second = 1000;
        if (rational.first >= 290 && rational.first < 340) { // test 1/3 s
            if (result.length() > 0 && result.right(1) != " ")
                result.append(" ");
            int diff = rational.first - 333;
            diff *= diff;
            if (diff > 272) // half of diffrend between 300 and 333 is 16.5; 16.5^2 = 272.25
                result += "3/10 s";
            else
                result += "1/3 s";
        }
        else if (rational.first >= 950) {
            if (result.length() > 0 && result.right(1) != " ")
                result.append(" ");
            result.append("s");
        }
        else {
            rational.first = rational.first * 0.1 + 0.5;
            rational.second = 100;
            if (result.length() > 0 && result.right(1) != " ")
                result.append(" ");
            result += timeString(&rational,"");
        }
    }
    else { // rational <= 1/4 s
        rational.first = 1;
        rational.second = pow(2, fabs(rationalPower.first) / abs(rationalPower.second)) + 0.5;
        if (result.length() > 0 && result.right(1) != " ")
            result.append(" ");
        result += timeString(&rational,"");
    }
    return result;
}

/** This is overloaded function, provides tools useful for both another
  * overloaded functions.\n
  * Returns valid fractional time string based on \b rational value and if it's
  * not defined (-1/1) on \b key value.
  */
QString MetadataUtils::Metadata::timeString(Exiv2::Rational *rational,
                                            const std::string &key) {
    QString result;
    bool isEmpty(rational->first == -1  && rational->second == 1);
    if (isEmpty) {
        if (!key.empty()) {
            *rational = exifData[key].toRational();
            isEmpty = false;
        }
        else
            result = MetadataUtils::String::noData();
    }
    if (!isEmpty) {
        if (rational->first == 0)
            result.clear();
        else if (rational->first < rational->second) {
            *rational = simpleRational(*rational);
            result = QString::number(rational->first) + "/" +
                    QString::number(rational->second);
        }
        else {
            short integer = rational->first / rational->second;
            result = QString::number(integer);
            int fraction = rational->first - integer*rational->second;
            if (fraction != 0) {
                rational->first = fraction;
                *rational = simpleRational(*rational);
                result += " " + QString::number(rational->first) +
                    "/" + QString::number(rational->second);
            }
        }
        if (result.length() > 0 && result.right(1) != " ")
            result.append(" ");
        result.append("s");
    }
    return result;
}

/** Returns simplest fraction which equal \b integer divided by 10. */
Exiv2::Rational MetadataUtils::Metadata::simpleRational(int integer) {
    Exiv2::Rational result;
    if (integer % 10 == 0) {
        result.first = integer / 10;
        result.second = 1;
    }
    else if (integer % 5 == 0) {
        result.first = integer / 5;
        result.second = 2;
    }
    else if (integer % 2 == 0) {
        result.first = integer / 2;
        result.second = 5;
    }
    else {
        result.first = integer;
        result.second = 10;
    }
    return result;
}

/** This is overloaded function.\n
  * Returns simplest fraction whitch equal \b rational value.
  * Maximum denominator expected is 100 (i.e. 25/100).
  */
Exiv2::Rational MetadataUtils::Metadata::simpleRational(const Exiv2::Rational &rational) {
    Exiv2::Rational result = rational;
    if (rational.first == 0)
        result.second = 1;
    else if (rational.second % rational.first == 0) {
        result.first = 1;
        result.second = rational.second / rational.first;
    }
    else if (rational.second % 25 == 0 && rational.first % 25 == 0) {
        result.first = rational.first / 25;
        result.second = rational.second / 25;
    }
    else if (rational.second % 20 == 0 && rational.first % 20 == 0) {
        result.first = rational.first / 20;
        result.second = rational.second / 20;
    }
    else if (rational.second % 10 == 0 && rational.first % 10 == 0) {
        result.first = rational.first / 10;
        result.second = rational.second / 10;
    }
    else if (rational.second % 5 == 0 && rational.first % 5 == 0) {
        result.first = rational.first / 5;
        result.second = rational.second / 5;
    }
    else if (rational.second % 2 == 0 && rational.first % 2 == 0) {
        result.first = rational.first / 2;
        result.second = rational.second / 2;
    }
    else {
        result.first = rational.first * 0.1 + 0.5;
        result.second = rational.second * 0.1 + 0.5;
    }
    return result;
}

/** Returns true if metadata support is enabled, otherwise false. */
bool MetadataUtils::Metadata::isEnabled() {
    return enabled;
}

/** Enables metadata support if \v is true, otherwise disables this. */
void MetadataUtils::Metadata::setEnabled(bool v) {
    enabled = v;
}

/** Returns true if metadata saving support is enabled, otherwise false. */
bool MetadataUtils::Metadata::isSave() {
    return save;
}

/** Enables metadata saving support if \v is true, otherwise disables this. */
void MetadataUtils::Metadata::setSave(bool v) {
    save = v;
}

/** Returns true if typed \b format format file is write supported, otherwise false. */
bool MetadataUtils::Metadata::isWriteSupportedFormat(const QString &format) {
    return saveMetadataFormats.contains(format);
}

/** This is overloaded function.\n
  * Sets \a Exif metadatum corresponding to \b key key to typed \b value.
  */
void MetadataUtils::Metadata::setExifDatum(const std::string &key, int value) {
    if (key.empty())
        return;
    exifData[key] = value;
}

/** This is overloaded function.\n
  * Sets \a Exif metadatum corresponding to \b key1 and \b key2 to typed \b value.
  * \note If value following key1 or key2 isn't set, it will be set just known
  * value. If both keys values are unknown, this function will do nothing and
  * just return.
  */
void MetadataUtils::Metadata::setExifDatum(
        const std::string &key1, const std::string &key2, int value) {
    if (key1.empty() && key2.empty())
        return;

    if (exifData[key1].toLong() == 0)
        exifData[key2] = value;
    else if (exifData[key2].toLong() == 0)
        exifData[key1] = value;
    else {
        exifData[key1] = value;
        exifData[key2] = value;
    }
}

/** This is overloaded function.\n
  * Sets \a Exif metadatum corresponding to \b key1 and \b key2 to typed \b value.
  * \note If value following key1 or key2 isn't set, it will be set just known
  * value. If both keys values are unknown, this function will do nothing and
  * just return.
  */
void MetadataUtils::Metadata::setExifDatum(
        const std::string &key1, const std::string &key2, const Exiv2::Rational &value) {
    if (key1.empty() && key2.empty())
        return;

    if (exifData[key1].toLong(0) == -1 && exifData[key1].toLong(1) == 1)
        exifData[key2] = value;
    else if (exifData[key2].toLong(0) == -1 && exifData[key2].toLong(1) == 1)
        exifData[key1] = value;
    else {
        exifData[key1] = value;
        exifData[key2] = value;
    }
}

/** This is overloaded function.\n
  * Sets \a Exif metadatum corresponding to \b key1 and \b key2 to typed \b value.
  * \note If value following key1 or key2 isn't set, it will be set just known
  * value. If both keys values are unknown, this function will do nothing and
  * just return.
  */
void MetadataUtils::Metadata::setExifDatum(
        const std::string &key1, const std::string &key2, const std::string &value) {
    if (key1.empty() && key2.empty())
        return;

    if (exifData[key1].toString().empty())
        exifData[key2] = value;
    else if (exifData[key2].toString().empty())
        exifData[key1] = value;
    else {
        exifData[key1] = value;
        exifData[key2] = value;
    }
}

/** This is overloaded function.\n
  * Sets \a Exif thumbnail based on image loaded from \b path file path.
  */
void MetadataUtils::Metadata::setExifThumbnail(const std::string &path) {
    Exiv2::ExifThumb thumb (exifData);
    thumb.erase();
    thumb.setJpegThumbnail(path);
}

/** This is overloaded function.\n
  * Sets \a Exif thumbnail based on \b image data saved as temporary file with
  * \b tid ID.\n
  * If exception catched this error data will be saved into lastError_ field.
  * \sa lastError
  */
bool MetadataUtils::Metadata::setExifThumbnail(QImage *image, int tid) {
    if (image->isNull())
        return false;
    QString filePath = QDir::tempPath() + QDir::separator() + "sir_thumb";
    filePath += QString::number(tid) + ".jpg";
    image->save(filePath);
    try {
        setExifThumbnail(filePath.toStdString());
    }
    catch (Exiv2::Error &e) {
        lastError_.copy(e);
        lastError_.setMessage(tr("Save thumnail failed"));
        return false;
    }
    return true;
}
