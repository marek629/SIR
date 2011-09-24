#ifndef METADATA_H
#define METADATA_H

#include <exiv2/exiv2.hpp>
#include "metadata/exif.h"
#include <QImage>

#define EXIF_STD_VERSION "0220"

class QString;

namespace MetadataUtils
{
    class Metadata
    {
    public:
        Metadata();
        void read(const QString& path);
        void write(const QString& path, const QImage& image = QImage());
        void setExifStruct();
        MetadataUtils::ExifStruct *ptrExifStruct() { return &exifStruct; }
        static void setEnabled(bool);
        static bool isEnabled();
        static void setSave(bool);
        static bool isSave();
        static const QString &noData();

    private:
        std::string nativeStdString(const QString& path) const;
        QString stringFloat2(const Exiv2::Metadatum &datum);
        void appendString(QString *str, const QString &add = "");
        Exiv2::Image::AutoPtr image;
        Exiv2::ExifData exifData;
        Exiv2::IptcData iptcData;
        Exiv2::XmpData xmpData;
        static bool enabled;
        static bool save;
        static QString noData_;
        MetadataUtils::ExifStruct exifStruct;
    };
}

#endif // METADATA_H
