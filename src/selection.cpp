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
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>

// ________________________________ Selection ________________________________

/** Selection object default constructor. */
Selection::Selection(ConvertDialog *parent) : QObject(parent) {
    convertDialog = parent;
}

/** Selection object destructor. */
Selection::~Selection() {
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
}

/** Shows selection conditions dialog and selects items on parents ConvertDialog
  * files tree widget.
  * \return Count of selected items.
  * \sa SelectionDialog importFiles
  */
int Selection::selectItems() {
    int itemsSelected = 0;
    SelectionParams params;
    SelectionDialog dialog(&params,false,convertDialog);
    if (dialog.exec() == QDialog::Rejected)
        return -1; // dialog canceled
    qDebug() << "File name expression:" << params.fileName;
    setupRegExps(params);
    qDebug() << "File size expression:" << params.fileSizeExp;
    bool (Selection::*compareFileSizeFnPtr)(qint64) =
            getCompareFileSizeFnPtr(params.fileSizeExp);
    bool (Selection::**compareImageSizeFnPtr)(int, const QList<int>&) =
            getCompareImageSizeFnPtrArray(params.imageSizeExp);
    convertDialog->setCursor(QCursor(Qt::WaitCursor));
    int itemCount = convertDialog->filesTreeView->topLevelItemCount();
    QProgressDialog progressDialog(convertDialog);
    progressDialog.setWindowTitle(tr("Checking selection conditions..."));
    progressDialog.setRange(0, itemCount);
    progressDialog.show();
    for (int i=0; i<itemCount; i++) {
        QTreeWidgetItem *item = convertDialog->filesTreeView->topLevelItem(i);
        QString filePath = item->text(2) + QDir::separator() + item->text(0) + '.' + item->text(1);
        qDebug() << filePath;
        progressDialog.setLabelText(filePath);
        progressDialog.setValue(i);
        QFileInfo info(filePath);
        if (!isCompatible(info.fileName(), fileNameListRx))
            continue;
        if (compareFileSizeFnPtr && !((this->*compareFileSizeFnPtr)(info.size())))
            continue;
        QImage img(info.filePath());
        if (!((this->*compareImageSizeFnPtr[0])
               (img.height(),imageSizeComparatorList[0]))
                && !((this->*compareImageSizeFnPtr[1])
                     (img.height(),imageSizeComparatorList[1])))
            continue;
        if (params.checkMetadata || params.checkExif || params.checkIPTC) {
            if (info.suffix().contains("svg",Qt::CaseInsensitive))
                continue;
            MetadataUtils::Metadata metadata;
            if (!metadata.read(info.filePath(),true))
                continue;
            if (params.checkMetadata) {
                // TODO: check any metadata code block
            }
            if (params.checkExif) {
                MetadataUtils::ExifStruct *str = metadata.exifStruct();
                if (!isCompatible(str->processingSoftware, exifProcessingSoftwareListRx))
                    continue;
                if (!isCompatible(str->cameraManufacturer, exifCameraManufacturerListRx))
                    continue;
                if (!isCompatible(str->cameraModel, exifCameraModelListRx))
                    continue;
            }
            if (params.checkIPTC) {
                MetadataUtils::IptcStruct *str = metadata.iptcStruct();
                if (!isCompatible(str->objectName, iptcObjectNameListRx))
                    continue;
                if (!isCompatible(str->keywords.split(' ', QString::SkipEmptyParts),
                                  iptcKeywordsListRx))
                    continue;
                if (!isCompatible(str->caption.split(' ', QString::SkipEmptyParts),
                                  iptcDescriptionListRx)
                        || !isCompatible(str->caption, iptcDescriptionListRx))
                    continue;
                if (!isCompatible(str->countryName, iptcCountryNameListRx))
                    continue;
                if (!isCompatible(str->city, iptcCityListRx))
                    continue;
                if (!isCompatible(str->editStatus, iptcEditStatusListRx))
                    continue;
            }
        }
        // if all conditions succed select item of tree widget and incremate items counter
        item->setSelected(true);
        itemsSelected++;
    }
    delete [] compareImageSizeFnPtr;
    convertDialog->enableConvertButtons();
    convertDialog->resizeColumnsToContents(convertDialog->filesTreeView);
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
    SelectionParams params;
    SelectionDialog dialog(&params,true,convertDialog);
    if (dialog.exec() == QDialog::Rejected)
        return -1; // dialog canceled
    qDebug() << "File name expression:" << params.fileName;
    setupRegExps(params);
    qDebug() << "File size expression:" << params.fileSizeExp;
    bool (Selection::*compareFileSizeFnPtr)(qint64) =
            getCompareFileSizeFnPtr(params.fileSizeExp);
    qDebug() << "Image size expression:" << params.imageSizeExp;
    bool (Selection::**compareImageSizeFnPtr)(int, const QList<int>&) =
            getCompareImageSizeFnPtrArray(params.imageSizeExp);
    if (params.path.isEmpty()) {
        qDebug("Selection::importFiles(): Directory path is empty!");
        return -1;
    }
    convertDialog->setCursor(QCursor(Qt::WaitCursor));
    QFileInfoList fileInfoList;
    qDebug() << "Loaded files into list:"
             << loadFileInfo(params.path, &fileInfoList, params.browseSubdirs);
    QProgressDialog progressDialog(convertDialog);
    progressDialog.setWindowTitle(tr("Checking import conditions..."));
    progressDialog.setRange(0, fileInfoList.length());
    progressDialog.show();
    foreach (QFileInfo info, fileInfoList) {
        qDebug() << info.filePath() << info.size()/1024 << "KiB";
        progressDialog.setLabelText(info.filePath());
        progressDialog.setValue(progressDialog.value()+1);
        if (!isCompatible(info.fileName(), fileNameListRx))
            continue;
        if (compareFileSizeFnPtr && !((this->*compareFileSizeFnPtr)(info.size())))
            continue;
        QImage img(info.filePath());
        if (!((this->*compareImageSizeFnPtr[0])
               (img.height(),imageSizeComparatorList[0]))
                && !((this->*compareImageSizeFnPtr[1])
                     (img.height(),imageSizeComparatorList[1])))
            continue;
        if (params.checkMetadata || params.checkExif || params.checkIPTC) {
            if (info.suffix().contains("svg",Qt::CaseInsensitive))
                continue;
            MetadataUtils::Metadata metadata;
            if (!metadata.read(info.filePath(),true))
                continue;
            if (params.checkMetadata) {
                // TODO: check any metadata code block
            }
            if (params.checkExif) {
                MetadataUtils::ExifStruct *str = metadata.exifStruct();
                if (!isCompatible(str->processingSoftware, exifProcessingSoftwareListRx))
                    continue;
                if (!isCompatible(str->cameraManufacturer, exifCameraManufacturerListRx))
                    continue;
                if (!isCompatible(str->cameraModel, exifCameraModelListRx))
                    continue;
            }
            if (params.checkIPTC) {
                MetadataUtils::IptcStruct *str = metadata.iptcStruct();
                if (!isCompatible(str->objectName, iptcObjectNameListRx))
                    continue;
                if (!isCompatible(str->keywords.split(' ', QString::SkipEmptyParts),
                                  iptcKeywordsListRx))
                    continue;
                if (!isCompatible(str->caption.split(' ', QString::SkipEmptyParts),
                                  iptcDescriptionListRx)
                        || !isCompatible(str->caption, iptcDescriptionListRx))
                    continue;
                if (!isCompatible(str->countryName, iptcCountryNameListRx))
                    continue;
                if (!isCompatible(str->city, iptcCityListRx))
                    continue;
                if (!isCompatible(str->editStatus, iptcEditStatusListRx))
                    continue;
            }
        }
        // if all conditions succed add to tree widget and incremate files counter
        QStringList itemList;
        itemList << info.completeBaseName() << info.suffix() << info.path()
                 << tr("Not converted yet");
        convertDialog->statusList->insert(info.absoluteFilePath(), NOTCONVERTED);
        convertDialog->filesTreeView->addTopLevelItem(new QTreeWidgetItem(itemList));
        filesImported++;
    }
    delete [] compareImageSizeFnPtr;
    convertDialog->enableConvertButtons();
    convertDialog->resizeColumnsToContents(convertDialog->filesTreeView);
    convertDialog->setCursor(QCursor(Qt::ArrowCursor));
    return filesImported;
}

