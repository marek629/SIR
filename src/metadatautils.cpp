#include "metadatautils.h"
#include "defines.h"
#include <QString>

#include <QDebug>

bool MetadataUtils::Metadata::enabled = false;
bool MetadataUtils::Metadata::save = false;

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
        xmpData = image->xmpData();
        image.reset();
        return true;
    }
    catch (Exiv2::Error &e) {
        qWarning("Error open file %s\nError code %d\n%s\n",
                 path.toNativeStdString().c_str(), e.code() ,e.what());
        lastError = &e;
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
        image->setXmpData(xmpData);
        image->writeMetadata();

        image.reset();
        return true;
    }
    catch (Exiv2::Error &e) {
        qWarning("Error write file %s\nError code %d\n%s\n",
                 path.toNativeStdString().c_str(), e.code(), e.what());
        lastError = &e;
        return false;
    }
}

bool MetadataUtils::Metadata::write(const QString &path, const QImage &image) {
    return write((const MetadataUtils::String&)path, image);
}

void MetadataUtils::Metadata::setExifData()
{
    // Image section
    exifData["Exif.Image.Orientation"] = exifStruct.orientation;
    setExifDatum("Exif.Photo.DateTimeOriginal","Exif.Image.DateTimeOriginal",
                 exifStruct.originalDate.toStdString() );
    exifData["Exif.Photo.DateTimeDigitized"] = exifStruct.digitizedDate.toStdString();

    // Photo section
    Exiv2::Rational rational;
    rational = shortRational( exifStruct.focalLength * 10 );
    exifData["Exif.Photo.FocalLength"] = rational;
    rational = shortRational( exifStruct.aperture * 10 );
    setExifDatum("Exif.Photo.ApertureValue","Exif.Photo.FNumber",rational);
    MetadataUtils::String mString (exifStruct.expTime);
    rational = mString.toRational();
    setExifDatum("Exif.Image.ExposureTime","Exif.Photo.ExposureTime",rational);
    rational = shortRational( exifStruct.expBias * 10 );
    setExifDatum("Exif.Image.ExposureBiasValue","Exif.Photo.ExposureBiasValue",rational);
    setExifDatum("Exif.Image.ISOSpeedRatings","Exif.Photo.ISOSpeedRatings",exifStruct.isoSpeed);
    setExifDatum("Exif.Image.Flash","Exif.Photo.Flash",exifStruct.flashMode);
    setExifDatum("Exif.Image.ExposureProgram","Exif.Photo.ExposureProgram",exifStruct.expProgram);
    setExifDatum("Exif.Image.MeteringMode","Exif.Photo.MeteringMode",exifStruct.meteringMode);

    // Camera section
    exifData["Exif.Image.Make"] = exifStruct.cameraManufacturer.toStdString();
    exifData["Exif.Image.Model"] = exifStruct.cameraModel.toStdString();

    // Author section
    exifData["Exif.Image.Artist"] = exifStruct.artist.toStdString();
    exifData["Exif.Image.Copyright"] = exifStruct.copyright.toStdString();
    exifData["Exif.Photo.UserComment"] = exifStruct.userComment.toNativeStdString();
}

