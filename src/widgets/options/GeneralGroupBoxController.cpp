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

#include <QThread>

#include "widgets/options/GeneralGroupBoxController.hpp"
#include "widgets/options/GeneralGroupBoxView.hpp"
#include "widgets/options/CommonOptions.hpp"

quint8 GeneralGroupBoxController::maxCoresCount_ = 50;

GeneralGroupBoxController::GeneralGroupBoxController(
        Settings::SettingsGroup *settingsModel, Settings::SizeGroup *sizeModel,
        GeneralGroupBoxView *view, QObject *parent) : AbstractOptionsController(parent) {
    this->modelSettings = settingsModel;
    this->modelSize = sizeModel;

    this->view = view;
    this->view->setController(this);
}

void GeneralGroupBoxController::loadSettings() {
    loadSettingsSettings();
    loadSizeSettings();
}

void GeneralGroupBoxController::saveSettings() {
    saveSettingsSettings();
    saveSizeSettings();
}

/** Returns CPU cores count. */
quint8 GeneralGroupBoxController::detectCoresCount() {
    int cores = QThread::idealThreadCount();
    if (cores == -1) {
        qWarning("GeneralGroupBoxView: cores count detect failed");
        return 1;
    }
    return qMin((quint8)cores, maxCoresCount_);
}

quint8 GeneralGroupBoxController::maxCoresCount() {
    return GeneralGroupBoxController::maxCoresCount_;
}

quint8 GeneralGroupBoxController::coresCount() {
    return coresCount_;
}

void GeneralGroupBoxController::setCoresCount(quint8 cores) {
    coresCount_ = cores;
}

void GeneralGroupBoxController::loadSettingsSettings() {
    view->targetFolderLineEdit->setText(modelSettings->targetFolder);

    view->targetFormatComboBox->setCurrentIndex(
                view->targetFormatComboBox->findText(modelSettings->targetFormat));

    view->targetPrefixLineEdit->setText(modelSettings->targetPrefix);
    view->targetSuffixLineEdit->setText(modelSettings->targetSuffix);

    view->qualitySpinBox->setValue(modelSettings->quality);

    view->languagesComboBox->setCurrentIndex(
                view->languagesComboBox->findText(modelSettings->languageNiceName,
                                            Qt::MatchExactly));

    coresCount_ = modelSettings->cores;
    if (coresCount_ == 0) {
        view->coresCheckBox->setChecked(true);
        view->coresSpinBoxChecked(true);
    }
    else {
        view->coresCheckBox->setChecked(false);
        view->coresSpinBoxChecked(false);
    }

    view->dateDisplayFormatLineEdit->setText(modelSettings->dateDisplayFormat);
    view->timeDisplayFormatLineEdit->setText(modelSettings->timeDisplayFormat);

    view->historySpinBox->setValue(CommonOptions::instance()->maxHistoryCount());
}

void GeneralGroupBoxController::loadSizeSettings() {
    view->widthPxSpinBox->setValue(modelSize->widthPx);
    view->widthPercentDoubleSpinBox->setValue(modelSize->widthPercent);

    view->heightPxSpinBox->setValue(modelSize->heightPx);
    view->heightPercentDoubleSpinBox->setValue(modelSize->heightPercent);

    view->fileSizeSpinBox->setValue(modelSize->fileSizeValue);
    view->fileSizeComboBox->setCurrentIndex(modelSize->fileSizeUnit);

    view->sizeUnitComboBox->setCurrentIndex(modelSize->sizeUnit);

    view->aspectRatioCheckBox->setChecked(modelSize->keepAspectRatio);
}

void GeneralGroupBoxController::saveSettingsSettings() {
    modelSettings->targetFolder = view->targetFolderLineEdit->text();

    modelSettings->targetFormat = view->targetFormatComboBox->currentText();

    modelSettings->targetPrefix = view->targetPrefixLineEdit->text();
    modelSettings->targetSuffix = view->targetSuffixLineEdit->text();

    modelSettings->quality = view->qualitySpinBox->value();

    modelSettings->languageNiceName = view->languagesComboBox->currentText();
    modelSettings->languageFileName = view->languageName(
                view->languagesComboBox->currentText());
    if (modelSettings->languageFileName.isEmpty())
        modelSettings->languageFileName = "sir_en.qm";

    modelSettings->dateDisplayFormat    = view->dateDisplayFormatLineEdit->text();
    modelSettings->timeDisplayFormat    = view->timeDisplayFormatLineEdit->text();

    if (view->coresCheckBox->isChecked())
        modelSettings->cores            = 0;
    else
        modelSettings->cores            = view->coresSpinBox->value();

    modelSettings->maxHistoryCount      = view->historySpinBox->value();
}

void GeneralGroupBoxController::saveSizeSettings() {
    modelSize->widthPx = view->widthPxSpinBox->value();
    modelSize->widthPercent = view->widthPercentDoubleSpinBox->value();

    modelSize->heightPx = view->heightPxSpinBox->value();
    modelSize->heightPercent = view->heightPercentDoubleSpinBox->value();

    modelSize->fileSizeValue = view->fileSizeSpinBox->value();
    modelSize->fileSizeUnit = view->fileSizeComboBox->currentIndex();

    modelSize->sizeUnit = view->sizeUnitComboBox->currentIndex();

    modelSize->keepAspectRatio = view->aspectRatioCheckBox->isChecked();
}
