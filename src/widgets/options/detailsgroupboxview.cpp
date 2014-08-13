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

#include "detailsgroupboxview.h"
#include "detailsgroupboxcontroller.h"
#include "optionsenums.h"

DetailsGroupBoxView::DetailsGroupBoxView(QWidget *parent) : AbstractOptionsGroupBox(parent) {
    setupUi(this);

    // create connections
    // push buttons
    connect(showPushButton, SIGNAL(clicked()), this, SLOT(showPushButtonClicked()));
    connect(hidePushButton, SIGNAL(clicked()), this, SLOT(hidePushButtonClicked()));
    // tabs
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}

void DetailsGroupBoxView::loadSettings() {
    controller->loadSettings();
}

void DetailsGroupBoxView::saveSettings() {
    controller->saveSettings();
}

void DetailsGroupBoxView::setController(DetailsGroupBoxController *controller) {
    this->controller = controller;
}

/** Enables or disables \em "Show all" and \em "Hide all" fields push buttons
  * depending on parameters values.
  * \param enableShow Enables or disables the \em "Show all" showPushButton push button
  * \param enableHide Enables or disables the \em "Hide all" hidePushButton push button
  */
void DetailsGroupBoxView::enableFieldButtons(bool enableShow, bool enableHide) {
    showPushButton->setEnabled(enableShow);
    hidePushButton->setEnabled(enableHide);
}

void DetailsGroupBoxView::checkExifImageCheckBoxes(int hash) {
    exifVersionCheckBox->setChecked(hash & DetailsOptions::ExifVersion);
    exifSoftCheckBox->setChecked(hash & DetailsOptions::ProcessingSoftware);
    exifOrientationCheckBox->setChecked(hash & DetailsOptions::Orientation);
    exifGeneratedTimeCheckBox->setChecked(hash & DetailsOptions::GeneratedDateAndTime);
    exifDigitizedTimeCheckBox->setChecked(hash & DetailsOptions::DigitizedDateAndTime);
}

void DetailsGroupBoxView::checkExifPhotoCheckBoxes(int hash) {
    exifFocalLengthCheckBox->setChecked(hash & DetailsOptions::FocalLenght);
    exifApertureCheckBox->setChecked(hash & DetailsOptions::Aperture);
    exifIsoCheckBox->setChecked(hash & DetailsOptions::IsoSpeed);
    exifShutterSpeedCheckBox->setChecked(hash & DetailsOptions::ShutterSpeed);
    exifExpTimeCheckBox->setChecked(hash & DetailsOptions::ExposureTime);
    exifExpBiasCheckBox->setChecked(hash & DetailsOptions::ExposureBias);
    exifExpProgramCheckBox->setChecked(hash & DetailsOptions::ExposureProgram);
    exifMeteringCheckBox->setChecked(hash & DetailsOptions::LightMeteringMode);
    exifFlashCheckBox->setChecked(hash & DetailsOptions::FlashMode);
}

void DetailsGroupBoxView::checkExifCameraCheckBoxes(int hash) {
    exifManufacturerCheckBox->setChecked(hash & DetailsOptions::Manufacturer);
    exifModelCheckBox->setChecked(hash & DetailsOptions::Model);
}

void DetailsGroupBoxView::checkExifAuthorCheckBoxes(int hash) {
    exifArtistCheckBox_D->setChecked(hash & DetailsOptions::Artist);
    exifCopyrightCheckBox_D->setChecked(hash & DetailsOptions::Copyright);
    exifUserCommentCheckBox_D->setChecked(hash & DetailsOptions::UserComment);
}

void DetailsGroupBoxView::checkIptcCheckBoxes(int hash) {
    iptcVersionCheckBox->setChecked(hash & DetailsOptions::ModelVersion);
    iptcBylineCheckBox->setChecked(hash & DetailsOptions::Byline);
    iptcCopyrightCheckBox->setChecked(hash & DetailsOptions::CopyrightIptc);
    iptcObjectNameCheckBox->setChecked(hash & DetailsOptions::ObjectName);
    iptcKeywordsCheckBox->setChecked(hash & DetailsOptions::Keywords);
    iptcCaptionCheckBox->setChecked(hash & DetailsOptions::Caption);
    iptcCountryNameCheckBox->setChecked(hash & DetailsOptions::CountryName);
    iptcCityCheckBox->setChecked(hash & DetailsOptions::City);
    iptcEditStatusCheckBox->setChecked(hash & DetailsOptions::EditStatus);
    iptcCreatedDateCheckBox->setChecked(hash & DetailsOptions::DateCreated);
    iptcCreatedTimeCheckBox->setChecked(hash & DetailsOptions::TimeCreated);
    iptcDigitizedDateCheckBox->setChecked(hash & DetailsOptions::DigitizedDate);
    iptcDigitizedTimeCheckBox->setChecked(hash & DetailsOptions::DigitizedTime);
}

