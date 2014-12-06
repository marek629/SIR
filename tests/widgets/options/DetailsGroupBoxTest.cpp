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

#include "tests/widgets/options/DetailsGroupBoxTest.hpp"
#include "Settings.hpp"

DetailsGroupBoxTest::DetailsGroupBoxTest() {
    Settings::enableTesting(true);
    Settings::DetailsGroup model = Settings::instance()->details;

    model.exifAuthor = 0;
    model.exifCamera = 0;
    model.exifImage = 0;
    model.exifPhoto = 0;

    model.iptc = 0;

    view = new DetailsGroupBoxView();
    controller = new DetailsGroupBoxController(&model, view, this);
    controller->loadSettings();
}

DetailsGroupBoxTest::~DetailsGroupBoxTest() {
    delete controller;
    delete view;
}

void DetailsGroupBoxTest::initTestCase() {
    QWidget *expectedCurrentTab = view->exifTab;
    QCOMPARE(view->tabWidget->currentWidget(), expectedCurrentTab);
}

void DetailsGroupBoxTest::cleanupTestCase() {}

void DetailsGroupBoxTest::buttonsEnabled_noExifSelected_noIptcSelected() {
    Q_ASSERT(controller->exifSelectedFields == 0);
    Q_ASSERT(controller->iptcSelectedFields == 0);

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(!view->hidePushButton->isEnabled());
}

QTEST_MAIN(DetailsGroupBoxTest)
#include "DetailsGroupBoxTest.moc"
