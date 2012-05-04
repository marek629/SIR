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

#include "iptc.h"

using namespace MetadataUtils;

short Iptc::modelVersion = -1;

/** Returns date object based on \a str ISO 8601 string.
  * \sa time
  */
QDate Iptc::date(const std::string &str) {
    if (str.length() != 10)
        return QDate(1,1,1);
    return QDate::fromString(QString::fromStdString(str), Qt::ISODate);
}

/** Returns time object based on \a str ISO 8601 string.
  * \sa date
  */
QTime Iptc::time(const std::string &str) {
    if (str.length() != 14)
        return QTime();
    return QTime::fromString(QString::fromStdString(str), Qt::ISODate);
}

/** Returns  ISO 8601 string based on \a date object.
  * \sa timeString
  */
std::string Iptc::dateString(const QDate &date) {
    return date.toString(Qt::ISODate).toStdString();
}

/** Returns  ISO 8601 string based on \a time object.
  * \sa dateString
  */
std::string Iptc::timeString(const QTime &time) {
    return time.toString(Qt::ISODate).toStdString();
}

/** Sets default value of IptcStruct fields - \em "no data" strings and null
  * date and time objects.
  */
void IptcStruct::reset() {
    modelVersion = String::noData();
    dateCreated = QDate();
    timeCreated = QTime();
    digitizationDate = QDate();
    digitizationTime = QTime();
    byline = String::noData();
    copyright = String::noData();
    objectName = String::noData();
    keywords = String::noData();
    caption = String::noData();
    countryName = String::noData();
    city = String::noData();
    editStatus = String::noData();
}
