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

#include <QDir>
#include <QThread>

#include "generalgroupboxtest.h"
#include "settings.h"

GeneralGroupBoxTest::GeneralGroupBoxTest() {
    groupBox = new GeneralGroupBox();
    groupBox->fileSizeComboBox->addItem("KiB");
    groupBox->fileSizeComboBox->addItem("MiB");
    groupBox->sizeUnitComboBox->addItem("px");
    groupBox->sizeUnitComboBox->addItem("%");
    groupBox->sizeUnitComboBox->addItem("B");
}

GeneralGroupBoxTest::~GeneralGroupBoxTest() {
    delete groupBox;
}

void GeneralGroupBoxTest::checkViewSettings() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    QCOMPARE(groupBox->targetFolderLineEdit->text(), modelSettings.targetFolder);

    int index = groupBox->targetFormatComboBox->findText(modelSettings.targetFormat);
    QCOMPARE(groupBox->targetFormatComboBox->currentIndex(), index);

    QCOMPARE(groupBox->targetPrefixLineEdit->text(), modelSettings.targetPrefix);
    QCOMPARE(groupBox->targetSuffixLineEdit->text(), modelSettings.targetSuffix);

    QCOMPARE(groupBox->qualitySpinBox->value(), modelSettings.quality);

    QCOMPARE(groupBox->dateDisplayFormatLineEdit->text(), modelSettings.dateDisplayFormat);
    QCOMPARE(groupBox->timeDisplayFormatLineEdit->text(), modelSettings.timeDisplayFormat);
}

void GeneralGroupBoxTest::checkViewSize() {
    Settings::SizeGroup &modelSize = Settings::instance()->size;

    QCOMPARE(groupBox->widthPxSpinBox->value(), modelSize.widthPx);
    QCOMPARE(groupBox->widthPercentDoubleSpinBox->value(), (double)modelSize.widthPercent);

    QCOMPARE(groupBox->heightPxSpinBox->value(), modelSize.heightPx);
    QCOMPARE(groupBox->heightPercentDoubleSpinBox->value(), (double)modelSize.heightPercent);

    QCOMPARE(groupBox->fileSizeSpinBox->value(), (double)modelSize.fileSizeValue);
    QCOMPARE(groupBox->fileSizeComboBox->currentIndex(), modelSize.fileSizeUnit);

    QCOMPARE(groupBox->sizeUnitComboBox->currentIndex(), modelSize.sizeUnit);

    QCOMPARE(groupBox->aspectRatioCheckBox->isChecked(), modelSize.keepAspectRatio);
}

void GeneralGroupBoxTest::initTestCase() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;
    Settings::SizeGroup &modelSize = Settings::instance()->size;

    modelSettings.targetFolder = QDir::tempPath();
    modelSettings.targetFormat = "png";
    modelSettings.targetPrefix = "test";
    modelSettings.targetSuffix = "ut";
    modelSettings.quality = 50;
    modelSettings.languageNiceName = "Polish";
    modelSettings.timeDisplayFormat = "HH:mm:ss";
    modelSettings.dateDisplayFormat = "dd.MM.yyyy";

    modelSize.widthPx = 3;
    modelSize.widthPercent = 30.f;
    modelSize.heightPx = 6;
    modelSize.heightPercent = 60.f;
    modelSize.fileSizeValue = 50.5f;
    modelSize.fileSizeUnit = 0;
    modelSize.sizeUnit = 2;
    modelSize.keepAspectRatio = false;
}

void GeneralGroupBoxTest::cleanupTestCase() {}

void GeneralGroupBoxTest::loadSettings_cores_0() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    modelSettings.cores = 0;

    groupBox->loadSettings();

    checkViewSettings();
    checkViewSize();

    QCOMPARE(groupBox->coresCheckBox->isChecked(), true);
    QCOMPARE(groupBox->coresSpinBox->value(), QThread::idealThreadCount());
}

void GeneralGroupBoxTest::loadSettings_cores_not_0() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    modelSettings.cores = 2;

    groupBox->loadSettings();

    checkViewSettings();
    checkViewSize();

    QCOMPARE(groupBox->coresCheckBox->isChecked(), false);
    QCOMPARE(groupBox->coresSpinBox->value(), modelSettings.cores);
}

void GeneralGroupBoxTest::saveSettings() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    groupBox->saveSettings();

    QCOMPARE(modelSettings.targetFolder, groupBox->targetFolderLineEdit->text());
    // TODO: increase code coverage!
}

QTEST_MAIN(GeneralGroupBoxTest)
#include "generalgroupboxtest.moc"
