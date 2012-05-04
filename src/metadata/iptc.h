/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2012  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://sir.projet-libre.org/
 */

#ifndef IPTC_H
#define IPTC_H

#include <QDate>
#include <QTime>
#include "string.h"

namespace MetadataUtils {
    class Iptc {
        static short modelVersion;
    public:
        Iptc();
        static void setVersion(short v) { modelVersion = v; }
        static short version() { return modelVersion; }
        static bool isVersionKnown() { return (modelVersion > 0); }
        static QDate date(const std::string &str);
        static QTime time(const std::string &str);
        static std::string dateString(const QDate &date);
        static std::string timeString(const QTime &time);
    };

    //! Stores \a IPTC metadata user readable.
    struct IptcStruct {
        String modelVersion;
        QDate dateCreated;
        QTime timeCreated;
        QDate digitizationDate;
        QTime digitizationTime;
        String byline;
        String copyright;
        String objectName;
        String keywords;
        String caption;
        String countryName;
        String city;
        String editStatus;

        // Methods
        void reset();
    };
}

#endif // IPTC_H
