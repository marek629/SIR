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
 * Program URL: http://marek629.github.io/sir/
 */

#include "widgets/options/MetadataGroupBoxController.hpp"
#include "widgets/options/MetadataGroupBoxView.hpp"
#include "widgets/options/CommonOptions.hpp"

MetadataGroupBoxController::MetadataGroupBoxController(
        Settings::MetadataGroup *metadataModel, Settings::ExifGroup *exifModel,
        MetadataGroupBoxView *view, QObject *parent)
    : AbstractOptionsController(parent) {
    this->modelMetadata = metadataModel;
    this->modelExif = exifModel;
    this->view = view;
    this->view->setController(this);
}

void MetadataGroupBoxController::loadSettings() {
    loadMetadataSettings();
    loadExifSettings();
}

void MetadataGroupBoxController::saveSettings() {
    saveMetadataSettings();
    saveExifSettings();
}

/** Enable or disable metadata check boxes depending on \a checked value. */
void MetadataGroupBoxController::enableMetadata(bool checked) {
    if (checked) {
        view->saveMetadataCheckBox->setEnabled(true);
        view->exifArtistCheckBox->setEnabled(true);
        view->exifCopyrightCheckBox->setEnabled(true);
        view->exifUserCommentCheckBox->setEnabled(true);
    }
    else {
        view->saveMetadataCheckBox->setChecked(false);
        view->saveMetadataCheckBox->setEnabled(false);
        view->exifArtistCheckBox->setChecked(false);
        view->exifArtistCheckBox->setEnabled(false);
        view->exifCopyrightCheckBox->setChecked(false);
        view->exifCopyrightCheckBox->setEnabled(false);
        view->exifUserCommentCheckBox->setChecked(false);
        view->exifUserCommentCheckBox->setEnabled(false);
    }
}

/** Enables or disables and (un)check checkboxes resposibled saving metadata
  * basing \a save value.
  */
void MetadataGroupBoxController::saveMetadata(bool save) {
    view->thumbUpdateCheckBox->setChecked(save);
    view->thumbUpdateCheckBox->setEnabled(save);
    view->thumbRotateCheckBox->setEnabled(save);
    view->exifOrientationRadioButton->setEnabled(save);
    view->exifArtistCheckBox->setEnabled(save);
    view->exifCopyrightCheckBox->setEnabled(save);
    view->exifUserCommentCheckBox->setEnabled(save);

    if (save)
        view->exifOrientationRadioButton->setChecked(true);
    else
        view->rotateRadioButton->setChecked(true);
}

/** Unchecks thumbRotateCheckBox if \a update is false, otherwise does nothing. */
void MetadataGroupBoxController::updateThumbnail(bool update) {
    view->thumbRotateCheckBox->setEnabled(update);

    if (!update)
        view->thumbRotateCheckBox->setChecked(false);
}


void MetadataGroupBoxController::loadMetadataSettings() {
    view->metadataCheckBox->setChecked(modelMetadata->enabled);

    if (!view->metadataCheckBox->isChecked()) {
        view->saveMetadataCheckBox->setEnabled(false);
        view->thumbUpdateCheckBox->setEnabled(false);
        view->thumbRotateCheckBox->setEnabled(false);
    }
    else
        view->saveMetadataCheckBox->setChecked(modelMetadata->saveMetadata);

    if (view->saveMetadataCheckBox->isChecked()) {
        view->thumbUpdateCheckBox->setChecked(modelMetadata->updateThumbnail);
        if (view->thumbUpdateCheckBox->isChecked())
            view->thumbRotateCheckBox->setChecked(modelMetadata->rotateThumbnail);
        view->rotateRadioButton->setChecked(modelMetadata->realRotate);
    }
    else {
        view->thumbUpdateCheckBox->setEnabled(false);
        view->thumbRotateCheckBox->setEnabled(false);
        view->rotateRadioButton->setChecked(true);
    }

    if (!view->rotateRadioButton->isChecked())
        view->exifOrientationRadioButton->setChecked(true);
}

void MetadataGroupBoxController::loadExifSettings() {
    bool exifOverwrite;
    HistoryMap exifDefaultMap;
    CommonOptions *common = CommonOptions::instance();

    exifOverwrite = modelExif->artistOverwrite;
    view->exifArtistCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.insert(tr("Camera owner: ; Photographer: "),false);
    view->exifArtistComboBox->importHistory(modelExif->artistMap,
                                            modelExif->artistList,
                                            common->maxHistoryCount());
    view->exifArtistComboBox->setEnabled(exifOverwrite);

    exifOverwrite = modelExif->copyrightOverwrite;
    view->exifCopyrightCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.clear();
    exifDefaultMap.insert(tr("Copyright owner"),false);
    view->exifCopyrightComboBox->importHistory(modelExif->copyrightMap,
                                            modelExif->copyrightList,
                                            common->maxHistoryCount());
    view->exifCopyrightComboBox->setEnabled(exifOverwrite);

    exifOverwrite = modelExif->userCommentOverwrite;
    view->exifUserCommentCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.clear();
    exifDefaultMap.insert(
                tr("This picture was edited with Simple Image Resizer"),false);
    view->exifUserCommentComboBox->importHistory(modelExif->userCommentMap,
                                            modelExif->userCommentList,
                                            common->maxHistoryCount());
    view->exifUserCommentComboBox->setEnabled(exifOverwrite);
}

void MetadataGroupBoxController::saveMetadataSettings() {
    modelMetadata->enabled = view->metadataCheckBox->isChecked();
    modelMetadata->saveMetadata = view->saveMetadataCheckBox->isChecked();
    modelMetadata->realRotate = view->rotateRadioButton->isChecked();
    modelMetadata->updateThumbnail = view->thumbUpdateCheckBox->isChecked();
    modelMetadata->rotateThumbnail = view->thumbRotateCheckBox->isChecked();
}

void MetadataGroupBoxController::saveExifSettings() {
    HistoryMap  historyMap;
    HistoryList historyList;
    CommonOptions *common = CommonOptions::instance();

    modelExif->artistOverwrite      = view->exifArtistCheckBox->isChecked();

    view->exifArtistComboBox->exportHistory(&historyMap, &historyList,
                                      common->maxHistoryCount());
    modelExif->artistMap            = historyMap;
    modelExif->artistList           = historyList;

    modelExif->copyrightOverwrite   = view->exifCopyrightCheckBox->isChecked();

    view->exifCopyrightComboBox->exportHistory(&historyMap, &historyList,
                                         common->maxHistoryCount());
    modelExif->copyrightMap         = historyMap;
    modelExif->copyrightList        = historyList;

    modelExif->userCommentOverwrite = view->exifUserCommentCheckBox->isChecked();

    view->exifUserCommentComboBox->exportHistory(&historyMap, &historyList,
                                           common->maxHistoryCount());
    modelExif->userCommentMap       = historyMap;
    modelExif->userCommentList      = historyList;
}
