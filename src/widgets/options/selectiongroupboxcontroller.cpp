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

#include "selectiongroupboxcontroller.h"
#include "settings.h"
#include "selectiongroupboxview.h"

SelectionGroupBoxController::SelectionGroupBoxController(
        Settings *model, SelectionGroupBoxView *view, QObject *parent)
    : AbstractOptionsController(parent) {
    this->model = model;
    this->view = view;
    this->view->setController(this);
}

void SelectionGroupBoxController::loadSettings() {
    view->clearSelectionCheckBox->setChecked(model->selection.clearSelection);
    view->importSubdirsCheckBox->setChecked(model->selection.subdirs);
    view->selectImportedCheckBox->setChecked(model->selection.selectImported);
    view->sFileSizeLineEdit->setText(model->selection.fileSizeSymbol);
    view->sImgWidthLineEdit->setText(model->selection.imageWidthSymbol);
    view->sImgHeightLineEdit->setText(model->selection.imageHeightSymbol);
}

void SelectionGroupBoxController::saveSettings() {
    model->selection.clearSelection = view->clearSelectionCheckBox->isChecked();
    model->selection.subdirs = view->importSubdirsCheckBox->isChecked();
    model->selection.selectImported = view->selectImportedCheckBox->isChecked();
    model->selection.fileSizeSymbol = view->sFileSizeLineEdit->text();
    model->selection.imageWidthSymbol = view->sImgWidthLineEdit->text();
    model->selection.imageHeightSymbol = view->sImgHeightLineEdit->text();
}
