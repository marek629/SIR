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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Program URL: http://sir.projet-libre.org/
 */

#include "selection.h"
#include "convertdialog.h"
#include "defines.h"
#include "expressiontree.h"
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>
#include <QSettings>

// static variables

// Remember update also function arrays in FuncionNode derivered classes!

/** Logical operators using in selection logical expressions.
  * \note And operator (&) must be first item on list! It's required by
  *       LogicalExpressionTree::init() function.
  * \note If you wish change or add a operator to this list, remember update
  *       also LogicalFunctionNode::fnArray array.
  * \sa comparisonOperators allOperators
  */
QStringList Selection::logicalOperators = QStringList() << "&" << "|" << "^";

/** Comparison operators using in selection logical expressions.
  * \note If you wish change or add a operator to this list, remember update
  *       also CompareFunctionNode::fnArray array.
  * \sa logicalOperators allOperators
  */
QStringList Selection::comparisonOperators = QStringList() << "=" << "<" << ">";

/** All operators using in selection logical expressions.
  * \sa comparisonOperators logicalOperators
  */
QStringList Selection::allOperators = QStringList() <<
        Selection::logicalOperators << Selection::comparisonOperators;

// ________________________________ Selection ________________________________

/** Selection object default constructor. */
Selection::Selection(ConvertDialog *parent) : QObject(parent) {
    convertDialog = parent;
    progressDialog = 0;
    loadSymbols();
    fileSizeExpTree = 0;
    fileSizeVector = new QVector<qint64>(fileSizeSymbols.count());
    imageSizeExpTree = 0;
    imageSizeVector = new QVector<qint64>(imageSizeSymbols.count());
}

/** Selection object destructor. */
Selection::~Selection() {
    delete progressDialog;
    clearPointerList(&fileNameListRx);
    clearPointerList(&exifProcessingSoftwareListRx);
    clearPointerList(&exifCameraManufacturerListRx);
    clearPointerList(&exifCameraModelListRx);
    clearPointerList(&iptcObjectNameListRx);
    clearPointerList(&iptcKeywordsListRx);
    clearPointerList(&iptcDescriptionListRx);
    clearPointerList(&iptcCountryNameListRx);
    clearPointerList(&iptcCityListRx);
    clearPointerList(&iptcEditStatusListRx);
    delete fileSizeExpTree;
    delete fileSizeVector;
    delete imageSizeExpTree;
    delete imageSizeVector;
}

/** Shows selection conditions dialog and selects items on parents ConvertDialog
  * files tree widget.
  * \return Count of selected items.
  * \sa SelectionDialog importFiles
  */
int Selection::selectItems() {
    int itemsSelected = 0;
    SelectionDialog dialog(&params,false,convertDialog);
    dialog.setWindowTitle(tr("Conditions of items selection - SIR"));
    if (dialog.exec() == QDialog::Rejected)
        return -1; // dialog canceled
    qDebug() << "File name expression:" << params.fileName;
    setupRegExps();
    qDebug() << "File size expression:" << params.fileSizeExp;
    setupExpressionTrees();
    convertDialog->setCursor(QCursor(Qt::WaitCursor));
    int itemCount = convertDialog->filesTreeWidget->topLevelItemCount();
    if (!progressDialog)
        progressDialog = new QProgressDialog(convertDialog);
    progressDialog->setWindowTitle(tr("Checking selection conditions..."));
    progressDialog->setMaximumWidth(convertDialog->width());
    progressDialog->setRange(0, itemCount);
    progressDialog->show();
    for (int i=0; i<itemCount && !progressDialog->wasCanceled(); i++) {
        QTreeWidgetItem *item = convertDialog->filesTreeWidget->topLevelItem(i);
        QString filePath = item->text(2) + QDir::separator() + item->text(0) +
                           '.' + item->text(1);
        qDebug() << filePath;
        progressDialog->setLabelText(filePath);
        progressDialog->setValue(i);
        QFileInfo info(filePath);
        if (!testFile(info))
            continue;
        // if all conditions succed select item of tree widget and incremate items counter
        item->setSelected(true);
        itemsSelected++;
        QApplication::processEvents();
    }
    progressDialog->close();
    if (itemsSelected > 0) {
        convertDialog->enableConvertButtons();
        convertDialog->resizeColumnsToContents(convertDialog->filesTreeWidget);
    }
    convertDialog->setCursor(QCursor(Qt::ArrowCursor));
    return itemsSelected;
}

