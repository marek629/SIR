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

#include <QDebug>
#include <QProgressDialog>
#include <QElapsedTimer>
#include "selection.h"
#include "convertdialog.h"
#include "defines.h"
#include "expressiontree.h"
#include "widgets/selectiondialog.h"
#include "regexputils.h"
#include "convertshareddata.h"

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

// Namespace setup

using namespace RegExpUtils;

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
#ifdef SIR_METADATA_SUPPORT
    // any
    clearPointerList(&anyAuthorListRx);
    clearPointerList(&anyCopyrightListRx);
    // Exif
    clearPointerList(&exifProcessingSoftwareListRx);
    clearPointerList(&exifCameraManufacturerListRx);
    clearPointerList(&exifCameraModelListRx);
    // IPTC
    clearPointerList(&iptcObjectNameListRx);
    clearPointerList(&iptcKeywordsListRx);
    clearPointerList(&iptcDescriptionListRx);
    clearPointerList(&iptcCountryNameListRx);
    clearPointerList(&iptcCityListRx);
    clearPointerList(&iptcEditStatusListRx);
#endif // SIR_METADATA_SUPPORT
    delete fileSizeExpTree;
    delete fileSizeVector;
    delete imageSizeExpTree;
    delete imageSizeVector;
}

/** Shows selection conditions dialog and selects items on parents ConvertDialog
  * files tree widget.
  * \return Count of selected items.
  * \sa SelectionDialog() importFiles()
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
    QElapsedTimer timer;
    timer.start();
    for (int i=0; i<itemCount && !progressDialog->wasCanceled(); i++) {
        QTreeWidgetItem *item = convertDialog->filesTreeWidget->topLevelItem(i);
        QString filePath = item->text(PathColumn) + QDir::separator()
                + item->text(NameColumn) + '.' + item->text(ExtColumn);
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
    qDebug() << timer.elapsed() << "ms";
    progressDialog->close();
    if (itemsSelected > 0) {
        convertDialog->enableConvertButtons();
        convertDialog->filesTreeWidget->resizeColumnsToContents();
    }
    convertDialog->setCursor(QCursor(Qt::ArrowCursor));
    return itemsSelected;
}

/** Shows selection conditions dialog and loads items to parents ConvertDialog
  * files tree widget.
  * \return Count of imported files; -1 if directory path is empty.
  * \sa SelectionDialog() selectItems()
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
    QElapsedTimer timer;
    timer.start();
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
        convertDialog->filesTreeWidget->statusList->insert(info.absoluteFilePath(),
                                                           NOTCONVERTED);
        QTreeWidgetItem *item = new QTreeWidgetItem(itemList);
        convertDialog->filesTreeWidget->addTopLevelItem(item);
        item->setSelected(params.selectImportedFiles);
        filesImported++;
    }
    qDebug() << timer.elapsed() << "ms";
    progressDialog->close();
    if (filesImported > 0) {
        convertDialog->enableConvertButtons();
        convertDialog->filesTreeWidget->resizeColumnsToContents();
    }
    convertDialog->setCursor(QCursor(Qt::ArrowCursor));
    return filesImported;
}

void Selection::loadSymbols() {
    Settings *s = Settings::instance();
    fileSizeSymbols <<  s->selection.fileSizeSymbol;
    imageSizeSymbols << s->selection.imageWidthSymbol
                     << s->selection.imageHeightSymbol;
}

/** Sets up this objects regular expression lists setupListRegExp() method.
  * \sa setupListRegExp()
  */
void Selection::setupRegExps() {
    setupListRegExp(params.fileName, &fileNameListRx);
#ifdef SIR_METADATA_SUPPORT
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
#endif // SIR_METADATA_SUPPORT
}

/** Clears and sets up list of pointers to regular expressions. This function
  * clears safe list using clearPointerList() method.
  * \param strExp Regular expression string.
  * \param listRx Pointer to list of pointers to regular expression objects.
  * \sa clearPointerList()
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
    QDir sourceDir(dir, convertDialog->csd->fileFilters);
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
  * Returns true if \a date and \a time object values are between value of first
  * and second item (including extreme values) of \a dtArray; otherwise returns false.
  * \note \a dtArray must be 2-item (or more) array; otherwise it will send SIGSEGV
  * (segmentation fault signal) to operating system.
  */
bool Selection::isInTime(const QDate &date, const QTime &time, QDateTime *dtArray) {
    return ( (dtArray[0].date() <= date && date <= dtArray[1].date()) &&
             (dtArray[0].time() <= time && time <= dtArray[1].time()) );
}

/** This is overloaded function.\n
  * Returns true if \a dateTime object value is between value of first
  * and second item (including extreme values) of \a dtArray; otherwise returns false.
  * \note \a dtArray must be 2-item (or more) array; otherwise it will send SIGSEGV
  * (segmentation fault signal) to operating system.
  */
bool Selection::isInTime(const QDateTime &dateTime, QDateTime *dtArray) {
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
    // file name
    if (!isCompatible(info.fileName(), fileNameListRx))
        return false;
    // file size
    fileSizeVector->replace(0, info.size());
    if (!fileSizeExpTree->rootNode()->solve())
        return false;
    // image size
    QSize imgSize;
#ifdef SIR_METADATA_SUPPORT
    MetadataUtils::Metadata *metadata = 0;
    bool setupStructs;
    if (Settings::instance()->metadata.enabled) {
        setupStructs = (params.checkMetadata || params.checkExif || params.checkIPTC);
        metadata = new MetadataUtils::Metadata();
    }
    else
        setupStructs = false;
    if (metadata && metadata->read(info.filePath(), setupStructs))
        imgSize = metadata->imageSize();
    else {
        imgSize = QImage(info.filePath()).size();
        setupStructs = false;
    }
#else
    imgSize = QImage(info.filePath()).size();
#endif // SIR_METADATA_SUPPORT
    imageSizeVector->replace(0, imgSize.width());
    imageSizeVector->replace(1, imgSize.height());
    if (!imageSizeExpTree->rootNode()->solve())
        return false;
#ifdef SIR_METADATA_SUPPORT
    // metadata
    if (setupStructs) {
        MetadataUtils::ExifStruct *exifStruct = metadata->exifStruct();
        MetadataUtils::IptcStruct *iptcStruct = metadata->iptcStruct();
        // check any metadata
        if (params.checkMetadata) {
            if (!isInTime(QDateTime::fromString(exifStruct->originalDate, Qt::ISODate),
                              params.createdDateTime) &&
                    !isInTime(iptcStruct->dateCreated, iptcStruct->timeCreated,
                                  params.createdDateTime))
                return false;
            if (!isInTime(QDateTime::fromString(exifStruct->digitizedDate, Qt::ISODate),
                              params.digitizedDateTime) &&
                    !isInTime(iptcStruct->digitizationDate, iptcStruct->digitizationTime,
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
        // check Exif metadata
        if (params.checkExif) {
            if (!isCompatible(exifStruct->processingSoftware,
                                           exifProcessingSoftwareListRx))
                return false;
            if (!isCompatible(exifStruct->cameraManufacturer,
                                           exifCameraManufacturerListRx))
                return false;
            if (!isCompatible(exifStruct->cameraModel, exifCameraModelListRx))
                return false;
        }
        // check IPTC metadata
        if (params.checkIPTC) {
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
#endif // SIR_METADATA_SUPPORT
    // test success!
    return true;
}
