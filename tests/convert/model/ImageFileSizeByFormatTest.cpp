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

#include "tests/convert/model/ImageFileSizeByFormatTest.hpp"

#include "convert/model/ImageFileSize.hpp"
#include "convert/model/ImageFormat.hpp"


ImageFileSizeByFormatTest::ImageFileSizeByFormatTest() : QObject() {}

void ImageFileSizeByFormatTest::test_bytesByFormat_data()
{
    QTest::addColumn<QString>("format");
    QTest::addColumn<double>("fileSize");
    QTest::addColumn<double>("expected");

    QTest::newRow("BMP") << "bmp" << 300.+54. << 100.;
    QTest::newRow("PPM") << "ppm" << 300.+17. << 100.;
    QTest::newRow("ICO") << "ico" << 400.+1422. << 100.;
    QTest::newRow("TIF") << "tif" << 400.+14308. << 100.;
    QTest::newRow("TIFF") << "tiff" << 400.+14308. << 100.;
    QTest::newRow("XBM") << "xbm" << 65.+60. << 100.;
    QTest::newRow("JPG") << "jpg" << 400. << 400.;
    QTest::newRow("JPEG") << "jpeg" << 400. << 400.;
}

void ImageFileSizeByFormatTest::test_bytesByFormat()
{
    QFETCH(QString, format);
    QFETCH(double, fileSize);
    QFETCH(double, expected);

    ImageFileSize imageFileSize(fileSize);

    QCOMPARE(imageFileSize.bytesByFormat(ImageFormat(format)), expected);
}

QTEST_APPLESS_MAIN(ImageFileSizeByFormatTest)
#include "ImageFileSizeByFormatTest.moc"
