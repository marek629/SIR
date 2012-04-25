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

#ifndef METADATA_H
#define METADATA_H

#include "metadata/exif.h"
#include "metadata/string.h"
#include "metadata/error.h"
#include <QObject>
#include <QImage>
#include <exiv2/exiv2.hpp>

//! Metadata tools.
namespace MetadataUtils {
    //! Main metadata manipulating class.
    class Metadata : public QObject {
        Q_OBJECT
    public:
        Metadata();
        ~Metadata();
        bool read(const String& path, bool setupStructs = false);
        bool read(const QString& path, bool setupStructs = false);
        bool write(const String& path, const QImage& image = QImage());
        bool write(const QString& path, const QImage& image = QImage());
        void close();
        void clearMetadata();
        void setExifData();
        void setExifStruct();
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
        Exiv2::Image::AutoPtr imageAutoPtr() { return image; }
        MetadataUtils::ExifStruct *exifStruct() { return &exifStruct_; } /**< Returns pointer
            to \a Exif struct. */
        Error *lastError() { return &lastError_; } /**< Returns pointer to last error object. */
        static void setEnabled(bool);
        static bool isEnabled();
        static void setSave(bool);
        static bool isSave();
        static bool isWriteSupportedFormat(const QString &format);


    private:
        void setData(const QImage &img);

        Exiv2::Image::AutoPtr image;
        Exiv2::ExifData exifData;
        Exiv2::IptcData iptcData;
#ifdef EXV_HAVE_XMP_TOOLKIT
        Exiv2::XmpData xmpData;
#endif // EXV_HAVE_XMP_TOOLKIT
        Error lastError_;
        static bool enabled;
        static bool save;
        static QStringList saveMetadataFormats;
        MetadataUtils::ExifStruct exifStruct_;
        MetadataUtils::Exif exif;
    };
}

#endif // METADATA_H
