#include "metadatautils.h"
#include "defines.h"
#include <QString>
#include <QDir>

#include <QDebug>

bool MetadataUtils::Metadata::enabled = false;
bool MetadataUtils::Metadata::save = false;
QStringList MetadataUtils::Metadata::saveMetadataFormats =
        QStringList() << "jpeg" << "jpg" << "png" << "tif" << "tiff";

MetadataUtils::Metadata::Metadata()
{
}

bool MetadataUtils::Metadata::read(const MetadataUtils::String& path, bool setupStructs) {
    try {
        std::string filePath = path.toNativeStdString();
        image = Exiv2::ImageFactory::open(filePath);
        image->readMetadata();

        exifData = image->exifData();
        MetadataUtils::Exif::setVersionKnown(
                    exifData["Exif.Photo.ExifVersion"].toString().empty() );
        if (setupStructs)
            setExifStruct();

        iptcData = image->iptcData();
#ifdef EXV_HAVE_XMP_TOOLKIT
        xmpData = image->xmpData();
#endif // EXV_HAVE_XMP_TOOLKIT
        image.reset();
        return true;
    }
    catch (Exiv2::Error &e) {
        lastError_.setMessage(tr("Error open file %1").arg( path.toQString() ) );
        lastError_.copy(e);
        return false;
    }
}

bool MetadataUtils::Metadata::read(const QString &path, bool setupStructs) {
    return read((const MetadataUtils::String&)path,setupStructs);
}

bool MetadataUtils::Metadata::write(const MetadataUtils::String& path, const QImage& qImage) {
    try {
        std::string filePath = path.toNativeStdString();
        image = Exiv2::ImageFactory::open(filePath);
        setData(qImage);
        image->writeMetadata();
        image.reset();
        return true;
    }
    catch (Exiv2::Error &e) {
        lastError_.setMessage(tr("Error write file %1").arg( path.toQString() ) );
        lastError_.copy(e);
        return false;
    }
}

bool MetadataUtils::Metadata::write(const QString &path, const QImage &image) {
    return write((const MetadataUtils::String&)path, image);
}

void MetadataUtils::Metadata::setData(const QImage& qImage) {
    if (MetadataUtils::Exif::isArtistOverwrite())
        exifData["Exif.Image.Artist"] =
                MetadataUtils::Exif::stringArtist().toStdString();
    if (MetadataUtils::Exif::isCopyrightOverwrite())
        exifData["Exif.Image.Copyright"] =
                MetadataUtils::Exif::stringCopyright().toStdString();
    if (MetadataUtils::Exif::isUserCommentOverwrite())
        exifData["Exif.Photo.UserComment"] =
                MetadataUtils::Exif::stringUserComment().toNativeStdString();

    if (!exifData.empty())
    {
        if (!MetadataUtils::Exif::isVersionKnown() )
        {
            Exiv2::byte version[4] = { 48, 50, 50, 48 };
            Exiv2::DataValue value(version,4);
            exifData["Exif.Photo.ExifVersion"] = value;
        }
        exifData["Exif.Image.ProcessingSoftware"] = "Simple Image Resizer "
                VERSION;
        if (!qImage.isNull())
        {
            exifData["Exif.Image.ImageWidth"] = qImage.width();
            exifData["Exif.Image.ImageLength"] = qImage.height();
        }
    }
    image->setExifData(exifData);
    image->setIptcData(iptcData);
#ifdef EXV_HAVE_XMP_TOOLKIT
    image->setXmpData(xmpData);
#endif // EXV_HAVE_XMP_TOOLKIT
}

void MetadataUtils::Metadata::setExifData()
{
    // Image section
    exifData["Exif.Image.Orientation"] = exifStruct_.orientation;
    setExifDatum("Exif.Photo.DateTimeOriginal","Exif.Image.DateTimeOriginal",
                 exifStruct_.originalDate.toStdString() );
    exifData["Exif.Photo.DateTimeDigitized"] = exifStruct_.digitizedDate.toStdString();

    // Photo section
    Exiv2::Rational rational;
    rational = shortRational( exifStruct_.focalLength * 10 );
    exifData["Exif.Photo.FocalLength"] = rational;
    rational = shortRational( exifStruct_.aperture * 10 );
    setExifDatum("Exif.Photo.ApertureValue","Exif.Photo.FNumber",rational);
    MetadataUtils::String mString (exifStruct_.expTime);
    rational = mString.toRational();
    setExifDatum("Exif.Image.ExposureTime","Exif.Photo.ExposureTime",rational);
    rational = shortRational( exifStruct_.expBias * 10 );
    setExifDatum("Exif.Image.ExposureBiasValue","Exif.Photo.ExposureBiasValue",rational);
    setExifDatum("Exif.Image.ISOSpeedRatings","Exif.Photo.ISOSpeedRatings",exifStruct_.isoSpeed);
    setExifDatum("Exif.Image.Flash","Exif.Photo.Flash",exifStruct_.flashMode);
    setExifDatum("Exif.Image.ExposureProgram","Exif.Photo.ExposureProgram",exifStruct_.expProgram);
    setExifDatum("Exif.Image.MeteringMode","Exif.Photo.MeteringMode",exifStruct_.meteringMode);

    // Camera section
    exifData["Exif.Image.Make"] = exifStruct_.cameraManufacturer.toStdString();
    exifData["Exif.Image.Model"] = exifStruct_.cameraModel.toStdString();

    // Author section
    exifData["Exif.Image.Artist"] = exifStruct_.artist.toStdString();
    exifData["Exif.Image.Copyright"] = exifStruct_.copyright.toStdString();
    exifData["Exif.Photo.UserComment"] = exifStruct_.userComment.toNativeStdString();
}

