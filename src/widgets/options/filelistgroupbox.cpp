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

#include "filelistgroupbox.h"
#include "settings.h"
#include "optionsenums.h"

using namespace TreeWidgetOptions;

/** Default constructor.
  *
  * Sets up UI and create connections.
  */
FileListGroupBox::FileListGroupBox(QWidget *parent) : AbstractOptionsGroupBox(parent) {
    setupUi(this);

    // setup check box list
    checkBoxList = columnsGroupBox->findChildren<QCheckBox*>();
    visibleColumnsCount = 0;

    // create connections
    // check boxes
    foreach (QCheckBox *checkBox, checkBoxList)
        connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(columnToggled(bool)));
    // push buttons
    connect(showPushButton, SIGNAL(clicked()), this, SLOT(showAllColumns()));
    connect(hidePushButton, SIGNAL(clicked()), this, SLOT(hideAllColumns()));
}

/** Load settings and sets member widgets values.
  * \sa Settings saveSettings()
  */
void FileListGroupBox::loadSettings() {
    int hex = Settings::instance()->treeWidget.columns;
    nameCheckBox->setChecked (hex & NameColumn);
    extCheckBox->setChecked  (hex & ExtColumn);
    pathCheckBox->setChecked (hex & PathColumn);
    imageSizeCheckBox->setChecked(hex & ImageSizeColumn);
    fileSizeCheckBox->setChecked (hex & FileSizeColumn);
    statusCheckBox->setChecked   (hex & StatusColumn);
}

/** Saves settings basing member widgets values.
  * \sa Settings loadSettings()
  */
void FileListGroupBox::saveSettings() {
    int hex = 0;
    if (nameCheckBox->isChecked())
        hex |= NameColumn;
    if (extCheckBox->isChecked())
        hex |= ExtColumn;
    if (pathCheckBox->isChecked())
        hex |= PathColumn;
    if (imageSizeCheckBox->isChecked())
        hex |= ImageSizeColumn;
    if (fileSizeCheckBox->isChecked())
        hex |= FileSizeColumn;
    if (statusCheckBox->isChecked())
        hex |= StatusColumn;
    Settings::instance()->treeWidget.columns = hex;
}

/** Returns true if at least 1 column check box is checked, otherwise returns
  * false.
  */
bool FileListGroupBox::isColumnChecked() {
    return bool(visibleColumnsCount > 0);
}

/** Counts how much check boxes are checked and enables or disables
  * \em "Show all" and \em "Hide all" push buttons.
  * \sa enableColumnButtons()
  */
void FileListGroupBox::columnToggled(bool checked) {
    if (checked)
        visibleColumnsCount++;
    else
        visibleColumnsCount--;
    if (visibleColumnsCount == checkBoxList.count()) // all columns checked
        enableColumnButtons(false, true);
    else if (visibleColumnsCount == 0) // no column checked
        enableColumnButtons(true, false);
    else
        enableColumnButtons();
}

/** Checks all columns check boxes.
  * \sa hideAllColumns()
  */
void FileListGroupBox::showAllColumns() {
    foreach (QCheckBox *checkBox, checkBoxList)
        checkBox->setChecked(true);
}

/** Unchecks all columns check boxes.
  * \sa showAllColumns()
  */
void FileListGroupBox::hideAllColumns() {
    foreach (QCheckBox *checkBox, checkBoxList)
        checkBox->setChecked(false);
}
