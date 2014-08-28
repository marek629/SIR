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

    groupBox->fileToNiceName->insert("English", "sir_en.qm");
    groupBox->fileToNiceName->insert("Polish", "sir_pl.qm");
    groupBox->fileToNiceName->insert("Portuguese", "sir_pt.qm");
    groupBox->languagesComboBox->addItems(groupBox->fileToNiceName->keys());
}

GeneralGroupBoxTest::~GeneralGroupBoxTest() {
    delete groupBox;
}

void GeneralGroupBoxTest::setModelSettings() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    modelSettings.targetFolder = QDir::tempPath();
    modelSettings.targetFormat = "png";
    modelSettings.targetPrefix = "test";
    modelSettings.targetSuffix = "ut";
    modelSettings.quality = 50;
    modelSettings.languageNiceName = "Polish";
    modelSettings.timeDisplayFormat = "HH:mm:ss";
    modelSettings.dateDisplayFormat = "dd.MM.yyyy";
}

void GeneralGroupBoxTest::setModelSize() {
    Settings::SizeGroup &modelSize = Settings::instance()->size;

    modelSize.widthPx = 3;
    modelSize.widthPercent = 30.f;
    modelSize.heightPx = 6;
    modelSize.heightPercent = 60.f;
    modelSize.fileSizeValue = 50.5f;
    modelSize.fileSizeUnit = 0;
    modelSize.sizeUnit = 2;
    modelSize.keepAspectRatio = false;
}

void GeneralGroupBoxTest::setViewModelSettingsWidgets() {
    groupBox->targetFolderLineEdit->setText(QDir::homePath());

    groupBox->targetFormatComboBox->setCurrentText("gif");

    groupBox->targetPrefixLineEdit->setText("save");
    groupBox->targetSuffixLineEdit->setText("settings");

    groupBox->qualitySpinBox->setValue(62);

    groupBox->languagesComboBox->setCurrentText("Portuguese");

    groupBox->dateDisplayFormatLineEdit->setText("yyyy-MM-dd");
    groupBox->timeDisplayFormatLineEdit->setText("HH:mm");
}

void GeneralGroupBoxTest::setViewModelSizeWidgets() {
    groupBox->widthPxSpinBox->setValue(500);
    groupBox->widthPercentDoubleSpinBox->setValue(50.);

    groupBox->heightPxSpinBox->setValue(1000);
    groupBox->heightPercentDoubleSpinBox->setValue(2.35);

    groupBox->fileSizeSpinBox->setValue(50.);
    groupBox->fileSizeComboBox->setCurrentText("MiB");

    groupBox->sizeUnitComboBox->setCurrentText("px");

    groupBox->aspectRatioCheckBox->setChecked(true);
}

void GeneralGroupBoxTest::checkSavedModelSettings() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    QCOMPARE(modelSettings.targetFolder, groupBox->targetFolderLineEdit->text());

    QCOMPARE(modelSettings.targetFormat, groupBox->targetFormatComboBox->currentText());

    QCOMPARE(modelSettings.targetPrefix, groupBox->targetPrefixLineEdit->text());
    QCOMPARE(modelSettings.targetSuffix, groupBox->targetSuffixLineEdit->text());

    QCOMPARE(modelSettings.quality, groupBox->qualitySpinBox->value());

    QCOMPARE(modelSettings.languageNiceName, groupBox->languagesComboBox->currentText());
    QCOMPARE(modelSettings.languageFileName, QString("sir_pt.qm"));

    QCOMPARE(modelSettings.dateDisplayFormat, groupBox->dateDisplayFormatLineEdit->text());
    QCOMPARE(modelSettings.timeDisplayFormat, groupBox->timeDisplayFormatLineEdit->text());
}

void GeneralGroupBoxTest::checkSavedModelSize() {
    Settings::SizeGroup &modelSize = Settings::instance()->size;

    QCOMPARE(modelSize.widthPx, groupBox->widthPxSpinBox->value());
    QCOMPARE(modelSize.widthPercent, (float)groupBox->widthPercentDoubleSpinBox->value());

    QCOMPARE(modelSize.heightPx, groupBox->heightPxSpinBox->value());
    QCOMPARE(modelSize.heightPercent, (float)groupBox->heightPercentDoubleSpinBox->value());

    QCOMPARE(modelSize.fileSizeValue, (float)groupBox->fileSizeSpinBox->value());
    QCOMPARE(modelSize.fileSizeUnit, groupBox->fileSizeComboBox->currentIndex());

    QCOMPARE(modelSize.sizeUnit, groupBox->sizeUnitComboBox->currentIndex());

    QCOMPARE(modelSize.keepAspectRatio, groupBox->aspectRatioCheckBox->isChecked());
}

