#include "metadatautils.h"
#include <QString>
#include <QObject>

bool MetadataUtils::enabled = false;
bool MetadataUtils::save = false;
bool MetadataUtils::overwriteExifArtist = false;
QString MetadataUtils::_stringExifArtist = "";
bool MetadataUtils::overwriteExifCopyright = false;
QString MetadataUtils::_stringExifCopyright = "";
bool MetadataUtils::overwriteExifUserComment = false;
QString MetadataUtils::_stringExifUserComment = "";

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

    if (overwriteExifArtist)
        exifData["Exif.Image.Artist"] = _stringExifArtist.toStdString();
    if (overwriteExifCopyright)
        exifData["Exif.Image.Copyright"] = _stringExifCopyright.toStdString();
    if (overwriteExifUserComment)
    {
        std::string userComment = "charset=\"Unicode\" ";
        userComment += nativeStdString(_stringExifUserComment);
        exifData["Exif.Photo.UserComment"] = userComment;
    }

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

bool MetadataUtils::isOverwriteExifArtist() {
    return overwriteExifArtist;
}

void MetadataUtils::setOverwriteExifArtist(bool v) {
    overwriteExifArtist = v;
}

QString MetadataUtils::stringExifArtist() {
    return _stringExifArtist;
}

void MetadataUtils::setStringExifArtist(const QString &s) {
    _stringExifArtist = s;
}

bool MetadataUtils::isOverwriteExifCopyright() {
    return overwriteExifCopyright;
}

void MetadataUtils::setOverwriteExifCopyright(bool v) {
    overwriteExifCopyright = v;
}

QString MetadataUtils::stringExifCopyright() {
    return _stringExifCopyright;
}

void MetadataUtils::setStringExifCopyright(const QString &s) {
    _stringExifCopyright = s;
}

bool MetadataUtils::isOverwriteExifUserComment() {
    return overwriteExifUserComment;
}

void MetadataUtils::setOverwriteExifUserComment(bool v){
    overwriteExifUserComment = v;
}

QString MetadataUtils::stringExifUserComment() {
    return _stringExifUserComment;
}

void MetadataUtils::setStringExifUserComment(const QString &s) {
    _stringExifUserComment = s;
}
