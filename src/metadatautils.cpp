#include "metadatautils.h"
#include <QString>
#include <QObject>

MetadataUtils::MetadataUtils()
{
}

void MetadataUtils::read(const QString& path) {
    std::string filePath = nativeString(path);
    image = Exiv2::ImageFactory::open(filePath);
    image->readMetadata();
    exifData = image->exifData();
    iptcData = image->iptcData();
    xmpData = image->xmpData();
    image.reset();
}

void MetadataUtils::write(const QString& path) {
    std::string filePath = nativeString(path);
    image = Exiv2::ImageFactory::open(filePath);
    exifData["Exif.Photo.UserComment"] = nativeString (
                  QObject::tr("This picture was changed with Simple Image Resiser") );
    image->setExifData(exifData);
    image->setIptcData(iptcData);
    image->setXmpData(xmpData);
    image->writeMetadata();
    image.reset();
}

std::string MetadataUtils::nativeString(const QString& str) const {
#ifdef _WIN32
    return str.toLocal8Bit().constData();
#else
    return str.toUtf8().constData();
#endif
}
