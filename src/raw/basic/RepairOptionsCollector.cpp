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

#include "raw/basic/RepairOptionsCollector.hpp"

#include "raw/basic/BasicRawScrollAreaAdapter.hpp"


RepairOptionsCollector::RepairOptionsCollector(BasicOptionsCollector *collector,
                                               BasicRawScrollAreaAdapter *ui)
    : CollectorDecorator(collector, ui) {}

RepairOptionsCollector::~RepairOptionsCollector() {}

QString RepairOptionsCollector::optionsString() const
{
    QString result = decoratedCollector->optionsString();

    int highlightLevel = -1;
    switch (view->repairHighlightOptionChecked()) {
    case Clip:
        highlightLevel = 0;
        break;
    case Unclip:
        highlightLevel = 1;
        break;
    case Blend:
        highlightLevel = 2;
        break;
    case Rebuild:
        highlightLevel = view->repairHighlightRebuildLevel();
        Q_ASSERT(highlightLevel >= 3);
        Q_ASSERT(highlightLevel <= 9);
        break;
    case NoneRepair:
    default:
        break;
    }
    if (highlightLevel >= 0 && highlightLevel <= 9) {
        result = QString("%1 -H %2").arg(result).arg(highlightLevel);
    }

    if (view->isEraseNoiseChecked()) {
        int eraseNoiseThreshold = view->eraseNoiseThreshold();
        result = QString("%1 -n %2").arg(result).arg(eraseNoiseThreshold);
    }

    return result;
}

void RepairOptionsCollector::setOptions(const QString &string)
{
    decoratedCollector->setOptions(string);

    QRegExp highlightRegExp = QRegExp("(-H)(\\s+)([0-9])");
    if (string.contains(highlightRegExp)) {
        int highlightLevel = highlightRegExp.cap(3).toInt();
        if (highlightLevel == 0) {
            view->setRepairHighlightOptionChecked(Clip);
        } else if (highlightLevel == 1) {
            view->setRepairHighlightOptionChecked(Unclip);
        } else if (highlightLevel == 2) {
            view->setRepairHighlightOptionChecked(Blend);
        } else if (highlightLevel >= 3 && highlightLevel <= 9) {
            view->setRepairHighlightOptionChecked(Rebuild);
            view->setRepairHighlightRebuildLevel(highlightLevel);
        }
    }

    QRegExp eraseNoiseRegExp = QRegExp("(-n)(\\s+)([0-9]+)");
    bool isEraseNoiseEnabled = string.contains(eraseNoiseRegExp);
    view->setEraseNoiseChecked(isEraseNoiseEnabled);
    if (isEraseNoiseEnabled) {
        int eraseNoiseThreshold = eraseNoiseRegExp.cap(3).toInt();
        view->setEraseNoiseThreshold(eraseNoiseThreshold);
    }
}
