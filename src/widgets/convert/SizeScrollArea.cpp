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

#include "widgets/convert/SizeScrollArea.hpp"
#include "ConvertSharedData.hpp"

/** Creates the SizeScrollArea object. Sets up GUI and creates connections. */
SizeScrollArea::SizeScrollArea(QWidget *parent) : QScrollArea(parent) {
    setupUi(this);

    // create connections
    connect(sizeUnitComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setSizeUnit(int)));
    connect(maintainCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(maintainCheckBoxChecked(bool)));

    if (maintainCheckBox->isChecked()) {
        switch (sizeUnitComboBox->currentIndex()) {
        case 0: // px
        case 1: // %
            connectSizeLinesEdit();
            break;
        default:
            break;
        }
    }
}

void SizeScrollArea::retranslateStrings() {
    retranslateUi(this);
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
        if (maintainCheckBox->isChecked()) {
            switch (index) {
            case 1: // %
                heightDoubleSpinBox->setValue(widthDoubleSpinBox->value());
            case 0: // px
                connectSizeLinesEdit();
                break;
            default:
                break;
            }
        }
    }
    lastIndex = index;
}

/** If it keeps aspect ratio, this function will be change width or heigth value
  * following the user change in adjacent spin box. Otherwise does nothing.
  * \sa maintainCheckBoxChecked()
  */
void SizeScrollArea::sizeChanged(double value) {
    if (!maintainCheckBox->isChecked())
        return;

    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(sender());

    disconnectSizeLinesEdit();

    switch (sizeUnitComboBox->currentIndex()) {
    case 0: // px
        if (spinBox == widthDoubleSpinBox)
            value /= aspectRatio;
        else
            value *= aspectRatio;
        break;
    case 1: // %
        break;
    default:
        return;
    }

    if (spinBox == widthDoubleSpinBox)
        heightDoubleSpinBox->setValue(value);
    else if (spinBox == heightDoubleSpinBox)
        widthDoubleSpinBox->setValue(value);

    connectSizeLinesEdit();
}

/** If \a keepAspect is true and pixels are current size unit, this function
  * will compute current aspectRatio rational. Otherwise does nothing.
  * \sa sizeChanged()
  */
void SizeScrollArea::maintainCheckBoxChecked(bool keepAspect) {
    if (keepAspect && sizeUnitComboBox->currentIndex() == 0) {
        switch (sizeUnitComboBox->currentIndex()) {
        case 0: // px
            aspectRatio = widthDoubleSpinBox->value() / heightDoubleSpinBox->value();
        case 1: // %
            connectSizeLinesEdit();
            break;
        default:
            break;
        }
    }
    else
        disconnectSizeLinesEdit();
}
