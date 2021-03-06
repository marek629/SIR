/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2016  Marek Jędryka   <jedryka89@gmail.com>
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

#include "widgets/OptionsDialog.hpp"

#include "optionsenums.h"
#include "OptionsGroupBoxManager.hpp"
#include "raw/RawModel.hpp"
#include "widgets/ConvertDialog.hpp"
#include "widgets/options/GeneralGroupBoxController.hpp"
#include "widgets/options/GeneralGroupBoxView.hpp"
#include "widgets/options/FileListGroupBoxController.hpp"
#include "widgets/options/FileListGroupBoxView.hpp"

#ifdef SIR_METADATA_SUPPORT
#include "widgets/options/DetailsGroupBoxController.hpp"
#include "widgets/options/DetailsGroupBoxView.hpp"
#include "widgets/options/MetadataGroupBoxController.hpp"
#include "widgets/options/MetadataGroupBoxView.hpp"
#endif // SIR_METADATA_SUPPORT

#include "widgets/options/SelectionGroupBoxController.hpp"
#include "widgets/options/SelectionGroupBoxView.hpp"
#include "widgets/options/RawGroupBoxView.hpp"

#include <QDialogButtonBox>
#include <QMessageBox>
#include <QListWidget>
#include <QScrollArea>


/** Sets up window, loads settings and creates connections.
  * \sa setupUi() loadSettings() createConnections()
  */
OptionsDialog::OptionsDialog(QWidget * parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi();
    loadSettings();
    createConnections();
}

/** Destructor. */
OptionsDialog::~OptionsDialog()
{
    delete groupBoxManager;

    delete fileListGroupBoxController;
    delete generalGroupBoxController;
#ifdef SIR_METADATA_SUPPORT
    delete metadataGroupBoxController;
    delete detailsGroupBoxController;
#endif // SIR_METADATA_SUPPORT
    delete selectionGroupBoxController;
}

/** Connects signals to slots. */
void OptionsDialog::createConnections()
{
    connect(listWidget, SIGNAL(currentRowChanged(int)),
            groupBoxManager, SLOT(onCategoryChange(int)));

    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(okButtonClicked()));

    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));
}

/** Load settings for the user input wigets.
  * \sa saveSettings()
  */
void OptionsDialog::loadSettings()
{
    groupBoxManager->loadSettings();
}

/** Writes settings based values of the user input wigets.
  * \sa loadSettings() okButtonClicked()
  */
void OptionsDialog::saveSettings()
{
    groupBoxManager->saveSettings();

    // Write file settings
    Settings::instance()->writeSettings();
}

/** Creates and sets up list widget.
  * \return Poiter to created list widget.
  * \sa insertItem()
  */
QListWidget *OptionsDialog::createListWidget() {
    QListWidget *listWidget = new QListWidget(this);

    insertItems(listWidget);

    listWidget->setObjectName(QString::fromUtf8("listWidget"));
    listWidget->setMaximumWidth(120);
    listWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    listWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    listWidget->setFlow(QListView::LeftToRight);
    listWidget->setGridSize(QSize(110, 60));
    listWidget->setViewMode(QListView::IconMode);

    return listWidget;
}

/** Creates intems into \a listWidget. */
void OptionsDialog::insertItems(QListWidget *listWidget) {
    QString imagesDirPath = QCoreApplication::applicationDirPath() +
                            "/../share/sir/images/";

    // General
    QIcon icon = QIcon::fromTheme("preferences-system",
                                  QIcon(imagesDirPath + "options_general.png"));
    QListWidgetItem *listWidgetItem = new QListWidgetItem(listWidget);
    listWidgetItem->setIcon(icon);
    listWidgetItem->setText(tr("General"));
    // File list
    icon = QIcon::fromTheme("preferences-desktop",
                            QIcon(imagesDirPath + "options_list.png"));
    listWidgetItem = new QListWidgetItem(listWidget);
    listWidgetItem->setIcon(icon);
    listWidgetItem->setText(tr("File list"));
#ifdef SIR_METADATA_SUPPORT
    // Metadata
    icon = QIcon::fromTheme("document-properties",
                            QIcon(imagesDirPath + "options_metadata.png"));
    listWidgetItem = new QListWidgetItem(listWidget);
    listWidgetItem->setIcon(icon);
    listWidgetItem->setText(tr("Metadata"));
    // File details
    icon = QIcon::fromTheme("x-office-document",
                            QIcon(imagesDirPath + "options_details.png"));
    listWidgetItem = new QListWidgetItem(listWidget);
    listWidgetItem->setIcon(icon);
    listWidgetItem->setText(tr("File details"));
#endif // SIR_METADATA_SUPPORT
    // Selection
    icon = QIcon::fromTheme("edit-find",
                            QIcon(imagesDirPath + "options_selection.png"));
    listWidgetItem = new QListWidgetItem(listWidget);
    listWidgetItem->setIcon(icon);
    listWidgetItem->setText(tr("Selection"));
    // Raw
    icon = QIcon::fromTheme("emblem-photos",
                            QIcon(imagesDirPath + "options_raw.png"));
    listWidgetItem = new QListWidgetItem(listWidget);
    listWidgetItem->setIcon(icon);
    listWidgetItem->setText(tr("Raw"));
}

