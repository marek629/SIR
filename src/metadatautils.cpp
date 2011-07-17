#include "metadatautils.h"
#include <QString>
#include <QObject>

bool MetadataUtils::enabled=false;
bool MetadataUtils::save=false;

MetadataUtils::MetadataUtils()
{
}

void MetadataUtils::read(const QString& path) {
    std::string filePath = nativeStdString(path);
    image = Exiv2::ImageFactory::open(filePath);
    image->readMetadata();
    exifData = image->exifData();
    iptcData = image->iptcData();
    xmpData = image->xmpData();
    image.reset();
}

void MetadataUtils::write(const QString& path) {
    std::string filePath = nativeStdString(path);
    image = Exiv2::ImageFactory::open(filePath);
    exifData["Exif.Photo.UserComment"] = nativeStdString (
                  QObject::tr("This picture was changed with Simple Image Resiser") );
    image->setExifData(exifData);
    image->setIptcData(iptcData);
    image->setXmpData(xmpData);
    image->writeMetadata();
    image.reset();
}

std::string MetadataUtils::nativeStdString(const QString& str) const {
#ifdef Q_OS_WIN32
    return str.toLocal8Bit().constData();
#else
    return str.toUtf8().constData();
#endif
}

short int MetadataUtils::exifOrientation() {
    return exifData["Exif.Image.Orientation"].toLong();
}

void MetadataUtils::setExifOrientation(short int v) {
    exifData["Exif.Image.Orientation"] = v;
}

bool MetadataUtils::isEnabled() {
    return enabled;
}

void MetadataUtils::setEnabled(bool v) {
    enabled = v;
}

bool MetadataUtils::isSave() {
    return save;
}

void MetadataUtils::setSave(bool v) {
    save = v;
}
