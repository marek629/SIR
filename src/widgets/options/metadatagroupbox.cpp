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

#include "metadatagroupbox.h"
#include "settings.h"

/** Default constructor.\n
  * Sets UI and creates connections.
  * \sa setupUi()
  */
MetadataGroupBox::MetadataGroupBox(QWidget *parent) : AbstractOptionsGroupBox(parent) {
    setupUi(this);

    // validate only ASCII characters
    validator = new QRegExpValidator(QRegExp("^[\\00-\\0177]+"),this);
    exifArtistComboBox->setValidator(validator);
    exifCopyrightComboBox->setValidator(validator);

    connect(metadataCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(enableMetadata(bool)));
    connect(saveMetadataCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(saveMetadata(bool)));
    connect(thumbUpdateCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(updateThumbnail(bool)));
    connect(exifArtistCheckBox, SIGNAL(toggled(bool)),
            exifArtistComboBox, SLOT(setEnabled(bool)));
    connect(exifCopyrightCheckBox, SIGNAL(toggled(bool)),
            exifCopyrightComboBox, SLOT(setEnabled(bool)));
    connect(exifUserCommentCheckBox, SIGNAL(toggled(bool)),
            exifUserCommentComboBox, SLOT(setEnabled(bool)));
}

/** Destructor. */
MetadataGroupBox::~MetadataGroupBox() {
    delete validator;
}

/** Load settings and sets member widgets values.
  * \sa Settings saveSettings()
  */
void MetadataGroupBox::loadSettings() {
    Settings &s = Settings::instance();
    // general metadata part
    metadataCheckBox->setChecked(           s.metadata.enabled);
    if (!metadataCheckBox->isChecked()) {
        saveMetadataCheckBox->setEnabled(false);
        thumbUpdateCheckBox->setEnabled(false);
        thumbRotateCheckBox->setEnabled(false);
    }
    else
        saveMetadataCheckBox->setChecked(   s.metadata.saveMetadata);
    if (saveMetadataCheckBox->isChecked()) {
        thumbUpdateCheckBox->setChecked(    s.metadata.updateThumbnail);
        if (thumbUpdateCheckBox->isChecked())
            thumbRotateCheckBox->setChecked(s.metadata.rotateThumbnail);
        rotateRadioButton->setChecked(      s.metadata.realRotate);
    }
    else {
        thumbUpdateCheckBox->setEnabled(false);
        thumbRotateCheckBox->setEnabled(false);
        rotateRadioButton->setChecked(true);
    }
    if (!rotateRadioButton->isChecked())
        exifOrientationRadioButton->setChecked(true);
    // Exif part
    bool exifOverwrite;
    HistoryMap exifDefaultMap;
    exifOverwrite =                         s.exif.artistOverwrite;
    exifArtistCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.insert(tr("Camera owner: ; Photographer: "),false);
    exifArtistComboBox->importHistory(      s.exif.artistMap,
                                            s.exif.artistList, common.maxHistoryCount());
    exifArtistComboBox->setEnabled(exifOverwrite);
    exifOverwrite =                         s.exif.copyrightOverwrite;
    exifCopyrightCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.clear();
    exifDefaultMap.insert(tr("Copyright owner"),false);
    exifCopyrightComboBox->importHistory(   s.exif.copyrightMap,
                                            s.exif.copyrightList, common.maxHistoryCount());
    exifCopyrightComboBox->setEnabled(exifOverwrite);

    exifOverwrite =                         s.exif.userCommentOverwrite;
    exifUserCommentCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.clear();
    exifDefaultMap.insert(
                tr("This picture was edited with Simple Image Resizer"),false);
    exifUserCommentComboBox->importHistory( s.exif.userCommentMap,
                                            s.exif.userCommentList, common.maxHistoryCount());
    exifUserCommentComboBox->setEnabled(exifOverwrite);
}

/** Saves settings basing member widgets values.
  * \sa Settings loadSettings()
  */
void MetadataGroupBox::saveSettings() {
    Settings &s = Settings::instance();
    // general part
    s.metadata.enabled          = metadataCheckBox->isChecked();
    s.metadata.saveMetadata     = saveMetadataCheckBox->isChecked();
    s.metadata.realRotate       = rotateRadioButton->isChecked();
    s.metadata.updateThumbnail  = thumbUpdateCheckBox->isChecked();
    s.metadata.rotateThumbnail  = thumbRotateCheckBox->isChecked();
    // Exif part
    HistoryMap  historyMap;
    HistoryList historyList;
    s.exif.artistOverwrite      = exifArtistCheckBox->isChecked();
    exifArtistComboBox->exportHistory(&historyMap,&historyList,common.maxHistoryCount());
    s.exif.artistMap            = historyMap;
    s.exif.artistList           = historyList;
    s.exif.copyrightOverwrite   = exifCopyrightCheckBox->isChecked();
    exifCopyrightComboBox->exportHistory(&historyMap,&historyList,common.maxHistoryCount());
    s.exif.copyrightMap         = historyMap;
    s.exif.copyrightList        = historyList;
    s.exif.userCommentOverwrite = exifUserCommentCheckBox->isChecked();
    exifUserCommentComboBox->exportHistory(&historyMap,&historyList,common.maxHistoryCount());
    s.exif.userCommentMap       = historyMap;
    s.exif.userCommentList      = historyList;
}

/** Enable or disable metadata check boxes depending on \a checked value. */
void MetadataGroupBox::enableMetadata(bool checked) {
    if (checked) {
        saveMetadataCheckBox->setEnabled(true);
        exifArtistCheckBox->setEnabled(true);
        exifCopyrightCheckBox->setEnabled(true);
        exifUserCommentCheckBox->setEnabled(true);
    }
    else {
        saveMetadataCheckBox->setChecked(false);
        saveMetadataCheckBox->setEnabled(false);
        exifArtistCheckBox->setChecked(false);
        exifArtistCheckBox->setEnabled(false);
        exifCopyrightCheckBox->setChecked(false);
        exifCopyrightCheckBox->setEnabled(false);
        exifUserCommentCheckBox->setChecked(false);
        exifUserCommentCheckBox->setEnabled(false);
    }
}

/** \em "Save metadata" checkbox slot. Enables or disables and (un)check
  * checkboxes resposibled saving metadata basing \a save value.
  */
void MetadataGroupBox::saveMetadata(bool save) {
    thumbUpdateCheckBox->setChecked(save);
    thumbUpdateCheckBox->setEnabled(save);
    thumbRotateCheckBox->setEnabled(save);
    exifOrientationRadioButton->setEnabled(save);
    exifArtistCheckBox->setEnabled(save);
    exifCopyrightCheckBox->setEnabled(save);
    exifUserCommentCheckBox->setEnabled(save);
    if (save)
        exifOrientationRadioButton->setChecked(true);
    else {
        rotateRadioButton->setChecked(true);
    }
}

/** Unchecks thumbRotateCheckBox if \a update is false, otherwise does nothing. */
void MetadataGroupBox::updateThumbnail(bool update) {
    if (!update)
        thumbRotateCheckBox->setChecked(false);
}
