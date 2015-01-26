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
 * Program URL: http://marek629.github.io/sir/
 */

#ifndef DETAILSGROUPBOXTEST_HPP
#define DETAILSGROUPBOXTEST_HPP

#include <QtTest/QTest>
#include "widgets/options/DetailsGroupBoxController.hpp"
#include "widgets/options/DetailsGroupBoxView.hpp"

class DetailsGroupBoxTest : public QObject {
    Q_OBJECT

public:
    explicit DetailsGroupBoxTest();
    ~DetailsGroupBoxTest();

private:
    DetailsGroupBoxController *controller;
    DetailsGroupBoxView *view;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void buttonsEnabled_noExifSelected_noIptcSelected();
    void buttonsEnabled_noExifSelected_allIptcSelected();
    void buttonsEnabled_noExifSelected_oneIptcSelected();
    void buttonsEnabled_oneExifSelected_noIptcSelected();
    void buttonsEnabled_allExifSelected_noIptcSelected();
    void buttonsEnabled_oneExifSelected_oneIptcSelected();
    void buttonsEnabled_oneExifSelected_allIptcSelected();
    void buttonsEnabled_allExifSelected_oneIptcSelected();
    void buttonsEnabled_allExifSelected_allIptcSelected();

    void setCurrentTab_Exif_noSelected();
    void setCurrentTab_Exif_oneSelected();
    void setCurrentTab_Exif_allSelected();
    void setCurrentTab_IPTC_noSelected();
    void setCurrentTab_IPTC_oneSelected();
    void setCurrentTab_IPTC_allSelected();
};

#endif // DETAILSGROUPBOXTEST_HPP
