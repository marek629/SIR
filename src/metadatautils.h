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

#ifndef METADATA_H
#define METADATA_H

#include <QImage>
#include "metadata/exif.h"
#include "metadata/string.h"
#ifdef SIR_METADATA_SUPPORT
#include <exiv2/exiv2.hpp>
#include "metadata/iptc.h"
#include "metadata/error.h"

//! Metadata tools.
namespace MetadataUtils {
//! Main metadata manipulating class.
class Metadata : public QObject {
    Q_DECLARE_TR_FUNCTIONS(MetadataUtils::Metadata)

public:
    // methods
    Metadata();
    ~Metadata();
    bool read(const String& path, bool setupStructs = false, bool fromSvg = false);
    bool read(const QString& path, bool setupStructs = false, bool fromSvg = false);
    bool write(const String& path, const QImage& image = QImage());
    bool write(const QString& path, const QImage& image = QImage());
    void close();
    void clearMetadata();
    void setExifData();
    void setExifStruct();
    void setIptcData();
    void setIptcStruct();
    void setFieldValue(char *field, const std::string &key);
    void setFieldValue(String *field, const std::string &key, const char *unit);
    void setFieldString(String *field, const std::string &key1, const std::string &key2);
    void setExifDatum(const std::string &key, int value);
    void setExifDatum(const std::string &key1, const std::string &key2, int value);
    void setExifDatum(const std::string &key1, const std::string &key2,
                      const Exiv2::Rational &value);
    void setExifDatum(const std::string &key1, const std::string &key2,
                      const std::string &value);
    void setExifThumbnail(const std::string &path);
    bool setExifThumbnail(QImage *image, int tid = 0);
    QString timeString(const std::string &key1, const std::string &key2 = "");
    QString timeString(const Exiv2::Rational &rational);
    QString timeString(Exiv2::Rational *rational, const std::string &key);
    static Exiv2::Rational simpleRational(int integer);
    static Exiv2::Rational simpleRational(const Exiv2::Rational &rationalPower);
    long getLong(const QString &key);
    Exiv2::Image::AutoPtr imageAutoPtr() { return image; }
    Exiv2::Metadatum &metadatum(const std::string &key);
    /** Returns size of readed image in pixels. */
    QSize imageSize() const { return QSize(image->pixelWidth(), image->pixelHeight()); }
    /** Returns pointer to \a Exif struct. */
    ExifStruct *exifStruct() { return &exifStruct_; }
    /** Returns pointer to \a IPTC struct. */
    IptcStruct *iptcStruct() { return &iptcStruct_; }
    /** Takes last item from errorList and returns pointer error object. */
    Error *lastError() { return errorList.takeLast(); }
    static bool isWriteSupportedFormat(const QString &format);

private:
    // fields
    Exiv2::Image::AutoPtr image;
    Exiv2::ExifData exifData;
    Exiv2::IptcData iptcData;
#ifdef EXV_HAVE_XMP_TOOLKIT
    Exiv2::XmpData xmpData;
#endif // EXV_HAVE_XMP_TOOLKIT
    QList<Error*> errorList;
    static QStringList saveMetadataFormats;
    ExifStruct exifStruct_;
    Exif exif;
    IptcStruct iptcStruct_;
    bool firstEmptyFieldSkipped;
    std::list<std::string> emptyFieldKeyList;
    // methods
    bool setData(const QImage &img);
    void removeDatum(const std::string &key);
    void removeEmptyFields();
};

bool isNullValue(char v);
bool isNullValue(int v);
bool isNullValue(float v);
bool isNullValue(const String &v);
bool isNullValue(const QImage &v);
}
#endif // SIR_METADATA_SUPPORT

#endif // METADATA_H