/** Shows selection conditions dialog and loads items to parents ConvertDialog
  * files tree widget.
  * \return Count of imported files; -1 if directory path is empty.
  * \sa SelectionDialog selectItems
  */
int Selection::importFiles() {
    int filesImported = 0;
    SelectionDialog dialog(&params,true,convertDialog);
    dialog.setWindowTitle(tr("Conditions of import files - SIR"));
    if (dialog.exec() == QDialog::Rejected)
        return -1; // dialog canceled
    qDebug() << "File name expression:" << params.fileName;
    setupRegExps();
    qDebug() << "File size expression:" << params.fileSizeExp;
    if (params.path.isEmpty()) {
        qDebug("Selection::importFiles(): Directory path is empty!");
        return -2;
    }
    setupExpressionTrees();
    convertDialog->setCursor(QCursor(Qt::WaitCursor));
    if (!progressDialog)
        progressDialog = new QProgressDialog(convertDialog);
    progressDialog->setWindowTitle(tr("Checking import conditions..."));
    progressDialog->setLabelText(tr("Scanning directories..."));
    progressDialog->setMaximumWidth(convertDialog->width());
    progressDialog->setRange(0,0);
    progressDialog->setValue(0);
    progressDialog->show();
    QFileInfoList fileInfoList;
    qDebug() << "Loaded files into list:"
             << loadFileInfo(params.path, &fileInfoList, params.browseSubdirs);
    progressDialog->setMaximum(fileInfoList.length());
    foreach (QFileInfo info, fileInfoList) {
        qDebug() << info.filePath() << info.size()/1024 << "KiB";
        QApplication::processEvents();
        if (progressDialog->wasCanceled())
            break;
        progressDialog->setLabelText(info.filePath());
        progressDialog->setValue(progressDialog->value()+1);
        if (!testFile(info))
            continue;
        // if all conditions succed add to tree widget and incremate files counter
        QStringList itemList;
        itemList << info.completeBaseName() << info.suffix() << info.path()
                 << tr("Not converted yet");
        convertDialog->statusList->insert(info.absoluteFilePath(), NOTCONVERTED);
        QTreeWidgetItem *item = new QTreeWidgetItem(itemList);
        convertDialog->filesTreeWidget->addTopLevelItem(item);
        item->setSelected(params.selectImportedFiles);
        filesImported++;
    }
    progressDialog->close();
    if (filesImported > 0) {
        convertDialog->enableConvertButtons();
        convertDialog->resizeColumnsToContents(convertDialog->filesTreeWidget);
    }
    convertDialog->setCursor(QCursor(Qt::ArrowCursor));
    return filesImported;
}

void Selection::loadSymbols() {
    QSettings settings("SIR");
    settings.beginGroup("Selection");
    fileSizeSymbols << settings.value("fileSizeSymbol","s").toString();
    imageSizeSymbols << settings.value("imageWidthSymbol","x").toString()
                     << settings.value("imageHeightSymbol","y").toString();
    settings.endGroup(); // Selection
}

/** Sets up this objects regular expression lists setupListRegExp() method.
  * \sa setupListRegExp()
  */
void Selection::setupRegExps() {
    setupListRegExp(params.fileName, &fileNameListRx);
    // any metadata regular expressions lists
    setupListRegExp(params.author, &anyAuthorListRx);
    setupListRegExp(params.copyright, &anyCopyrightListRx);
    // Exif regular expressions lists
    setupListRegExp(params.processingSoftware, &exifProcessingSoftwareListRx);
    setupListRegExp(params.cameraManufacturer, &exifCameraManufacturerListRx);
    setupListRegExp(params.cameraModel, &exifCameraModelListRx);
    // IPTC regular expressions lists
    setupListRegExp(params.objectName, &iptcObjectNameListRx);
    setupListRegExp(params.keywords, &iptcKeywordsListRx);
    setupListRegExp(params.description, &iptcDescriptionListRx);
    setupListRegExp(params.countryName, &iptcCountryNameListRx);
    setupListRegExp(params.city, &iptcCityListRx);
    setupListRegExp(params.editStatus, &iptcEditStatusListRx);
}

/** Clears and sets up list of pointers to regular expressions. This function
  * clears safe list using clearPointerList() method.
  * \param strExp Regular expression string.
  * \param listRx Pointer to list of pointers to regular expression objects.
  * \sa clearPointerList
  */
