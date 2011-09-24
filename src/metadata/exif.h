#ifndef EXIF_H
#define EXIF_H

#include <QString>
#include <QPixmap>

namespace MetadataUtils
{
    class Exif
    {
    public:
        static void setVersionKnown(bool);
        static bool isVersionKnown();
        static void setArtistOverwrite(bool);
        static bool isArtistOverwrite();
        static void setArtistString(const QString&);
        static QString stringArtist();
        static void setCopyrightOverwrite(bool);
        static bool isCopyrightOverwrite();
        static void setCopyrightString(const QString&);
        static QString stringCopyright();
        static void setUserCommentOverwrite(bool);
        static bool isUserCommentOverwrite();
        static void setUserCommentString(const QString&);
        static QString stringUserComment();

    private:
        static bool versionKnown;
        static bool artistOverwrite;
        static QString artistString;
        static bool copyrightOverwrite;
        static QString copyrightString;
        static bool userCommentOverwrite;
        static QString userCommentString;
    };

    struct ExifStruct
    {
        QString version;
        QString processingSoftware;
        QString imageWidth;
        QString imageHeight;
        char orientation;
        QString originalDate;
        QString digitizedDate;

        QPixmap thumbnailPixmap;
        QString thumbnailWidth;
        QString thumbnailHeight;

        float focalLength;
        float aperture;
        QString shutterSpeed;
        int isoSpeed;
        QString expTime;
        float expBias;
        uchar expProgram;
        uchar meteringMode;

        QString cameraManufacturer;
        QString cameraModel;
    };
}

#endif // EXIF_H
