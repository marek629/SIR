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

#include <QFileDialog>
#include <QSpacerItem>
#include <QScrollArea>
#include <QDialogButtonBox>
#include "selectiondialog.h"
#include "convertdialog.h"
#ifndef SIR_METADATA_SUPPORT
#include "widgets/historycombobox.h"
#endif // SIR_METADATA_SUPPORT

/** SelectionDialog object constructor.\n
  * Setup conpletely GUI and loads settings.
  * \param params Pointer to selection parameters structure.
  * \param getDirPath If true shows directory widget in this dialog.
  * \param parent Pointer to parent widget.
  * \param f Window flags.
  * \sa setupUi() setupTabOrder() loadSettings()
  */
SelectionDialog::SelectionDialog(SelectionParams *params, bool getDirPath,
                                 QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent,f) {
    this->convertDialog = static_cast<ConvertDialog*> (parent);
    this->params = params;
    this->params->path = Settings::instance().settings.lastDir;
    setupUi();
    QGridLayout *scrollGridLayout = (QGridLayout*)scrollAreaWidgetContents->layout();
    int columnSpan = scrollGridLayout->columnCount();
    // add dir widget
    if (!getDirPath) {
        dirWidget = 0;
        uiDirWidget = 0;
    }
    else {
        dirWidget = new QWidget(scrollAreaWidgetContents);
        uiDirWidget = new Ui::DirWidget;
        uiDirWidget->setupUi(dirWidget);
        scrollGridLayout->addWidget(dirWidget, 1, 0, 1, columnSpan);
        uiDirWidget->dirLineEdit->setText(this->params->path);
        connect(uiDirWidget->dirPushButton, SIGNAL(clicked()), this, SLOT(browseDir()));
    }
    // add metadata group boxes
#ifdef SIR_METADATA_SUPPORT
    anyMetadataGroupBox = new AnyMetadataGroupBox(scrollAreaWidgetContents);
    scrollGridLayout->addWidget(anyMetadataGroupBox,
                                scrollGridLayout->rowCount(), 0, 1, columnSpan);
    exifGroupBox = new QGroupBox(scrollAreaWidgetContents);
    uiExif.setupUi(exifGroupBox);
    scrollGridLayout->addWidget(exifGroupBox,
                                scrollGridLayout->rowCount(), 0, 1, columnSpan);
    iptcGroupBox = new QGroupBox(scrollAreaWidgetContents);
    uiIptc.setupUi(iptcGroupBox);
    scrollGridLayout->addWidget(iptcGroupBox,
                                scrollGridLayout->rowCount(), 0, 1, columnSpan);
#endif // SIR_METADATA_SUPPORT
    // add vertical spacer
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Fixed,
                                                  QSizePolicy::Expanding);
    scrollGridLayout->addItem(verticalSpacer, scrollGridLayout->rowCount(), 0);
    // setup tab order
    setupTabOrder();
    // load settings
    loadSettings();
}

/** Setup base GUI. Additional widgets are added in
  * \ref SelectionDialog() "constructor".
  * \sa SelectionDialog() setupTabOrder()
  */
void SelectionDialog::setupUi() {
    /* tooltip string for line edits (e.g. in history combo boxes) accepting
     * regular expressions */
    QString acceptRegExpString = tr("Accepts regular expression");
    // preparing dialog
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("SelectionDialog"));
    this->resize(640, 480);
    // main layout
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    // scroll area
    scrollArea = new QScrollArea(this);
    scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    // scroll area's widget
    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 622, 425));
    // layout of scroll area's widget
    gridLayout_4 = new QGridLayout(scrollAreaWidgetContents);
    gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
    // clear selection check box
    clearSelectionCheckBox = new QCheckBox(scrollAreaWidgetContents);
    clearSelectionCheckBox->setObjectName(QString::fromUtf8("clearSelectionCheckBox"));
    clearSelectionCheckBox->setText(tr("Clear current selection"));
#ifndef QT_NO_TOOLTIP
    clearSelectionCheckBox->setToolTip(tr("Unselect items selected before this action"));