/** Sets up this objects regular expression lists corresponding \a params argument
  * using setupListRegExp() method.
  * \sa setupListRegExp()
  */
void Selection::setupRegExps(const SelectionParams &params) {
    setupListRegExp(params.fileName, &fileNameListRx);
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
        QRegExp *rx = new QRegExp("*", Qt::CaseInsensitive, QRegExp::WildcardUnix);
        listRx->append(rx);
        return;
    }
    foreach (QString str, strExp.split(' ', QString::SkipEmptyParts)) {
        QRegExp *rx = new QRegExp(str, Qt::CaseInsensitive, QRegExp::WildcardUnix);
        listRx->append(rx);
        rx = new QRegExp(str, Qt::CaseInsensitive, QRegExp::FixedString);
        listRx->append(rx);
        rx = new QRegExp(str, Qt::CaseInsensitive, QRegExp::RegExp2);
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
  */
int Selection::loadFileInfo(const QString &dir, QFileInfoList *list, bool recursive) {
    int result = 0;
    qDebug() << "Watching dir:" << dir;
    QDir sourceDir(dir, convertDialog->fileFilters);
    sourceDir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Readable);
    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    list->append(fileInfoList);
    result += fileInfoList.length();
    if (recursive) {
        QDir directory(dir);
        directory.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::Readable
                            | QDir::NoDotAndDotDot);
        foreach (QString d, directory.entryList())
            result += loadFileInfo(dir + QDir::separator() + d, list, recursive);
    }
    return result;
}

