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

#include "widgets/options/FileListGroupBoxController.hpp"
#include "widgets/options/FileListGroupBoxView.hpp"
#include "optionsenums.h"

using namespace TreeWidgetOptions;

FileListGroupBoxController::FileListGroupBoxController(
        Settings::TreeWidgetGroup *model,
        FileListGroupBoxView *view,
        QObject *parent)
    : AbstractOptionsController(parent) {
    // setup check box list
    checkBoxList = view->columnsGroupBox->findChildren<QCheckBox*>();
    visibleColumnsCount = 0;

    this->model = model;

    this->view = view;
    this->view->setController(this);

    foreach (QCheckBox *checkBox, checkBoxList)
        connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(updateViewButtons(bool)));
}

void FileListGroupBoxController::loadSettings() {
    int hex = model->columns;

    view->nameCheckBox->setChecked(hex & NameColumn);
    view->extCheckBox->setChecked(hex & ExtColumn);
    view->pathCheckBox->setChecked(hex & PathColumn);

    view->imageSizeCheckBox->setChecked(hex & ImageSizeColumn);
    view->fileSizeCheckBox->setChecked(hex & FileSizeColumn);

    view->statusCheckBox->setChecked(hex & StatusColumn);
}

void FileListGroupBoxController::saveSettings() {
    int hex = 0;

    if (view->nameCheckBox->isChecked())
        hex |= NameColumn;

    if (view->extCheckBox->isChecked())
        hex |= ExtColumn;

    if (view->pathCheckBox->isChecked())
        hex |= PathColumn;

    if (view->imageSizeCheckBox->isChecked())
        hex |= ImageSizeColumn;

    if (view->fileSizeCheckBox->isChecked())
        hex |= FileSizeColumn;

    if (view->statusCheckBox->isChecked())
        hex |= StatusColumn;

    model->columns = hex;
}

/** Enables or disables \em "Show all" and \em "Hide all" columns push buttons
  * depending on parameters values.
  * \param enableShow Enables or disables the \em "Show all" showPushButton push button
  * \param enableHide Enables or disables the \em "Hide all" hidePushButton push button
  */
void FileListGroupBoxController::enableColumnButtons(bool enableShow, bool enableHide) {
    view->showPushButton->setEnabled(enableShow);
    view->hidePushButton->setEnabled(enableHide);
}

/** Counts how much check boxes are checked and enables or disables
  * \em "Show all" and \em "Hide all" push buttons.
  * \sa enableColumnButtons()
  */
void FileListGroupBoxController::updateViewButtons(bool columnToggled) {
    if (columnToggled)
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

void FileListGroupBoxController::checkAllColumns(bool checked) {
    foreach (QCheckBox *checkBox, checkBoxList)
            checkBox->setChecked(checked);
}

bool FileListGroupBoxController::isAnyColumnChecked() const {
    return bool(visibleColumnsCount > 0);
}
