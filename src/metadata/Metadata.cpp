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

#ifdef SIR_METADATA_SUPPORT

#include <QStringList>
#include <QDir>
#include <cmath>
#include "metadata/MetadataUtils.hpp"
#include "metadata/Metadata.hpp"
#include "Version.hpp"

using namespace MetadataUtils;
using namespace sir;

// setup static variables
QStringList Metadata::saveMetadataFormats =
        QStringList() << "jpeg" << "jpg" << "png" << "tif" << "tiff";

/** Default constructor. */
Metadata::Metadata() {}

/** Destructor.\n
  * Closes current image file.
  */
Metadata::~Metadata() {
    close();
}

/** Reads metadata from file.
  * \param path         file path
  * \param setupStructs setup ExifStruct if it's true
  * \param fromSvg      create new image object, don't read metadata from file
  * \return Read success value.\n
  *         If exception catched this error data will be appended to errorList
  *         list and returns false.
  * \sa lastError()
  */
bool Metadata::read(const String& path, bool setupStructs, bool fromSvg) {
    close();
    std::string filePath = path.toNativeStdString();
    try {
        if (fromSvg) {
#if EXIV2_TEST_VERSION(0,28,0)
            image = Exiv2::ImageFactory::create(static_cast<Exiv2::ImageType>(1));
#else
            image = Exiv2::ImageFactory::create(1);
#endif
        }
        else {
            image = Exiv2::ImageFactory::open(filePath);
            image->readMetadata();
        }
        // load Exif data
        exifData = image->exifData();
        exif.setVersion(exifData["Exif.Photo.ExifVersion"]);
        // load IPTC data
        iptcData = image->iptcData();
#if EXIV2_TEST_VERSION(0,28,0)
        Iptc::setVersion(iptcData["Iptc.Envelope.ModelVersion"].toInt64());
#else
        Iptc::setVersion(iptcData["Iptc.Envelope.ModelVersion"].toLong());
#endif
#ifdef EXV_HAVE_XMP_TOOLKIT
        // load XMP data
        xmpData = image->xmpData();
#endif // EXV_HAVE_XMP_TOOLKIT
        // prepare structs setup
        if (setupStructs) {
            firstEmptyFieldSkipped = true;
            exifStruct_.clear();
        }
        // setup struct skipping invalid metadata items
        while (setupStructs) {
            try {
                setExifStruct();
                setIptcStruct();
                break;
            }
            catch (Exiv2::Error &e) { // invalid metadatum
                firstEmptyFieldSkipped = false;
                QString message = tr("Error open file %1").arg(path.toQString());
                errorList += new Error(message,e);
                continue;
            }
        }
        return true;
    }
    catch (Exiv2::Error &e) { // read error
        QString message = tr("Error open file %1").arg(path.toQString());
        errorList += new Error(message,e);
        return false;
    }
}

/** This is overloaded function. */
bool Metadata::read(const QString &path, bool setupStructs, bool fromSvg) {
    return read((const String&)path, setupStructs, fromSvg);
}

/** Writes metadata about file corresponding with \a path file path and
  * \a qImage image, and returns read success value.
  *
  * Before write removes empty and broken metadata fields.
  *
  * If exception catched this error data will be appended to errorList list.
  * \sa lastError() removeEmptyFields()
  */
bool Metadata::write(const String& path, const QImage& qImage) {
    close();
    std::string filePath = path.toNativeStdString();
    try {
        image = Exiv2::ImageFactory::open(filePath);
        image->readMetadata();
        image->clearMetadata();
        removeEmptyFields();
        if (setData(qImage)) {
            image->writeMetadata();
            return true;
        }
    }
    catch (Exiv2::Error &e) {
        QString message = tr("Write file %1 error").arg(path.toQString());
        errorList += new Error(message,e);
    }
    return false;
}

/** This is overloaded function. */
bool Metadata::write(const QString &path, const QImage &image) {
    return write((const String&)path, image);
}

/** Closes last file opened to read or write.
  * \sa open() write()
  */
void Metadata::close() {
    image.reset();
}

/** Clears metadata. */
void Metadata::clearMetadata() {
    exifData.clear();
    iptcData.clear();
#ifdef EXV_HAVE_XMP_TOOLKIT
    xmpData.clear();
#endif // EXV_HAVE_XMP_TOOLKIT
}