/** Returns true if one member of \a rxList list is \a string string compatible.
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

/** Returns true if one member of \a rxList list is one member of \a list
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

/** Returns pointer to function basing on \a exp string. Returns null function
  * pointer if \a exp expression is invalid.
  * \param exp Math expression string.
  * \sa compareFileSize compareFileSizeLeft compareFileSizeRight
  */
Selection::compareFileSizeFnPtr Selection::getCompareFileSizeFnPtr(
        const MetadataUtils::String &exp) {
    fileSizeComparatorList.clear();
    QRegExp rx("(\\d+)"); // what about float ?
    QStringList expressions = exp.split('=');
    if (expressions.length() == 2) {
        int digitPos = (expressions.at(0).contains(rx)) ? 0 : 1;
        appendFileSize(&expressions, digitPos, rx);
        return &Selection::compareFileSize;
    }
    expressions = exp.split('<');
    if (expressions.length() == 2 || expressions.length() == 3) {
        if (expressions.length() == 2) {
            int digitPos = (expressions.at(0).contains(rx)) ? 0 : 1;
            appendFileSize(&expressions, digitPos, rx);
            if (digitPos == 0)
                return &Selection::compareFileSizeRight;
        }
        else {
            appendFileSize(&expressions, 0, rx);
            appendFileSize(&expressions, 2, rx);
        }
        return &Selection::compareFileSizeLeft;
    }
    expressions = exp.split('>');
    if (expressions.length() == 2 || expressions.length() == 3) {
        if (expressions.length() == 2) {
            int digitPos = (expressions.at(0).contains(rx)) ? 0 : 1;
            appendFileSize(&expressions, digitPos, rx);
            if (digitPos == 0)
                return &Selection::compareFileSizeLeft;
        }
        else {
            appendFileSize(&expressions, 0, rx);
            appendFileSize(&expressions, 2, rx);
        }
        return &Selection::compareFileSizeRight;
    }
    int value = exp.toInt();
    if (expressions.length() == 1 && value > 0) {
        fileSizeComparatorList << value;
        return &Selection::compareFileSize;
    }
    return 0; // returns null function pointer if expression is invalid
}

void Selection::appendFileSize(QStringList *expressionList, int index, const QRegExp &rx) {
    expressionList->replace(index, expressionList->value(index).remove(' '));
    rx.indexIn(expressionList->at(index));
    QString digitString = rx.cap();
    double value = digitString.toDouble();
    expressionList->replace(index, expressionList->value(index).remove(digitString));
    if (expressionList->at(index).startsWith('K', Qt::CaseInsensitive))
        value *= 1024.;
    else if (expressionList->at(index).startsWith('M', Qt::CaseInsensitive))
        value *= 1048576.;
    fileSizeComparatorList << static_cast<qint64> (value);
}

/** Returns true if \a value is equal first item of file size comparator list.
  * \sa getCompareFileSizeFnPtr compareFileSizeLeft compareFileSizeRight
  */
bool Selection::compareFileSize(qint64 value) {
    return (value == fileSizeComparatorList.first());
}

