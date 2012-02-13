#include "string.h"
#include <QObject>
#include <QStringList>
#include <cmath>

MetadataUtils::String & MetadataUtils::String::operator =(const char *str)
{
    QString::operator =(str);
    return *this;
}

MetadataUtils::String & MetadataUtils::String::operator =(const QString& other)
{
    QString::operator =(other);
    return *this;
}

void MetadataUtils::String::appendUnit(const char *unit)
{
    if ( (this->toDouble() == -1.) || this->isEmpty() )
        *this = noData();
    else
        this->append(unit);
}

std::string MetadataUtils::String::toNativeStdString() const
{
#ifdef Q_OS_WIN32
    return toLocal8Bit().constData();
#else
    return toUtf8().constData();
#endif
}

Exiv2::Rational MetadataUtils::String::toRational() const
{
    Exiv2::Rational result;
    if (this == noData()) {
        result.first = -1;
        result.second = 1;
        return result;
    }

    QStringList list = this->split(' ');
    QString last = list.last();
    if (last == "s")
        list.removeLast();
    else if (last.right(1) == "s")
        last.remove(last.lastIndexOf("s"),1);
    last = list.last();

    QStringList fractionList = last.split('/');
    result.first = fractionList.first().toInt();
    result.second = fractionList.last().toInt();
    if (list.count() == 2)
        result.first += list.first().toInt() * result.second;

    return result;
}

Exiv2::Rational MetadataUtils::String::toRationalPower() const {
    Exiv2::Rational rational = toRational();
    Exiv2::Rational result;
    const quint16 multiplier = 0xffff;
    result.first = multiplier * log((double)rational.first / rational.second);
    result.second = multiplier * log(0.5);
    return result;
}

MetadataUtils::String MetadataUtils::String::fromStdString(const std::string &s)
{
    return MetadataUtils::String( QString::fromStdString(s) );
}

QString MetadataUtils::String::noData()
{
    return QObject::tr("no data");
}
