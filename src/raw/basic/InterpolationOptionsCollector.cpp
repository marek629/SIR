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

#include "raw/basic/InterpolationOptionsCollector.hpp"


InterpolationOptionsCollector::InterpolationOptionsCollector(
        BasicOptionsCollector *collector, Ui::BasicRawScrollArea *ui)
    : CollectorDecorator(collector, ui) {}

InterpolationOptionsCollector::~InterpolationOptionsCollector() {}

QString InterpolationOptionsCollector::optionsString() const
{
    QString result = decoratedCollector->optionsString();

    if (ui->interpolationGroupBox->isEnabled()) {
        int qualityIndex = ui->interpolationQualityComboBox->currentIndex();
        if (qualityIndex > -1) {
            result = QString("%1 -q %2").arg(result).arg(qualityIndex);
        }

        if (ui->interpolation4ColorsCheckBox->isChecked()) {
            result = QString("%1 -f").arg(result);
        }

        if (ui->interpolationPostProcessingCheckBox->isChecked()) {
            int postProcessingCycles = ui->interpolationPostProcessingSpinBox->value();
            Q_ASSERT(postProcessingCycles > 0);
            result = QString("%1 -m %2").arg(result).arg(postProcessingCycles);
        }
    }

    return result.simplified();
}

void InterpolationOptionsCollector::setOptions(const QString &string)
{
    QRegExp qualityRegExp = QRegExp("(-q)(\\s+)([0-3])");
    if (string.contains(qualityRegExp)) {
        int qualityIndex = qualityRegExp.cap(3).toInt();
        ui->interpolationQualityComboBox->setCurrentIndex(qualityIndex);
    }

    ui->interpolation4ColorsCheckBox->setChecked(string.contains("-f"));

    QRegExp postProcessingRegExp = QRegExp("(-m)(\\s+)(\\d+)");
    bool isPostProcessingEnabled = string.contains(postProcessingRegExp);
    if (isPostProcessingEnabled) {
        int postProcessingCycles = postProcessingRegExp.cap(3).toInt();
        ui->interpolationPostProcessingSpinBox->setValue(postProcessingCycles);
    }
    ui->interpolationPostProcessingCheckBox->setChecked(isPostProcessingEnabled);
}
