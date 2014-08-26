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
 * Program URL: http://marek629.github.io/sir/
 */

#ifndef CONVERTEFFECTSTEST_H
#define CONVERTEFFECTSTEST_H

#include <QtTest/QTest>
#include "converteffects.h"

class ConvertEffectsTest : public QObject {
    Q_OBJECT

public:
    ConvertEffectsTest();

private:
    QImage testImg;
    QImage initialImage;
    SharedInformation info;
    ConvertEffects effects;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void combine_color_loop();
    void combine_color_painter();
    void combine_color_compare();
    void combine_brush();
    void getTransformOriginPoint_pixels_zero();
    void getTransformOriginPoint_pixels_positive();
    void getTransformOriginPoint_pixels_negative();
    void getTransformOriginPoint_pixels_negative_y();
    void getTransformOriginPoint_percent_zero();
    void getTransformOriginPoint_percent_positive();
    void getTransformOriginPoint_percent_negative();
    void addText_center();
    void addText_topLeftCorner();
    void addText_middleBottomEdge();
};

#endif // CONVERTEFFECTSTEST_H
