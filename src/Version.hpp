/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2016  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://marek629.github.io/SIR/
 */

#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QVector>

#define VERSION "3.0"


//! SIR version information class.
class Version
{
    friend class VersionTest;

public:
    Version(const char * v = VERSION);
    QString version() const;
    bool isAtLeast(const QString &ver);

private:
    /** Version string in format "M.m.p[-s]"
      *
      *     M - major version number
      *     m - minor version number
      *     p - patch version number
      *     s - development status; optional, after minus sign
      *
      * Supported development status strings are:
      * - dev
      * - alpha
      * - beta
      *
      * \sa version()
      */
    const QString versionString;
    /** Stores values of versionString.
      * \sa vector()
      */
    const QVector<int> versionVector;

    QVector<int> vector(const QString &ver);
};

#endif // VERSION_H
