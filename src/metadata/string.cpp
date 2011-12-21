#include "string.h"
#include <exiv2/metadatum.hpp>
#include <QObject>
#include <QStringList>

QString MetadataUtils::String::noData_ = QObject::tr("no data");

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
        *this = noData_;
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
    if (this == noData_) {
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

MetadataUtils::String MetadataUtils::String::fromMetadatum(const Exiv2::Metadatum &datum)
{
    int size = datum.size();
    if (size == 0)
        return "";
    char ch[size];
    for (int i=0; i<size; i++)
        ch[i] = datum.toLong(i);
    QString str = QString(QByteArray(ch,size));
    str.insert(size-2,'.');
    if (str[size-1] == '0')
        str.remove(size-1,1);
    if (str[0] == '0')
        str.remove(0,1);
    return MetadataUtils::String(str);
}

MetadataUtils::String MetadataUtils::String::fromStdString(const std::string &s)
{
    return MetadataUtils::String( QString::fromStdString(s) );
}

const QString &MetadataUtils::String::noData()
{
    return noData_;
}
