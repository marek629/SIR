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

#include "tests/image/QImageLoaderTest.hpp"

#include "image/QImageLoader.hpp"
#include "image/SvgParameters.hpp"

#include "tests/fake/ConvertThreadFake.hpp"


QImageLoaderTest::QImageLoaderTest(QObject *parent) : QObject(parent) {}

void QImageLoaderTest::initTestCase() {}

void QImageLoaderTest::cleanupTestCase() {}

void QImageLoaderTest::test_loadSvgImage_keepAspectRatio_data()
{
    QString svgContentPattern =
          "<svg width=\"%1\" height=\"%2\">"
            "<rect width=\"%3\" height=\"%4\" style=\"fill:rgb(0,0,255);\" />"
          "</svg>";
    QString svgFileNamePattern =
            QDir::tempPath() + QDir::separator() + "sir_%1.svg";

    QString svg100x100FileName = svgFileNamePattern.arg("100x100");
    int svg100x100Width = 100;
    int svg100x100Height = 100;
    QString svg100x100Content = svgContentPattern
            .arg(svg100x100Width).arg(svg100x100Height)
            .arg(svg100x100Width).arg(svg100x100Height);
    QFile svg100x100File(svg100x100FileName);
    if (svg100x100File.open(QIODevice::WriteOnly)) {
        QTextStream stream(&svg100x100File);
        stream << svg100x100Content;
    }
    svg100x100File.close();

    QTest::addColumn<bool>("isModifiersEnabled");
    QTest::addColumn<QString>("imagePath");
    QTest::addColumn<bool>("isMaintainAspect");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("expectedWidth");
    QTest::addColumn<int>("expectedHeight");

    QTest::newRow("disabled keeping aspect ratio")
            << false << svg100x100FileName << false
            << svg100x100Width << svg100x100Height
            << svg100x100Width << svg100x100Height;
    QTest::newRow("enabled keeping aspect ratio, same shape")
            << false << svg100x100FileName << true
            << svg100x100Width << svg100x100Height
            << svg100x100Width << svg100x100Height;
    QTest::newRow("enabled keeping aspect ratio, diffrent shape, more height")
            << false << svg100x100FileName << true
            << svg100x100Width + 200 << svg100x100Height + 500
            << svg100x100Width + 200 << svg100x100Height + 200;
    QTest::newRow("enabled keeping aspect ratio, diffrent shape, more width")
            << false << svg100x100FileName << true
            << svg100x100Width + 500 << svg100x100Height + 200
            << svg100x100Width + 200 << svg100x100Height + 200;
}

void QImageLoaderTest::test_loadSvgImage_keepAspectRatio()
{
    QFETCH(bool, isModifiersEnabled);
    QFETCH(QString, imagePath);
    QFETCH(bool, isMaintainAspect);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, expectedWidth);
    QFETCH(int, expectedHeight);

    SvgParameters params;
    params.setSvgModifiersEnabled(isModifiersEnabled);
    params.setMaintainAspect(isMaintainAspect);
    params.setWidth(width);
    params.setHeight(height);

    ConvertThreadFake thread(NULL, 1);
    QImageLoader loader(NULL, &thread);
    loader.setSvgParameters(params);

    QImage *image = loader.loadSvgImage(imagePath);
    QVERIFY(image != NULL);

    params = loader.getSvgParameters();

    QCOMPARE(params.getWidth(), expectedWidth);
    QCOMPARE(params.getHeight(), expectedHeight);
}

QTEST_MAIN(QImageLoaderTest)
#include "QImageLoaderTest.moc"
