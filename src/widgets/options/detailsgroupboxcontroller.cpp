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

#include "detailsgroupboxcontroller.h"
#include "detailsgroupboxview.h"
#include "optionsenums.h"

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
    int hex;

    // exif
    hex = model->exifImage;
    view->exifVersionCheckBox->setChecked(hex & DetailsOptions::ExifVersion);
    view->exifSoftCheckBox->setChecked(hex & DetailsOptions::ProcessingSoftware);
    view->exifOrientationCheckBox->setChecked(hex & DetailsOptions::Orientation);
    view->exifGeneratedTimeCheckBox->setChecked(hex & DetailsOptions::GeneratedDateAndTime);
    view->exifDigitizedTimeCheckBox->setChecked(hex & DetailsOptions::DigitizedDateAndTime);

    hex = model->exifPhoto;
    view->exifFocalLengthCheckBox->setChecked(hex & DetailsOptions::FocalLenght);
    view->exifApertureCheckBox->setChecked(hex & DetailsOptions::Aperture);
    view->exifIsoCheckBox->setChecked(hex & DetailsOptions::IsoSpeed);
    view->exifShutterSpeedCheckBox->setChecked(hex & DetailsOptions::ShutterSpeed);
    view->exifExpTimeCheckBox->setChecked(hex & DetailsOptions::ExposureTime);
    view->exifExpBiasCheckBox->setChecked(hex & DetailsOptions::ExposureBias);
    view->exifExpProgramCheckBox->setChecked(hex & DetailsOptions::ExposureProgram);
    view->exifMeteringCheckBox->setChecked(hex & DetailsOptions::LightMeteringMode);
    view->exifFlashCheckBox->setChecked(hex & DetailsOptions::FlashMode);

    hex = model->exifCamera;
    view->exifManufacturerCheckBox->setChecked(hex & DetailsOptions::Manufacturer);
    view->exifModelCheckBox->setChecked(hex & DetailsOptions::Model);

    hex = model->exifAuthor;
    view->exifArtistCheckBox_D->setChecked(hex & DetailsOptions::Artist);
    view->exifCopyrightCheckBox_D->setChecked(hex & DetailsOptions::Copyright);
    view->exifUserCommentCheckBox_D->setChecked(hex & DetailsOptions::UserComment);

    // iptc
    hex = model->iptc;
    view->iptcVersionCheckBox->setChecked(hex & DetailsOptions::ModelVersion);
    view->iptcBylineCheckBox->setChecked(hex & DetailsOptions::Byline);
    view->iptcCopyrightCheckBox->setChecked(hex & DetailsOptions::CopyrightIptc);
    view->iptcObjectNameCheckBox->setChecked(hex & DetailsOptions::ObjectName);
    view->iptcKeywordsCheckBox->setChecked(hex & DetailsOptions::Keywords);
    view->iptcCaptionCheckBox->setChecked(hex & DetailsOptions::Caption);
    view->iptcCountryNameCheckBox->setChecked(hex & DetailsOptions::CountryName);
    view->iptcCityCheckBox->setChecked(hex & DetailsOptions::City);
    view->iptcEditStatusCheckBox->setChecked(hex & DetailsOptions::EditStatus);
    view->iptcCreatedDateCheckBox->setChecked(hex & DetailsOptions::DateCreated);
    view->iptcCreatedTimeCheckBox->setChecked(hex & DetailsOptions::TimeCreated);
    view->iptcDigitizedDateCheckBox->setChecked(hex & DetailsOptions::DigitizedDate);
    view->iptcDigitizedTimeCheckBox->setChecked(hex & DetailsOptions::DigitizedTime);
}

