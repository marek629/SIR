/*
 * This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto
 * 2011-2012  Marek Jędryka
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
 *                 Marek Jędryka   <jedryka89@gmail.com>
 * Program URL: http://sir.projet-libre.org/
 *
 */

#ifndef STRING_H
#define STRING_H

#include <QString>
#include <exiv2/types.hpp>

namespace MetadataUtils
{
    //! Class provides additional methods for QString.
    class String : public QString {
    public:
        String();
        String(const char *str);
        String(const QString &other);
        void appendUnit(const char *unit);
        std::string toNativeStdString() const;
        QString toQString() const { return *this; }
        Exiv2::Rational toRational() const;
        Exiv2::Rational toRationalPower() const;
        String & operator =(const char *str);
        String & operator =(const QString &other);
        String & operator =(const std::string &str);
        String & operator +=(const std::string &str);
        static QString noData();
        static String fromStdString(const std::string &s);
    };
}

#endif // STRING_H
