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

#ifndef DETAILSGROUPBOXVIEW_H
#define DETAILSGROUPBOXVIEW_H

#include "ui_detailsgroupbox.h"
#include "abstractoptionsgroupbox.h"

class DetailsGroupBoxController;

//! Details group box class used in OptionsDialog dialog.
class DetailsGroupBoxView : public AbstractOptionsGroupBox, public Ui::DetailsGroupBox  {
    Q_OBJECT

public:
    explicit DetailsGroupBoxView(QWidget *parent = 0);
    void loadSettings();
    void saveSettings();
    void setController(DetailsGroupBoxController* controller);
    void enableFieldButtons(bool enableShow = true, bool enableHide = true);
    void checkExifImageCheckBoxes(int hash);
    void checkExifPhotoCheckBoxes(int hash);
    void checkExifCameraCheckBoxes(int hash);
    void checkExifAuthorCheckBoxes(int hash);
    void checkIptcCheckBoxes(int hash);
    int getExifImageCheckBoxesHash();
    int getExifPhotoCheckBoxesHash();
    int getExifCameraCheckBoxesHash();
    int getExifAuthorCheckBoxesHash();
    int getIptcCheckBoxesHash();

private:
    DetailsGroupBoxController *controller;

private slots:
    void exifFieldToggled(bool checked);
    void iptcFieldToggled(bool checked);
    void showPushButtonClicked();
    void hidePushButtonClicked();
    void tabChanged(int currentTab);
};

#endif // DETAILSGROUPBOXVIEW_H
