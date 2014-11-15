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

#include "widgets/options/DetailsGroupBoxController.hpp"
#include "widgets/options/DetailsGroupBoxView.hpp"

DetailsGroupBoxController::DetailsGroupBoxController(
        Settings::DetailsGroup *model, DetailsGroupBoxView *view, QObject *parent)
    : AbstractOptionsController(parent) {
    this->model = model;
    this->view = view;
    this->view->setController(this);

    // setup check box lists
    exifCheckBoxes = this->view->exifTab->findChildren<QCheckBox*>();
    iptcCheckBoxes = this->view->iptcTab->findChildren<QCheckBox*>();
    exifSelectedFields = 0;
    iptcSelectedFields = 0;

    // create connections
    // check boxes
    foreach (QCheckBox *checkBox, exifCheckBoxes)
        connect(checkBox, SIGNAL(toggled(bool)), view, SLOT(exifFieldToggled(bool)));
    foreach (QCheckBox *checkBox, iptcCheckBoxes)
        connect(checkBox, SIGNAL(toggled(bool)), view, SLOT(iptcFieldToggled(bool)));
}

void DetailsGroupBoxController::loadSettings() {
    loadExifSettings();
    loadIptcSettings();
}

void DetailsGroupBoxController::saveSettings() {
    saveExifSettings();
    saveIptcSettings();
}

void DetailsGroupBoxController::checkExifFields(bool checked) {
    if (checked)
        exifSelectedFields++;
    else
        exifSelectedFields--;

    if (exifSelectedFields == exifCheckBoxes.count()) // all columns checked
        view->enableFieldButtons(false, true);
    else if (exifSelectedFields == 0) // no column checked
        view->enableFieldButtons(true, false);
    else
        view->enableFieldButtons();
}

void DetailsGroupBoxController::checkIptcFields(bool checked) {
    if (checked)
        iptcSelectedFields++;
    else
        iptcSelectedFields--;

    if (iptcSelectedFields == iptcCheckBoxes.count()) // all columns checked
        view->enableFieldButtons(false, true);
    else if (iptcSelectedFields == 0) // no column checked
        view->enableFieldButtons(true, false);
    else
        view->enableFieldButtons();
}

void DetailsGroupBoxController::setCurrentTab(int tabId) {
    switch (tabId) {
    case Exif_tab:
        if (exifSelectedFields == exifCheckBoxes.count())
            view->enableFieldButtons(false, true);
        else if (exifSelectedFields == 0)
            view->enableFieldButtons(true, false);
        else
            view->enableFieldButtons();
        break;
    case IPTC_tab:
        if (iptcSelectedFields == iptcCheckBoxes.count())
            view->enableFieldButtons(false, true);
        else if (iptcSelectedFields == 0)
            view->enableFieldButtons(true, false);
        else
            view->enableFieldButtons();
        break;
    default:
        view->enableFieldButtons(false, false);
        break;
    }
}

/** Checks all fields in current metadata tab.
  * \sa hideAllFields()
  */
void DetailsGroupBoxController::showAllFields() {
    if (view->tabWidget->currentWidget() == view->exifTab)
        foreach (QCheckBox *checkBox, exifCheckBoxes)
            checkBox->setChecked(true);
    else
        foreach (QCheckBox *checkBox, iptcCheckBoxes)
            checkBox->setChecked(true);
}

/** Unchecks all fields in current metadata tab.
  * \sa showAllFields()
  */
void DetailsGroupBoxController::hideAllFields() {
    if (view->tabWidget->currentWidget() == view->exifTab)
        foreach (QCheckBox *checkBox, exifCheckBoxes)
            checkBox->setChecked(false);
    else
        foreach (QCheckBox *checkBox, iptcCheckBoxes)
            checkBox->setChecked(false);
}

void DetailsGroupBoxController::loadExifSettings() {
    int hex;

    hex = model->exifImage;
    view->checkExifImageCheckBoxes(hex);

    hex = model->exifPhoto;
    view->checkExifPhotoCheckBoxes(hex);

    hex = model->exifCamera;
    view->checkExifCameraCheckBoxes(hex);

    hex = model->exifAuthor;
    view->checkExifAuthorCheckBoxes(hex);
}

void DetailsGroupBoxController::loadIptcSettings() {
    int hex = model->iptc;
    view->checkIptcCheckBoxes(hex);
}

void DetailsGroupBoxController::saveExifSettings() {
    model->exifImage = view->getExifImageCheckBoxesHash();
    model->exifPhoto = view->getExifPhotoCheckBoxesHash();
    model->exifCamera = view->getExifCameraCheckBoxesHash();
    model->exifAuthor = view->getExifAuthorCheckBoxesHash();
}

void DetailsGroupBoxController::saveIptcSettings() {
    model->iptc = view->getIptcCheckBoxesHash();
}