void MetadataUtils::Metadata::setExifStruct()
{
    // Image section
    exifStruct_.version = MetadataUtils::String::exifVersion(
                exifData["Exif.Photo.ExifVersion"] );
    exifStruct_.processingSoftware = MetadataUtils::String::fromStdString(
                exifData["Exif.Image.ProcessingSoftware"].toString() );
    if (exifStruct_.processingSoftware.isEmpty())
        exifStruct_.processingSoftware = MetadataUtils::String::noData();
    exifStruct_.imageWidth = QString::number(
                exifData["Exif.Image.ImageWidth"].toLong() );
    exifStruct_.imageWidth.appendUnit(" px");
    exifStruct_.imageHeight = QString::number(
                exifData["Exif.Image.ImageLength"].toLong() );
    exifStruct_.imageHeight.appendUnit(" px");
    exifStruct_.orientation = exifData["Exif.Image.Orientation"].toLong();
    exifStruct_.originalDate = QString::fromStdString(
                exifData["Exif.Photo.DateTimeOriginal"].toString() );
    if (exifStruct_.originalDate.isEmpty())
        exifStruct_.originalDate = QString::fromStdString(
                    exifData["Exif.Image.DateTimeOriginal"].toString() );
    exifStruct_.digitizedDate = QString::fromStdString(
                exifData["Exif.Photo.DateTimeDigitized"].toString() );
    if (exifStruct_.digitizedDate.isEmpty())
        exifStruct_.digitizedDate = QString::fromStdString(
                    exifData["Exif.Image.DateTimeDigitized"].toString() );

    // Thumbnail section
    Exiv2::PreviewManager previewMenager(*image);
    Exiv2::PreviewImage thumbnail = previewMenager.getPreviewImage(
                previewMenager.getPreviewProperties()[0] );
    exifStruct_.thumbnailImage = QImage(thumbnail.width(), thumbnail.height(),
                                        QImage::Format_ARGB32);
    exifStruct_.thumbnailImage.loadFromData(thumbnail.pData(), thumbnail.size());
    exifStruct_.thumbnailWidth = MetadataUtils::String::number( thumbnail.width() );
    exifStruct_.thumbnailWidth.appendUnit(" px");
    exifStruct_.thumbnailHeight = MetadataUtils::String::number( thumbnail.height() );
    exifStruct_.thumbnailHeight.appendUnit(" px");

    // Photo section
    Exiv2::Rational rational = exifData["Exif.Photo.FocalLength"].toRational();
    exifStruct_.focalLength = (float)rational.first / rational.second;
    rational = exifData["Exif.Photo.FNumber"].toRational();
    if ( rational.first == -1 )
        rational = exifData["Exif.Photo.ApertureValue"].toRational();
    exifStruct_.aperture = (float)rational.first / rational.second;
    rational = exifData["Exif.Photo.ExposureTime"].toRational();
    if ( rational.first == -1 )
        exifStruct_.expTime = MetadataUtils::String::noData();
    else if ( rational.first < rational.second )
        exifStruct_.expTime = QString::number(rational.first) + "/" +
                QString::number(rational.second) + " s";
    else
    {
        short integer = rational.first / rational.second;
        exifStruct_.expTime =  QString::number(integer, 'f', 1);
        int fraction = rational.first - integer*rational.second;
        if (fraction != 0)
            exifStruct_.expTime += " " + QString::number(fraction) +
                "/" + QString::number(rational.second);
        exifStruct_.expTime.append(" s");
    }
    exifStruct_.isoSpeed = exifData["Exif.Photo.ISOSpeedRatings"].toLong();
    if ( exifStruct_.isoSpeed == -1 )
        exifStruct_.isoSpeed = exifData["Exif.Image.ISOSpeedRatings"].toLong();
    rational = exifData["Exif.Photo.ExposureBiasValue"].toRational();
    exifStruct_.expBias = (float)rational.first / rational.second;
    exifStruct_.expProgram = exifData["Exif.Photo.ExposureProgram"].toLong();
    exifStruct_.meteringMode = exifData["Exif.Photo.MeteringMode"].toLong();
    if ( exifStruct_.meteringMode == -1 )
        exifStruct_.meteringMode = exifData["Exif.Image.MeteringMode"].toLong();
    if ( exifStruct_.meteringMode == 255 ) //when value equal 'other'
        exifStruct_.meteringMode = 7;
    exifStruct_.flashMode = exifData["Exif.Photo.Flash"].toLong();
    if ( exifStruct_.flashMode == -1 )
        exifStruct_.flashMode = exifData["Exif.Image.Flash"].toLong();

    // Camera section
    exifStruct_.cameraManufacturer = QString::fromStdString(
                exifData["Exif.Image.Make"].toString() );
    exifStruct_.cameraModel = QString::fromStdString(
                exifData["Exif.Image.Model"].toString() );

    // Author section
    exifStruct_.artist = QString::fromStdString(
                exifData["Exif.Image.Artist"].toString() );
    if (exifStruct_.artist.isEmpty())
        exifStruct_.artist = MetadataUtils::String::noData();
    exifStruct_.copyright = QString::fromStdString(
                exifData["Exif.Image.Copyright"].toString() );
    if (exifStruct_.copyright.isEmpty())
        exifStruct_.copyright = MetadataUtils::String::noData();
    exifStruct_.userComment = QString::fromStdString(
                exifData["Exif.Photo.UserComment"].toString() );
    if (exifStruct_.userComment.isEmpty())
        exifStruct_.userComment = MetadataUtils::String::noData();
}

