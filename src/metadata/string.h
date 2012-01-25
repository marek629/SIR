#ifndef STRING_H
#define STRING_H

#include <QString>
#include <exiv2/types.hpp>

namespace Exiv2
{
    class Metadatum;
}

namespace MetadataUtils
{
    class String : public QString
    {
    public:
        String() : QString() {}
        String(const char *str) : QString(str) {}
        String(const QString &other) : QString(other) {}
        void appendUnit(const char *unit);
        std::string toNativeStdString() const;
        QString toQString() const { return *this; }
        Exiv2::Rational toRational() const;
        Exiv2::Rational toRationalPower() const;
        String & operator =(const char *str);
        String & operator =(const QString &other);
        static const QString &noData();
        static String exifVersion(const Exiv2::Metadatum &datum);
        static String fromStdString(const std::string &s);

    private:
        static QString noData_;
    };
}

#endif // STRING_H
