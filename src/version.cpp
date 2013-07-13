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

#include <QStringList>
#include "version.h"

/** Creates the Version object. */
Version::Version() : versionString(VERSION), versionVector(vector(versionString)) {}

/** Returns value of versionString. */
QString Version::version() const {
    return versionString;
}

/** Returns true if \a ver string value equals or is greater than versionString
  * value.
  * \sa vector()
  */
bool Version::isAtLeast(const QString &ver) {
    QVector<int> vec = vector(ver);
    const int minSize = qMin(versionVector.size(), vec.size());
    const int maxSize = qMax(versionVector.size(), vec.size());
    int i;
    for (i=0; i<minSize; i++) {
        if (versionVector[i] < vec[i])
            return false;
        if (versionVector[i] > vec[i])
            return true;
    }
    const QVector<int> *vectorNoNull;
    if (versionVector.size() < vec.size())
        vectorNoNull = &vec;
    else
        vectorNoNull = &versionVector;
    for (; i<maxSize; i++) {
        if (vectorNoNull->at(i) < 0)
            return false;
    }
    return true;
}

/** Returns values of \a ver string. Development status value is negative.
  * \sa versionString
  */
QVector<int> Version::vector(const QString &ver) {
    QStringList versionList = ver.split('-');
    QStringList list = versionList.first().split('.');
    QVector<int> vec(list.length()+versionList.length()-1);
    for (int i=0; i<list.size(); i++)
        vec[i] = list[i].toInt();
    if (versionList.length() > 1) {
        QString status = versionList[1];
        if (status == "dev")
            vec.last() = -3;
        else if (status == "alpha")
            vec.last() = -2;
        else if (status == "beta")
            vec.last() = -1;
        else
            vec.last() = -4;
    }
    return vec;
}
