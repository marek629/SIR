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
 * Program URL: http://marek629.github.io/SIR/
 */

#ifndef CONVERTDIALOGTEST_HPP
#define CONVERTDIALOGTEST_HPP

#include <QtTest/QTest>

#include "widgets/ConvertDialog.hpp"


class ConvertDialogTest : public QObject
{
    Q_OBJECT

public:
    explicit ConvertDialogTest();
    ~ConvertDialogTest();

private:
    ConvertDialog *convertDialog;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void convert_defaultPath();
    void convert_histogram_stretch();
    void convert_histogram_equalize();
    void convert_histogram_no();
    void convert_filter_colorBlackAndWhite();
    void convert_filter_colorSepia();
    void convert_filter_colorCustom();
    void convert_filter_gradient();
    void convert_filter_no();
    void convert_addFrame_withBorder();
    void convert_addFrame_withoutBorder();
    void convert_addFrame_no();
    void convert_addText_fontPt();
    void convert_addText_fontPx();
    void convert_addText_textIsEmpty();
    void convert_addText_no();
    void convert_addImage_imageNotNull();
    void convert_addImage_no();
    void convert_svg_removeText();
    void convert_svg_doNotRemoveText();
};

#endif // CONVERTDIALOGTEST_HPP
