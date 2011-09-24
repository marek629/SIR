#include "exif.h"

bool MetadataUtils::Exif::versionKnown = false;
bool MetadataUtils::Exif::artistOverwrite = false;
QString MetadataUtils::Exif::artistString = "";
bool MetadataUtils::Exif::copyrightOverwrite = false;
QString MetadataUtils::Exif::copyrightString = "";
bool MetadataUtils::Exif::userCommentOverwrite = false;
QString MetadataUtils::Exif::userCommentString = "";

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

QString MetadataUtils::Exif::stringArtist()
{
    return artistString;
}

void MetadataUtils::Exif::setArtistOverwrite(bool v)
{
    artistOverwrite = v;
}

void MetadataUtils::Exif::setArtistString(const QString &v)
{
    artistString = v;
}

bool MetadataUtils::Exif::isCopyrightOverwrite()
{
    return copyrightOverwrite;
}

QString MetadataUtils::Exif::stringCopyright()
{
    return copyrightString;
}

void MetadataUtils::Exif::setCopyrightOverwrite(bool v)
{
    copyrightOverwrite = v;
}

void MetadataUtils::Exif::setCopyrightString(const QString &v)
{
    copyrightString = v;
}

bool MetadataUtils::Exif::isUserCommentOverwrite()
{
    return userCommentOverwrite;
}

QString MetadataUtils::Exif::stringUserComment()
{
    return userCommentString;
}

void MetadataUtils::Exif::setUserCommentOverwrite(bool v)
{
    userCommentOverwrite = v;
}

void MetadataUtils::Exif::setUserCommentString(const QString &v)
{
    userCommentString = v;
}