void DetailsGroupBoxController::saveSettings() {
    // Exif image
    int hex = 0;
    if (view->exifVersionCheckBox->isChecked())
        hex |= DetailsOptions::ExifVersion;
    if (view->exifSoftCheckBox->isChecked())
        hex |= DetailsOptions::ProcessingSoftware;
    if (view->exifOrientationCheckBox->isChecked())
        hex |= DetailsOptions::Orientation;
    if (view->exifGeneratedTimeCheckBox->isChecked())
        hex |= DetailsOptions::GeneratedDateAndTime;
    if (view->exifDigitizedTimeCheckBox->isChecked())
        hex |= DetailsOptions::DigitizedDateAndTime;
    model->exifImage = hex;

    // Exif photo
    hex = 0;
    if (view->exifFocalLengthCheckBox->isChecked())
        hex |= DetailsOptions::FocalLenght;
    if (view->exifApertureCheckBox->isChecked())
        hex |= DetailsOptions::Aperture;
    if (view->exifExpTimeCheckBox->isChecked())
        hex |= DetailsOptions::ExposureTime;
    if (view->exifShutterSpeedCheckBox->isChecked())
        hex |= DetailsOptions::ShutterSpeed;
    if (view->exifExpBiasCheckBox->isChecked())
        hex |= DetailsOptions::ExposureBias;
    if (view->exifIsoCheckBox->isChecked())
        hex |= DetailsOptions::IsoSpeed;
    if (view->exifExpProgramCheckBox->isChecked())
        hex |= DetailsOptions::ExposureProgram;
    if (view->exifMeteringCheckBox->isChecked())
        hex |= DetailsOptions::LightMeteringMode;
    if (view->exifFlashCheckBox->isChecked())
        hex |= DetailsOptions::FlashMode;
    model->exifPhoto = hex;

    // Exif camera
    hex = 0;
    if (view->exifManufacturerCheckBox->isChecked())
        hex |= DetailsOptions::Manufacturer;
    if (view->exifModelCheckBox->isChecked())
        hex |= DetailsOptions::Model;
    model->exifCamera = hex;

    // Exif author
    hex = 0;
    if (view->exifArtistCheckBox_D->isChecked())
        hex |= DetailsOptions::Artist;
    if (view->exifCopyrightCheckBox_D->isChecked())
        hex |= DetailsOptions::Copyright;
    if (view->exifUserCommentCheckBox_D->isChecked())
        hex |= DetailsOptions::UserComment;
    model->exifAuthor = hex;

    // IPTC
    hex = 0;
    if (view->iptcVersionCheckBox->isChecked())
        hex |= DetailsOptions::ModelVersion;
    if (view->iptcBylineCheckBox->isChecked())
        hex |= DetailsOptions::Byline;
    if (view->iptcCopyrightCheckBox->isChecked())
        hex |= DetailsOptions::CopyrightIptc;
    if (view->iptcObjectNameCheckBox->isChecked())
        hex |= DetailsOptions::ObjectName;
    if (view->iptcKeywordsCheckBox->isChecked())
        hex |= DetailsOptions::Keywords;
    if (view->iptcCaptionCheckBox->isChecked())
        hex |= DetailsOptions::Caption;
    if (view->iptcCountryNameCheckBox->isChecked())
        hex |= DetailsOptions::CountryName;
    if (view->iptcCityCheckBox->isChecked())
        hex |= DetailsOptions::City;
    if (view->iptcEditStatusCheckBox->isChecked())
        hex |= DetailsOptions::EditStatus;
    if (view->iptcCreatedDateCheckBox->isChecked())
        hex |= DetailsOptions::DateCreated;
    if (view->iptcCreatedTimeCheckBox->isChecked())
        hex |= DetailsOptions::TimeCreated;
    if (view->iptcDigitizedDateCheckBox->isChecked())
        hex |= DetailsOptions::DigitizedDate;
    if (view->iptcDigitizedTimeCheckBox->isChecked())
        hex |= DetailsOptions::DigitizedTime;
    model->iptc = hex;
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
    // Exif
    case 0:
        if (exifSelectedFields == exifCheckBoxes.count())
            view->enableFieldButtons(false, true);
        else if (exifSelectedFields == 0)
            view->enableFieldButtons(true, false);
        else
            view->enableFieldButtons();
        break;
    // IPTC
    case 1:
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
