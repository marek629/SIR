#include "exif.h"

bool MetadataUtils::Exif::versionKnown = false;
bool MetadataUtils::Exif::artistOverwrite = false;
MetadataUtils::String MetadataUtils::Exif::artistString = "";
bool MetadataUtils::Exif::copyrightOverwrite = false;
MetadataUtils::String MetadataUtils::Exif::copyrightString = "";
bool MetadataUtils::Exif::userCommentOverwrite = false;
MetadataUtils::String MetadataUtils::Exif::userCommentString = "";

QString MetadataUtils::Exif::flashString(short num)
{
    if (num == -1)
        return QObject::tr("Unknown");
    QString result;
    QString bin = QString::number(num,2);
    // invert bin
    for (short i=0, j=bin.length()-1; i<bin.length(); i++, j--) {
        QChar temp = bin[j];
        bin[j] = bin[i];
        bin[i] = temp;
    }
    short toAppend = 7 - bin.length();
    for (short i=0; i<toAppend; i++)
        bin.append('0');

    // bit 5 indicating the presence of a flash function
    if ( bin[5] == '1' )
        return MetadataUtils::Flash::noFlashFunction();

    // bit 0 indicating whether the flash fired
    if ( bin[0] == '1' )
        result.append(MetadataUtils::Flash::fired());
    else
        result.append(MetadataUtils::Flash::noFired());

    // bits 3 and 4 indicating the camera's flash mode
    if ( bin[3]=='1' && bin[4]=='1' )
        result.append(MetadataUtils::Flash::autoMode());
    else if ( (bin[3]=='0' && bin[4]=='1') || (bin[3]=='1' && bin[4]=='0') )
        result.append(MetadataUtils::Flash::compulsoryMode());

    // bits 1 and 2 indicating the status of returned light
    if ( bin[1] == '1' ) {
        if ( bin[2] == '1' )
            result.append(MetadataUtils::Flash::strobeReturnDetected());
        else
            result.append(MetadataUtils::Flash::strobeReturnNotDetected());
    }

    // bit 6 indicating the camera's red-eye mode
    if ( bin[6] == '1' )
        result.append(MetadataUtils::Flash::redEyeReduction());

    result.append('.');
    return result;
}

short MetadataUtils::Exif::flashShort(const QString &str)
{
    // bit 5 indicating the presence of a flash function
    if ( str == MetadataUtils::Flash::noFlashFunction() )
        return 32; // 2^5

    short result = 0;

    // bit 0 indicating whether the flash fired
    if (str.contains(MetadataUtils::Flash::fired()))
        result += 1; // 2^0

    // bits 1 and 2 indicating the status of returned light
    if (str.contains(MetadataUtils::Flash::strobeReturnDetected()))
        result += 6; // 2^1 + 2^2
    else if (str.contains(MetadataUtils::Flash::strobeReturnNotDetected()))
        result += 2; // 2^1

    // bits 3 and 4 indicating the camera's flash mode
    if (str.contains(MetadataUtils::Flash::autoMode()))
        result += 24; // 2^3 + 2^4
    else if (str.contains(MetadataUtils::Flash::compulsoryMode()))
        result += 16; // 2^4

    // bit 6 indicating the camera's red-eye mode
    if (str.contains(MetadataUtils::Flash::redEyeReduction()))
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

char MetadataUtils::Exif::getOrientation(short rotation, int flip) {
    using namespace MetadataUtils;
    switch (rotation) {
    case 0:
        if (flip == None)
            return 1;
        else if (flip == Horizontal)
            return 2;
        else if (flip == Vertical)
            return 4;
        break;
    case 180:
        if (flip == None)
            return 3;
        break;
    case 90:
        if (flip == None)
            return 6;
        else if (flip == Horizontal)
            return 5;
        else if (flip == Vertical)
            return 7;
        break;
    case -90:
        if (flip == None)
            return 8;
        else if (flip == Horizontal)
            return 7;
        else if (flip == Vertical)
            return 5;
        break;
    default:
        return -1;
        break;
    }
    return -2;
}

short MetadataUtils::Exif::rotationAngle(char orientation) {
    short result = 0;
    switch (orientation) {
    case 1:
        break;
    case 2:
        break;
    case 3:
        result = 180;
        break;
    case 4:
        break;
    case 5:
        result = 90;
        break;
    case 6:
        result = 90;
        break;
    case 7:
        result = -90;
        break;
    case 8:
        result = -90;
        break;
    default: // invalid orientation value
        result = -360;
        break;
    }
    return result;
}

short MetadataUtils::Exif::rotationAngle(char orientation, int *flip) {
    using namespace MetadataUtils;
    *flip = None;
    short result = 0;
    switch (orientation) {
    case 1:
        break;
    case 2:
        *flip = Horizontal;
        break;
    case 3:
        result = 180;
        break;
    case 4:
        *flip = Vertical;
        break;
    case 5:
        result = 90;
        *flip = Horizontal;
        break;
    case 6:
        result = 90;
        break;
    case 7:
        result = -90;
        *flip = Horizontal;
        break;
    case 8:
        result = -90;
        break;
    default: // invalid orientation value
        result = -360;
        *flip = VerticalAndHorizontal;
        break;
    }
    return result;
}

MetadataUtils::Flip MetadataUtils::Exif::flipValue(char orientation) {
    using namespace MetadataUtils;
    Flip result = None;
    switch (orientation) {
    case 1:
        break;
    case 2:
        result = Horizontal;
        break;
    case 3:
        break;
    case 4:
        result = Vertical;
        break;
    case 5:
        result = Horizontal;
        break;
    case 6:
        break;
    case 7:
        result = Horizontal;
        break;
    case 8:
        break;
    default: // invalid orientation value
        result = VerticalAndHorizontal;
        break;
    }
    return result;
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
    thumbnailImage = QImage();
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
