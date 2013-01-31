/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Program URL: http://sir.projet-libre.org/
 */

#include "string.h"
#include <QStringList>
#include <cmath>
#include <QDateTime>

using namespace MetadataUtils;

/** Default constructor.\n
  * Constructs empty string.
  */
String::String() : QString() {}

/** Constructs string from \a str. */
String::String(const char *str) : QString(str) {}

/** Constructs string from \a other. */
String::String(const QString &other) : QString(other) {}

/** Assigns \a str to this string and returns a reference to this string. */
String & String::operator =(const char *str) {
    QString::operator =(str);
    return *this;
}

/** Assigns \a other to this string and returns a reference to this string. */
String & String::operator =(const QString& other) {
    QString::operator =(other);
    return *this;
}

/** Assigns \a str to this string and returns a reference to this string. */
String & String::operator =(const std::string &str) {
    String::operator =(QString::fromStdString(str));
    return *this;
}

/** Adds \a str to this string and returns a reference to this string. */
String & String::operator +=(const std::string &str) {
    this->append(QString::fromStdString(str));
    return *this;
}

/** Appends \a unit to this string if the string is a number;
  * otherwise the string will be assigns noData() string.\n
  * If \a unit is null pointer or empty string this function has no effect.
  * \sa noData
  */
void String::appendUnit(const char *unit) {
    if (!unit || unit[0] == '\0')
        return;
    if ( (this->toDouble() == -1.) || this->isEmpty() )
        *this = noData();
    else
        this->append(unit);
}

/** Returns copy of this string converted to local 8 bit code on Windows,
  * otherwise to will be converted to UTF-8.
  */
std::string String::toNativeStdString() const {
#ifdef Q_OS_WIN32
    return toLocal8Bit().constData();
#else
    return toUtf8().constData();
#endif
}

#ifdef SIR_METADATA_SUPPORT
/** Converts this string to improper fraction and returns this fraction. */
Exiv2::Rational String::toRational() const {
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
    if (last.contains('/')) {
        QStringList fractionList = last.split('/');
        result.first = fractionList.first().toInt();
        result.second = fractionList.last().toInt();
        if (list.count() == 2)
            result.first += list.first().toInt() * result.second;
    }
    else {
        result.first = last.toInt();
        result.second = 1;
    }
    return result;
}

/** Converts this string to improper fraction of exponent and returns this fraction. */
Exiv2::Rational String::toRationalPower() const {
    Exiv2::Rational rational = toRational();
    Exiv2::Rational result;
    const quint16 multiplier = 10000;
    result.first = multiplier * log((double)rational.first / rational.second);
    result.second = multiplier * log(0.5);
    return result;
}
#endif // SIR_METADATA_SUPPORT

/** Converts this string to boolean value.
  * \return True if the string is \e yes, \e y, \e true, \e t or integer value
  *         of the string isn't 0. Otherwise false.
  */
bool String::toBool() const {
    static QStringList list;
    if (list.isEmpty())
        list << "yes" << "y" << "true" << "t";
    if (list.contains(*this, Qt::CaseInsensitive) || this->toInt() != 0)
        return true;
    return false;
}

/** Returns string converted from \a s. */
String String::fromStdString(const std::string &s) {
    return String( QString::fromStdString(s) );
}

/** Returns date and time string in \a formatOut format converted from
  * \a dateTimeStr date and time string in \a formatIn format.
  * \sa currentDateTimeString()
  */
String String::fromDateTimeString(const String &dateTimeStr,
                                  const QString &formatIn, const QString &formatOut) {
    return QDateTime::fromString(dateTimeStr, formatIn).toString(formatOut);
}

/** Returns date and time string in \a format format.
  * \sa fromDateTimeString()
  */
String String::currentDateTimeString(const QString &format) {
    return QDateTime::currentDateTime().toString(format);
}

/** Returns translated \em "no data" string. */
QString String::noData() {
    return tr("no data");
}