/** Returns true if \a value is lower first item of file size comparator list
  * (for 1-item list), true if upper first item of file size comparator list and
  * lower second item of file size comparator list (for 2-items list).
  * Otherwise returns false.\n
  * Name of this function corresponding compare operator (<) direction.
  * I.e. for 1-item list expression is \em s<100; for 2-item \em 100<s<200.
  * \sa getCompareFileSizeFnPtr compareFileSize compareFileSizeRight
  */
bool Selection::compareFileSizeLeft(qint64 value) {
    if (fileSizeComparatorList.length() == 1)
        return (value < fileSizeComparatorList.first());
    else if (fileSizeComparatorList.length() == 2)
        return (fileSizeComparatorList.first() < value
                && value < fileSizeComparatorList.last());
    else
        return false;
}

/** Returns true if \a value is upper first item of file size comparator list
  * (for 1-item list), true if lower first item of file size comparator list and
  * upper second item of file size comparator list (for 2-items list).
  * Otherwise returns false.\n
  * Name of this function corresponding compare operator (>) direction.
  * I.e. for 1-item list expression is \em s>100; for 2-item \em 100>s>200.
  * \sa getCompareFileSizeFnPtr compareFileSize compareFileSizeLeft
  */
bool Selection::compareFileSizeRight(qint64 value) {
    if (fileSizeComparatorList.length() == 1)
        return (value > fileSizeComparatorList.first());
    else if (fileSizeComparatorList.length() == 2)
        return (fileSizeComparatorList.first() > value
                && value > fileSizeComparatorList.last());
    else
        return false;
}

/** Returns double pointer (pointer to dinamic allocated pointers to function array)
  * to function basing on \a exp string. Returns pointer to compareImageSizeTrue()
  * function if \a exp expression is invalid.
  * \param exp Math expression string.
  * \sa compareImageSize compareImageSizeLeft compareImageSizeRight compareImageSizeTrue
  */
Selection::compareImageSizeFnPtr * Selection::getCompareImageSizeFnPtrArray(
        const MetadataUtils::String &exp) {
    imageSizeComparatorList[0].clear();
    imageSizeComparatorList[1].clear();
    Selection::compareImageSizeFnPtr *fnPtrArray = new Selection::compareImageSizeFnPtr[2];
    QRegExp rx("(\\d+)");
    LogicalOperationFlag logicalOperation = None;
    QStringList clauses = exp.split('&', QString::SkipEmptyParts);
    if (clauses.length() == 2)
        logicalOperation = And;
    if (logicalOperation != And) {
        clauses = exp.split('|', QString::SkipEmptyParts);
        if (clauses.length() == 2)
            logicalOperation = Or;
    }
    int arrayIndex = -1;
    if (clauses.isEmpty() || clauses.length() > 2) {
        qDebug("Selection::getCompareImageSizeFnPtrArray(): to much logical operators");
        fnPtrArray[0] = &Selection::compareImageSizeTrue;
        fnPtrArray[1] = &Selection::compareImageSizeTrue;
        return fnPtrArray; // if expression is invalid
    }
    else if (clauses.at(0).contains('y', Qt::CaseInsensitive)) {
        if (clauses.length() == 2) { // 'x' clause must be first!
            QString temp = clauses[0];
            clauses.replace(0, clauses[1]);
            clauses.replace(1, temp);
        }
        else if (clauses.length() == 1) {
            fnPtrArray[0] = &Selection::compareImageSizeTrue;
            arrayIndex = 0;
        }
    }
    foreach (QString clause, clauses) {
        arrayIndex++;
        QStringList expressions = clause.split('=');
        if (expressions.length() == 2) {
            int digitPos = (expressions.at(0).contains(rx)) ? 0 : 1;
            appendImageSize(&expressions, digitPos, rx, arrayIndex);
            fnPtrArray[arrayIndex] = &Selection::compareImageSize;
            continue;
        }
        expressions = clause.split('<');
        if (expressions.length() == 2 || expressions.length() == 3) {
            if (expressions.length() == 2) {
                int digitPos = (expressions.at(0).contains(rx)) ? 0 : 1;
                appendImageSize(&expressions, digitPos, rx, arrayIndex);
                if (digitPos == 0) {
                    fnPtrArray[arrayIndex] = &Selection::compareImageSizeRight;
                    continue;
                }
            }
            else {
                appendImageSize(&expressions, 0, rx, arrayIndex);
                appendImageSize(&expressions, 2, rx, arrayIndex);
            }
            fnPtrArray[arrayIndex] = &Selection::compareImageSizeLeft;
            continue;
        }
        expressions = clause.split('>');
        if (expressions.length() == 2 || expressions.length() == 3) {
            if (expressions.length() == 2) {
                int digitPos = (expressions.at(0).contains(rx)) ? 0 : 1;
                appendImageSize(&expressions, digitPos, rx, arrayIndex);
                if (digitPos == 0)
                    fnPtrArray[arrayIndex] = &Selection::compareImageSizeLeft;
                continue;
            }
            else {
                appendImageSize(&expressions, 0, rx, arrayIndex);
                appendImageSize(&expressions, 2, rx, arrayIndex);
            }
            fnPtrArray[arrayIndex] = &Selection::compareImageSizeRight;
            continue;
        }
        int value = exp.toInt();
        if (expressions.length() == 1 && value > 0) {
            imageSizeComparatorList[arrayIndex] << value;
            fnPtrArray[arrayIndex] = &Selection::compareImageSize;
            continue;
        }
    }
    return fnPtrArray;
}

