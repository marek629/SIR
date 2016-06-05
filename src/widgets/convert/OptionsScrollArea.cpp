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

#include "widgets/convert/OptionsScrollArea.hpp"

/** Creates the OptionsScrollArea object. Sets up GUI and creates connections. */
OptionsScrollArea::OptionsScrollArea(QWidget *parent) : QScrollArea(parent)
{
    setupUi(this);
    createConnections();
}

void OptionsScrollArea::retranslateStrings()
{
    retranslateUi(this);
}

void OptionsScrollArea::onTargetFormatChanged(const QString &format)
{
    hideSpecialFormatInputs();
    if (format == "jpg" || format == "jpeg") {
        showJpegFormatInputs();
    }
    else if (format == "png") {
        showPngFormatInputs();
    }
//    else {
//        showGeneralFormatInputs();
//    }
}

/** Rotate checkbox slot.
  *
  * Disables/enables rotation angle line edit.
  * \param status Status of the checkbox.
  */
void OptionsScrollArea::verifyRotate(int status)
{
    bool enableRotate = (status == Qt::Checked);
    rotateDoubleSpinBox->setEnabled(enableRotate);
    rotateHorizontalSlider->setEnabled(enableRotate);
}

void OptionsScrollArea::onRotateSliderValueChanged(int value)
{
    rotateDoubleSpinBox->setValue(value);
}

void OptionsScrollArea::onRotateSplitterValueChanged(double value)
{
    rotateHorizontalSlider->setValue(value);
}

void OptionsScrollArea::createConnections()
{
    connect(rotateCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(verifyRotate(int)));
    connect(rotateDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(onRotateSplitterValueChanged(double)));
    connect(rotateHorizontalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onRotateSliderValueChanged(int)));
}

void OptionsScrollArea::hideSpecialFormatInputs()
{
    progressiveWriteCheckBox->hide();
    optimizedWriteCheckBox->hide();
}

void OptionsScrollArea::showJpegFormatInputs()
{
    progressiveWriteCheckBox->show();
    optimizedWriteCheckBox->show();
}

void OptionsScrollArea::showPngFormatInputs()
{
    progressiveWriteCheckBox->show();
}
