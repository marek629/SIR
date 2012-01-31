#ifndef EXIF_H
#define EXIF_H

#include "string.h"
#include <QPixmap>

namespace MetadataUtils
{
    class String;
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
        String version; /** < Contains information about version of Exiv library */
        String processingSoftware;  /** < Contains information about processing software */
        String imageWidth; /** < Contains image width with " px" suffix */
        String imageHeight;  /** < Contains image height with " px" suffix */
        char orientation; /** < Information about orientation of image*/
        String originalDate; /** < Creation date and time */
        String digitizedDate; /** < Digitalization date and time */
        // Thumbnail section
        QPixmap thumbnailPixmap; /** < Contains photo thumbnail */
        String thumbnailWidth; /** < Contains thumbnail width with " px" suffix */
        String thumbnailHeight;  /** < Contains thumbnail height with " px" suffix */
        // Photo section
        float focalLength; /** < Contains Focal Length */
        float aperture; /** < Contains Aperture */
        int isoSpeed; /** < Contains ISO Speed */
        QString shutterSpeed; /** < Contains Shutter Speed */
        QString expTime;  /** < Contains Exposure time*/
        float expBias;  /** < Contains Exposure bias */
        uchar expProgram; /** < Contains information about Exposure program */
        short meteringMode; /** < Contains information about Light metering mode */
        short flashMode; /** < Contains information about flash mode */
        // Camera section
        String cameraManufacturer;  /** < Contains information about Light metering mode */
        String cameraModel; /** < Contains information about Camera's model */
        // Author section
        String artist; /** < Contains information about autor */
        String copyright; /** < Contains copyright information */
        String userComment; /** < Contains user comment */

        // Methods
        void reset(); /** < Sets default values */
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