Exiv2::Rational MetadataUtils::Metadata::shortRational(int integer)
{
    Exiv2::Rational result;
    if ( integer % 10 == 0 )
    {
        result.first = integer / 10;
        result.second = 1;
    }
    else if ( integer % 5 == 0 )
    {
        result.first = integer / 5;
        result.second = 2;
    }
    else if ( integer % 2 == 0 )
    {
        result.first = integer / 2;
        result.second = 5;
    }
    else
    {
        result.first = integer;
        result.second = 10;
    }
    return result;
}

bool MetadataUtils::Metadata::isEnabled() {
    return enabled;
}

void MetadataUtils::Metadata::setEnabled(bool v) {
    enabled = v;
}

bool MetadataUtils::Metadata::isSave() {
    return save;
}

void MetadataUtils::Metadata::setSave(bool v) {
    save = v;
}

bool MetadataUtils::Metadata::isWriteSupportedFormat(const QString &format) {
    return saveMetadataFormats.contains(format);
}

void MetadataUtils::Metadata::setExifDatum(const std::string &key, int value) {
    if (key.empty())
        return;
    exifData[key] = value;
}

void MetadataUtils::Metadata::setExifDatum(
        const std::string &key1, const std::string &key2, int value) {
    if ( key1.empty() || key2.empty() )
        return;

    if ( exifData[key1].toLong() == 0 )
        exifData[key2] = value;
    else if ( exifData[key2].toLong() == 0 )
        exifData[key1] = value;
    else {
        exifData[key1] = value;
        exifData[key2] = value;
    }
}

void MetadataUtils::Metadata::setExifDatum(
        const std::string &key1, const std::string &key2, Exiv2::Rational value) {
    if ( key1.empty() || key2.empty() )
        return;

    if ( exifData[key1].toLong(0) == -1 && exifData[key1].toLong(1) == 1 )
        exifData[key2] = value;
    else if ( exifData[key2].toLong(0) == -1 && exifData[key2].toLong(1) == 1 )
        exifData[key1] = value;
    else {
        exifData[key1] = value;
        exifData[key2] = value;
    }
}

void MetadataUtils::Metadata::setExifDatum(
        const std::string &key1, const std::string &key2, const std::string &value) {
    if ( key1.empty() || key2.empty() )
        return;

    if ( exifData[key1].toString().empty() )
        exifData[key2] = value;
    else if ( exifData[key2].toString().empty() )
        exifData[key1] = value;
    else {
        exifData[key1] = value;
        exifData[key2] = value;
    }
}

void MetadataUtils::Metadata::setExifThumbnail(const std::string &path) {
    Exiv2::ExifThumb thumb (exifData);
    thumb.erase();
    thumb.setJpegThumbnail(path);
}

bool MetadataUtils::Metadata::setExifThumbnail(QImage *image, int tid) {
    if (image->isNull())
        return false;
    QString filePath = QDir::tempPath() + QDir::separator() + "sir_thumb";
    filePath += QString::number(tid) + ".jpg";
    image->save(filePath);
    try {
        setExifThumbnail(filePath.toStdString());
    }
    catch (Exiv2::Error &e) {
        lastError_.copy(e);
        lastError_.setMessage(tr("Save thumnail failed"));
        return false;
    }
    return true;
}
