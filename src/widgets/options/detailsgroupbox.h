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
 * Program URL: http://sir.projet-libre.org/
 */

#ifndef DETAILSGROUPBOX_H
#define DETAILSGROUPBOX_H

#include "ui_detailsgroupbox.h"
#include "abstractoptionsgroupbox.h"

//! Details group box class used in OptionsDialog dialog.
class DetailsGroupBox : public AbstractOptionsGroupBox, public Ui::DetailsGroupBox  {
    Q_OBJECT

public:
    explicit DetailsGroupBox(QWidget *parent = 0);
    void loadSettings();
    void saveSettings();

private:
    // fields
    QList<QCheckBox*> exifCheckBoxes;
    int exifSelectedFields;
    QList<QCheckBox*> iptcCheckBoxes;
    int iptcSelectedFields;
    // methods
    inline void enableFieldButtons(bool enableShow = true, bool enableHide = true);

private slots:
    void exifFieldToggled(bool checked);
    void iptcFieldToggled(bool checked);
    void showAllFields();
    void hideAllFields();
    void tabChanged(int currentTab);
};

/** Enables or disables \em "Show all" and \em "Hide all" fields push buttons
  * depending on parameters values.
  * \param enableShow Enables or disables the \em "Show all" showPushButton push button
  * \param enableHide Enables or disables the \em "Hide all" hidePushButton push button
  */
void DetailsGroupBox::enableFieldButtons(bool enableShow, bool enableHide) {
    showPushButton->setEnabled(enableShow);
    hidePushButton->setEnabled(enableHide);
}

#endif // DETAILSGROUPBOX_H
