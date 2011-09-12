#ifndef METADATA_H
#define METADATA_H

#include <exiv2/exiv2.hpp>

class QString;

class MetadataUtils
{
public:
    MetadataUtils();
    void read(const QString& path);
    void write(const QString& path);
    short int exifOrientation();
    void setExifOrientation(short int);
    static void setEnabled(bool);
    static bool isEnabled();
    static void setSave(bool);
    static bool isSave();
    static void setOverwriteExifArtist(bool);
    static bool isOverwriteExifArtist();
    static void setStringExifArtist(const QString&);
    static QString stringExifArtist();
    static void setOverwriteExifCopyright(bool);
    static bool isOverwriteExifCopyright();
    static void setStringExifCopyright(const QString&);
    static QString stringExifCopyright();
    static void setOverwriteExifUserComment(bool);
    static bool isOverwriteExifUserComment();
    static void setStringExifUserComment(const QString&);
    static QString stringExifUserComment();

private:
    std::string nativeStdString(const QString& path) const;
    Exiv2::Image::AutoPtr image;
    Exiv2::ExifData exifData;
    Exiv2::IptcData iptcData;
    Exiv2::XmpData xmpData;
    static bool enabled;
    static bool save;
    static bool overwriteExifArtist;
    static QString _stringExifArtist;
    static bool overwriteExifCopyright;
    static QString _stringExifCopyright;
    static bool overwriteExifUserComment;
    static QString _stringExifUserComment;
};

#endif // METADATA_H
