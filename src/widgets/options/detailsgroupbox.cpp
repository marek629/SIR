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

#include "detailsgroupbox.h"
#include "settings.h"
#include "detailsoptions.h"

/** Default constructor.\n
  * Sets UI and loads settings.
  * \sa setupUi() loadSettings()
  */
DetailsGroupBox::DetailsGroupBox(QWidget *parent) : QGroupBox(parent) {
    setupUi(this);
    loadSettings();
}

/** Load settings and sets member widgets values.
  * \sa Settings saveSettings()
  */
void DetailsGroupBox::loadSettings() {
    Settings &s = Settings::instance();
    int hex;
    // exif
    hex = s.details.exifImage;
    exifVersionCheckBox->setChecked(hex & DetailsOptions::ExifVersion);
    exifSoftCheckBox->setChecked(hex & DetailsOptions::ProcessingSoftware);
    exifOrientationCheckBox->setChecked(hex & DetailsOptions::Orientation);
    exifGeneratedTimeCheckBox->setChecked(hex & DetailsOptions::GeneratedDateAndTime);
    exifDigitizedTimeCheckBox->setChecked(hex & DetailsOptions::DigitizedDateAndTime);
    hex = s.details.exifPhoto;
    exifFocalLengthCheckBox->setChecked(hex & DetailsOptions::FocalLenght);
    exifApertureCheckBox->setChecked(hex & DetailsOptions::Aperture);
    exifIsoCheckBox->setChecked(hex & DetailsOptions::IsoSpeed);
    exifShutterSpeedCheckBox->setChecked(hex & DetailsOptions::ShutterSpeed);
    exifExpTimeCheckBox->setChecked(hex & DetailsOptions::ExposureTime);
    exifExpBiasCheckBox->setChecked(hex & DetailsOptions::ExposureBias);
    exifExpProgramCheckBox->setChecked(hex & DetailsOptions::ExposureProgram);
    exifMeteringCheckBox->setChecked(hex & DetailsOptions::LightMeteringMode);
    exifFlashCheckBox->setChecked(hex & DetailsOptions::FlashMode);
    hex = s.details.exifCamera;
    exifManufacturerCheckBox->setChecked(hex & DetailsOptions::Manufacturer);
    exifModelCheckBox->setChecked(hex & DetailsOptions::Model);
    hex = s.details.exifAuthor;
    exifArtistCheckBox_D->setChecked(hex & DetailsOptions::Artist);
    exifCopyrightCheckBox_D->setChecked(hex & DetailsOptions::Copyright);
    exifUserCommentCheckBox_D->setChecked(hex & DetailsOptions::UserComment);
    // iptc
    hex = s.details.iptc;
    iptcVersionCheckBox->setChecked(hex & DetailsOptions::ModelVersion);
    iptcBylineCheckBox->setChecked(hex & DetailsOptions::Byline);
    iptcCopyrightCheckBox->setChecked(hex & DetailsOptions::CopyrightIptc);
    iptcObjectNameCheckBox->setChecked(hex & DetailsOptions::ObjectName);
    iptcKeywordsCheckBox->setChecked(hex & DetailsOptions::Keywords);
    iptcCaptionCheckBox->setChecked(hex & DetailsOptions::Caption);
    iptcCountryNameCheckBox->setChecked(hex & DetailsOptions::CountryName);
    iptcCityCheckBox->setChecked(hex & DetailsOptions::City);
    iptcEditStatusCheckBox->setChecked(hex & DetailsOptions::EditStatus);
    iptcCreatedDateCheckBox->setChecked(hex & DetailsOptions::DateCreated);
    iptcCreatedTimeCheckBox->setChecked(hex & DetailsOptions::TimeCreated);
    iptcDigitizedDateCheckBox->setChecked(hex & DetailsOptions::DigitizedDate);
    iptcDigitizedTimeCheckBox->setChecked(hex & DetailsOptions::DigitizedTime);
}

/** Saves settings basing member widgets values.
  * \sa Settings loadSettings()
  */