/** Sets metadata of all supported standards.\n
  * If Exif metadata isn't empty and qImage isn't null save image size
  * in \em Exif.Image.ImageWidth and \em Exif.Image.ImageLength; otherwise this
  * metadata fields will filled from current Exiv2::Image \a image file.
  */
bool Metadata::setData(const QImage& qImage) {
    bool result = false;
    if (!exifData.empty()) {
        if (!exif.isVersionKnown()) {
            Exiv2::byte version[4] = { 48, 50, 50, 48 }; // ASCII codes of '0220'
            Exiv2::DataValue value(version,4);
            exifData["Exif.Photo.ExifVersion"] = value;
        }
        exifData["Exif.Image.ProcessingSoftware"] = "Simple Image Resizer " VERSION;
        if (!qImage.isNull()) {
            exifData["Exif.Image.ImageWidth"] = qImage.width();
            exifData["Exif.Image.ImageLength"] = qImage.height();
        }
        else {
            exifData["Exif.Image.ImageWidth"] = image->pixelWidth();
            exifData["Exif.Image.ImageLength"] = image->pixelHeight();
        }
        if (Exif::isArtistOverwrite())
            exifData["Exif.Image.Artist"] =
                    Exif::stringArtist().toStdString();
        if (Exif::isCopyrightOverwrite())
            exifData["Exif.Image.Copyright"] =
                    Exif::stringCopyright().toStdString();
        if (Exif::isUserCommentOverwrite())
            exifData["Exif.Photo.UserComment"] =
                    Exif::stringUserComment().toNativeStdString();
        std::string currentDateTimeString =
                String::currentDateTimeString(Exif::dateTimeFormat).toNativeStdString();
        exifData["Exif.Image.DateTime"] = currentDateTimeString;
        if (exifData["Exif.Image.DateTimeOriginal"].toString().empty())
            exifData["Exif.Image.DateTimeOriginal"] = currentDateTimeString;
        if (exifData["Exif.Photo.DateTimeOriginal"].toString().empty())
            exifData["Exif.Photo.DateTimeOriginal"] = currentDateTimeString;
        if (exifData["Exif.Photo.DateTimeDigitized"].toString().empty())
            exifData["Exif.Photo.DateTimeDigitized"] = currentDateTimeString;
        image->setExifData(exifData);
        result = true;
    }

#if EXIV2_TEST_VERSION(0,28,0)
    int64_t mv = iptcData["Iptc.Envelope.ModelVersion"].toInt64();
#else
    long mv = iptcData["Iptc.Envelope.ModelVersion"].toLong();
#endif
    if (!iptcData.empty() && mv != -1) {
        image->setIptcData(iptcData);
        result = true;
    }
#ifdef EXV_HAVE_XMP_TOOLKIT
    if (!xmpData.empty()) {
        image->setXmpData(xmpData);
        result = true;
    }
#endif // EXV_HAVE_XMP_TOOLKIT
    return result;
}

/** Returns a reference to metadatum object basing \a key string.
  * If first 4 characters of \a key string are \em "Exif" searchs metadatum
  * in exifData, otherwise in iptcData.
  */
Exiv2::Metadatum & Metadata::metadatum(const std::string &key) {
    if (key.substr(0,4).compare("Exif") == 0)
        return exifData[key];
    else
        return iptcData[key];
}

/** This is overloaded function, created for Exif orientation metadatum.\n
  * If \a field value is different from 0 this function has no effect.\n
  * Otherwise if firstEmptyFieldSkipped is true sets the \a field value to
  * integer representation of metadatum value basing \a key key.
  * Otherwise, if firstEmptyFieldSkipped is false sets them to true.
  * \sa isNullValue(char) setFieldString() metadatum()
  */
void Metadata::setFieldValue(char *field, const std::string &key) {
    if (!isNullValue(*field))
        return;

    if (firstEmptyFieldSkipped) {
        Exiv2::Metadatum &datum = metadatum(key);

        if (datum.count() > 0) {
#if EXIV2_TEST_VERSION(0,28,0)
            *field = datum.toInt64();
#else
            *field = datum.toLong();
#endif
        }
        else {
            *field = 1;
        }

        // validation structs field for rigth combobox index
        if (*field == -1)
            *field = 1;
    }
    else {
        firstEmptyFieldSkipped = true;
        emptyFieldKeyList.push_back(key);
    }
}