void MetadataUtils::Metadata::setExifStruct()
{
    // Image section
    exifStruct.version = MetadataUtils::String::fromMetadatum(
                exifData["Exif.Photo.ExifVersion"] );
    exifStruct.processingSoftware = MetadataUtils::String::fromStdString(
                exifData["Exif.Image.ProcessingSoftware"].toString() );
    if (exifStruct.processingSoftware.isEmpty())
        exifStruct.processingSoftware = MetadataUtils::String::noData();
    exifStruct.imageWidth = QString::number(
                exifData["Exif.Image.ImageWidth"].toLong() );
    exifStruct.imageWidth.appendUnit(" px");
    exifStruct.imageHeight = QString::number(
                exifData["Exif.Image.ImageLength"].toLong() );
    exifStruct.imageHeight.appendUnit(" px");
    exifStruct.orientation = exifData["Exif.Image.Orientation"].toLong();
    exifStruct.originalDate = QString::fromStdString(
                exifData["Exif.Photo.DateTimeOriginal"].toString() );
    if (exifStruct.originalDate.isEmpty())
        exifStruct.originalDate = QString::fromStdString(
                    exifData["Exif.Image.DateTimeOriginal"].toString() );
    exifStruct.digitizedDate = QString::fromStdString(
                exifData["Exif.Photo.DateTimeDigitized"].toString() );
    if (exifStruct.digitizedDate.isEmpty())
        exifStruct.digitizedDate = QString::fromStdString(
                    exifData["Exif.Image.DateTimeDigitized"].toString() );

    // Thumbnail section
    Exiv2::PreviewManager previewMenager(*image);
    Exiv2::PreviewImage thumbnail = previewMenager.getPreviewImage(
                previewMenager.getPreviewProperties()[0] );
    exifStruct.thumbnailPixmap = QPixmap(thumbnail.width(), thumbnail.height());
    exifStruct.thumbnailPixmap.loadFromData(thumbnail.pData(), thumbnail.size());
    exifStruct.thumbnailWidth = MetadataUtils::String::number( thumbnail.width() );
    exifStruct.thumbnailWidth.appendUnit(" px");
    exifStruct.thumbnailHeight = MetadataUtils::String::number( thumbnail.height() );
    exifStruct.thumbnailHeight.appendUnit(" px");

    // Photo section
    Exiv2::Rational rational = exifData["Exif.Photo.FocalLength"].toRational();
    exifStruct.focalLength = (float)rational.first / rational.second;
    rational = exifData["Exif.Photo.FNumber"].toRational();
    if ( rational.first == -1 )
        rational = exifData["Exif.Photo.ApertureValue"].toRational();
    exifStruct.aperture = (float)rational.first / rational.second;
    rational = exifData["Exif.Photo.ExposureTime"].toRational();
    if ( rational.first == -1 )
        exifStruct.expTime = MetadataUtils::String::noData();
    else if ( rational.first < rational.second )
        exifStruct.expTime = QString::number(rational.first) + "/" +
                QString::number(rational.second) + " s";
    else
    {
        short integer = rational.first / rational.second;
        exifStruct.expTime =  QString::number(integer, 'f', 1);
        int fraction = rational.first - integer*rational.second;
        if (fraction != 0)
            exifStruct.expTime += " " + QString::number(fraction) +
                "/" + QString::number(rational.second);
        exifStruct.expTime.append(" s");
    }
    exifStruct.isoSpeed = exifData["Exif.Photo.ISOSpeedRatings"].toLong();
    if ( exifStruct.isoSpeed == -1 )
        exifStruct.isoSpeed = exifData["Exif.Image.ISOSpeedRatings"].toLong();
    rational = exifData["Exif.Photo.ExposureBiasValue"].toRational();
    exifStruct.expBias = (float)rational.first / rational.second;
    exifStruct.expProgram = exifData["Exif.Photo.ExposureProgram"].toLong();
    exifStruct.meteringMode = exifData["Exif.Photo.MeteringMode"].toLong();
    if ( exifStruct.meteringMode == -1 )
        exifStruct.meteringMode = exifData["Exif.Image.MeteringMode"].toLong();
    if ( exifStruct.meteringMode == 255 ) //when value equal 'other'
        exifStruct.meteringMode = 7;
    exifStruct.flashMode = exifData["Exif.Photo.Flash"].toLong();
    if ( exifStruct.flashMode == -1 )
        exifStruct.flashMode = exifData["Exif.Image.Flash"].toLong();

    // Camera section
    exifStruct.cameraManufacturer = QString::fromStdString(
                exifData["Exif.Image.Make"].toString() );
    exifStruct.cameraModel = QString::fromStdString(
                exifData["Exif.Image.Model"].toString() );

    // Author section
    exifStruct.artist = QString::fromStdString(
                exifData["Exif.Image.Artist"].toString() );
    if (exifStruct.artist.isEmpty())
        exifStruct.artist = MetadataUtils::String::noData();
    exifStruct.copyright = QString::fromStdString(
                exifData["Exif.Image.Copyright"].toString() );
    if (exifStruct.copyright.isEmpty())
        exifStruct.copyright = MetadataUtils::String::noData();
    exifStruct.userComment = QString::fromStdString(
                exifData["Exif.Photo.UserComment"].toString() );
    if (exifStruct.userComment.isEmpty())
        exifStruct.userComment = MetadataUtils::String::noData();
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
