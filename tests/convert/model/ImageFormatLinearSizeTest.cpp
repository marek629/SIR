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

#include "tests/convert/model/ImageFormatLinearSizeTest.hpp"

#include "ConvertThread.hpp"


ImageFormatLinearSizeTest::ImageFormatLinearSizeTest() : QObject() {}

void ImageFormatLinearSizeTest::test_isLinearFileSizeFormat_data()
{
    QTest::addColumn<QString>("format");
    QTest::addColumn<bool>("expected");

    QTest::newRow("BMP") << "bmp" << true;
    QTest::newRow("PPM") << "ppm" << true;
    QTest::newRow("ICO") << "ico" << true;
    QTest::newRow("TIF") << "tif" << true;
    QTest::newRow("TIFF") << "tiff" << true;
    QTest::newRow("XBM") << "xbm" << true;
    QTest::newRow("JPG") << "jpg" << false;
    QTest::newRow("JPEG") << "jpeg" << false;
}

void ImageFormatLinearSizeTest::test_isLinearFileSizeFormat()
{
    QFETCH(QString, format);
    QFETCH(bool, expected);

    ConvertThread thread(this, 1);
    thread.shared.targetImage = TargetImage(ImageFormat(format));

    QCOMPARE(thread.isLinearFileSizeFormat(), expected);
}

QTEST_APPLESS_MAIN(ImageFormatLinearSizeTest)
#include "ImageFormatLinearSizeTest.moc"
