/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://sir.projet-libre.org/
 */

#include "sizescrollarea.h"
#include "convertshareddata.h"

/** Creates the SizeScrollArea object. Sets up GUI and creates connections. */
SizeScrollArea::SizeScrollArea(QWidget *parent) : QScrollArea(parent) {
    setupUi(this);
    // create connections
    connect(sizeUnitComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setSizeUnit(int)));
    if (maintainCheckBox->isChecked() && sizeUnitComboBox->currentIndex()==1) // %
        connectSizeLinesEdit();
}

/** Shows size values corresponding index of size unit combo box. */
void SizeScrollArea::setSizeUnit(int index) {
    if (index < 0)
        return;
    static int lastIndex = index;
    static int lastIndexPxPercent = index+1;
    static bool keepAspectRatio = false;
    if (index == 2) { // bytes
        geometryWidget->hide();
        fileSizeWidget->show();
        keepAspectRatio = maintainCheckBox->isChecked();
        maintainCheckBox->setChecked(true);
        maintainCheckBox->setEnabled(false);
    }
    else { // px or %
        fileSizeWidget->hide();
        geometryWidget->show();
        disconnectSizeLinesEdit();
        if (lastIndexPxPercent != index) {
            ConvertSharedData *csd = ConvertSharedData::instance();
            float tmp = csd->sizeWidth;
            csd->sizeWidth = widthDoubleSpinBox->value();
            widthDoubleSpinBox->setValue(tmp);
            tmp = csd->sizeHeight;
            csd->sizeHeight = heightDoubleSpinBox->value();
            heightDoubleSpinBox->setValue(tmp);
            lastIndexPxPercent = index;
            QString suffix;
            int decimals;
            double max;
            if (index == 0) { // px
                suffix = " px";
                decimals = 0;
                max = 100000.;
            }
            else { // %
                suffix = " %";
                decimals = 2;
                max = 10000.;
            }
            widthDoubleSpinBox->setSuffix(suffix);
            widthDoubleSpinBox->setDecimals(decimals);
            widthDoubleSpinBox->setMaximum(max);
            heightDoubleSpinBox->setSuffix(suffix);
            heightDoubleSpinBox->setDecimals(decimals);
            heightDoubleSpinBox->setMaximum(max);
        }
        if (lastIndex == 2) {
            maintainCheckBox->setEnabled(true);
            maintainCheckBox->setChecked(keepAspectRatio);
        }
        if (maintainCheckBox->isChecked() && index == 1) { // %
            heightDoubleSpinBox->setValue(widthDoubleSpinBox->value());
            connectSizeLinesEdit();
        }
    }
    lastIndex = index;
}

/** If desired size unit is percent and it keeps aspect ratio this function will
  * be change width or heigth percent value following the user change in
  * adjacent spin box. Otherwise does nothing.
  */
void SizeScrollArea::sizeChanged(double value) {
    if (sizeUnitComboBox->currentIndex() != 1 && !maintainCheckBox->isChecked())
        return;
    // size unit is % and maintainCheckBox is checked
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(sender());
    if (spinBox == widthDoubleSpinBox)
        heightDoubleSpinBox->setValue(value);
    else if (spinBox == heightDoubleSpinBox)
        widthDoubleSpinBox->setValue(value);
}
