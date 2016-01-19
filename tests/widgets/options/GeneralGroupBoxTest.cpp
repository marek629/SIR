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

#include <QDir>
#include <QThread>

#include "tests/widgets/options/GeneralGroupBoxTest.hpp"
#include "Settings.hpp"

GeneralGroupBoxTest::GeneralGroupBoxTest() {
    Settings::enableTesting(true);
    Settings *model = Settings::instance();

    view = new GeneralGroupBoxView();
    controller = new GeneralGroupBoxController(&(model->settings),
                                               &(model->size),
                                               view,
                                               this);

    view->fileSizeComboBox->addItem("KiB");
    view->fileSizeComboBox->addItem("MiB");

    view->sizeUnitComboBox->addItem("px");
    view->sizeUnitComboBox->addItem("%");
    view->sizeUnitComboBox->addItem("B");

    view->fileToNiceName->insert("English", "sir_en.qm");
    view->fileToNiceName->insert("Polish", "sir_pl.qm");
    view->fileToNiceName->insert("Portuguese", "sir_pt.qm");
    view->languagesComboBox->addItems(view->fileToNiceName->keys());
}

GeneralGroupBoxTest::~GeneralGroupBoxTest() {
    delete controller;
    delete view;
}

void GeneralGroupBoxTest::setModelSettings() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    modelSettings.targetFolder = QDir::tempPath();
    modelSettings.targetFormat = "png";
    modelSettings.targetPrefix = "test";
    modelSettings.targetSuffix = "ut";
    modelSettings.quality = 50;
    modelSettings.maxHistoryCount = 50;
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
    int idx;

    view->targetFolderLineEdit->setText(QDir::homePath());

    idx = view->targetFormatComboBox->findText("gif");
    view->targetFormatComboBox->setCurrentIndex(idx);

    view->targetPrefixLineEdit->setText("save");
    view->targetSuffixLineEdit->setText("settings");

    view->qualitySpinBox->setValue(62);
    view->historySpinBox->setValue(5);

    idx = view->languagesComboBox->findText("Portuguese");
    view->languagesComboBox->setCurrentIndex(idx);

    view->dateDisplayFormatLineEdit->setText("yyyy-MM-dd");
    view->timeDisplayFormatLineEdit->setText("HH:mm");
}

void GeneralGroupBoxTest::setViewModelSizeWidgets() {
    int idx;

    view->widthPxSpinBox->setValue(500);
    view->widthPercentDoubleSpinBox->setValue(50.);

    view->heightPxSpinBox->setValue(1000);
    view->heightPercentDoubleSpinBox->setValue(2.35);

    view->fileSizeSpinBox->setValue(50.);
    idx = view->fileSizeComboBox->findText("MiB");
    view->fileSizeComboBox->setCurrentIndex(idx);

    idx = view->sizeUnitComboBox->findText("px");
    view->sizeUnitComboBox->setCurrentIndex(idx);

    view->aspectRatioCheckBox->setChecked(true);
}

void GeneralGroupBoxTest::checkSavedModelSettings() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    QCOMPARE(modelSettings.targetFolder, view->targetFolderLineEdit->text());

    QCOMPARE(modelSettings.targetFormat, view->targetFormatComboBox->currentText());

    QCOMPARE(modelSettings.targetPrefix, view->targetPrefixLineEdit->text());
    QCOMPARE(modelSettings.targetSuffix, view->targetSuffixLineEdit->text());

    QCOMPARE(modelSettings.quality, view->qualitySpinBox->value());
    QCOMPARE(modelSettings.maxHistoryCount, view->historySpinBox->value());

    QCOMPARE(modelSettings.languageNiceName, view->languagesComboBox->currentText());
    QCOMPARE(modelSettings.languageFileName, QString("sir_pt.qm"));

    QCOMPARE(modelSettings.dateDisplayFormat, view->dateDisplayFormatLineEdit->text());
    QCOMPARE(modelSettings.timeDisplayFormat, view->timeDisplayFormatLineEdit->text());
}

void GeneralGroupBoxTest::checkSavedModelSize() {
    Settings::SizeGroup &modelSize = Settings::instance()->size;

    QCOMPARE(modelSize.widthPx, view->widthPxSpinBox->value());
    QCOMPARE(modelSize.widthPercent, (float)view->widthPercentDoubleSpinBox->value());

    QCOMPARE(modelSize.heightPx, view->heightPxSpinBox->value());
    QCOMPARE(modelSize.heightPercent, (float)view->heightPercentDoubleSpinBox->value());

    QCOMPARE(modelSize.fileSizeValue, (float)view->fileSizeSpinBox->value());
    QCOMPARE(modelSize.fileSizeUnit, view->fileSizeComboBox->currentIndex());

    QCOMPARE(modelSize.sizeUnit, view->sizeUnitComboBox->currentIndex());

    QCOMPARE(modelSize.keepAspectRatio, view->aspectRatioCheckBox->isChecked());
}

