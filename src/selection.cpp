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

// static variables
QStringList Selection::logicalOperators = QStringList() << "&" << "|" << "^";
QStringList Selection::comparisonOperators = QStringList() << "=" << "<" << ">";
QStringList Selection::allOperators = QStringList() <<
        Selection::logicalOperators << Selection::comparisonOperators;
CompareFunctionNode::fnPtr CompareFunctionNode::fnArray[3] = {
    &CompareFunctionNode::isEqual,
    &CompareFunctionNode::isLower,
    &CompareFunctionNode::isUpper
};
LogicalFunctionNode::fnPtr LogicalFunctionNode::fnArray[3] = {
    &LogicalFunctionNode::logicalAnd,
    &LogicalFunctionNode::logicalOr,
    &LogicalFunctionNode::logicalXor
};

// ________________________________ Selection ________________________________

/** Selection object default constructor. */
Selection::Selection(ConvertDialog *parent) : QObject(parent) {
    convertDialog = parent;
    fileSizeExpTree = 0;
    fileSizeSymbols << "s";
    fileSizeVector = new QVector<int>(fileSizeSymbols.count());
    imageSizeExpTree = 0;
    imageSizeSymbols << "x" << "y";
    imageSizeVector = new QVector<int>(imageSizeSymbols.count());
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
    SelectionParams params;
    SelectionDialog dialog(&params,false,convertDialog);
    if (dialog.exec() == QDialog::Rejected)
        return -1; // dialog canceled
    qDebug() << "File name expression:" << params.fileName;
    setupRegExps(params);
    qDebug() << "File size expression:" << params.fileSizeExp;
    delete fileSizeExpTree;
    fileSizeExpTree = new LogicalExpressionTree(params.fileSizeExp, fileSizeSymbols,
                                                fileSizeVector);
    delete imageSizeExpTree;
    imageSizeExpTree = new LogicalExpressionTree(params.imageSizeExp, imageSizeSymbols,
                                                 imageSizeVector);
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
        bool b = fileSizeExpTree->rootNode()->solve();
        qDebug() << "file size" << b;
        if (!b)
            continue;
        QImage img(info.filePath());
        imageSizeVector->replace(0, img.width());
        imageSizeVector->replace(1, img.height());
        b = imageSizeExpTree->rootNode()->solve();
        qDebug() << "image size" << b <<
                    imageSizeVector->size() << imageSizeVector->at(0) << imageSizeVector->at(1);
        if (!b)
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
    if (params.path.isEmpty()) {
        qDebug("Selection::importFiles(): Directory path is empty!");
        return -1;
    }
    delete fileSizeExpTree;
    fileSizeExpTree = new LogicalExpressionTree(params.fileSizeExp, fileSizeSymbols,
                                                fileSizeVector);
    delete imageSizeExpTree;
    imageSizeExpTree = new LogicalExpressionTree(params.imageSizeExp, imageSizeSymbols,
                                                 imageSizeVector);
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
//        if (compareFileSizeFnPtr && !((this->*compareFileSizeFnPtr)(info.size())))
//            continue;
        QImage img(info.filePath());
//        if (!((this->*compareImageSizeFnPtr[0])
//               (img.height(),imageSizeComparatorList[0]))
//                && !((this->*compareImageSizeFnPtr[1])
//                     (img.height(),imageSizeComparatorList[1])))
//            continue;
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

// =============================================================================
// ========================== Expression tree section ==========================
// =============================================================================

// ___________________________________ Node ___________________________________

Node::~Node() {}

// __________________________________ IntNode __________________________________

IntNode::IntNode(int *value, bool constant) {
    this->valuePtr = value;
    this->constant = constant;
}

IntNode::~IntNode() {
    if (constant)
        delete valuePtr;
}

// ____________________________ CompareFunctionNode ____________________________

FunctionNode::~FunctionNode() {}

bool FunctionNode::solve() { return true; }

// ____________________________ CompareFunctionNode ____________________________

CompareFunctionNode::CompareFunctionNode() {
    function = 0;
    child1 = 0;
    child2 = 0;
}

CompareFunctionNode::CompareFunctionNode(fnPtr fn, IntNode *leftChild,
                                         IntNode *rightChild) {
    setFunction(fn);
    setLeftChild(leftChild);
    setRightChild(rightChild);
}

CompareFunctionNode::~CompareFunctionNode() {
    delete child1;
    delete child2;
}

bool CompareFunctionNode::solve() {
    if (!function)
        return true;
    if (!child1 || !child2)
        return false;
    int a = child1->value();
    int b = child2->value();
    if (a == -1 || b == -1)
        return false;
    return (*function)(a,b);
}

void CompareFunctionNode::setFunction(fnPtr f) {
    this->function = f;
}

void CompareFunctionNode::setLeftChild(IntNode *c) {
    this->child1 = c;
}

void CompareFunctionNode::setRightChild(IntNode *c) {
    this->child2 = c;
}

bool CompareFunctionNode::isEqual(int a, int b) {
    return (a == b);
}

bool CompareFunctionNode::isUpper(int a, int b) {
    return (a > b);
}

bool CompareFunctionNode::isLower(int a, int b) {
    return (a < b);
}

// ____________________________ LogicalFunctionNode ____________________________

LogicalFunctionNode::LogicalFunctionNode() {
    function = 0;
    child1 = 0;
    child2 = 0;
}

LogicalFunctionNode::LogicalFunctionNode(fnPtr fn, FunctionNode *leftChild,
                                       FunctionNode *rightChild) {
    setFunction(fn);
    setLeftChild(leftChild);
    setRightChild(rightChild);
}

LogicalFunctionNode::~LogicalFunctionNode() {
    delete child1;
    delete child2;
}

bool LogicalFunctionNode::solve() {
    if (!child1 || !child2 || !function)
        return false;
    return ((*function)(child1->solve(), child2->solve()));
}

void LogicalFunctionNode::setFunction(fnPtr f) {
    this->function = f;
}

void LogicalFunctionNode::setLeftChild(FunctionNode *c) {
    this->child1 = c;
}

void LogicalFunctionNode::setRightChild(FunctionNode *c) {
    this->child2 = c;
}

bool LogicalFunctionNode::logicalAnd(bool a, bool b) {
    return (a && b);
}

bool LogicalFunctionNode::logicalOr(bool a, bool b) {
    return (a || b);
}

bool LogicalFunctionNode::logicalXor(bool a, bool b) {
    return (a ^ b);
}

// ___________________________ LogicalExpressionTree ___________________________

LogicalExpressionTree::LogicalExpressionTree(const QString &exp,
                                             const QStringList &symbols, QVector<int> *vars) {
    if (exp.isEmpty()) {
        root = new FunctionNode();
        return;
    }
    if (symbols.length() != vars->size()) {
        root = new CompareFunctionNode(0,0,0);
        return;
    }
    QString logicalOperatorRxString = "\\" + Selection::logicalOperators.first();
    for (int i=1; i<Selection::logicalOperators.count(); i++)
        logicalOperatorRxString += "|\\" + Selection::logicalOperators[i];
    QString comparisonOperatorRxString = Selection::comparisonOperators.first();
    for (int i=1; i<Selection::comparisonOperators.count(); i++)
        comparisonOperatorRxString += '|' + Selection::comparisonOperators[i];
    QString symbolRxString = symbols.first();
    for (int i=1; i<symbols.length(); i++)
        symbolRxString += '|' + symbols[i];
    QRegExp logicalOperatorRx(logicalOperatorRxString);
    QRegExp comparisonOperatorRx(comparisonOperatorRxString);
    QRegExp symbolRx(symbolRxString);
    if (!exp.contains(comparisonOperatorRx)) {
        root = new CompareFunctionNode(0,0,0);
        return;
    }
    QString expression = exp;
    expression.replace(QRegExp("\\s"),"");
    foreach (QString comparisonExp, exp.split(logicalOperatorRx, QString::SkipEmptyParts)) {
        QStringList valueList = comparisonExp.split(comparisonOperatorRx,
                                                    QString::SkipEmptyParts);
        if (valueList.length() != 3)
            continue;
        QStringList ops;
        int idx = 0;
        for (int j=1; j<valueList.length(); j++) {
            idx++;
            ops << rxString(comparisonExp, valueList[j][0], comparisonOperatorRx, &idx);
        }
        if (ops[0] != ops[1]) {
            root = new LogicalFunctionNode(0,0,0);
            return;
        }
        QString op = ops[0];
        QString newComparisonExp = valueList[0] + op + valueList[1]
                // AND operator should be first operator on logicalOperators list
                + Selection::logicalOperators[0]
                + valueList[1] + op + valueList[2];
        expression.replace(comparisonExp, newComparisonExp);
    }
    root = 0;
    IntNode *leftIntNode = 0;
    IntNode *rigthIntNode = 0;
    FunctionNode *leftFunctionNode = 0;
    FunctionNode *rigthFunctionNode = 0;
    QStringList comparisonExpList = expression.split(logicalOperatorRx,
                                                     QString::SkipEmptyParts);
    for (int i=0; i<comparisonExpList.length(); i++) {
        QString comparisonExp = comparisonExpList[i];
        QStringList valuesExpList = comparisonExp.split(comparisonOperatorRx,
                                                        QString::SkipEmptyParts);
        if (valuesExpList.count() != 2) {
            if (i%2 == 0)
                leftFunctionNode = new CompareFunctionNode(0,0,0);
            else
                rigthFunctionNode = new CompareFunctionNode(0,0,0);
            continue;
        }
        if (valuesExpList.first().contains(symbolRx)) {
            for (int j=0; j<symbols.length(); j++) {
                if (symbols[j] == valuesExpList.first()) {
                    leftIntNode = new IntNode((int*)(&vars->at(j)),false);
                    break;
                }
                else
                    leftIntNode = 0;
            }
        }
        else
            leftIntNode = new IntNode(new int(valuesExpList.first().toInt()));
        if (valuesExpList.at(1).contains(symbolRx)) {
            for (int j=0; j<symbols.length(); j++) {
                if (symbols[j] == valuesExpList[1]) {
                    rigthIntNode = new IntNode((int*)(&vars->at(j)),false);
                    break;
                }
                else
                    rigthIntNode = 0;
            }
        }
        else
            rigthIntNode = new IntNode(new int(valuesExpList.at(1).toInt()));
        CompareFunctionNode::fnPtr compareFn = 0;
        QString op = rxString(comparisonExp, valuesExpList[1][0], comparisonOperatorRx);
        for (int j=0; j<Selection::comparisonOperators.length(); j++) {
            if (op == Selection::comparisonOperators[j]) {
                compareFn = CompareFunctionNode::fnArray[j];
                break;
            }
        }
        if (i%2 == 0) {
            leftFunctionNode = new CompareFunctionNode(compareFn, leftIntNode, rigthIntNode);
            continue;
        }
        else
            rigthFunctionNode = new CompareFunctionNode(compareFn, leftIntNode, rigthIntNode);
        LogicalFunctionNode::fnPtr logicalFn = 0;
        op = rxString(expression, comparisonExp[0], logicalOperatorRx);
        for (int j=0; j<Selection::logicalOperators.length(); j++) {
            if (op == Selection::logicalOperators[j]) {
                logicalFn = LogicalFunctionNode::fnArray[j];
                break;
            }
        }
        if (i > 2) {
            if (i%2 != 0)
                root = new LogicalFunctionNode(logicalFn, leftFunctionNode, root);
            else
                root = new LogicalFunctionNode(logicalFn, root, rigthFunctionNode);
        }
        else
            root = new LogicalFunctionNode(logicalFn, leftFunctionNode, rigthFunctionNode);
    }
    if (!root)
        root = leftFunctionNode;
}

LogicalExpressionTree::~LogicalExpressionTree() {
    delete root;
}

QString LogicalExpressionTree::rxString(const QString &str, QChar c,
                                        const QRegExp &rx, int *from) {
    int index = 0;
    if (from)
        index = *from;
    index = str.indexOf(rx, index);
    if (from)
        *from = index;
    int length = str.indexOf(c, index) - index;
    return str.mid(index, length);
}