void GeneralGroupBoxTest::checkLoadedModelSettings() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    QCOMPARE(groupBox->targetFolderLineEdit->text(), modelSettings.targetFolder);

    int index = groupBox->targetFormatComboBox->findText(modelSettings.targetFormat);
    QCOMPARE(groupBox->targetFormatComboBox->currentIndex(), index);

    QCOMPARE(groupBox->targetPrefixLineEdit->text(), modelSettings.targetPrefix);
    QCOMPARE(groupBox->targetSuffixLineEdit->text(), modelSettings.targetSuffix);

    QCOMPARE(groupBox->qualitySpinBox->value(), modelSettings.quality);

    QCOMPARE(groupBox->languagesComboBox->currentText(), modelSettings.languageNiceName);

    QCOMPARE(groupBox->dateDisplayFormatLineEdit->text(), modelSettings.dateDisplayFormat);
    QCOMPARE(groupBox->timeDisplayFormatLineEdit->text(), modelSettings.timeDisplayFormat);
}

void GeneralGroupBoxTest::checkLoadedModelSize() {
    Settings::SizeGroup &modelSize = Settings::instance()->size;

    QCOMPARE(groupBox->widthPxSpinBox->value(), modelSize.widthPx);
    QCOMPARE((float)groupBox->widthPercentDoubleSpinBox->value(), modelSize.widthPercent);

    QCOMPARE(groupBox->heightPxSpinBox->value(), modelSize.heightPx);
    QCOMPARE((float)groupBox->heightPercentDoubleSpinBox->value(), modelSize.heightPercent);

    QCOMPARE((float)groupBox->fileSizeSpinBox->value(), modelSize.fileSizeValue);
    QCOMPARE(groupBox->fileSizeComboBox->currentIndex(), modelSize.fileSizeUnit);

    QCOMPARE(groupBox->sizeUnitComboBox->currentIndex(), modelSize.sizeUnit);

    QCOMPARE(groupBox->aspectRatioCheckBox->isChecked(), modelSize.keepAspectRatio);
}

void GeneralGroupBoxTest::initTestCase() {
    QCOMPARE(groupBox->fileSizeComboBox->count(), 2);
    QCOMPARE(groupBox->sizeUnitComboBox->count(), 3);
}

void GeneralGroupBoxTest::cleanupTestCase() {}

void GeneralGroupBoxTest::loadSettings_cores_0() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    setModelSettings();
    setModelSize();

    modelSettings.cores = 0;

    groupBox->loadSettings();

    checkLoadedModelSettings();
    checkLoadedModelSize();

    QCOMPARE(groupBox->coresCheckBox->isChecked(), true);
    QCOMPARE(groupBox->coresSpinBox->value(), QThread::idealThreadCount());
}

void GeneralGroupBoxTest::loadSettings_cores_not_0() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    setModelSettings();
    setModelSize();

    modelSettings.cores = 2;

    groupBox->loadSettings();

    checkLoadedModelSettings();
    checkLoadedModelSize();

    QCOMPARE(groupBox->coresCheckBox->isChecked(), false);
    QCOMPARE(groupBox->coresSpinBox->value(), modelSettings.cores);
}

void GeneralGroupBoxTest::saveSettings_detect_cores_count() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    setViewModelSettingsWidgets();
    setViewModelSizeWidgets();

    groupBox->coresCheckBox->setChecked(true);

    groupBox->saveSettings();

    checkSavedModelSettings();
    checkSavedModelSize();

    QCOMPARE(modelSettings.cores, 0);
}

void GeneralGroupBoxTest::saveSettings_type_cores_count() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    setViewModelSettingsWidgets();
    setViewModelSizeWidgets();

    groupBox->coresCheckBox->setChecked(false);
    groupBox->coresSpinBox->setValue(3);

    groupBox->saveSettings();

    checkSavedModelSettings();
    checkSavedModelSize();

    QCOMPARE(modelSettings.cores, groupBox->coresSpinBox->value());
}

QTEST_MAIN(GeneralGroupBoxTest)
#include "generalgroupboxtest.moc"