void GeneralGroupBoxTest::checkLoadedModelSettings() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    QCOMPARE(view->targetFolderLineEdit->text(), modelSettings.targetFolder);

    int index = view->targetFormatComboBox->findText(modelSettings.targetFormat);
    QCOMPARE(view->targetFormatComboBox->currentIndex(), index);

    QCOMPARE(view->targetPrefixLineEdit->text(), modelSettings.targetPrefix);
    QCOMPARE(view->targetSuffixLineEdit->text(), modelSettings.targetSuffix);

    QCOMPARE(view->qualitySpinBox->value(), modelSettings.quality);
    QCOMPARE(view->historySpinBox->value(), modelSettings.maxHistoryCount);

    QCOMPARE(view->languagesComboBox->currentText(), modelSettings.languageNiceName);

    QCOMPARE(view->dateDisplayFormatLineEdit->text(), modelSettings.dateDisplayFormat);
    QCOMPARE(view->timeDisplayFormatLineEdit->text(), modelSettings.timeDisplayFormat);
}

void GeneralGroupBoxTest::checkLoadedModelSize() {
    Settings::SizeGroup &modelSize = Settings::instance()->size;

    QCOMPARE(view->widthPxSpinBox->value(), modelSize.widthPx);
    QCOMPARE((float)view->widthPercentDoubleSpinBox->value(), modelSize.widthPercent);

    QCOMPARE(view->heightPxSpinBox->value(), modelSize.heightPx);
    QCOMPARE((float)view->heightPercentDoubleSpinBox->value(), modelSize.heightPercent);

    QCOMPARE((float)view->fileSizeSpinBox->value(), modelSize.fileSizeValue);
    QCOMPARE(view->fileSizeComboBox->currentIndex(), modelSize.fileSizeUnit);

    QCOMPARE(view->sizeUnitComboBox->currentIndex(), modelSize.sizeUnit);

    QCOMPARE(view->aspectRatioCheckBox->isChecked(), modelSize.keepAspectRatio);
}

void GeneralGroupBoxTest::initTestCase() {
    QCOMPARE(view->fileSizeComboBox->count(), 2);
    QCOMPARE(view->sizeUnitComboBox->count(), 3);
}

void GeneralGroupBoxTest::cleanupTestCase() {}

void GeneralGroupBoxTest::loadSettings_cores_0() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    setModelSettings();
    setModelSize();

    modelSettings.cores = 0;

    controller->loadSettings();

    checkLoadedModelSettings();
    checkLoadedModelSize();

    QCOMPARE(view->coresCheckBox->isChecked(), true);
    QCOMPARE(view->coresSpinBox->value(), QThread::idealThreadCount());
}

void GeneralGroupBoxTest::loadSettings_cores_not_0() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    setModelSettings();
    setModelSize();

    modelSettings.cores = 2;

    controller->loadSettings();

    checkLoadedModelSettings();
    checkLoadedModelSize();

    QCOMPARE(view->coresCheckBox->isChecked(), false);
    QCOMPARE(view->coresSpinBox->value(), modelSettings.cores);
}

void GeneralGroupBoxTest::saveSettings_detect_cores_count() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    setViewModelSettingsWidgets();
    setViewModelSizeWidgets();

    view->coresCheckBox->setChecked(true);

    controller->saveSettings();

    checkSavedModelSettings();
    checkSavedModelSize();

    QCOMPARE(modelSettings.cores, 0);
}

void GeneralGroupBoxTest::saveSettings_type_cores_count() {
    Settings::SettingsGroup &modelSettings = Settings::instance()->settings;

    setViewModelSettingsWidgets();
    setViewModelSizeWidgets();

    view->coresCheckBox->setChecked(false);
    view->coresSpinBox->setValue(3);

    controller->saveSettings();

    checkSavedModelSettings();
    checkSavedModelSize();

    QCOMPARE(modelSettings.cores, view->coresSpinBox->value());
}

QTEST_MAIN(GeneralGroupBoxTest)
#include "GeneralGroupBoxTest.moc"