/** This is overloaded function.\n
  * If \a field value is non null string this function has no effect.\n
  * Otherwise if firstEmptyFieldSkipped is true sets the \a field value to
  * decimal string based integer representation of metadatum value basing
  * \a key key and append \a unit c-string using String::appendUnit() function.
  * Otherwise, if firstEmptyFieldSkipped is false sets them to true.
  * \sa isNullValue(String) String::appendUnit() setFieldString() metadatum()
  */
void Metadata::setFieldValue(String *field, const std::string &key, const char *unit) {
    if (!isNullValue(*field))
        return;
    if (firstEmptyFieldSkipped) {
        Exiv2::Metadatum &datum = metadatum(key);

#if EXIV2_TEST_VERSION(0,28,0)
        *field = String::number(datum.toInt64());
#else
        *field = String::number(datum.toLong());
#endif
        field->appendUnit(unit);
    }
    else {
        firstEmptyFieldSkipped = true;
        emptyFieldKeyList.push_back(key);
    }
}

/** If \a field value is non null string this function has no effect.\n
  * Otherwise if firstEmptyFieldSkipped is true sets the \a field value to
  * string based string representation of metadatum value basing \a key key.
  * If metadatum corresponding \a key1 key is empty it reads metadatum
  * coresponding \a key2 key.\n
  * Otherwise, if firstEmptyFieldSkipped is false sets them to true.
  * \sa isNullValue(String) String::appendUnit() setFieldValue() metadatum()
  */
void Metadata::setFieldString(String *field, const std::string &key1,
                              const std::string &key2) {
    if (!isNullValue(*field))
        return;
    if (firstEmptyFieldSkipped) {
        Exiv2::Metadatum *datum = &metadatum(key1);
        *field = String::fromStdString(datum->toString());
        if (field->isEmpty() && !key2.empty()) {
            datum = &metadatum(key2);
            *field = String::fromStdString(datum->toString());
        }
        if (field->isEmpty())
            *field = String::noData();
    }
    else {
        firstEmptyFieldSkipped = true;
        emptyFieldKeyList.push_back(key1);
        emptyFieldKeyList.push_back(key2);
    }
}

/** Sets Exif metadata based on exifStruct_ data.
  * \sa ExifStruct setExifStruct() setIptcStruct()
  */
