/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
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
class QDialogButtonBox;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;
class AbstractOptionsGroupBox;
class GeneralGroupBox;
class FileListGroupBox;
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
    OptionsDialog( QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~OptionsDialog();

private:
    // fields
    AbstractOptionsGroupBox** groupBoxes;
    quint8 currentListItem;
#ifdef SIR_METADATA_SUPPORT
    static const quint8 categoriesCount = 6;
#else
    static const quint8 categoriesCount = 4;
#endif // SIR_METADATA_SUPPORT
    // basic UI fields
    QVBoxLayout *verticalLayout;
    QListWidget *listWidget;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QDialogButtonBox *buttonBox;
    // group boxes
    GeneralGroupBox *generalGroupBox;
    FileListGroupBox *fileListGroupBox;
#ifdef SIR_METADATA_SUPPORT
    DetailsGroupBox *detailsGroupBox;
    MetadataGroupBox *metadataGroupBox;
#endif //SIR_METADATA_SUPPORT
    SelectionGroupBox *selectionGroupBox;
    RawGroupBox *rawGroupBox;
    // methods
    void createConnections();
    void setupUi();
    void loadSettings();
    void saveSettings();
    QListWidget *createListWidget();
    void insertItems(QListWidget *listWidget);

private slots:
    void okButtonClicked();
    void categoryChanged(int current);
};
#endif
