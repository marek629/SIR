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

#include "metadatautilstest.h"

#ifdef SIR_METADATA_SUPPORT

using namespace MetadataUtils;
using namespace sir;

MetadataUtilsTest::MetadataUtilsTest() {}

void MetadataUtilsTest::isNullValue_char_data() {
    QTest::addColumn< char >("input");
    QTest::addColumn< bool >("result");

    QTest::newRow("end of string character") << '\0' << true;
    QTest::newRow("zero character") << '0' << false;
    QTest::newRow("an character") << 'a' << false;
//    QTest::newRow("an empty character") << '' << false;
}

void MetadataUtilsTest::isNullValue_char() {
    QFETCH(char, input);
    QFETCH(bool, result);

    QCOMPARE(isNullValue(input), result);
}

void MetadataUtilsTest::isNullValue_int_data() {
    QTest::addColumn< int >("input");
    QTest::addColumn< bool >("result");

    QTest::newRow("zero") << 0 << true;
    QTest::newRow("more then zero") << 205060 << false;
    QTest::newRow("less then zero") << -20954 << false;
}

void MetadataUtilsTest::isNullValue_int() {
    QFETCH(int, input);
    QFETCH(bool, result);

    QCOMPARE(isNullValue(input), result);
}

void MetadataUtilsTest::isNullValue_float_data() {
    QTest::addColumn< float >("input");
    QTest::addColumn< bool >("result");

    QTest::newRow("zero from int") << float(0) << true;
    QTest::newRow("zero from float (long form)") << 0.0f << true;
    QTest::newRow("zero from float (short form)") << 0.f << true;
    QTest::newRow("zero from double (long form)") << float(0.0) << true;
    QTest::newRow("zero from double (short form)") << float(0.) << true;
    QTest::newRow("more then zero") << 205.060f << false;
    QTest::newRow("less then zero") << -.20954f << false;
}

void MetadataUtilsTest::isNullValue_float() {
    QFETCH(float, input);
    QFETCH(bool, result);

    QCOMPARE(isNullValue(input), result);
}

void MetadataUtilsTest::isNullValue_sir_String_data() {
    QTest::addColumn< String >("input");
    QTest::addColumn< bool >("result");

    QTest::newRow("null string") << String() << true;
    QTest::newRow("empty string") << String("") << false;
    QTest::newRow("a word string") << String("cat") << false;
}

void MetadataUtilsTest::isNullValue_sir_String() {
    QFETCH(String, input);
    QFETCH(bool, result);

    QCOMPARE(isNullValue(input), result);
}

void MetadataUtilsTest::isNullValue_QImage_data() {
    QTest::addColumn< QImage >("input");
    QTest::addColumn< bool >("result");

    QTest::newRow("null image") << QImage() << true;
    QTest::newRow("empty image") << QImage(0, 0, QImage::Format_RGB32) << true;
    QTest::newRow("very small image") << QImage(1, 1, QImage::Format_RGB32) << false;
}

void MetadataUtilsTest::isNullValue_QImage() {
    QFETCH(QImage, input);
    QFETCH(bool, result);

    QCOMPARE(isNullValue(input), result);
}

QTEST_APPLESS_MAIN(MetadataUtilsTest)
#include "metadatautilstest.moc"

#else // SIR_METADATA_SUPPORT
#error "Metadata build of SIR is needed for MetadataUtilsTest."
#endif // SIR_METADATA_SUPPORT
