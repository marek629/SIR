/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2015  Marek Jędryka   <jedryka89@gmail.com>
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

#ifndef METADATAUTILSTEST_H
#define METADATAUTILSTEST_H

#ifdef SIR_METADATA_SUPPORT

#include <QtTest/QTest>
#include "metadata/MetadataUtils.hpp"

class MetadataUtilsTest : public QObject {
    Q_OBJECT

public:
    MetadataUtilsTest();

private slots:
    void isNullValue_char_data();
    void isNullValue_char();
    void isNullValue_int_data();
    void isNullValue_int();
    void isNullValue_float_data();
    void isNullValue_float();
    void isNullValue_sir_String_data();
    void isNullValue_sir_String();
    void isNullValue_QImage_data();
    void isNullValue_QImage();
};

Q_DECLARE_METATYPE( sir::String )

#endif // SIR_METADATA_SUPPORT

#endif // METADATAUTILSTEST_H
