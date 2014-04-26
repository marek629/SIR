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

#include "converteffectstest.h"
#include <cstdlib>
#include <ctime>
#include <QPainter>

ConvertEffectsTest::ConvertEffectsTest() : testImg(300, 500, QImage::Format_ARGB32) {
    srand(time(NULL));
    const int maxValue = 256;
    for (int y=0; y<testImg.height(); y++) {
        for (int x=0; x<testImg.width(); x++) {
            int r = rand() % maxValue;
            int g = rand() % maxValue;
            int b = rand() % maxValue;
            testImg.setPixel(x, y, qRgb(r, g, b));
        }
    }
    initialImage = testImg.copy();
    effects = ConvertEffects(&info);
}

void ConvertEffectsTest::initTestCase() {
    QImage img(testImg);
    QCOMPARE(img.size(), testImg.size());
    QCOMPARE(img.format(), testImg.format());
}

void ConvertEffectsTest::cleanupTestCase() {
    QCOMPARE(testImg, initialImage);
}

void ConvertEffectsTest::combine_color_loop() {
    QImage img(testImg);
    effects.setImage(&img);

    QColor color(Qt::green);

    QBENCHMARK_ONCE {
        effects.combineLoop(color);
    }
}

void ConvertEffectsTest::combine_color_painter() {
    QImage img(testImg);
    effects.setImage(&img);

    QColor color(Qt::green);

    QBENCHMARK_ONCE {
        effects.combine(color);
    }
}

void ConvertEffectsTest::combine_color_compare() {
    QElapsedTimer timer;
    QImage img;
    QColor color(Qt::green);

    img = testImg;
    effects.setImage(&img);

    timer.start();
    effects.combine(color);
    qint64 msPainter = timer.elapsed();


    img = testImg;
    effects.setImage(&img);

    timer.start();
    effects.combineLoop(color);
    qint64 msLoop = timer.elapsed();

    QVERIFY(msPainter < msLoop);
}

void ConvertEffectsTest::combine_brush() {
    QImage img(testImg);
    effects.setImage(&img);

    QLinearGradient gradient(0, 0, img.width()-1, img.height()-1);
    gradient.setColorAt(0.0, QColor(Qt::red));
    gradient.setColorAt(1.0, QColor(Qt::yellow));
    QBrush brush(gradient);

    QBENCHMARK_ONCE {
        effects.combine(brush);
    }
}

void ConvertEffectsTest::getTransformOriginPoint_pixels_zero() {
    QImage img(testImg);
    effects.setImage(&img);

    QPoint testPoint(0, 0);
    PosUnitPair unitPair(Pixel, Pixel);
    QPoint expected(testPoint);
    QPoint result = effects.getTransformOriginPoint(testPoint, unitPair);
    QCOMPARE(result, expected);
}

void ConvertEffectsTest::getTransformOriginPoint_pixels_positive() {
    QImage img(testImg);
    effects.setImage(&img);

    QPoint testPoint(img.width(), img.height());
    PosUnitPair unitPair(Pixel, Pixel);
    QPoint expected(testPoint);
    QPoint result = effects.getTransformOriginPoint(testPoint, unitPair);
    QCOMPARE(result, expected);
}

void ConvertEffectsTest::getTransformOriginPoint_pixels_negative() {
    QImage img(testImg);
    effects.setImage(&img);

    QPoint testPoint(-10, -20);
    PosUnitPair unitPair(Pixel, Pixel);
    QPoint expected;
    expected.setX(testPoint.x() + img.width());
    expected.setY(testPoint.y() + img.height());

    QPoint result = effects.getTransformOriginPoint(testPoint, unitPair);
    QCOMPARE(result, expected);
}

void ConvertEffectsTest::getTransformOriginPoint_pixels_negative_y() {
    QImage img(testImg);
    effects.setImage(&img);

    QPoint testPoint(100, -20);
    PosUnitPair unitPair(Pixel, Pixel);
    QPoint expected;
    expected.setX(testPoint.x());
    expected.setY(testPoint.y() + img.height());

    QPoint result = effects.getTransformOriginPoint(testPoint, unitPair);
    QCOMPARE(result, expected);
}

void ConvertEffectsTest::getTransformOriginPoint_percent_zero() {
    QImage img(testImg);
    effects.setImage(&img);

    QPoint testPoint(0, 0);
    PosUnitPair unitPair(Percent, Percent);
    QPoint expected(testPoint);
    QPoint result = effects.getTransformOriginPoint(testPoint, unitPair);
    QCOMPARE(result, expected);
}

void ConvertEffectsTest::getTransformOriginPoint_percent_positive() {
    QImage img(testImg);
    effects.setImage(&img);

    QPoint testPoint(100, 50);
    PosUnitPair unitPair(Percent, Percent);
    QPoint expected(img.width(), img.height()/2);
    QPoint result = effects.getTransformOriginPoint(testPoint, unitPair);
    QCOMPARE(result, expected);
}

void ConvertEffectsTest::getTransformOriginPoint_percent_negative() {
    QImage img(testImg);
    effects.setImage(&img);

    QPoint testPoint(-10, -20);
    PosUnitPair unitPair(Percent, Percent);
    QPoint expected;
    expected.setX(-img.width()/10 + img.width());
    expected.setY(-img.height()/5 + img.height());

    QPoint result = effects.getTransformOriginPoint(testPoint, unitPair);
    QCOMPARE(result, expected);
}

void ConvertEffectsTest::addText_center() {
    QImage result(testImg);
    effects.setImage(&result);

    info.textPos = QPoint(0, 0);
    info.textUnitPair.first = Pixel;
    info.textUnitPair.second = Pixel;
    info.textFont.setFamily("DejaVu Sans");
    info.textFont.setPointSize(20);
    info.textString = "test string";
    info.textPosModifier = Center;
    info.textColor = Qt::green;
    info.textOpacity = 0.5;
    info.textRotation = 0;
    info.textFrame = false;

    effects.addText();

    QImage expected(testImg);
    QPoint centerPoint(expected.width()/2, expected.height()/2);
    QFontMetrics fontMetrics(info.textFont, &expected);
    QRect rect = fontMetrics.boundingRect(info.textString);
    const int dx = 5;
    const int dy = 1;
    rect.adjust(-dx, -dy, dx, dy);
    rect.moveCenter(centerPoint);
    QPainter p(&expected);
    p.setPen(info.textColor);
    p.setFont(info.textFont);
    p.setOpacity(info.textOpacity);
    p.drawText(rect, Qt::AlignCenter, info.textString);

    result.save(QDir::tempPath() + "/sir_test_addText_center_result.bmp");
    expected.save(QDir::tempPath() + "/sir_test_addText_center_expected.bmp");
    QCOMPARE(result, expected);
}

QTEST_MAIN(ConvertEffectsTest)
#include "converteffectstest.moc"
