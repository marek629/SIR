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

#include "widgets/OptionsDialog.hpp"

#include "optionsenums.h"
#include "raw/RawModelSettings.hpp"
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
#include "widgets/options/RawGroupBoxController.hpp"
#include "widgets/options/RawGroupBoxView.hpp"

#include <QDialogButtonBox>
#include <QMessageBox>
#include <QListWidget>
#include <QScrollArea>


/** Sets up window, loads settings and creates connections.
  * \sa setupUi() loadSettings() createConnections()
  */
OptionsDialog::OptionsDialog(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f) {
    setupUi();
    loadSettings();
    createConnections();
}

/** Destructor. */
OptionsDialog::~OptionsDialog() {
    delete groupBoxes;

    delete fileListGroupBoxController;
    delete generalGroupBoxController;
#ifdef SIR_METADATA_SUPPORT
    delete metadataGroupBoxController;
#endif // SIR_METADATA_SUPPORT
    delete selectionGroupBoxController;
    delete rawGroupBoxController;
}

/** Connects signals to slots. */
void OptionsDialog::createConnections() {
    // general
    connect(listWidget, SIGNAL(currentRowChanged(int)),
            this, SLOT(categoryChanged(int)));
    // raw
    connect(rawGroupBoxController, SIGNAL(ok()),
            (ConvertDialog*)parent(), SLOT(loadSettings()));
    // ok button
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(okButtonClicked()));
}

/** Hides last choosed category (group box) and shows just selected category.
  * \sa setupWindow
  */
void OptionsDialog::categoryChanged(int current) {
    if (current == currentListItem)
        return;

    groupBoxes[currentListItem]->hide();
    groupBoxes[current]->show();
    currentListItem = current;
}

/** Load settings for the user input wigets.
  * \sa saveSettings()
  */
void OptionsDialog::loadSettings() {
    // load settings for eatch group box
    for (quint8 i=0; i<categoriesCount; i++)
        groupBoxes[i]->loadSettings();
}

/** Writes settings based values of the user input wigets.
  * \sa loadSettings() okButtonClicked()
  */
void OptionsDialog::saveSettings() {
    // save settings for eatch group box
    for (quint8 i=0; i<categoriesCount; i++)
        groupBoxes[i]->saveSettings();
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
    rawGroupBox = new RawGroupBoxView(scrollAreaWidgetContents);
    RawModelSettings rawModel = RawModelSettings(model->raw);
    rawGroupBoxController = new RawGroupBoxController(&rawModel, rawGroupBox,
                                                      this);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
}

void OptionsDialog::createGroupBoxesArray() {
    groupBoxes = new AbstractOptionsGroupBox*[categoriesCount];

    quint8 i = 0;

    groupBoxes[i++] = generalGroupBox;
    groupBoxes[i++] = fileListGroupBox;

#ifdef SIR_METADATA_SUPPORT
    groupBoxes[i++] = metadataGroupBox;
    groupBoxes[i++] = detailsGroupBox;
#endif // SIR_METADATA_SUPPORT

    groupBoxes[i++] = selectionGroupBox;
    groupBoxes[i] = rawGroupBox;
}

void OptionsDialog::setupGroupBoxesLayout() {
    quint8 i;

    // adding group boxes to layout
    for (i=0; i<categoriesCount; i++)
        verticalLayout_2->addWidget(groupBoxes[i]);
    verticalLayout_2->addItem(verticalSpacer);
    scrollAreaWidgetContents->setLayout(verticalLayout_2);

    // hiding group boxes
    for (i=0; i<categoriesCount; i++)
        groupBoxes[i]->hide();
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
    createGroupBoxesArray();
    setupGroupBoxesLayout();

    currentListItem = 0;
    listWidget->setCurrentRow(currentListItem);
    groupBoxes[currentListItem]->show();

    scrollArea->setWidget(scrollAreaWidgetContents);
    horizontalLayout->addWidget(scrollArea);

    setupComboBoxesModels();

    verticalLayout->addItem(horizontalLayout);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                     Qt::Horizontal, this);
    buttonBox->setObjectName("buttonBox");
    verticalLayout->addWidget(buttonBox);

    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));
}
