#ifndef METADATA_H
#define METADATA_H

#include "metadata/exif.h"
#include "metadata/string.h"
#include "metadata/error.h"
#include <QObject>
#include <QImage>
#include <exiv2/exiv2.hpp>

namespace MetadataUtils
{
    class Metadata : public QObject
    {
        Q_OBJECT
    public:
        Metadata();

        // Read metadata and (optionaly) setup exif struct.
        // If you would like use any metadata structs, like exif struct type setupStructs as true.
        bool read(const String& path, bool setupStructs = false);
        bool read(const QString& path, bool setupStructs = false);

        bool write(const String& path, const QImage& image = QImage());
        bool write(const QString& path, const QImage& image = QImage());
        void clearMetadata();
        void setExifData();
        void setExifStruct();
        void setExifDatum(const std::string &key1, const std::string &key2, int value);
        void setExifDatum(const std::string &key1, const std::string &key2,
                          const Exiv2::Rational &value);
        void setExifDatum(const std::string &key1, const std::string &key2,
                          const std::string &value);
        QString timeString(const std::string &key1, const std::string &key2 = "");
        static Exiv2::Rational shortRational(int integer);
        MetadataUtils::ExifStruct *exifStruct() { return &exifStruct_; }
        Error *lastError() { return &lastError_; }
        static void setEnabled(bool);
        static bool isEnabled();
        static void setSave(bool);
        static bool isSave();


    private:
        Exiv2::Image::AutoPtr image;
        Exiv2::ExifData exifData;
        Exiv2::IptcData iptcData;
#ifdef EXV_HAVE_XMP_TOOLKIT
        Exiv2::XmpData xmpData;
#endif // EXV_HAVE_XMP_TOOLKIT
        Error lastError_;
        static bool enabled;
        static bool save;
        MetadataUtils::ExifStruct exifStruct_;
    };
}

#endif // METADATA_H