void DetailsGroupBox::saveSettings() {
    Settings &s = Settings::instance();
    // Exif image
    int hex = 0;
    if (exifVersionCheckBox->isChecked())
        hex |= DetailsOptions::ExifVersion;
    if (exifSoftCheckBox->isChecked())
        hex |= DetailsOptions::ProcessingSoftware;
    if (exifOrientationCheckBox->isChecked())
        hex |= DetailsOptions::Orientation;
    if (exifGeneratedTimeCheckBox->isChecked())
        hex |= DetailsOptions::GeneratedDateAndTime;
    if (exifDigitizedTimeCheckBox->isChecked())
        hex |= DetailsOptions::DigitizedDateAndTime;
    s.details.exifImage     = hex;
    // Exif photo
    hex = 0;
    if (exifFocalLengthCheckBox->isChecked())
        hex |= DetailsOptions::FocalLenght;
    if (exifApertureCheckBox->isChecked())
        hex |= DetailsOptions::Aperture;
    if (exifExpTimeCheckBox->isChecked())
        hex |= DetailsOptions::ExposureTime;
    if (exifShutterSpeedCheckBox->isChecked())
        hex |= DetailsOptions::ShutterSpeed;
    if (exifExpBiasCheckBox->isChecked())
        hex |= DetailsOptions::ExposureBias;
    if (exifIsoCheckBox->isChecked())
        hex |= DetailsOptions::IsoSpeed;
    if (exifExpProgramCheckBox->isChecked())
        hex |= DetailsOptions::ExposureProgram;
    if (exifMeteringCheckBox->isChecked())
        hex |= DetailsOptions::LightMeteringMode;
    if (exifFlashCheckBox->isChecked())
        hex |= DetailsOptions::FlashMode;
    s.details.exifPhoto     = hex;
    // Exif camera
    hex = 0;
    if (exifManufacturerCheckBox->isChecked())
        hex |= DetailsOptions::Manufacturer;
    if (exifModelCheckBox->isChecked())
        hex |= DetailsOptions::Model;
    s.details.exifCamera    = hex;
    // Exif author
    hex = 0;
    if (exifArtistCheckBox_D->isChecked())
        hex |= DetailsOptions::Artist;
    if (exifCopyrightCheckBox_D->isChecked())
        hex |= DetailsOptions::Copyright;
    if (exifUserCommentCheckBox_D->isChecked())
        hex |= DetailsOptions::UserComment;
    s.details.exifAuthor    = hex;
    // IPTC
    hex = 0;
    if (iptcVersionCheckBox->isChecked())
        hex |= DetailsOptions::ModelVersion;
    if (iptcBylineCheckBox->isChecked())
        hex |= DetailsOptions::Byline;
    if (iptcCopyrightCheckBox->isChecked())
        hex |= DetailsOptions::CopyrightIptc;
    if (iptcObjectNameCheckBox->isChecked())
        hex |= DetailsOptions::ObjectName;
    if (iptcKeywordsCheckBox->isChecked())
        hex |= DetailsOptions::Keywords;
    if (iptcCaptionCheckBox->isChecked())
        hex |= DetailsOptions::Caption;
    if (iptcCountryNameCheckBox->isChecked())
        hex |= DetailsOptions::CountryName;
    if (iptcCityCheckBox->isChecked())
        hex |= DetailsOptions::City;
    if (iptcEditStatusCheckBox->isChecked())
        hex |= DetailsOptions::EditStatus;
    if (iptcCreatedDateCheckBox->isChecked())
        hex |= DetailsOptions::DateCreated;
    if (iptcCreatedTimeCheckBox->isChecked())
        hex |= DetailsOptions::TimeCreated;
    if (iptcDigitizedDateCheckBox->isChecked())
        hex |= DetailsOptions::DigitizedDate;
    if (iptcDigitizedTimeCheckBox->isChecked())
        hex |= DetailsOptions::DigitizedTime;
    s.details.iptc          = hex;
}