void Selection::setupListRegExp(const MetadataUtils::String &strExp,
                                QList<QRegExp*> *listRx) {
    clearPointerList(listRx);
    if (strExp.isEmpty()) {
        QRegExp *rx = new QRegExp("*", Qt::CaseSensitive, QRegExp::WildcardUnix);
        listRx->append(rx);
        return;
    }
    foreach (QString str, strExp.split(' ', QString::SkipEmptyParts)) {
        QRegExp *rx = new QRegExp(str, Qt::CaseSensitive, QRegExp::FixedString);
        listRx->append(rx);
        rx = new QRegExp(str, Qt::CaseSensitive, QRegExp::WildcardUnix);
        listRx->append(rx);
        rx = new QRegExp(str, Qt::CaseSensitive, QRegExp::RegExp2);
        listRx->append(rx);
    }
}

/** Clears \a list and deallocates regular expression objects safe.
  * \param list Pointer to list of pointers to regular expression objects.
  */
void Selection::clearPointerList(QList<QRegExp *> *list) {
    while (!list->isEmpty())
        delete list->takeLast();
}

/** Loads information about files from \a dir directory to \a list.
  * \param dir Full path of directory.
  * \param list List contain file info objects.
  * \param recursive If true this function will browse directory recursive.
  * \note This function calls QApplication::processEvents().
  * \return Count of file info loaded into \a list.
  */
int Selection::loadFileInfo(const QString &dir, QFileInfoList *list, bool recursive) {
    int result = 0;
    qDebug() << "Watching dir:" << dir;
    QDir sourceDir(dir, convertDialog->fileFilters);
    sourceDir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Readable);
    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    list->append(fileInfoList);
    result += fileInfoList.length();
    QApplication::processEvents();
    if (progressDialog->wasCanceled())
        qDebug() << "canceled";
    if (recursive && !progressDialog->wasCanceled()) {
        QDir directory(dir);
        directory.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::Readable
                            | QDir::NoDotAndDotDot);
        foreach (QString d, directory.entryList())
            result += loadFileInfo(dir + QDir::separator() + d, list, recursive);
    }
    return result;
}

/** This is overloaded function.\n
  * Returns true if one member of \a rxList list is \a string string compatible.
  * \param string Testing string.
  * \param rxList Pointer to list of pointers to regular expression objects.
  */
bool Selection::isCompatible(const QString &string, const QList<QRegExp*> &rxList) {
    bool result = false;
    foreach (QRegExp *rx, rxList) {
        result = string.contains(*rx);
        if (result)
            break;
    }
    return result;
}

/** This is overloaded function.\n
  * Returns true if one member of \a rxList list is one member of \a list
  * list of strings compatible.
  * \param string Testing list of strings.
  * \param rxList Pointer to list of pointers to regular expression objects.
  */
bool Selection::isCompatible(const QStringList &list, const QList<QRegExp*> &rxList) {
    bool result = false;
    foreach (QString str, list) {
        result = isCompatible(str, rxList);
        if (result)
            break;
    }
    return result;
}

/** This is overloaded function.\n
  * Returns true if \a date and \a time object values are between value of first
  * and second item (including extreme values) of \a dtArray; otherwise returns false.
  * \note \a dtArray must be 2-item (or more) array; otherwise it will send SIGSEGV
  * (segmentation fault signal) to operating system.
  */
bool Selection::isCompatible(const QDate &date, const QTime &time, QDateTime *dtArray) {
    return ( (dtArray[0].date() <= date && date <= dtArray[1].date()) &&
             (dtArray[0].time() <= time && time <= dtArray[1].time()) );
}

/** This is overloaded function.\n
  * Returns true if \a dateTime object value is between value of first
  * and second item (including extreme values) of \a dtArray; otherwise returns false.
  * \note \a dtArray must be 2-item (or more) array; otherwise it will send SIGSEGV
  * (segmentation fault signal) to operating system.
  */
bool Selection::isCompatible(const QDateTime &dateTime, QDateTime *dtArray) {
    return (dtArray[0] <= dateTime && dateTime <= dtArray[1]);
}

