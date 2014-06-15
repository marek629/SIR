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

#ifndef FILELISTGROUPBOX_H
#define FILELISTGROUPBOX_H

#include "ui_filelistgroupbox.h"
#include "abstractoptionsgroupbox.h"

//! File list group box class used in OptionsDialog dialog.
class FileListGroupBox : public AbstractOptionsGroupBox, private Ui::FileListGroupBox {
    Q_OBJECT

public:
    explicit FileListGroupBox(QWidget *parent = 0);
    void loadSettings();
    void saveSettings();
    bool isColumnChecked();

private:
    // fields
    QList<QCheckBox*> checkBoxList;
    int visibleColumnsCount;
    // methods
    inline void enableColumnButtons(bool enableShow = true, bool enableHide = true);

private slots:
    void columnToggled(bool checked);
    void showAllColumns();
    void hideAllColumns();
};

/** Enables or disables \em "Show all" and \em "Hide all" columns push buttons
  * depending on parameters values.
  * \param enableShow Enables or disables the \em "Show all" showPushButton push button
  * \param enableHide Enables or disables the \em "Hide all" hidePushButton push button
  */
void FileListGroupBox::enableColumnButtons(bool enableShow, bool enableHide) {
    showPushButton->setEnabled(enableShow);
    hidePushButton->setEnabled(enableHide);
}

#endif // FILELISTGROUPBOX_H
