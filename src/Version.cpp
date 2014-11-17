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
 * Program URL: http://marek629.github.io/sir/
 */

#include <QStringList>
#include "Version.hpp"

/** Creates the Version object. */
Version::Version(const char *v)
    : versionString(v), versionVector(vector(versionString)) {}

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
    QVector<int> versionVectorCopy(versionVector);
    const int maxSize = qMax(versionVectorCopy.size(), vec.size());

    vec.reserve(maxSize);
    versionVectorCopy.reserve(maxSize);

    int difSize = maxSize - versionVectorCopy.size();
    for (int i=0; i<difSize; i++)
        versionVectorCopy << 0;
    difSize = maxSize - vec.size();
    for (int i=0; i<difSize; i++)
        vec << 0;

    for (int i=0; i<maxSize; i++) {
        if (versionVectorCopy[i] > vec[i])
            return false;
        if (versionVectorCopy[i] < vec[i])
            return true;
    }

    return true;
}

/** Returns values of \a ver string. Development status value is negative.
  * \sa versionString
  */
QVector<int> Version::vector(const QString &ver) {
    QStringList versionList = ver.split('-');
    QStringList list = versionList.first().split('.');
    QVector<int> vec(list.length() + versionList.length() - 1);
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