/** Clears old trees and constructs new LogicalExpressionTree trees. */
void Selection::setupExpressionTrees() {
    delete fileSizeExpTree;
    fileSizeExpTree = new LogicalExpressionTree(params.fileSizeExp, fileSizeSymbols,
                                                fileSizeVector, this);
    delete imageSizeExpTree;
    imageSizeExpTree = new LogicalExpressionTree(params.imageSizeExp, imageSizeSymbols,
                                                 imageSizeVector, this);
}

/** Compares info and metadata of file with data corresponding selection params.\n
  * Returns true if all comparisons succed, otherwise returns false; for SVG file
  * allways returns false.
  */
bool Selection::testFile(const QFileInfo &info) {
    if (!isCompatible(info.fileName(), fileNameListRx))
        return false;
    fileSizeVector->replace(0, info.size());
    if (!fileSizeExpTree->rootNode()->solve())
        return false;
    QImage img(info.filePath());
    imageSizeVector->replace(0, img.width());
    imageSizeVector->replace(1, img.height());
    if (!imageSizeExpTree->rootNode()->solve())
        return false;
    if (params.checkMetadata || params.checkExif || params.checkIPTC) {
        if (info.suffix().contains("svg",Qt::CaseInsensitive))
            return false;
        MetadataUtils::Metadata metadata;
        if (!metadata.read(info.filePath(),true))
            return false;
        MetadataUtils::ExifStruct *exifStruct = 0;
        MetadataUtils::IptcStruct *iptcStruct = 0;
        if (params.checkMetadata) {
            exifStruct = metadata.exifStruct();
            iptcStruct = metadata.iptcStruct();
            if (!isCompatible(QDateTime::fromString(exifStruct->originalDate, Qt::ISODate),
                              params.createdDateTime) &&
                    !isCompatible(iptcStruct->dateCreated, iptcStruct->timeCreated,
                                  params.createdDateTime))
                return false;
            if (!isCompatible(QDateTime::fromString(exifStruct->digitizedDate, Qt::ISODate),
                              params.digitizedDateTime) &&
                    !isCompatible(iptcStruct->digitizationDate, iptcStruct->digitizationTime,
                                  params.digitizedDateTime))
                return false;
            QStringList strList;
            strList << exifStruct->artist << iptcStruct->byline;
            if (!isCompatible(strList, anyAuthorListRx))
                return false;
            strList.clear();
            strList << exifStruct->copyright << iptcStruct->copyright;
            if (!isCompatible(strList, anyCopyrightListRx))
                return false;
        }
        if (params.checkExif) {
            if (!exifStruct)
                exifStruct = metadata.exifStruct();
            if (!isCompatible(exifStruct->processingSoftware, exifProcessingSoftwareListRx))
                return false;
            if (!isCompatible(exifStruct->cameraManufacturer, exifCameraManufacturerListRx))
                return false;
            if (!isCompatible(exifStruct->cameraModel, exifCameraModelListRx))
                return false;
        }
        if (params.checkIPTC) {
            if (!iptcStruct)
                iptcStruct = metadata.iptcStruct();
            if (!isCompatible(iptcStruct->objectName, iptcObjectNameListRx))
                return false;
            if (!isCompatible(iptcStruct->keywords.split(' ', QString::SkipEmptyParts),
                              iptcKeywordsListRx))
                return false;
            if (!isCompatible(iptcStruct->caption.split(' ', QString::SkipEmptyParts),
                              iptcDescriptionListRx)
                    || !isCompatible(iptcStruct->caption, iptcDescriptionListRx))
                return false;
            if (!isCompatible(iptcStruct->countryName, iptcCountryNameListRx))
                return false;
            if (!isCompatible(iptcStruct->city, iptcCityListRx))
                return false;
            if (!isCompatible(iptcStruct->editStatus, iptcEditStatusListRx))
                return false;
        }
    }
    return true;
}

// _____________________________ SelectionDialog _____________________________

/** SelectionDialog object constructor.
  * \param params Pointer to selection parameters structure.
  * \param getDirPath If true shows directory widget in this dialog.
  * \param parent Pointer to parent widget.
  * \param f Window flags.
  */
SelectionDialog::SelectionDialog(SelectionParams *params, bool getDirPath,
                                 QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent,f) {
    this->convertDialog = static_cast<ConvertDialog*> (parent);
    this->params = params;
    this->params->path = this->convertDialog->lastDir;
    setupUi(this);
    if (!getDirPath) {
        delete dirWidget;
        dirWidget = 0;
    }
    else {
        dirLineEdit->setText(this->params->path);
        connect(dirPushButton, SIGNAL(clicked()), this, SLOT(browseDir()));
    }
    loadSettings();
}