void OptionsDialog::createGroupBoxes() {
    Settings *model = Settings::instance();
    generalGroupBox = new GeneralGroupBoxView(scrollAreaWidgetContents);
    generalGroupBoxController = new GeneralGroupBoxController(&(model->settings),
                                                              &(model->size),
                                                              generalGroupBox,
                                                              this);
    fileListGroupBox = new FileListGroupBoxView(scrollAreaWidgetContents);
    fileListGroupBoxController = new FileListGroupBoxController(&(model->treeWidget),
                                                                fileListGroupBox,
                                                                this);

#ifdef SIR_METADATA_SUPPORT
    metadataGroupBox = new MetadataGroupBoxView(scrollAreaWidgetContents);
    metadataGroupBoxController = new MetadataGroupBoxController(&(model->metadata),
                                                                &(model->exif),
                                                                metadataGroupBox,
                                                                this);
    detailsGroupBox = new DetailsGroupBoxView(scrollAreaWidgetContents);
    detailsGroupBoxController = new DetailsGroupBoxController(&(model->details),
                                                              detailsGroupBox,
                                                              this);
#endif // SIR_METADATA_SUPPORT

    selectionGroupBox = new SelectionGroupBoxView(scrollAreaWidgetContents);
    selectionGroupBoxController = new SelectionGroupBoxController(
                &(model->selection), selectionGroupBox, this);

    RawTabWidget *rawTabWidget = new RawTabWidget(scrollAreaWidgetContents);
    rawGroupBox = new RawGroupBoxView(rawTabWidget, scrollAreaWidgetContents);
}

void OptionsDialog::createGroupBoxManager()
{
#ifdef SIR_METADATA_SUPPORT
    groupBoxManager = new OptionsGroupBoxManager(6);
#else
    groupBoxManager = new OptionsGroupBoxManager(4);
#endif // SIR_METADATA_SUPPORT

    groupBoxManager->append(generalGroupBox);
    groupBoxManager->append(fileListGroupBox);

#ifdef SIR_METADATA_SUPPORT
    groupBoxManager->append(metadataGroupBox);
    groupBoxManager->append(detailsGroupBox);
#endif // SIR_METADATA_SUPPORT

    groupBoxManager->append(selectionGroupBox);
    groupBoxManager->append(rawGroupBox);
}

void OptionsDialog::setupGroupBoxesLayout()
{
    groupBoxManager->addToLayout(verticalLayout_2);
    scrollAreaWidgetContents->setLayout(verticalLayout_2);

    groupBoxManager->hideAll();
}

void OptionsDialog::setupComboBoxesModels() {
    // copying items of combo boxes from parent ConvertDialog window
    ConvertDialog* mainWindow = (ConvertDialog*)parent();
    generalGroupBox->sizeUnitComboBox->setModel(
                mainWindow->sizeScrollArea->sizeUnitComboBox->model());
    generalGroupBox->fileSizeComboBox->setModel(
                mainWindow->sizeScrollArea->fileSizeComboBox->model());
}

/** \e Ok button clicked slot.\n
  * Checks values in user input widgets and saves settings if checking was
  * successful.
  * \sa saveSettings()
  */
void OptionsDialog::okButtonClicked() {
    if (!fileListGroupBoxController->isAnyColumnChecked()) {
        QMessageBox::warning(this, tr("Select a column"),
                             tr("Select at least 1 column to show in file list."));
        if (!fileListGroupBox->isVisible())
            listWidget->setCurrentRow(Options::FileList);
        return;
    }
    saveSettings();
    reinterpret_cast<ConvertDialog *>(parent())->loadSettings();
}

/** Setups user interface of options dialog. */
void OptionsDialog::setupUi() {
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("OptionsDialog"));
    this->setWindowTitle(tr("Sir - Configure Options"));
    this->setWindowModality(Qt::WindowModal);
    this->resize(680, 550);
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName("horizontalLayout");
    horizontalLayout->setSpacing(8);

    listWidget = createListWidget();

    horizontalLayout->addWidget(listWidget);
    scrollArea = new QScrollArea(this);
    scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
    scrollArea->setWidgetResizable(true);
    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 409, 332));
    verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));

    createGroupBoxes();
    createGroupBoxManager();
    setupGroupBoxesLayout();

    const int currentListItem = 0;
    listWidget->setCurrentRow(currentListItem);
    groupBoxManager->setCurrentGroupBoxByIndex(currentListItem);

    scrollArea->setWidget(scrollAreaWidgetContents);
    horizontalLayout->addWidget(scrollArea);

    setupComboBoxesModels();

    verticalLayout->addItem(horizontalLayout);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                     Qt::Horizontal, this);
    buttonBox->setObjectName("buttonBox");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    verticalLayout->addWidget(buttonBox);
}
