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

#include <QImageWriter>
#include <QPainter>
#include <QTemporaryFile>


ConvertThreadTest::ConvertThreadTest() {}

void ConvertThreadTest::initTestCase() {}

void ConvertThreadTest::cleanupTestCase()
{
    QFileInfoList removedFileInfoList;

    foreach (QFileInfo fileInfo, createdFileInfoList) {
        QFile file(fileInfo.absoluteFilePath());
        if (file.remove()) {
            removedFileInfoList << fileInfo;
        }
    }

    QCOMPARE(removedFileInfoList.length(), createdFileInfoList.length());
}

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

void ConvertThreadTest::test_loadImage_data()
{
    QImage testImage(10, 20, QImage::Format_ARGB32);
    testImage.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&testImage);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::SolidPattern);
    painter.fillRect(5, 10, 5, 10, Qt::black);
    painter.end();
    QString tempFileNamePattern("%1%2%3");
    tempFileNamePattern = tempFileNamePattern.arg(QDir::tempPath(),
                                                  QDir::separator());

    QTemporaryFile pngTempFile(tempFileNamePattern.arg("sir-XXXXXX.png"));
    pngTempFile.setAutoRemove(false);
    testImage.save(&pngTempFile, "PNG");
    QFileInfo pngTempFileInfo(pngTempFile);
    createdFileInfoList << pngTempFileInfo;

    QTemporaryFile regularTempFile(tempFileNamePattern.arg("sir-XXXXXX.bmp"));
    regularTempFile.setAutoRemove(false);
    testImage.save(&regularTempFile);
    QFileInfo regularTempFileInfo(regularTempFile);
    createdFileInfoList << regularTempFileInfo;

    QTest::addColumn<bool>("isRawImage");
    QTest::addColumn<bool>("isSvgImage");
    QTest::addColumn<QString>("imagePath");
    QTest::addColumn<QString>("targetFormat");
    QTest::addColumn<QColor>("customBackgroundColor");
    QTest::addColumn<QColor>("expectedBackgroundColor");

    QTest::newRow("load regular image")
            << false << false << regularTempFileInfo.absoluteFilePath()
            << "png" << QColor() << QColor(Qt::black);
    QTest::newRow("load regular image with custom background color")
            << false << false << regularTempFileInfo.absoluteFilePath()
            << "png" << QColor(Qt::red) << QColor(Qt::black);
    QTest::newRow("load transparent PNG image")
            << false << false << regularTempFileInfo.absoluteFilePath()
            << "png" << QColor() << QColor(Qt::black);
    QTest::newRow("load transparent PNG image with custom background color, target PNG format")
            << false << false << pngTempFileInfo.absoluteFilePath()
            << "png" << QColor(Qt::red) << QColor(Qt::red);
    QTest::newRow("load transparent PNG image with custom background color, target BMP format")
            << false << false << pngTempFileInfo.absoluteFilePath()
            << "bmp" << QColor(Qt::red) << QColor(Qt::red);
}

void ConvertThreadTest::test_loadImage()
{
    QFETCH(bool, isRawImage);
    QFETCH(bool, isSvgImage);
    QFETCH(QString, imagePath);
    QFETCH(QString, targetFormat);
    QFETCH(QColor, customBackgroundColor);
    QFETCH(QColor, expectedBackgroundColor);

    SharedInformation sharedInfo;
    sharedInfo.backgroundColor = customBackgroundColor;
    sharedInfo.format = targetFormat;
    ConvertThread::setSharedInfo(sharedInfo);

    ConvertThread thread(this, 1);

    RawModel rawModel(isRawImage, "", "");

    QImage *image = thread.loadImage(imagePath, &rawModel, isSvgImage);

    QCOMPARE(image->pixel(1, 1), expectedBackgroundColor.rgba());

    delete image;
}

QTEST_MAIN(ConvertThreadTest)
#include "ConvertThreadTest.moc"