/** Saves selection parameters to \a params structure. */
void SelectionDialog::accept() {
    if (clearSelectionCheckBox->isChecked())
        convertDialog->filesTreeWidget->clearSelection();
    params->fileName = fileNameComboBox->lineEdit()->text();
    params->fileSizeExp = fileSizeComboBox->lineEdit()->text();
    params->imageSizeExp = imageSizeComboBox->lineEdit()->text();
    params->checkMetadata = anyGroupBox->isChecked();
    if (params->checkMetadata) {
        params->createdDateTime[0] = createdAfterDateTimeEdit->dateTime();
        params->createdDateTime[1] = createdBeforeDateTimeEdit->dateTime();
        params->digitizedDateTime[0] = digitizedAfterDateTimeEdit->dateTime();
        params->digitizedDateTime[1] = digitizedBeforeDateTimeEdit->dateTime();
        params->author = authorComboBox->lineEdit()->text();
        params->copyright = copyrightComboBox->lineEdit()->text();
    }
    params->checkExif = exifGroupBox->isChecked();
    if (params->checkExif) {
        params->processingSoftware = exifSoftwareComboBox->lineEdit()->text();
        params->cameraManufacturer = exifCameraManufacturerComboBox->lineEdit()->text();
        params->cameraModel = exifCameraModelComboBox->lineEdit()->text();
    }
    params->checkIPTC = iptcGroupBox->isChecked();
    if (params->checkIPTC) {
        params->objectName = iptcObjectNameComboBox->lineEdit()->text();
        params->keywords = iptcKeywordsComboBox->lineEdit()->text();
        params->description = iptcDescriptionComboBox->lineEdit()->text();
        params->countryName = iptcCountryNameComboBox->lineEdit()->text();
        params->city = iptcCityComboBox->lineEdit()->text();
        params->editStatus = iptcEditStatusComboBox->lineEdit()->text();
    }
    if (dirWidget) {
        params->path = dirLineEdit->text();
        params->browseSubdirs = dirCheckBox->isChecked();
        params->selectImportedFiles = selectImportedCheckBox->isChecked();
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
                       convertDialog->lastDir,
                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirPath.isEmpty())
        return;
    dirPath = QDir::convertSeparators(dirPath);
    params->path = dirPath;
    convertDialog->lastDir = dirPath;
    dirLineEdit->setText(dirPath);
}

/** Loads selection settings and imports selection dialogs text edit history
  * from settings file.
  * \sa saveSettings
  */
