/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2014  Marek Jędryka   <jedryka89@gmail.com>
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

#ifndef _SIR_STRING_H
#define _SIR_STRING_H

#ifndef SIR_CMAKE
#define SIR_METADATA_SUPPORT
#endif // SIR_CMAKE

#include <QCoreApplication>
#ifdef SIR_METADATA_SUPPORT
#include <exiv2/types.hpp>
#endif // SIR_METADATA_SUPPORT

namespace sir {
//! Class provides additional methods for QString.
class String : public QString {
    Q_DECLARE_TR_FUNCTIONS(MetadataUtils::String)

public:
    String();
    String(const char *str);
    String(const QString &other);
    void appendUnit(const char *unit);
    std::string toNativeStdString() const;
    QString toQString() const { return *this; }
#ifdef SIR_METADATA_SUPPORT
    Exiv2::Rational toRational() const;
    Exiv2::Rational toRationalPower() const;
#endif // SIR_METADATA_SUPPORT
    bool toBool() const;
    String & operator =(const char *str);
    String & operator =(const QString &other);
    String & operator =(const std::string &str);
    String & operator +=(const std::string &str);
    static QString noData();
    static String fromStdString(const std::string &s);
    static String fromDateTimeString(const String &dateTimeStr,
                                     const QString &formatIn, const QString &formatOut);
    static String currentDateTimeString(const QString &format);
};
}

#endif // _SIR_STRING_H
