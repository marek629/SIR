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

#ifdef SIR_METADATA_SUPPORT

#include "metadata/metadatautils.h"

using namespace sir;

/** This is overloaded function.\n
  * Returns true if \a v value equals null character, otherwise returns false.
  */
bool MetadataUtils::isNullValue(char v) {
    return (v == '\0');
}

/** This is overloaded function.\n
  * Returns true if \a v value equals 0, otherwise returns false.
  */
bool MetadataUtils::isNullValue(int v) {
    return (v == 0);
}

/** This is overloaded function.\n
  * Returns true if \a v value equals 0.0f, otherwise returns false.
  */
bool MetadataUtils::isNullValue(float v) {
    return (v == 0.f);
}

/** This is overloaded function.\n
  * Returns true if \a v string is null string object, otherwise returns false.
  */
bool MetadataUtils::isNullValue(const String &v) {
    return v.isNull();
}

/** This is overloaded function.\n
  * Returns true if \a v image is null image object, otherwise returns false.
  */
bool MetadataUtils::isNullValue(const QImage &v) {
    return v.isNull();
}

#endif // SIR_METADATA_SUPPORT