void SelectionDialog::loadSettings() {
    QSettings settings("SIR");
    settings.beginGroup("Settings");
    // set display format for date/time edit widgets
    QString dateTimeFormat = settings.value("dateDisplayFormat","dd.MM.yyyy").toString()
            + ' ' + settings.value("timeDisplayFormat","HH:mm:ss").toString();
    createdBeforeDateTimeEdit->setDisplayFormat(dateTimeFormat);
    createdAfterDateTimeEdit->setDisplayFormat(dateTimeFormat);
    digitizedBeforeDateTimeEdit->setDisplayFormat(dateTimeFormat);
    digitizedAfterDateTimeEdit->setDisplayFormat(dateTimeFormat);
    // max history count for HistoryComboBox's import functions
    int maxHistoryCount = settings.value("maxHistoryCount", 5).toInt();
    settings.endGroup(); // Settings

    settings.beginGroup("Selection");
    clearSelectionCheckBox->setChecked(settings.value("clearSelection",false).toBool());
    if (dirWidget) {
        dirCheckBox->setChecked(settings.value("subdirs",false).toBool());
        selectImportedCheckBox->setChecked(settings.value("selectImported",false).toBool());
    }
    settings.endGroup(); // Selection

    settings.beginGroup("SelectionDialog");
    // file
    fileNameComboBox->importHistory(settings.value("fileNameMap").toMap(),
                                    settings.value("fileNameList").toList(),
                                    maxHistoryCount);
    fileSizeComboBox->importHistory(settings.value("fileSizeMap").toMap(),
                                    settings.value("fileSizeList").toList(),
                                    maxHistoryCount);
    imageSizeComboBox->importHistory(settings.value("imageSizeMap").toMap(),
                                     settings.value("imageSizeList").toList(),
                                     maxHistoryCount);
    // any metadata
    authorComboBox->importHistory(settings.value("anyAuthorMap").toMap(),
                                  settings.value("anyAuthorList").toList(),
                                  maxHistoryCount);
    copyrightComboBox->importHistory(settings.value("anyCopyrightMap").toMap(),
                                     settings.value("anyCopyrightList").toList(),
                                     maxHistoryCount);
    // Exif
    exifSoftwareComboBox->importHistory(settings.value("exifSoftMap").toMap(),
                                        settings.value("exifSoftList").toList(),
                                        maxHistoryCount);
    exifCameraManufacturerComboBox->importHistory(settings.value("exifCameraManufacturerMap").toMap(),
                                                  settings.value("exifCameraManufacturerList").toList(),
                                                  maxHistoryCount);
    exifCameraModelComboBox->importHistory(settings.value("exifCameraModelMap").toMap(),
                                           settings.value("exifCameraModelList").toList(),
                                           maxHistoryCount);
    // IPTC
    iptcObjectNameComboBox->importHistory(settings.value("iptcObjectNameMap").toMap(),
                                          settings.value("iptcObjectNameList").toList(),
                                          maxHistoryCount);
    iptcKeywordsComboBox->importHistory(settings.value("iptcKeywordsMap").toMap(),
                                        settings.value("iptcKeywordsList").toList(),
                                        maxHistoryCount);
    iptcDescriptionComboBox->importHistory(settings.value("iptcDescriptionMap").toMap(),
                                           settings.value("iptcDescriptionList").toList(),
                                           maxHistoryCount);
    iptcCountryNameComboBox->importHistory(settings.value("iptcCountryNameMap").toMap(),
                                           settings.value("iptcCountryNameList").toList(),
                                           maxHistoryCount);
    iptcCityComboBox->importHistory(settings.value("iptcCityMap").toMap(),
                                    settings.value("iptcCityList").toList(),
                                    maxHistoryCount);
    iptcEditStatusComboBox->importHistory(settings.value("iptcEditStatusMap").toMap(),
                                          settings.value("iptcEditStatusList").toList(),
                                          maxHistoryCount);
    settings.endGroup(); // SelectionDialog
}

/** Exports selection dialogs text edit history to settings file.
  * \sa loadSettings
  */
void SelectionDialog::saveSettings() {
    QMap<QString,QVariant> map;
    QList<QVariant> list;
    QSettings settings("SIR");
    settings.beginGroup("Settings");
    int maxHistoryCount = settings.value("maxHistoryCount", 5).toInt();
    settings.endGroup(); // Settings
    settings.beginGroup("SelectionDialog");
    // file
    fileNameComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("fileNameMap", map);
    settings.setValue("fileNameList", list);
    fileSizeComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("fileSizeMap", map);
    settings.setValue("fileSizeList", list);
    imageSizeComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("imageSizeMap", map);
    settings.setValue("imageSizeList", list);
    // any metadata
    authorComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("anyAuthorMap", map);
    settings.setValue("anyAuthorList", list);
    copyrightComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("anyCopyrightMap", map);
    settings.setValue("anyCopyrightList", list);
    // Exif
    exifSoftwareComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("exifSoftMap", map);
    settings.setValue("exifSoftList", list);
    exifCameraManufacturerComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("exifCameraManufacturerMap", map);
    settings.setValue("exifCameraManufacturerList", list);
    exifCameraModelComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("exifCameraModelMap", map);
    settings.setValue("exifCameraModelList", list);
    // IPTC
    iptcObjectNameComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("iptcObjectNameMap", map);
    settings.setValue("iptcObjectNameList", list);
    iptcKeywordsComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("iptcKeywordsMap", map);
    settings.setValue("iptcKeywordsList", list);
    iptcDescriptionComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("iptcDescriptionMap", map);
    settings.setValue("iptcDescriptionList", list);
    iptcCountryNameComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("iptcCountryNameMap", map);
    settings.setValue("iptcCountryNameList", list);
    iptcCityComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("iptcCityMap", map);
    settings.setValue("iptcCityList", list);
    iptcEditStatusComboBox->exportHistory(&map, &list, maxHistoryCount);
    settings.setValue("iptcEditStatusMap", map);
    settings.setValue("iptcEditStatusList", list);
    settings.endGroup(); // SelectionDialog
}
