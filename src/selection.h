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

#ifndef SELECTION_H
#define SELECTION_H

#include "ui_selectiondialog.h"
#include "metadatautils.h"
#include <QFileInfoList>

class ConvertDialog;

class LogicalExpressionTree;

//! Conditionaly selection parameters struct.
struct SelectionParams {
    // directory
    QString path;
    bool browseSubdirs;
    // file
    MetadataUtils::String fileName;
    MetadataUtils::String fileSizeExp; // file size expression
    MetadataUtils::String imageSizeExp;
    // any metadata
    bool checkMetadata;
    QDateTime createdDateTime[2];
    QDateTime digitizedDateTime[2];
    MetadataUtils::String author;
    MetadataUtils::String copyright;
    // Exif
    bool checkExif;
    MetadataUtils::String processingSoftware;
    MetadataUtils::String cameraManufacturer;
    MetadataUtils::String cameraModel;
    // IPTC
    bool checkIPTC;
    MetadataUtils::String objectName;
    MetadataUtils::String keywords;
    MetadataUtils::String description;
    MetadataUtils::String countryName;
    MetadataUtils::String city;
    MetadataUtils::String editStatus;
};

//! Files conditionaly selection class.
class Selection : public QObject {
    Q_OBJECT
    friend class LogicalExpressionTree;
    typedef bool(Selection::*compareFileSizeFnPtr)(qint64);
    typedef bool(Selection::*compareImageSizeFnPtr)(int, const QList<int>&);
    typedef bool(Selection::*logicalFnPtr)(bool,bool);

public:
    Selection(ConvertDialog *parent = 0);
    ~Selection();
    int selectItems();
    int importFiles();

private:
    // fields
    SelectionParams params;
    ConvertDialog *convertDialog;
    QList<QRegExp*> fileNameListRx;
    QList<QRegExp*> anyAuthorListRx;
    QList<QRegExp*> anyCopyrightListRx;
    QList<QRegExp*> exifProcessingSoftwareListRx;
    QList<QRegExp*> exifCameraManufacturerListRx;
    QList<QRegExp*> exifCameraModelListRx;
    QList<QRegExp*> iptcObjectNameListRx;
    QList<QRegExp*> iptcKeywordsListRx;
    QList<QRegExp*> iptcDescriptionListRx;
    QList<QRegExp*> iptcCountryNameListRx;
    QList<QRegExp*> iptcCityListRx;
    QList<QRegExp*> iptcEditStatusListRx;
    static QStringList logicalOperators;
    static QStringList comparisonOperators;
    static QStringList allOperators;
    QStringList fileSizeSymbols;
    LogicalExpressionTree *fileSizeExpTree;
    QVector<qint64> *fileSizeVector;
    QStringList imageSizeSymbols;
    LogicalExpressionTree *imageSizeExpTree;
    QVector<qint64> *imageSizeVector;
    // methods
    void setupRegExps();
    void setupListRegExp(const MetadataUtils::String &strExp, QList<QRegExp*> *listRx);
    int loadFileInfo(const QString &dir, QFileInfoList *list, bool recursive);
    bool isCompatible(const QString &string, const QList<QRegExp*> &rxList);
    bool isCompatible(const QStringList &list, const QList<QRegExp*> &rxList);
    bool isCompatible(const QDate &date, const QTime &time, QDateTime *dtArray);
    bool isCompatible(const QDateTime &dateTime, QDateTime *dtArray);
    bool testFile(const QFileInfo &info);
    void clearPointerList(QList<QRegExp*> *list);
    void setupExpressionTrees();
};

//! Selection conditions dialog class.
class SelectionDialog : public QDialog, public Ui::SelectionDialog {
    Q_OBJECT

public:
    explicit SelectionDialog(SelectionParams *params, bool getDirPath,
                             QWidget *parent = 0, Qt::WindowFlags f = 0);

public slots:
    void accept();
    void browseDir();

private:
    SelectionParams *params;
    ConvertDialog *convertDialog;
};

//========================== Expression tree section ==========================

/** \brief Base abstract class of logical expression tree using in Selection class.
  *
  * This class is empty; exist for derived classes type compatibility only.
  * \sa LogicalExpressionTree
  */
class Node {
public:
    virtual ~Node() = 0;
};

//! The numerical (64-bit integer) value node class.
class IntNode : public Node {
public:
    IntNode(qint64 value = 0);
    IntNode(qint64 *value);
    ~IntNode();
    bool isConst() const { return (!valuePtr); }
    qint64 value() const { return (valuePtr) ? (*valuePtr) : valueInt; }
    int value32() const { return (valuePtr) ? (*valuePtr) : valueInt; }
    void setValue(qint64 *value);
    void setValue(qint64 value);

private:
    // fields
    qint64 *valuePtr;
    qint64 valueInt;
    // methods
    void init();
};

//! Base class of expression solver function nodes.
class FunctionNode : public Node {
public:
    virtual ~FunctionNode();
    virtual bool solve(); /**< Solves operation function and returns result
                                   of this function. */
};

class CompareFunctionNode : public FunctionNode {

public:
    typedef bool(*fnPtr)(qint64,qint64);
    CompareFunctionNode();
    CompareFunctionNode(fnPtr fn, IntNode *leftChild, IntNode *rightChild);
    virtual ~CompareFunctionNode();
    virtual bool solve();
    void setFunction(fnPtr);
    void setLeftChild(IntNode *);
    void setRightChild(IntNode *);
    static bool isEqual(qint64,qint64);
    static bool isUpper(qint64,qint64);
    static bool isLower(qint64,qint64);
    static fnPtr fnArray[3];

private:
    fnPtr function;
    IntNode *child1;
    IntNode *child2;
};

class LogicalFunctionNode : public FunctionNode {

public:
    typedef bool(*fnPtr)(bool,bool);
    LogicalFunctionNode();
    LogicalFunctionNode(fnPtr fn, FunctionNode *leftChild, FunctionNode *rightChild);
    virtual ~LogicalFunctionNode();
    virtual bool solve();
    void setFunction(fnPtr);
    void setLeftChild(FunctionNode *);
    void setRightChild(FunctionNode *);
    static bool logicalAnd(bool,bool);
    static bool logicalOr(bool,bool);
    static bool logicalXor(bool,bool);
    static fnPtr fnArray[3];

private:
    fnPtr function;
    FunctionNode *child1;
    FunctionNode *child2;
};

//! Binary tree of logical operations needed to solve image size expression.
class LogicalExpressionTree {
public:
    LogicalExpressionTree();
    LogicalExpressionTree(const QString &exp, const QStringList &symbols, QVector<qint64> *vars);
    ~LogicalExpressionTree();
    FunctionNode *rootNode() const { return root; }

private:
    // fields
    FunctionNode *root;
    // methods
    void init(const QString &exp, const QStringList &symbols, QVector<qint64> *vars);
    QString rxString(const QString &str, QChar c, const QRegExp &rx, int *from = 0);
};

#endif // SELECTION_H
