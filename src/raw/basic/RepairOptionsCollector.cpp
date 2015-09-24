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

#include "raw/basic/RepairOptionsCollector.hpp"


RepairOptionsCollector::RepairOptionsCollector(BasicOptionsCollector *collector,
                                               Ui::BasicRawScrollArea *ui)
    : CollectorDecorator(collector, ui) {}

RepairOptionsCollector::~RepairOptionsCollector() {}

QString RepairOptionsCollector::optionsString() const
{
    QString result = decoratedCollector->optionsString();

    int highlightLevel = -1;

    if (ui->repairHighlightClipRadioButton->isChecked()) {
        highlightLevel = 0;
    } else if (ui->repairHighlightUnclipRadioButton->isChecked()) {
        highlightLevel = 1;
    } else if (ui->repairHighlightBlendRadioButton->isChecked()) {
        highlightLevel = 2;
    } else if (ui->repairHighlightRebuildRadioButton->isChecked()) {
        highlightLevel = ui->repairHighlightRebuildSpinBox->value();
        Q_ASSERT(highlightLevel >= 3);
        Q_ASSERT(highlightLevel <= 9);
    }

    if (highlightLevel >= 0 && highlightLevel <= 9) {
        result = QString("%1 -H %2").arg(result).arg(highlightLevel);
    }

    return result;
}

void RepairOptionsCollector::setOptions(const QString &string)
{
    QRegExp highlightRegExp = QRegExp("(-H)(\\s+)([0-9])");
    if (string.contains(highlightRegExp)) {
        int highlightLevel = highlightRegExp.cap(3).toInt();
        if (highlightLevel == 0) {
            ui->repairHighlightClipRadioButton->setChecked(true);
        } else if (highlightLevel == 1) {
            ui->repairHighlightUnclipRadioButton->setChecked(true);
        } else if (highlightLevel == 2) {
            ui->repairHighlightBlendRadioButton->setChecked(true);
        } else if (highlightLevel >= 3 && highlightLevel <= 9) {
            ui->repairHighlightRebuildRadioButton->setChecked(true);
            ui->repairHighlightRebuildSpinBox->setValue(highlightLevel);
        }
    }
}