void Selection::appendImageSize(QStringList *expressionList, int index, const QRegExp &rx, int listId) {
    expressionList->replace(index, expressionList->value(index).remove(' '));
    rx.indexIn(expressionList->at(index));
    QString digitString = rx.cap();
    imageSizeComparatorList[listId] << digitString.toInt();
}

/** Returns true if \a value is equal first item of image size comparator list.
  * \sa getCompareImageSizeFnPtrArray compareImageSizeLeft compareImageSizeRight compareImageSizeTrue
  */
bool Selection::compareImageSize(int value, const QList<int> &list) {
    return (value == list.first());
}

/** Returns true if \a value is lower first item of image size comparator list
  * (for 1-item list), true if upper first item of image size comparator list and
  * lower second item of image size comparator list (for 2-items list).
  * Otherwise returns false.\n
  * Name of this function corresponding compare operator (<) direction.
  * I.e. for 1-item list expression is \em x<100; for 2-item \em 100<x<200.
  * \sa getCompareImageSizeFnPtrArray compareImageSize compareImageSizeRight compareImageSizeTrue
  */
bool Selection::compareImageSizeLeft(int value, const QList<int> &list) {
    if (list.length() == 1)
        return (value < list.first());
    else if (list.length() == 2)
        return (list.first() < value && value < list.last());
    else
        return false;
}

/** Returns true if \a value is upper first item of image size comparator list
  * (for 1-item list), true if lower first item of image size comparator list and
  * upper second item of image size comparator list (for 2-items list).
  * Otherwise returns false.\n
  * Name of this function corresponding compare operator (>) direction.
  * I.e. for 1-item list expression is \em x>100; for 2-item \em 100>x>200.
  * \sa getCompareImageSizeFnPtrArray compareImageSize compareImageSizeLeft compareImageSizeTrue
  */
bool Selection::compareImageSizeRight(int value, const QList<int> &list) {
    if (list.length() == 1)
        return (value > list.first());
    else if (list.length() == 2)
        return (list.first() > value && value > list.last());
    else
        return false;
}

/** Always returns true. This function have provided for pointer to function compatibility.
  * \sa getCompareImageSizeFnPtrArray compareImageSize compareImageSizeLeft compareImageSizeRight
  */
bool Selection::compareImageSizeTrue(int, const QList<int> &) {
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
}

/** Saves selection parameters to \a params structure. */
void SelectionDialog::accept() {
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
    }
    else {
        params->path = "";
        params->browseSubdirs = false;
    }
    QDialog::accept();
}

/** Directory button slot. Gets from the user new directory path and shows this
  * on directory line edit widget.
  */
void SelectionDialog::browseDir() {
    if(convertDialog->lastDir == "") {
        convertDialog->lastDir = QDir::homePath();
    }

    QString dirPath = QFileDialog::getExistingDirectory(
                       this,
                       tr("Choose a directory"),
                       convertDialog->lastDir,
                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    dirPath = QDir::convertSeparators(dirPath);
    params->path = dirPath;
    convertDialog->lastDir = dirPath;
    dirLineEdit->setText(dirPath);
}