int DetailsGroupBoxView::getExifImageCheckBoxesHash() {
    int hash = 0;

    if (exifVersionCheckBox->isChecked())
        hash |= DetailsOptions::ExifVersion;

    if (exifSoftCheckBox->isChecked())
        hash |= DetailsOptions::ProcessingSoftware;

    if (exifOrientationCheckBox->isChecked())
        hash |= DetailsOptions::Orientation;

    if (exifGeneratedTimeCheckBox->isChecked())
        hash |= DetailsOptions::GeneratedDateAndTime;

    if (exifDigitizedTimeCheckBox->isChecked())
        hash |= DetailsOptions::DigitizedDateAndTime;

    return hash;
}

int DetailsGroupBoxView::getExifPhotoCheckBoxesHash() {
    int hash = 0;

    if (exifFocalLengthCheckBox->isChecked())
        hash |= DetailsOptions::FocalLenght;

    if (exifApertureCheckBox->isChecked())
        hash |= DetailsOptions::Aperture;

    if (exifExpTimeCheckBox->isChecked())
        hash |= DetailsOptions::ExposureTime;

    if (exifShutterSpeedCheckBox->isChecked())
        hash |= DetailsOptions::ShutterSpeed;

    if (exifExpBiasCheckBox->isChecked())
        hash |= DetailsOptions::ExposureBias;

    if (exifIsoCheckBox->isChecked())
        hash |= DetailsOptions::IsoSpeed;

    if (exifExpProgramCheckBox->isChecked())
        hash |= DetailsOptions::ExposureProgram;

    if (exifMeteringCheckBox->isChecked())
        hash |= DetailsOptions::LightMeteringMode;

    if (exifFlashCheckBox->isChecked())
        hash |= DetailsOptions::FlashMode;

    return hash;
}

int DetailsGroupBoxView::getExifCameraCheckBoxesHash() {
    int hash = 0;

    if (exifManufacturerCheckBox->isChecked())
        hash |= DetailsOptions::Manufacturer;

    if (exifModelCheckBox->isChecked())
        hash |= DetailsOptions::Model;

    return hash;
}

int DetailsGroupBoxView::getExifAuthorCheckBoxesHash() {
    int hash = 0;

    if (exifArtistCheckBox_D->isChecked())
        hash |= DetailsOptions::Artist;

    if (exifCopyrightCheckBox_D->isChecked())
        hash |= DetailsOptions::Copyright;

    if (exifUserCommentCheckBox_D->isChecked())
        hash |= DetailsOptions::UserComment;

    return hash;
}

int DetailsGroupBoxView::getIptcCheckBoxesHash() {
    int hash = 0;

    if (iptcVersionCheckBox->isChecked())
        hash |= DetailsOptions::ModelVersion;

    if (iptcBylineCheckBox->isChecked())
        hash |= DetailsOptions::Byline;

    if (iptcCopyrightCheckBox->isChecked())
        hash |= DetailsOptions::CopyrightIptc;

    if (iptcObjectNameCheckBox->isChecked())
        hash |= DetailsOptions::ObjectName;

    if (iptcKeywordsCheckBox->isChecked())
        hash |= DetailsOptions::Keywords;

    if (iptcCaptionCheckBox->isChecked())
        hash |= DetailsOptions::Caption;

    if (iptcCountryNameCheckBox->isChecked())
        hash |= DetailsOptions::CountryName;

    if (iptcCityCheckBox->isChecked())
        hash |= DetailsOptions::City;

    if (iptcEditStatusCheckBox->isChecked())
        hash |= DetailsOptions::EditStatus;

    if (iptcCreatedDateCheckBox->isChecked())
        hash |= DetailsOptions::DateCreated;

    if (iptcCreatedTimeCheckBox->isChecked())
        hash |= DetailsOptions::TimeCreated;

    if (iptcDigitizedDateCheckBox->isChecked())
        hash |= DetailsOptions::DigitizedDate;

    if (iptcDigitizedTimeCheckBox->isChecked())
        hash |= DetailsOptions::DigitizedTime;

    return hash;
}

void DetailsGroupBoxView::exifFieldToggled(bool checked) {
    controller->checkExifFields(checked);
}

void DetailsGroupBoxView::iptcFieldToggled(bool checked) {
    controller->checkIptcFields(checked);
}

void DetailsGroupBoxView::showPushButtonClicked() {
    controller->showAllFields();
}

void DetailsGroupBoxView::hidePushButtonClicked() {
    controller->hideAllFields();
}

void DetailsGroupBoxView::tabChanged(int currentTab) {
    controller->setCurrentTab(currentTab);
}
