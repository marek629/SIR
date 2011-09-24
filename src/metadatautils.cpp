#include "metadatautils.h"
#include "defines.h"
#include <QString>
#include <QObject>

#include <QDebug>

bool MetadataUtils::Metadata::enabled = false;
bool MetadataUtils::Metadata::save = false;
QString MetadataUtils::Metadata::noData_ = QObject::tr("no data");

MetadataUtils::Metadata::Metadata()
{
}

void MetadataUtils::Metadata::read(const QString& path) {
    std::string filePath = nativeStdString(path);
    image = Exiv2::ImageFactory::open(filePath);
    image->readMetadata();

    exifData = image->exifData();
    MetadataUtils::Exif::setVersionKnown(
                exifData["Exif.Photo.ExifVersion"].toString().empty() );
    setExifStruct();

    iptcData = image->iptcData();
    xmpData = image->xmpData();
    image.reset();
}

void MetadataUtils::Metadata::write(const QString& path, const QImage& qImage) {
    std::string filePath = nativeStdString(path);
    image = Exiv2::ImageFactory::open(filePath);

    if (MetadataUtils::Exif::isArtistOverwrite())
        exifData["Exif.Image.Artist"] =
                MetadataUtils::Exif::stringArtist().toStdString();
    if (MetadataUtils::Exif::isCopyrightOverwrite())
        exifData["Exif.Image.Copyright"] =
                MetadataUtils::Exif::stringCopyright().toStdString();
    if (MetadataUtils::Exif::isUserCommentOverwrite())
    {
        std::string userComment = "charset=\"Unicode\" ";
        userComment += nativeStdString(MetadataUtils::Exif::stringUserComment());
        exifData["Exif.Photo.UserComment"] = userComment;
    }

    if (!exifData.empty())
    {
        if (!MetadataUtils::Exif::isVersionKnown() )
            exifData["Exif.Photo.ExifVersion"] = EXIF_STD_VERSION;
        exifData["Exif.Image.ProcessingSoftware"] = "Simple Image Resizer v"
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
}

void MetadataUtils::Metadata::setExifStruct()
{
    exifStruct.version = stringFloat2( exifData["Exif.Photo.ExifVersion"] );
    exifStruct.processingSoftware = QString::fromStdString(
                exifData["Exif.Image.ProcessingSoftware"].toString() );
    if (exifStruct.processingSoftware.isEmpty())
        exifStruct.processingSoftware = noData_;
    exifStruct.imageWidth = QString::number(
                exifData["Exif.Image.ImageWidth"].toLong() );
    appendString( &exifStruct.imageWidth, " px");
    exifStruct.imageHeight = QString::number(
                exifData["Exif.Image.ImageLength"].toLong() );
    appendString( &exifStruct.imageHeight, " px");
    exifStruct.orientation = exifData["Exif.Image.Orientation"].toLong();
    exifStruct.originalDate = QString::fromStdString(
                exifData["Exif.Photo.DateTimeOriginal"].toString() );
    if (exifStruct.originalDate.isEmpty())
        exifStruct.originalDate = QString::fromStdString(
                    exifData["Exif.Image.DateTimeOriginal"].toString() );
    exifStruct.digitizedDate = QString::fromStdString(
                exifData["Exif.Photo.DateTimeDigitized"].toString() );

    Exiv2::PreviewManager previewMenager(*image);
    Exiv2::PreviewImage thumbnail = previewMenager.getPreviewImage(
                previewMenager.getPreviewProperties()[0] );
    exifStruct.thumbnailPixmap = QPixmap(thumbnail.width(), thumbnail.height());
    exifStruct.thumbnailPixmap.loadFromData(thumbnail.pData(), thumbnail.size());
    exifStruct.thumbnailWidth = QString::number( thumbnail.width() );
    appendString( &exifStruct.thumbnailWidth, " px");
    exifStruct.thumbnailHeight = QString::number( thumbnail.height() );
    appendString( &exifStruct.thumbnailHeight, " px");

    Exiv2::Rational rational = exifData["Exif.Photo.FocalLength"].toRational();
    exifStruct.focalLength = (float)rational.first / rational.second;
    rational = exifData["Exif.Photo.FNumber"].toRational();
    if ( rational.first == -1 )
        rational = exifData["Exif.Photo.ApertureValue"].toRational();
    exifStruct.aperture = (float)rational.first / rational.second;
    rational = exifData["Exif.Photo.ExposureTime"].toRational();
    if ( rational.first == -1 )
        exifStruct.expTime = noData_;
    else if ( rational.first < rational.second )
        exifStruct.expTime = QString::number(rational.first) + "/" +
                QString::number(rational.second) + " s";
    else
    {
        short integer = rational.first / rational.second;
        exifStruct.expTime =  QString::number(integer, 'f', 1) + " " +
                QString::number(rational.first - integer*rational.second) +
                "/" + QString::number(rational.second) + " s";
    }
    rational = exifData["Exif.Photo.ShutterSpeedValue"].toRational();
    if ( rational.first == -1 )
        exifStruct.expTime = noData_;
    else if ( rational.first < rational.second )
        exifStruct.shutterSpeed = QString::number(rational.first) + "/" +
                QString::number(rational.second) + " s";
    else
    {
        short integer = rational.first / rational.second;
        exifStruct.shutterSpeed =  QString::number(integer, 'f', 1) + " " +
                QString::number(rational.first - integer*rational.second) +
                "/" + QString::number(rational.second) + " s";
    }
    exifStruct.isoSpeed = exifData["Exif.Photo.ISOSpeedRatings"].toLong();
    if ( exifStruct.isoSpeed == -1 )
        exifStruct.isoSpeed = exifData["Exif.Image.ISOSpeedRatings"].toLong();
    rational = exifData["Exif.Photo.ExposureBiasValue"].toRational();
    exifStruct.expBias = (float)rational.first / rational.second;
    exifStruct.expProgram = exifData["Exif.Photo.ExposureProgram"].toLong();

    exifStruct.cameraManufacturer = QString::fromStdString(
                exifData["Exif.Image.Make"].toString() );
    exifStruct.cameraModel = QString::fromStdString(
                exifData["Exif.Image.Model"].toString() );
}

std::string MetadataUtils::Metadata::nativeStdString(const QString& str) const {
#ifdef Q_OS_WIN32
    return str.toLocal8Bit().constData();
#else
    return str.toUtf8().constData();
#endif
}

QString MetadataUtils::Metadata::stringFloat2(const Exiv2::Metadatum &datum)
{
    int size = datum.size();
    char ch[size];
    for (int i=0; i<size; i++)
        ch[i] = datum.toLong(i);
    QString string = QString(QByteArray(ch,size));
    string.insert(size-2,'.');
    if (string[size-1] == '0')
        string.remove(size-1,1);
    if (string[0] == '0')
        string.remove(0,1);
    return string;
}

void MetadataUtils::Metadata::appendString(QString *str, const QString &add)
{
    if ( (*(str) == "-1") || str->isEmpty() || (*(str) == "-1.0") )
        *(str) = noData_;
    else
        str->append(add);
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

const QString &MetadataUtils::Metadata::noData() {
    return noData_;
}
