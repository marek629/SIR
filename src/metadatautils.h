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

private:
    std::string nativeString(const QString& path) const;
    Exiv2::Image::AutoPtr image;
    Exiv2::ExifData exifData;
    Exiv2::IptcData iptcData;
    Exiv2::XmpData xmpData;
};

#endif // METADATA_H
