#ifndef METADATA_H
#define METADATA_H

#include <exiv2/exiv2.hpp>

class QString;

class MetadataUtils
{
public:
    MetadataUtils();
    void read(const QString& path);
    void write(const QString& path);
    short int exifOrientation();
    void setExifOrientation(short int);
    static void setEnabled(bool);
    static bool isEnabled();
    static void setSave(bool);
    static bool isSave();

private:
    std::string nativeStdString(const QString& path) const;
    Exiv2::Image::AutoPtr image;
    Exiv2::ExifData exifData;
    Exiv2::IptcData iptcData;
    Exiv2::XmpData xmpData;
    static bool enabled;
    static bool save;
};

#endif // METADATA_H
