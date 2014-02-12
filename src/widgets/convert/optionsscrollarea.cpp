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
 * Program URL: http://sir.projet-libre.org/
 */

#include "optionsscrollarea.h"

/** Creates the OptionsScrollArea object. Sets up GUI and creates connections. */
OptionsScrollArea::OptionsScrollArea(QWidget *parent) : QScrollArea(parent) {
    setupUi(this);
    // create connections
    connect(rotateCheckBox, SIGNAL(stateChanged(int)), SLOT(verifyRotate(int)));
    // quality spin box & slider
    connect(qualitySpinBox, SIGNAL(valueChanged(int)), qualitySlider, SLOT(setValue(int)));
    connect(qualitySlider, SIGNAL(valueChanged(int)), qualitySpinBox, SLOT(setValue(int)));
}

/** Rotate checkbox slot.
  *
  * Disables/enables rotation angle line edit.
  * \param status Status of the checkbox.
  */
void OptionsScrollArea::verifyRotate(int status) {
    if (status == Qt::Checked)
        rotateLineEdit->setEnabled(true);
    else
        rotateLineEdit->setEnabled(false);
}
