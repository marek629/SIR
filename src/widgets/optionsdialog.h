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

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

class QSplitter;
class QListWidget;
class QScrollArea;
class QGroupBox;
class QDialogButtonBox;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;
class GeneralGroupBox;
#ifdef SIR_METADATA_SUPPORT
class DetailsGroupBox;
class MetadataGroupBox;
#endif // SIR_METADATA_SUPPORT
class SelectionGroupBox;
class RawGroupBox;

//! Settings wizard window.
class OptionsDialog : public QDialog {
    Q_OBJECT

public:
    OptionsDialog( QWidget * parent = 0, Qt::WFlags f = 0);
    ~OptionsDialog();

private:
    // fields
    QGroupBox** groupBoxes;
    quint8 currentListItem;
    // basic UI fields
    QVBoxLayout *verticalLayout;
    QListWidget *listWidget;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QDialogButtonBox *buttonBox;
    QPushButton *okButton;
    QPushButton *cancelButton;
    // group boxes
    GeneralGroupBox *generalGroupBox;
#ifdef SIR_METADATA_SUPPORT
    DetailsGroupBox *detailsGroupBox;
    MetadataGroupBox *metadataGroupBox;
#endif //SIR_METADATA_SUPPORT
    SelectionGroupBox *selectionGroupBox;
    RawGroupBox *rawGroupBox;
    // methods
    void createConnections();
    void setupUi();

private slots:
    void saveSettings();
    void categoryChanged(int current);
};
#endif
