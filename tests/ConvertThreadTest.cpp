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

#include "ConvertThreadTest.hpp"


ConvertThreadTest::ConvertThreadTest() {}

void ConvertThreadTest::initTestCase() {}

void ConvertThreadTest::cleanupTestCase() {}

void ConvertThreadTest::test_fillImage_data()
{
    QTest::addColumn<QColor>("color");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QColor>("expected");

    QTest::newRow("valid background color")
            << QColor(Qt::red) << "" << QColor(Qt::red);
    QTest::newRow("invalid background color, GIF image format")
            << QColor() << "gif" << QColor(Qt::transparent);
    QTest::newRow("invalid background color, PNG image format")
            << QColor() << "png" << QColor(Qt::transparent);
    QTest::newRow("invalid background color, other image format")
            << QColor() << "bmp" << QColor(Qt::white);
}

void ConvertThreadTest::test_fillImage()
{
    QImage testImage(10, 20, QImage::Format_ARGB32);

    QFETCH(QColor, color);
    QFETCH(QString, format);
    QFETCH(QColor, expected);

    SharedInformation sharedInfo;
    sharedInfo.backgroundColor = color;
    sharedInfo.format = format;
    ConvertThread::setSharedInfo(sharedInfo);

    ConvertThread thread(this, 1);

    thread.fillImage(&testImage);

    QCOMPARE(testImage.pixel(1,1), expected.rgba());
}

QTEST_MAIN(ConvertThreadTest)
#include "ConvertThreadTest.moc"
