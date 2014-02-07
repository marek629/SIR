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

#include "converteffectstest.h"
#include <cstdlib>
#include <ctime>

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
    effects = ConvertEffects(&info);
}

void ConvertEffectsTest::initTestCase() {
    QImage img(testImg);
    QCOMPARE(img.size(), testImg.size());
    QCOMPARE(img.format(), testImg.format());
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

QTEST_MAIN(ConvertEffectsTest)
#include "converteffectstest.moc"