void Metadata::setExifData() {
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
    String mString (exifStruct_.expTime);
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

/** Loads exifStruct_ data from Exif metadata stored in exifData.
  * \sa ExifStruct setExifData() setIptcData()
  */
void Metadata::setExifStruct() {
    // Image section
    /* I know from experience that this section often throws an exception
     * caused by invalid metadata in same files so I provide setField*(...)
     * functions here. --Marek */
    if (isNullValue(exifStruct_.version)) // Exif version
        exifStruct_.version = exif.getVersion();
    if (isNullValue(exifStruct_.processingSoftware)) { // processing software
        exifStruct_.processingSoftware = String::fromStdString(
                    exifData["Exif.Image.ProcessingSoftware"].toString() );
        if (exifStruct_.processingSoftware.isEmpty())
            exifStruct_.processingSoftware = String::noData();
    }
    setFieldValue(&exifStruct_.imageWidth, "Exif.Image.ImageWidth", " px");
    setFieldValue(&exifStruct_.imageHeight, "Exif.Image.ImageLength", " px");
    setFieldValue(&exifStruct_.orientation, "Exif.Image.Orientation");
    setFieldString(&exifStruct_.originalDate,
                   "Exif.Photo.DateTimeOriginal", "Exif.Image.DateTimeOriginal");
    setFieldString(&exifStruct_.digitizedDate,
                   "Exif.Photo.DateTimeDigitized", "Exif.Image.DateTimeDigitized");

    // Thumbnail section
    Exiv2::PreviewManager previewManager(*image);
    Exiv2::PreviewPropertiesList previewProperties = previewManager.getPreviewProperties();
    if (previewProperties.empty()) {
        exifStruct_.thumbnailImage = QImage();
        exifStruct_.thumbnailWidth = String::noData();
        exifStruct_.thumbnailHeight = String::noData();
    }
    else {
        Exiv2::PreviewImage thumbnail = previewManager.getPreviewImage(
                    previewManager.getPreviewProperties()[0] );
        exifStruct_.thumbnailImage = QImage(thumbnail.width(), thumbnail.height(),
                                            QImage::Format_ARGB32);
        exifStruct_.thumbnailImage.loadFromData(thumbnail.pData(), thumbnail.size());
        exifStruct_.thumbnailWidth = String::number( thumbnail.width() );
        exifStruct_.thumbnailWidth.appendUnit(" px");
        exifStruct_.thumbnailHeight = String::number( thumbnail.height() );
        exifStruct_.thumbnailHeight.appendUnit(" px");
    }

    // Photo section
    exifStruct_.focalLength = Exif::floatRational(exifData["Exif.Photo.FocalLength"]);
    exifStruct_.aperture = Exif::floatRational(exifData["Exif.Photo.FNumber"]);
    if (exifStruct_.aperture == -1)
        exifStruct_.aperture = Exif::floatRational(exifData["Exif.Photo.ApertureValue"]);
    exifStruct_.expTime = timeString("Exif.Image.ExposureTime","Exif.Photo.ExposureTime");
    exifStruct_.shutterSpeed = timeString("Exif.Photo.ShutterSpeedValue",
                                          "Exif.Image.ShutterSpeedValue");
    if (exifStruct_.shutterSpeed.isEmpty() || exifStruct_.shutterSpeed == "1/-1 s")
        exifStruct_.shutterSpeed = String::noData();
    exifStruct_.isoSpeed = Exif::getLong(exifData["Exif.Photo.ISOSpeedRatings"]);
    if ( exifStruct_.isoSpeed == -1 )
        exifStruct_.isoSpeed = Exif::getLong(exifData["Exif.Image.ISOSpeedRatings"]);
    exifStruct_.expBias = Exif::floatRational(exifData["Exif.Photo.ExposureBiasValue"]);
    if (exifStruct_.expBias == -1.f)
        exifStruct_.expBias = -101.f;
    exifStruct_.expProgram = Exif::getLong(exifData["Exif.Photo.ExposureProgram"]);
    if ( exifStruct_.expProgram == -1 ) //when value is unknown
        exifStruct_.expProgram = 0; //index of first item of combobox in MetadataDialog
    exifStruct_.meteringMode = Exif::getLong(exifData["Exif.Photo.MeteringMode"]);
    if ( exifStruct_.meteringMode == -1 )
        exifStruct_.meteringMode = Exif::getLong(exifData["Exif.Image.MeteringMode"]);
    if ( exifStruct_.meteringMode == -1 )  //when value is unknown
        exifStruct_.meteringMode = 0; //index of first item of combobox in MetadataDialog
    if ( exifStruct_.meteringMode == 255 ) //when value equal 'other'
        exifStruct_.meteringMode = 7;
    exifStruct_.flashMode = Exif::getLong(exifData["Exif.Photo.Flash"]);
    if ( exifStruct_.flashMode == -1 )
        exifStruct_.flashMode = Exif::getLong(exifData["Exif.Image.Flash"]);

    // Camera section
    exifStruct_.cameraManufacturer = String::fromStdString(
                exifData["Exif.Image.Make"].toString() );
    exifStruct_.cameraModel = String::fromStdString(
                exifData["Exif.Image.Model"].toString() );

    // Author section
    exifStruct_.artist = String::fromStdString(
                exifData["Exif.Image.Artist"].toString() );
    exifStruct_.copyright = String::fromStdString(
                exifData["Exif.Image.Copyright"].toString() );
    exifStruct_.userComment = String::fromStdString(
                exifData["Exif.Photo.UserComment"].toString() );
}

/** Loads iptcStruct_ data from IPTC metadata.
  * \sa IptcStruct setIptcData() setExifData()
  */
void Metadata::setIptcData() {
    if (!Iptc::isVersionKnown() || Iptc::version() < 2)
        iptcData["Iptc.Envelope.ModelVersion"] = 2;
    std::string keyString = "Iptc.Application2.DateCreated";
    if (iptcStruct_.dateCreated.isNull())
        removeDatum(keyString);
    else
        iptcData[keyString] = Iptc::dateString(iptcStruct_.dateCreated);
    keyString = "Iptc.Application2.TimeCreated";
    if (iptcStruct_.timeCreated.isNull())
        removeDatum(keyString);
    else
        iptcData[keyString] = Iptc::timeString(iptcStruct_.timeCreated);
    keyString = "Iptc.Application2.DigitizationDate";
    if (iptcStruct_.digitizationDate.isNull())
        removeDatum(keyString);
    else
        iptcData[keyString] = Iptc::dateString(iptcStruct_.digitizationDate);
    keyString = "Iptc.Application2.DigitizationTime";
    if (iptcStruct_.digitizationTime.isNull())
        removeDatum(keyString);
    else
        iptcData[keyString] = Iptc::timeString(iptcStruct_.digitizationTime);
    iptcData["Iptc.Application2.Byline"] = iptcStruct_.byline.toStdString();
    iptcData["Iptc.Application2.Copyright"] = iptcStruct_.copyright.toStdString();
    iptcData["Iptc.Application2.ObjectName"] = iptcStruct_.objectName.toStdString();
    keyString = "Iptc.Application2.Keywords";
    Exiv2::IptcKey key(keyString);
    Exiv2::IptcMetadata::iterator i = iptcData.findKey(key);
    // erase all old keywords
    while (i != iptcData.end() && keyString.compare(i->key()) == 0)
        i = iptcData.erase(i);
    QStringList keywords = iptcStruct_.keywords.split(' ', QString::SkipEmptyParts);
    // insert new keywords
    foreach (QString word, keywords) {
#if EXIV2_TEST_VERSION(0,28,0)
        Exiv2::Value::UniquePtr value = Exiv2::Value::create(Exiv2::string);
#else
        Exiv2::Value::AutoPtr value = Exiv2::Value::create(Exiv2::string);
#endif
        value->read(word.toStdString());
        iptcData.add(key,value.get());
    }
    iptcData["Iptc.Application2.Caption"] = iptcStruct_.caption.toStdString();
    iptcData["Iptc.Application2.CountryName"] = iptcStruct_.countryName.toStdString();
    iptcData["Iptc.Application2.City"] = iptcStruct_.city.toStdString();
    iptcData["Iptc.Application2.EditStatus"] = iptcStruct_.editStatus.toStdString();
}

/** Sets IPTC metadata based on iptcStruct_ data.
  * \sa IptcStruct setIptcStruct() setExifStruct()
  */
void Metadata::setIptcStruct() {
    if (Iptc::isVersionKnown())
        iptcStruct_.modelVersion = String::number(Iptc::version());
    else
        iptcStruct_.modelVersion = String::noData();
    iptcStruct_.dateCreated = Iptc::date(
                iptcData["Iptc.Application2.DateCreated"].toString() );
    iptcStruct_.timeCreated = Iptc::time(
                iptcData["Iptc.Application2.TimeCreated"].toString() );
    iptcStruct_.digitizationDate = Iptc::date(
                iptcData["Iptc.Application2.DigitizationDate"].toString() );
    iptcStruct_.digitizationTime = Iptc::time(
                iptcData["Iptc.Application2.DigitizationTime"].toString() );
    iptcStruct_.byline = iptcData["Iptc.Application2.Byline"].toString();
    iptcStruct_.copyright = iptcData["Iptc.Application2.Copyright"].toString();
    iptcStruct_.objectName = iptcData["Iptc.Application2.ObjectName"].toString();
    iptcStruct_.keywords = "";
    std::string keyString = "Iptc.Application2.Keywords";
    Exiv2::IptcKey key(keyString);
    Exiv2::IptcMetadata::iterator i = iptcData.findKey(key);
    Exiv2::IptcMetadata::iterator end = iptcData.end();
    while (i != end && keyString.compare(i->key()) == 0) {
        iptcStruct_.keywords += i->value().toString() + " ";
        ++i;
    }
    iptcStruct_.keywords.resize(iptcStruct_.keywords.size()-1); // remove last space
    iptcStruct_.caption = iptcData["Iptc.Application2.Caption"].toString();
    iptcStruct_.countryName = iptcData["Iptc.Application2.CountryName"].toString();
    iptcStruct_.city = iptcData["Iptc.Application2.City"].toString();
    iptcStruct_.editStatus = iptcData["Iptc.Application2.EditStatus"].toString();
}

void Metadata::removeDatum(const std::string &key) {
    QString standard = QString::fromStdString(key).split('.').first();
    if (standard == "Exif") {
        Exiv2::ExifMetadata::iterator i = exifData.findKey(Exiv2::ExifKey(key));
        if (i != exifData.end())
            exifData.erase(i);
    }
    else if (standard == "Iptc") {
        Exiv2::IptcMetadata::iterator i = iptcData.findKey(Exiv2::IptcKey(key));
        if (i != iptcData.end())
            iptcData.erase(i);
    }
}

/** Removes empty and broken fields from exifData and iptcData.
  * \sa emptyFieldKeyList
  */
void Metadata::removeEmptyFields() {
    emptyFieldKeyList.unique();
    std::list<std::string>::iterator it;
    for (it = emptyFieldKeyList.begin(); it != emptyFieldKeyList.end(); it++) {
        std::string familyName = (*it).substr(0,4);
        if (familyName.compare("Exif") == 0) {
            Exiv2::ExifKey key(*it);
            Exiv2::ExifMetadata::iterator itField = exifData.findKey(key);
            while (itField != exifData.end()) {
                exifData.erase(itField);
                itField = exifData.findKey(key);
            }
        }
        else {
            Exiv2::IptcKey key(*it);
            Exiv2::IptcMetadata::iterator itField = iptcData.findKey(key);
            while (itField != iptcData.end()) {
                iptcData.erase(itField);
                itField = iptcData.findKey(key);
            }
        }
    }
}

/** This is overloaded function useful for getting exposure time.\n
  * Returns valid fractional time string suffixed by /em " s" based on metadata
  * corresponding with \a key1 and \a key2 keys. If \a key1 is empty this function
  * returns empty string.\n
  * If \a key1 value isn't defined will be read \a key2 value. Otherwise will be
  * returned string based on \a key1.
  */
QString Metadata::timeString(const std::string &key1, const std::string &key2) {
    Q_ASSERT(!key1.empty());

    QString result;
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
  * Returns valid fractional time string based on \a rationalPower power of 0.5.
  */
QString Metadata::timeString(const Exiv2::Rational &rationalPower) {
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
  * Returns valid fractional time string based on \a rational value and if it's
  * not defined (-1/1) on \a key value.
  */
QString Metadata::timeString(Exiv2::Rational *rational, const std::string &key) {
    QString result;
    bool isEmpty((rational->first == -1  && rational->second == 1) ||
                 (rational->first == 1  && rational->second == -1));
    if (isEmpty) {
        if (!key.empty()) {
            *rational = Exif::rational(exifData[key]);
            isEmpty = false;
        }
        else
            result = String::noData();
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

/** Returns simplest fraction which equal \a integer divided by 10. */
Exiv2::Rational Metadata::simpleRational(int integer) {
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
  * Returns simplest fraction whitch equal \a rational value.
  * Maximum denominator expected is 100 (i.e. 25/100).
  */
Exiv2::Rational Metadata::simpleRational(const Exiv2::Rational &rational) {
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

/** Returns metadata value corresponding \a key string. Exif and IPTC metadata
  * are supported.\n
  * When this function fault returns negative value (-2 or -3).
  */
long Metadata::getLong(const QString &key) {
    QString standard = key.split('.').first();
    std::string str = key.toStdString();
    if (standard == "Exif") {
        Exiv2::ExifMetadata::iterator i = exifData.findKey(Exiv2::ExifKey(str));
        if (i != exifData.end()) {
#if EXIV2_TEST_VERSION(0,28,0)
            return i->value().toInt64();
#else
            return i->value().toLong();
#endif
        }
    }
    else if (standard == "Iptc") {
        Exiv2::IptcMetadata::iterator i = iptcData.findKey(Exiv2::IptcKey(str));
        if (i != iptcData.end()) {
#if EXIV2_TEST_VERSION(0,28,0)
            return i->value().toInt64();
#else
            return i->value().toLong();
#endif
        }
    }
    else
        return -3;
    return -2;
}

/** Returns true if typed \a format format file is write supported, otherwise false. */
bool Metadata::isWriteSupportedFormat(const QString &format) {
    return saveMetadataFormats.contains(format);
}

/** This is overloaded function.\n
  * Sets Exif metadatum corresponding to \a key key to typed \a value.
  */
void Metadata::setExifDatum(const std::string &key, int value) {
    if (key.empty())
        return;
    exifData[key] = value;
}

/** This is overloaded function.\n
  * Sets Exif metadatum corresponding to \a key1 and \a key2 to typed \a value.
  * \note If value following key1 or key2 isn't set, it will be set just known
  * value. If both keys values are unknown, this function will do nothing and
  * just return.
  */
void Metadata::setExifDatum(
        const std::string &key1, const std::string &key2, int value) {
    if (key1.empty() && key2.empty())
        return;

#if EXIV2_TEST_VERSION(0,28,0)
    int64_t key1_value = exifData[key1].toInt64();
    int64_t key2_value = exifData[key2].toInt64();
#else
    long key1_value = exifData[key1].toLong();
    long key2_value = exifData[key2].toLong();
#endif

    if (key1_value == 0)
        exifData[key2] = value;
    else if (key2_value == 0)
        exifData[key1] = value;
    else {
        exifData[key1] = value;
        exifData[key2] = value;
    }
}

/** This is overloaded function.\n
  * Sets Exif metadatum corresponding to \a key1 and \a key2 to typed \a value.
  * \note If value following key1 or key2 isn't set, it will be set just known
  * value. If both keys values are unknown, this function will do nothing and
  * just return.
  */
void Metadata::setExifDatum(
        const std::string &key1, const std::string &key2, const Exiv2::Rational &value) {
    if (key1.empty() && key2.empty())
        return;

#if EXIV2_TEST_VERSION(0,28,0)
    int64_t key1_value0 = exifData[key1].toInt64(0);
    int64_t key1_value1 = exifData[key1].toInt64(1);

    int64_t key2_value0 = exifData[key2].toInt64(0);
    int64_t key2_value1 = exifData[key2].toInt64(1);
#else
    long key1_value0 = exifData[key1].toLong(0);
    long key1_value1 = exifData[key1].toLong(1);

    long key2_value0 = exifData[key2].toLong(0);
    long key2_value1 = exifData[key2].toLong(1);
#endif

    if (key1_value0 == -1 && key1_value1 == 1)
        exifData[key2] = value;
    else if (key2_value0 == -1 && key2_value1 == 1)
        exifData[key1] = value;
    else {
        exifData[key1] = value;
        exifData[key2] = value;
    }
}

/** This is overloaded function.\n
  * Sets Exif metadatum corresponding to \a key1 and \a key2 to typed \a value.
  * \note If value following key1 or key2 isn't set, it will be set just known
  * value. If both keys values are unknown, this function will do nothing and
  * just return.
  */
void Metadata::setExifDatum(
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

/** This is overloaded function.
  *
  * Sets Exif thumbnail based on image loaded from \a path file path.
  */
void Metadata::setExifThumbnail(const std::string &path) {
    Exiv2::ExifThumb thumb (exifData);
    thumb.erase();
    thumb.setJpegThumbnail(path);
}

/** This is overloaded function.
  *
  * Sets \a Exif thumbnail based on \a image data saved as temporary file with
  * \a tid ID.
  *
  * If exception catched this error data will be appended to errorList list.
  *
  * If \a image is null pointer or image this function removes thumbnail from
  * exifData.
  *
  * \return True value if setting EXIF thumbnail succed or \a image is
  *         null pointer or image.
  * \return False value if setting EXIF thumbnail failed.
  * \sa lastError()
  */
bool Metadata::setExifThumbnail(QImage *image, int tid) {
    if (!image || image->isNull()) {
        Exiv2::ExifThumb thumb(exifData);
        thumb.erase();
        return true;
    }
    QString filePath = QDir::tempPath() + QDir::separator() + "sir_thumb";
    filePath += QString::number(tid) + ".jpg";
    image->save(filePath);
    try {
        setExifThumbnail(filePath.toStdString());
    }
    catch (Exiv2::Error &e) {
        errorList += new Error(tr("Save thumnail failed"),e);
        return false;
    }
    return true;
}

#endif // SIR_METADATA_SUPPORT
