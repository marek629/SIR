#ifndef EXIF_H
#define EXIF_H

#include "string.h"
#include <QImage>

namespace MetadataUtils
{
    enum Flip {
        None = 0x0,
        Vertical = 0x1,
        Horizontal = 0x2,
        VerticalAndHorizontal = 0x3
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

    //! Stores \a Exif metadata user readable.
    struct ExifStruct
    {   // Image section
        String version; /**< Contains information about version of Exiv library. */
        String processingSoftware;  /**< Contains information about processing software. */
        String imageWidth; /**< Contains image width with " px" suffix. */
        String imageHeight;  /**< Contains image height with " px" suffix. */
        char orientation; /**< Information about orientation of image. */
        String originalDate; /**< Creation date and time. */
        String digitizedDate; /**< Digitalization date and time. */
        // Thumbnail section
        QImage thumbnailImage; /**< Contains photo thumbnail. */
        String thumbnailWidth; /**< Contains thumbnail width with " px" suffix. */
        String thumbnailHeight;  /**< Contains thumbnail height with " px" suffix. */
        // Photo section
        float focalLength; /**< Contains Focal Length value in milimeters. */
        float aperture; /**< Contains Aperture value. */
        int isoSpeed; /**< Contains ISO Speed value. */
        QString shutterSpeed; /**< Contains Shutter Speed. */
        QString expTime;  /**< Contains Exposure time. */
        float expBias;  /**< Contains Exposure bias value in EV. */
        uchar expProgram; /**< Contains information about Exposure program. */
        short meteringMode; /**< Contains information about Light metering mode. */
        short flashMode; /**< Contains information about flash mode. */
        // Camera section
        String cameraManufacturer;  /**< Contains information about Light metering mode. */
        String cameraModel; /**< Contains information about Camera's model. */
        // Author section
        String artist; /**< Contains information about autor. */
        String copyright; /**< Contains copyright information. */
        String userComment; /**< Contains user comment. */

        // Methods
        void reset(); /**< Sets default values. */
    };

    class Flash {
    public:
        static QString noFlashFunction() { return QObject::tr("No flash function."); }
        static QString fired() { return QObject::tr("Flash fired"); }
        static QString noFired() { return QObject::tr("Flash didn't fire"); }
        static QString compulsoryMode() { return QObject::tr(", compulsory flash mode"); }
        static QString autoMode() { return QObject::tr(", auto mode"); }
        static QString redEyeReduction() { return QObject::tr(", red-eye reduction"); }
        static QString strobeReturnDetected() {
            return QObject::tr(", strobe return light detected"); }
        static QString strobeReturnNotDetected() {
            return QObject::tr(", strobe return light not detected"); }
    };
}

#endif // EXIF_H
