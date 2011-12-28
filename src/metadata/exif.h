#ifndef EXIF_H
#define EXIF_H

#include "string.h"
#include <QPixmap>

namespace MetadataUtils
{
    enum Flip {
        None,
        Vertical,
        Horizontal,
        VerticalAndHorizontal
    };

    class Exif
    {
    public:
        static QString flashString(short);
        static short flashShort(const QString&);
        static void setVersionKnown(bool);
        static bool isVersionKnown();
        static void setArtistOverwrite(bool);
        static bool isArtistOverwrite();
        static void setArtistString(const String&);
        static String stringArtist();
        static void setCopyrightOverwrite(bool);
        static bool isCopyrightOverwrite();
        static void setCopyrightString(const String&);
        static String stringCopyright();
        static void setUserCommentOverwrite(bool);
        static bool isUserCommentOverwrite();
        static void setUserCommentString(const String&);
        static String stringUserComment();
        static char getOrientation(short rotation, int flipValue = None);
        static short rotationAngle(char orientation);
        static short rotationAngle(char orientation, int *flip);
        static Flip flipValue(char orientation);

    private:
        static bool versionKnown;
        static bool artistOverwrite;
        static String artistString;
        static bool copyrightOverwrite;
        static String copyrightString;
        static bool userCommentOverwrite;
        static String userCommentString;
    };

    struct ExifStruct
    {   // Image section
        String version;
        String processingSoftware;
        String imageWidth;
        String imageHeight;
        char orientation;
        String originalDate;
        String digitizedDate;
        // Thumbnail section
        QPixmap thumbnailPixmap;
        String thumbnailWidth;
        String thumbnailHeight;
        // Photo section
        float focalLength;
        float aperture;
        int isoSpeed;
        QString expTime;
        float expBias;
        uchar expProgram;
        short meteringMode;
        short flashMode;
        // Camera section
        String cameraManufacturer;
        String cameraModel;
        // Author section
        String artist;
        String copyright;
        String userComment;

        // Methods
        void reset();
    };

    struct FlashStruct
    {
        static QString noFlashFunction;
        static QString fired;
        static QString noFired;
        static QString compulsoryMode;
        static QString autoMode;
        static QString redEyeReduction;
        static QString strobeReturnDetected;
        static QString strobeReturnNotDetected;
    };
}

#endif // EXIF_H
