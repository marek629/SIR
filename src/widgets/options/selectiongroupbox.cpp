/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2012  Marek Jędryka   <jedryka89@gmail.com>
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

#include "selectiongroupbox.h"
#include "settings.h"

/** Default constructor.\n
  * Sets UI.
  * \sa setupUi()
  */
SelectionGroupBox::SelectionGroupBox(QWidget *parent) : AbstractOptionsGroupBox(parent) {
    setupUi(this);
}

/** Load settings and sets member widgets values.
  * \sa Settings saveSettings()
  */
void SelectionGroupBox::loadSettings() {
    Settings *s = Settings::instance();
    clearSelectionCheckBox->setChecked( s->selection.clearSelection);
    importSubdirsCheckBox->setChecked(  s->selection.subdirs);
    selectImportedCheckBox->setChecked( s->selection.selectImported);
    sFileSizeLineEdit->setText(         s->selection.fileSizeSymbol);
    sImgWidthLineEdit->setText(         s->selection.imageWidthSymbol);
    sImgHeightLineEdit->setText(        s->selection.imageHeightSymbol);
}

/** Saves settings basing member widgets values.
  * \sa Settings loadSettings()
  */
void SelectionGroupBox::saveSettings() {
    Settings *s = Settings::instance();
    s->selection.clearSelection      = clearSelectionCheckBox->isChecked();
    s->selection.subdirs             = importSubdirsCheckBox->isChecked();
    s->selection.selectImported      = selectImportedCheckBox->isChecked();
    s->selection.fileSizeSymbol      = sFileSizeLineEdit->text();
    s->selection.imageWidthSymbol    = sImgWidthLineEdit->text();
    s->selection.imageHeightSymbol   = sImgHeightLineEdit->text();
}
