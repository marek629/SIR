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

#include "tests/widgets/options/DetailsGroupBoxTest.hpp"
#include "Settings.hpp"
#include "optionsenums.h"

DetailsGroupBoxTest::DetailsGroupBoxTest() {
    Settings::enableTesting(true);
    Settings::DetailsGroup &model = Settings::instance()->details;

    view = new DetailsGroupBoxView();
    controller = new DetailsGroupBoxController(&model, view, this);
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
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = 0;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == 0);
    Q_ASSERT(controller->iptcSelectedFields == 0);

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(!view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::buttonsEnabled_noExifSelected_allIptcSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = 0xFFFF;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == 0);
    Q_ASSERT(controller->iptcSelectedFields == controller->iptcCheckBoxes.length());

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(!view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::buttonsEnabled_noExifSelected_oneIptcSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = DetailsOptions::TimeCreated;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == 0);
    Q_ASSERT(controller->iptcSelectedFields == 1);

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(!view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::buttonsEnabled_oneExifSelected_noIptcSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = DetailsOptions::Artist;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = 0;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == 1);
    Q_ASSERT(controller->iptcSelectedFields == 0);

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::buttonsEnabled_allExifSelected_noIptcSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0xFFFF;
    model->exifCamera = 0xFFFF;
    model->exifImage = 0xFFFF;
    model->exifPhoto = 0xFFFF;
    model->iptc = 0;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == controller->exifCheckBoxes.length());
    Q_ASSERT(controller->iptcSelectedFields == 0);

    QVERIFY(!view->showPushButton->isEnabled());
    QVERIFY(view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::buttonsEnabled_oneExifSelected_oneIptcSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = DetailsOptions::Artist;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = DetailsOptions::TimeCreated;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == 1);
    Q_ASSERT(controller->iptcSelectedFields == 1);

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::buttonsEnabled_oneExifSelected_allIptcSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = DetailsOptions::Artist;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = 0xFFFF;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == 1);
    Q_ASSERT(controller->iptcSelectedFields == controller->iptcCheckBoxes.length());

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::buttonsEnabled_allExifSelected_oneIptcSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0xFFFF;
    model->exifCamera = 0xFFFF;
    model->exifImage = 0xFFFF;
    model->exifPhoto = 0xFFFF;
    model->iptc = DetailsOptions::TimeCreated;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == controller->exifCheckBoxes.length());
    Q_ASSERT(controller->iptcSelectedFields == 1);

    QVERIFY(!view->showPushButton->isEnabled());
    QVERIFY(view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::buttonsEnabled_allExifSelected_allIptcSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0xFFFF;
    model->exifCamera = 0xFFFF;
    model->exifImage = 0xFFFF;
    model->exifPhoto = 0xFFFF;
    model->iptc = 0xFFFF;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == controller->exifCheckBoxes.length());
    Q_ASSERT(controller->iptcSelectedFields == controller->iptcCheckBoxes.length());

    QVERIFY(!view->showPushButton->isEnabled());
    QVERIFY(view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::setCurrentTab_Exif_noSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = 0;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == 0);

    controller->setCurrentTab(DetailsGroupBoxController::Exif_tab);

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(!view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::setCurrentTab_Exif_oneSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = DetailsOptions::Artist;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = 0;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == 1);

    controller->setCurrentTab(DetailsGroupBoxController::Exif_tab);

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::setCurrentTab_Exif_allSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0xFFFF;
    model->exifCamera = 0xFFFF;
    model->exifImage = 0xFFFF;
    model->exifPhoto = 0xFFFF;
    model->iptc = 0;
    controller->loadSettings();

    Q_ASSERT(controller->exifSelectedFields == controller->exifCheckBoxes.count());

    controller->setCurrentTab(DetailsGroupBoxController::Exif_tab);

    QVERIFY(!view->showPushButton->isEnabled());
    QVERIFY(view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::setCurrentTab_IPTC_noSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = 0;
    controller->loadSettings();

    Q_ASSERT(controller->iptcSelectedFields == 0);

    controller->setCurrentTab(DetailsGroupBoxController::IPTC_tab);

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(!view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::setCurrentTab_IPTC_oneSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = DetailsOptions::TimeCreated;
    controller->loadSettings();

    Q_ASSERT(controller->iptcSelectedFields == 1);

    controller->setCurrentTab(DetailsGroupBoxController::IPTC_tab);

    QVERIFY(view->showPushButton->isEnabled());
    QVERIFY(view->hidePushButton->isEnabled());
}

void DetailsGroupBoxTest::setCurrentTab_IPTC_allSelected() {
    Settings::DetailsGroup *model = controller->model;
    model->exifAuthor = 0;
    model->exifCamera = 0;
    model->exifImage = 0;
    model->exifPhoto = 0;
    model->iptc = 0xFFFF;
    controller->loadSettings();

    Q_ASSERT(controller->iptcSelectedFields == controller->iptcCheckBoxes.count());

    controller->setCurrentTab(DetailsGroupBoxController::IPTC_tab);

    QVERIFY(!view->showPushButton->isEnabled());
    QVERIFY(view->hidePushButton->isEnabled());
}

QTEST_MAIN(DetailsGroupBoxTest)
#include "DetailsGroupBoxTest.moc"
