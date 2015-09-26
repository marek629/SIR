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

#include "raw/basic/BasicRawScrollAreaAdapter.hpp"


BasicRawScrollAreaAdapter::BasicRawScrollAreaAdapter(Ui::BasicRawScrollArea *ui)
{
    this->ui = ui;
}

BasicRawScrollAreaAdapter::~BasicRawScrollAreaAdapter() {}

bool BasicRawScrollAreaAdapter::isWhiteBalanceFromCameraChecked() const
{
    return ui->whiteBalanceFromCameraCheckBox->isChecked();
}

void BasicRawScrollAreaAdapter::setWhiteBalanceFromCameraChecked(bool checked)
{
    ui->whiteBalanceFromCameraCheckBox->setChecked(checked);
}

bool BasicRawScrollAreaAdapter::isWhiteBalanceFromWholeImageChecked() const
{
    return ui->whiteBalanceFromWholeImageCheckBox->isChecked();
}

void BasicRawScrollAreaAdapter::setWhiteBalanceFromWholeImageChecked(bool checked)
{
    ui->whiteBalanceFromWholeImageCheckBox->setChecked(checked);
}

bool BasicRawScrollAreaAdapter::isColorMatrixFromCameraChecked() const
{
    return ui->colorMatrixFromCameraCheckBox->isChecked();
}

void BasicRawScrollAreaAdapter::setColorMatrixFromCameraChecked(bool checked)
{
    ui->colorMatrixFromCameraCheckBox->setChecked(checked);
}

int BasicRawScrollAreaAdapter::interpolationQualityIndex() const
{
    return ui->interpolationQualityComboBox->currentIndex();
}

void BasicRawScrollAreaAdapter::setInterpolationQualityIndex(int index)
{
    ui->interpolationQualityComboBox->setCurrentIndex(index);
}

bool BasicRawScrollAreaAdapter::isInterpolationFourColorsChecked() const
{
    return ui->interpolation4ColorsCheckBox->isChecked();
}

void BasicRawScrollAreaAdapter::setInterpolationFourColorsChecked(bool checked)
{
    ui->interpolation4ColorsCheckBox->setChecked(checked);
}

bool BasicRawScrollAreaAdapter::isInterpolationPostProcessingChecked() const
{
    return ui->interpolationPostProcessingCheckBox->isChecked();
}

void BasicRawScrollAreaAdapter::setInterpolationPostProcessingChecked(bool checked)
{
    ui->interpolationPostProcessingCheckBox->setChecked(checked);
}

int BasicRawScrollAreaAdapter::interpolationPostProcessingCycles() const
{
    return ui->interpolationPostProcessingSpinBox->value();
}

void BasicRawScrollAreaAdapter::setInterpolationPostProcessingCycles(int cycles)
{
    ui->interpolationPostProcessingSpinBox->setValue(cycles);
}

RepairHighlightOption BasicRawScrollAreaAdapter::repairHighlightOptionChecked() const
{
    if (ui->repairHighlightClipRadioButton->isChecked()) {
        return Clip;
    } else if (ui->repairHighlightUnclipRadioButton->isChecked()) {
        return Unclip;
    } else if (ui->repairHighlightBlendRadioButton->isChecked()) {
        return Blend;
    } else if (ui->repairHighlightRebuildRadioButton->isChecked()) {
        return Rebuild;
    } else {
        return NoneRepair;
    }
}

void BasicRawScrollAreaAdapter::setRepairHighlightOptionChecked(RepairHighlightOption option)
{
    switch (option) {
    case Clip:
        ui->repairHighlightClipRadioButton->setChecked(true);
        break;
    case Unclip:
        ui->repairHighlightUnclipRadioButton->setChecked(true);
        break;
    case Blend:
        ui->repairHighlightBlendRadioButton->setChecked(true);
        break;
    case Rebuild:
        ui->repairHighlightRebuildRadioButton->setChecked(true);
        break;
    case NoneRepair:
    default:
        break;
    }
}

int BasicRawScrollAreaAdapter::repairHighlightRebuildLevel() const
{
    return ui->repairHighlightRebuildSpinBox->value();
}

void BasicRawScrollAreaAdapter::setRepairHighlightRebuildLevel(int level)
{
    ui->repairHighlightRebuildSpinBox->setValue(level);
}