#endif // QT_NO_TOOLTIP
    gridLayout_4->addWidget(clearSelectionCheckBox, 0, 0, 1, 4);
    // file name label
    fileNameLabel = new QLabel(scrollAreaWidgetContents);
    fileNameLabel->setObjectName(QString::fromUtf8("fileNameLabel"));
    fileNameLabel->setText(tr("File name"));
    gridLayout_4->addWidget(fileNameLabel, 2, 0, 1, 1); /* leaving 1 row in grid
                                                         * layout for dirWidget */
    // file name history combo box
    fileNameComboBox = new HistoryComboBox(scrollAreaWidgetContents);
    fileNameComboBox->setObjectName(QString::fromUtf8("fileNameComboBox"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(fileNameComboBox->sizePolicy().hasHeightForWidth());
    fileNameComboBox->setSizePolicy(sizePolicy);
#ifndef QT_NO_TOOLTIP
    fileNameComboBox->setToolTip(acceptRegExpString);
#endif // QT_NO_TOOLTIP
    gridLayout_4->addWidget(fileNameComboBox, 2, 1, 1, 3);
    // file size history combo box
    fileSizeComboBox = new HistoryComboBox(scrollAreaWidgetContents);
    fileSizeComboBox->setObjectName(QString::fromUtf8("fileSizeComboBox"));
    sizePolicy.setHeightForWidth(fileSizeComboBox->sizePolicy().hasHeightForWidth());
    fileSizeComboBox->setSizePolicy(sizePolicy);
#ifndef QT_NO_TOOLTIP
    fileSizeComboBox->setToolTip(acceptRegExpString);
#endif // QT_NO_TOOLTIP
    gridLayout_4->addWidget(fileSizeComboBox, 3, 1, 1, 1);
    // file size label
    fileSizeLabel = new QLabel(scrollAreaWidgetContents);
    fileSizeLabel->setObjectName(QString::fromUtf8("fileSizeLabel"));
    fileSizeLabel->setText(tr("File size"));
    gridLayout_4->addWidget(fileSizeLabel, 3, 0, 1, 1);
    // image size label
    imageSizeLabel = new QLabel(scrollAreaWidgetContents);
    imageSizeLabel->setObjectName(QString::fromUtf8("imageSizeLabel"));
    imageSizeLabel->setText(tr("Image size [px]"));
    gridLayout_4->addWidget(imageSizeLabel, 3, 2, 1, 1);
    // image size history combo box
    imageSizeComboBox = new HistoryComboBox(scrollAreaWidgetContents);
    imageSizeComboBox->setObjectName(QString::fromUtf8("imageSizeComboBox"));
    sizePolicy.setHeightForWidth(imageSizeComboBox->sizePolicy().hasHeightForWidth());
    imageSizeComboBox->setSizePolicy(sizePolicy);
#ifndef QT_NO_TOOLTIP
    imageSizeComboBox->setToolTip(acceptRegExpString);
#endif // QT_NO_TOOLTIP
    gridLayout_4->addWidget(imageSizeComboBox, 3, 3, 1, 1);
    // set scroll area's widget
    scrollArea->setWidget(scrollAreaWidgetContents);
    // add scroll area to main layout
    verticalLayout->addWidget(scrollArea);
    // button box
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    verticalLayout->addWidget(buttonBox);
    // create connections
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

/** Sets swich widgets order by tabulator key in this dialog.
  * \sa setupUi()
  */
void SelectionDialog::setupTabOrder() {
    setTabOrder(scrollArea, clearSelectionCheckBox);
    if (dirWidget) {
        setTabOrder(clearSelectionCheckBox, dirWidget);
        setTabOrder(dirWidget, fileNameComboBox);
    }
    else
        setTabOrder(clearSelectionCheckBox, fileNameComboBox);
    setTabOrder(fileNameComboBox, fileSizeComboBox);
    setTabOrder(fileSizeComboBox, imageSizeComboBox);
#ifdef SIR_METADATA_SUPPORT
    setTabOrder(imageSizeComboBox, anyMetadataGroupBox);
    setTabOrder(anyMetadataGroupBox, exifGroupBox);
    setTabOrder(exifGroupBox, iptcGroupBox);
    setTabOrder(iptcGroupBox, buttonBox);
#else
    setTabOrder(imageSizeComboBox, buttonBox);
#endif // SIR_METADATA_SUPPORT
    setTabOrder(buttonBox, scrollArea);
}

/** Saves selection parameters to \a params structure. */
void SelectionDialog::accept() {
    if (clearSelectionCheckBox->isChecked())
        convertDialog->filesTreeWidget->clearSelection();
    params->fileName = fileNameComboBox->lineEdit()->text();
    params->fileSizeExp = fileSizeComboBox->lineEdit()->text();
    params->imageSizeExp = imageSizeComboBox->lineEdit()->text();
#ifdef SIR_METADATA_SUPPORT
    params->checkMetadata = anyMetadataGroupBox->isChecked();
    if (params->checkMetadata) {
        params->createdDateTime[0] =
                anyMetadataGroupBox->createdAfterDateTimeEdit->dateTime();
        params->createdDateTime[1] =
                anyMetadataGroupBox->createdBeforeDateTimeEdit->dateTime();
        params->digitizedDateTime[0] =
                anyMetadataGroupBox->digitizedAfterDateTimeEdit->dateTime();
        params->digitizedDateTime[1] =
                anyMetadataGroupBox->digitizedBeforeDateTimeEdit->dateTime();
        params->author = anyMetadataGroupBox->authorComboBox->lineEdit()->text();
        params->copyright = anyMetadataGroupBox->copyrightComboBox->lineEdit()->text();
    }
    params->checkExif = exifGroupBox->isChecked();
    if (params->checkExif) {
        params->processingSoftware = uiExif.softwareComboBox->lineEdit()->text();
        params->cameraManufacturer =
                uiExif.cameraManufacturerComboBox->lineEdit()->text();
        params->cameraModel = uiExif.cameraModelComboBox->lineEdit()->text();
    }
    params->checkIPTC = iptcGroupBox->isChecked();
    if (params->checkIPTC) {
        params->objectName = uiIptc.objectNameComboBox->lineEdit()->text();
        params->keywords = uiIptc.keywordsComboBox->lineEdit()->text();
        params->description = uiIptc.descriptionComboBox->lineEdit()->text();
        params->countryName = uiIptc.countryNameComboBox->lineEdit()->text();
        params->city = uiIptc.cityComboBox->lineEdit()->text();
        params->editStatus = uiIptc.editStatusComboBox->lineEdit()->text();
    }
#endif // SIR_METADATA_SUPPORT
    if (dirWidget) {
        params->path = uiDirWidget->dirLineEdit->text();
        params->browseSubdirs = uiDirWidget->dirCheckBox->isChecked();
        params->selectImportedFiles = uiDirWidget->selectImportedCheckBox->isChecked();
    }
    else {
        params->path = "";
        params->browseSubdirs = false;
        params->selectImportedFiles = false;
    }
    saveSettings();
    QDialog::accept();
}

/** Directory button slot. Gets from the user new directory path and shows this
  * on directory line edit widget.
  */
void SelectionDialog::browseDir() {
    QString dirPath = QFileDialog::getExistingDirectory(
                       this,
                       tr("Choose a directory"),
                       Settings::instance().settings.lastDir,
                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirPath.isEmpty())
        return;
    dirPath = QDir::convertSeparators(dirPath);
    params->path = dirPath;
    Settings::instance().settings.lastDir = dirPath;
    uiDirWidget->dirLineEdit->setText(dirPath);
}

/** Loads selection settings and imports selection dialogs text edit history
  * from settings file.
  * \sa saveSettings()
  */
void SelectionDialog::loadSettings() {
    Settings &s = Settings::instance();
    // general settings
#ifdef SIR_METADATA_SUPPORT
    // set display format for date/time edit widgets
    QString dateTimeFormat =    s.settings.dateDisplayFormat
            + ' ' + s.settings.timeDisplayFormat;
    anyMetadataGroupBox->createdBeforeDateTimeEdit->setDisplayFormat(dateTimeFormat);
    anyMetadataGroupBox->createdAfterDateTimeEdit->setDisplayFormat(dateTimeFormat);
    anyMetadataGroupBox->digitizedBeforeDateTimeEdit->setDisplayFormat(dateTimeFormat);
    anyMetadataGroupBox->digitizedAfterDateTimeEdit->setDisplayFormat(dateTimeFormat);
#endif // SIR_METADATA_SUPPORT
    // max history count for HistoryComboBox's import functions
    int maxHistoryCount =       s.settings.maxHistoryCount;
    // selection settings
    clearSelectionCheckBox->setChecked(     s.selection.clearSelection);
    if (dirWidget) {
        uiDirWidget->dirCheckBox->setChecked(            s.selection.subdirs);
        uiDirWidget->selectImportedCheckBox->setChecked( s.selection.selectImported);
    }
    // settings of selection dialog
    Settings::SelectionDialogGroup &sd = s.selectionDialog;
    // file
    fileNameComboBox->importHistory(                    sd.fileNameMap,
                                                        sd.fileNameList,
                                                        maxHistoryCount);
    fileSizeComboBox->importHistory(                    sd.fileSizeMap,
                                                        sd.fileSizeList,
                                                        maxHistoryCount);
    imageSizeComboBox->importHistory(                   sd.imageSizeMap,
                                                        sd.imageSizeList,
                                                        maxHistoryCount);
#ifdef SIR_METADATA_SUPPORT
    // any metadata
    anyMetadataGroupBox->authorComboBox->importHistory( sd.anyAuthorMap,
                                                        sd.anyAuthorList,
                                                        maxHistoryCount);
    anyMetadataGroupBox->copyrightComboBox->importHistory(sd.anyCopyrightMap,
                                                        sd.anyCopyrightList,
                                                        maxHistoryCount);
    // Exif
    uiExif.softwareComboBox->importHistory(             sd.exifSoftMap,
                                                        sd.exifSoftList,
                                                        maxHistoryCount);
    uiExif.cameraManufacturerComboBox->importHistory(   sd.exifCameraManufacturerMap,
                                                        sd.exifCameraManufacturerList,
                                                        maxHistoryCount);
    uiExif.cameraModelComboBox->importHistory(          sd.exifCameraModelMap,
                                                        sd.exifCameraModelList,
                                                        maxHistoryCount);
    // IPTC
    uiIptc.objectNameComboBox->importHistory(           sd.iptcObjectNameMap,
                                                        sd.iptcObjectNameList,
                                                        maxHistoryCount);
    uiIptc.keywordsComboBox->importHistory(             sd.iptcKeywordsMap,
                                                        sd.iptcKeywordsList,
                                                        maxHistoryCount);
    uiIptc.descriptionComboBox->importHistory(          sd.iptcDescriptionMap,
                                                        sd.iptcDescriptionList,
                                                        maxHistoryCount);
    uiIptc.countryNameComboBox->importHistory(          sd.iptcCountryNameMap,
                                                        sd.iptcCountryNameList,
                                                        maxHistoryCount);
    uiIptc.cityComboBox->importHistory(                 sd.iptcCityMap,
                                                        sd.iptcCityList,
                                                        maxHistoryCount);
    uiIptc.editStatusComboBox->importHistory(           sd.iptcEditStatusMap,
                                                        sd.iptcEditStatusList,
                                                        maxHistoryCount);
#endif // SIR_METADATA_SUPPORT
}

/** Exports selection dialogs text edit history to settings file.
  * \sa loadSettings()
  */
void SelectionDialog::saveSettings() {
    Settings &s = Settings::instance();
    HistoryMap  map;
    HistoryList list;
    // general settings
    int maxHistoryCount = s.settings.maxHistoryCount;
    // settings of selection dialog
    // file
    fileNameComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.fileNameMap                   = map;
    s.selectionDialog.fileNameList                  = list;
    fileSizeComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.fileSizeMap                   = map;
    s.selectionDialog.fileSizeList                  = list;
    imageSizeComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.imageSizeMap                  = map;
    s.selectionDialog.imageSizeList                 = list;
#ifdef SIR_METADATA_SUPPORT
    // any metadata
    anyMetadataGroupBox->authorComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.anyAuthorMap                  = map;
    s.selectionDialog.anyAuthorList                 = list;
    anyMetadataGroupBox->copyrightComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.anyCopyrightMap               = map;
    s.selectionDialog.anyCopyrightList              = list;
    // Exif
    uiExif.softwareComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.exifSoftMap                   = map;
    s.selectionDialog.exifSoftList                  = list;
    uiExif.cameraManufacturerComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.exifCameraManufacturerMap     = map;
    s.selectionDialog.exifCameraManufacturerList    = list;
    uiExif.cameraModelComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.exifCameraModelMap            = map;
    s.selectionDialog.exifCameraModelList           = list;
    // IPTC
    uiIptc.objectNameComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.iptcObjectNameMap             = map;
    s.selectionDialog.iptcObjectNameList            = list;
    uiIptc.keywordsComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.iptcKeywordsMap               = map;
    s.selectionDialog.iptcKeywordsList              = list;
    uiIptc.descriptionComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.iptcDescriptionMap            = map;
    s.selectionDialog.iptcDescriptionList           = list;
    uiIptc.countryNameComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.iptcCountryNameMap            = map;
    s.selectionDialog.iptcCountryNameList           = list;
    uiIptc.cityComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.iptcCityMap                   = map;
    s.selectionDialog.iptcCityList                  = list;
    uiIptc.editStatusComboBox->exportHistory(&map, &list, maxHistoryCount);
    s.selectionDialog.iptcEditStatusMap             = map;
    s.selectionDialog.iptcEditStatusList            = list;
#endif // SIR_METADATA_SUPPORT
}
