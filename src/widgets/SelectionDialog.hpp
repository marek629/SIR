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
 * Program URL: http://marek629.github.io/SIR/
 */

#ifndef SELECTIONDIALOG_H
#define SELECTIONDIALOG_H

#include <QDialog>
#include "ui_DirWidget.h"
#ifdef SIR_METADATA_SUPPORT
#include "widgets/selection/AnyMetadataGroupBox.hpp"
#include "ui_ExifGroupBox.h"
#include "ui_IptcGroupBox.h"
#endif // SIR_METADATA_SUPPORT
#include "Selection.hpp"

class QScrollArea;
class QDialogButtonBox;
#ifndef SIR_METADATA_SUPPORT
class HistoryComboBox;
#endif // SIR_METADATA_SUPPORT

//! Selection conditions dialog class.
class SelectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit SelectionDialog(SelectionParams *params, bool getDirPath,
                             QWidget *parent = 0, Qt::WindowFlags f = 0);

public slots:
    void accept();
    void browseDir();

private:
    // fields
    SelectionParams *params;
    ConvertDialog *convertDialog;
    Ui::DirWidget *uiDirWidget;
    QWidget *dirWidget;
#ifdef SIR_METADATA_SUPPORT
    AnyMetadataGroupBox *anyMetadataGroupBox;
    Ui::ExifGroupBox uiExif;
    QGroupBox *exifGroupBox;
    Ui::IptcGroupBox uiIptc;
    QGroupBox *iptcGroupBox;
#endif // SIR_METADATA_SUPPORT
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_4;
    QCheckBox *clearSelectionCheckBox;
    QLabel *fileNameLabel;
    HistoryComboBox *fileNameComboBox;
    HistoryComboBox *fileSizeComboBox;
    QLabel *fileSizeLabel;
    QLabel *imageSizeLabel;
    HistoryComboBox *imageSizeComboBox;
    QDialogButtonBox *buttonBox;
    // methods
    void loadSettings();
    void saveSettings();
    void setupUi();
    void setupTabOrder();
};
#endif // SELECTIONDIALOG_H
