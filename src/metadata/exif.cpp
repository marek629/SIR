#include "exif.h"

bool MetadataUtils::Exif::versionKnown = false;
bool MetadataUtils::Exif::artistOverwrite = false;
MetadataUtils::String MetadataUtils::Exif::artistString = "";
bool MetadataUtils::Exif::copyrightOverwrite = false;
MetadataUtils::String MetadataUtils::Exif::copyrightString = "";
bool MetadataUtils::Exif::userCommentOverwrite = false;
MetadataUtils::String MetadataUtils::Exif::userCommentString = "";

// setup flash struct
QString MetadataUtils::FlashStruct::noFlashFunction = QObject::tr("No flash function.");
QString MetadataUtils::FlashStruct::fired = QObject::tr("Flash fired");
QString MetadataUtils::FlashStruct::noFired = QObject::tr("Flash didn't fire");
QString MetadataUtils::FlashStruct::strobeReturnDetected =
        QObject::tr(", strobe return light detected");
QString MetadataUtils::FlashStruct::strobeReturnNotDetected =
        QObject::tr(", strobe return light not detected");
QString MetadataUtils::FlashStruct::autoMode = QObject::tr(", auto mode");
QString MetadataUtils::FlashStruct::compulsoryMode = QObject::tr(", compulsory flash mode");
QString MetadataUtils::FlashStruct::redEyeReduction = QObject::tr(", red-eye reduction");


QString MetadataUtils::Exif::flashString(short num)
{
    if (num == -1)
        return QObject::tr("Unknown");
    QString result;
    QString bin = QString::number(num,2);
    // invert bin
    for (short i=0, j=bin.length()-1; i<bin.length(); i++, j--)
    {
        QChar temp = bin[j];
        bin[j] = bin[i];
        bin[i] = temp;
    }
    short toAppend = 7 - bin.length();
    for (short i=0; i<toAppend; i++)
        bin.append('0');

    // bit 5 indicating the presence of a flash function
    if ( bin[5] == '1' )
        return MetadataUtils::FlashStruct::noFlashFunction;

    // bit 0 indicating whether the flash fired
    if ( bin[0] == '1' )
        result.append(MetadataUtils::FlashStruct::fired);
    else
        result.append(MetadataUtils::FlashStruct::noFired);

    // bits 1 and 2 indicating the status of returned light
    if ( bin[1] == '1' )
    {
        if ( bin[2] == '1' )
            result.append(MetadataUtils::FlashStruct::strobeReturnDetected);
        else
            result.append(MetadataUtils::FlashStruct::strobeReturnNotDetected);
    }

    // bits 3 and 4 indicating the camera's flash mode
    if ( bin[3]=='1' && bin[4]=='1' )
            result.append(MetadataUtils::FlashStruct::autoMode);
    else if ( (bin[3]=='0' && bin[4]=='1') || (bin[3]=='1' && bin[4]=='0') )
        result.append(MetadataUtils::FlashStruct::compulsoryMode);

    // bit 6 indicating the camera's red-eye mode
    if ( bin[6] == '1' )
        result.append(MetadataUtils::FlashStruct::redEyeReduction);

    result.append('.');
    return result;
}

short MetadataUtils::Exif::flashShort(const QString &str)
{
    // bit 5 indicating the presence of a flash function
    if ( str == MetadataUtils::FlashStruct::noFlashFunction )
        return 32; // 2^5

    short result = 0;

    // bit 0 indicating whether the flash fired
    if (str.contains(MetadataUtils::FlashStruct::fired))
        result += 1; // 2^0

    // bits 1 and 2 indicating the status of returned light
    if (str.contains(MetadataUtils::FlashStruct::strobeReturnDetected))
        result += 6; // 2^1 + 2^2
    else if (str.contains(MetadataUtils::FlashStruct::strobeReturnNotDetected))
        result += 2; // 2^1

    // bits 3 and 4 indicating the camera's flash mode
    if (str.contains(MetadataUtils::FlashStruct::autoMode))
        result += 24; // 2^3 + 2^4
    else if (str.contains(MetadataUtils::FlashStruct::compulsoryMode))
        result += 16; // 2^4

    // bit 6 indicating the camera's red-eye mode
    if (str.contains(MetadataUtils::FlashStruct::redEyeReduction))
        result += 64; // 2^6

    return result;
}

bool MetadataUtils::Exif::isVersionKnown()
{
    return versionKnown;
}

void MetadataUtils::Exif::setVersionKnown(bool v)
{
    versionKnown = v;
}

bool MetadataUtils::Exif::isArtistOverwrite()
{
    return artistOverwrite;
}

MetadataUtils::String MetadataUtils::Exif::stringArtist()
{
    return artistString;
}

void MetadataUtils::Exif::setArtistOverwrite(bool v)
{
    artistOverwrite = v;
}

void MetadataUtils::Exif::setArtistString(const MetadataUtils::String &v)
{
    artistString = v;
}

bool MetadataUtils::Exif::isCopyrightOverwrite()
{
    return copyrightOverwrite;
}

MetadataUtils::String MetadataUtils::Exif::stringCopyright()
{
    return copyrightString;
}

void MetadataUtils::Exif::setCopyrightOverwrite(bool v)
{
    copyrightOverwrite = v;
}

void MetadataUtils::Exif::setCopyrightString(const MetadataUtils::String &v)
{
    copyrightString = v;
}

bool MetadataUtils::Exif::isUserCommentOverwrite()
{
    return userCommentOverwrite;
}

MetadataUtils::String MetadataUtils::Exif::stringUserComment()
{
    return userCommentString;
}

void MetadataUtils::Exif::setUserCommentOverwrite(bool v)
{
    userCommentOverwrite = v;
}

void MetadataUtils::Exif::setUserCommentString(const MetadataUtils::String &v)
{
    userCommentString = v;
}

void MetadataUtils::ExifStruct::reset() {
    // Image section
    version = MetadataUtils::String::noData();
    processingSoftware = MetadataUtils::String::noData();
    imageWidth = MetadataUtils::String::noData();
    imageHeight = MetadataUtils::String::noData();
    orientation = 1;
    originalDate = MetadataUtils::String::noData();
    digitizedDate = MetadataUtils::String::noData();
    // Thumbnail section
    thumbnailPixmap = QPixmap();
    thumbnailWidth = MetadataUtils::String::noData();
    thumbnailHeight = MetadataUtils::String::noData();
    // Photo section
    focalLength = 0.0;
    aperture = 0.0;
    isoSpeed = 0;
    expTime = MetadataUtils::String::noData();
    shutterSpeed= MetadataUtils::String::noData();
    expBias = 0.0;
    expProgram = 0;
    meteringMode = 0;
    flashMode = -1;
    // Camera section
    cameraManufacturer = MetadataUtils::String::noData();
    cameraModel = MetadataUtils::String::noData();
    // Author section
    artist = MetadataUtils::String::noData();
    copyright = MetadataUtils::String::noData();
    userComment = MetadataUtils::String::noData();
}
