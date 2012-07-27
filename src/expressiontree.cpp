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

#include "expressiontree.h"
#include "selection.h"
#include <QStringList>
#include <QRegExp>

// static const variables

/** Comparison function pointers array corresponding
  * Selection::comparisonOperators string list.
  */
const CompareFunctionNode::fnPtr CompareFunctionNode::fnArray[3] = {
    &CompareFunctionNode::isEqual,
    &CompareFunctionNode::isLower,
    &CompareFunctionNode::isUpper
};

/** Logical function pointers array corresponding Selection::logicalOperators
  * string list.
  */
const LogicalFunctionNode::fnPtr LogicalFunctionNode::fnArray[3] = {
    &LogicalFunctionNode::logicalAnd,
    &LogicalFunctionNode::logicalOr,
    &LogicalFunctionNode::logicalXor
};

// ___________________________________ Node ___________________________________

/** Pure virtual destructor. */
Node::~Node() {}

// __________________________________ IntNode __________________________________

/** Default constructor of IntNode object. \sa init setValue */
IntNode::IntNode(qint64 value) {
    init();
    setValue(value);
}

/** Constructor of IntNode object. \sa init setValue */
IntNode::IntNode(qint64 *value) {
    init();
    setValue(value);
}

/** Destructor of IntNode object. */
IntNode::~IntNode() {}

/** Sets value pointer valuePtr to \a value and valueInt to 0. */
void IntNode::setValue(qint64 *value) {
    valuePtr = value;
    valueInt = 0;
}

/** Sets valueInt to \a value and valuePtr to null pointer. */
void IntNode::setValue(qint64 value) {
    valuePtr = 0;
    valueInt = value;
}

/** Sets value pointer valuePtr to null address and valueInt to 0 while IntNodes
  * object initialization. */
void IntNode::init() {
    valuePtr = 0;
    valueInt = 0;
}

// _______________________________ FunctionNode _______________________________

/** Default constructor of FunctionNode object. */
FunctionNode::~FunctionNode() {}

/** Always returns true. */
bool FunctionNode::solve() { return true; }

// ____________________________ CompareFunctionNode ____________________________

/** Default constructor of CompareFunctionNode object.
  * \sa setFunction setLeftChild setRightChild
  */
CompareFunctionNode::CompareFunctionNode(fnPtr fn, IntNode *leftChild,
                                         IntNode *rightChild) {
    setFunction(fn);
    setLeftChild(leftChild);
    setRightChild(rightChild);
}

/** Deletes both child nodes. */
CompareFunctionNode::~CompareFunctionNode() {
    delete child1;
    delete child2;
}

/** Solves operation function and returns result of this function.
  * \note If function pointer is null returns true.\n
  *       If same child pointer is null or value of same children is invalid
  *       returns false.
  * \sa isEqual isUpper isLower
  */
bool CompareFunctionNode::solve() {
    if (!function)
        return true;
    if (!child1 || !child2)
        return false;
    qint64 a = child1->value();
    qint64 b = child2->value();
    if (a == 0 || b == 0)
        return false;
    return (*function)(a,b);
}

/** Sets function pointer to \a f. \sa isEqual isUpper isLower */
void CompareFunctionNode::setFunction(fnPtr f) {
    this->function = f;
}

/** Sets left child pointer to \a c. */
void CompareFunctionNode::setLeftChild(IntNode *c) {
    this->child1 = c;
}

/** Sets right child pointer to \a c. */
void CompareFunctionNode::setRightChild(IntNode *c) {
    this->child2 = c;
}

/** Returns true if \a a equals \a b. */
bool CompareFunctionNode::isEqual(qint64 a, qint64 b) {
    return (a == b);
}

/** Returns true if \a a is upper than \a b. */
bool CompareFunctionNode::isUpper(qint64 a, qint64 b) {
    return (a > b);
}

/** Returns true if \a a is lower than \a b. */
bool CompareFunctionNode::isLower(qint64 a, qint64 b) {
    return (a < b);
}

// ____________________________ LogicalFunctionNode ____________________________

/** Default constructor of LogicalFunctionNode object.
  * \sa setFunction setLeftChild setRightChild
  */
LogicalFunctionNode::LogicalFunctionNode(fnPtr fn, FunctionNode *leftChild,
                                         FunctionNode *rightChild) {
    setFunction(fn);
    setLeftChild(leftChild);
    setRightChild(rightChild);
}

/** Deletes both child nodes. */
LogicalFunctionNode::~LogicalFunctionNode() {
    delete child1;
    delete child2;
}

/** Returns result of \a function. If same child or function pointer is null
  * returns false.
  * \sa logicalAnd logicalOr logicalXor
  */
bool LogicalFunctionNode::solve() {
    if (!child1 || !child2 || !function)
        return false;
    return ((*function)(child1->solve(), child2->solve()));
}

/** Sets function pointer to \a f. \sa logicalAnd logicalOr logicalXor */
void LogicalFunctionNode::setFunction(fnPtr f) {
    this->function = f;
}

/** Sets left child pointer to \a c. */
void LogicalFunctionNode::setLeftChild(FunctionNode *c) {
    this->child1 = c;
}

/** Sets right child pointer to \a c. */
void LogicalFunctionNode::setRightChild(FunctionNode *c) {
    this->child2 = c;
}

/** Returns true if both arguments are true, otherwise returns false. */
bool LogicalFunctionNode::logicalAnd(bool a, bool b) {
    return (a && b);
}

/** Returns true if at least 1 of arguments is true, otherwise returns false. */
bool LogicalFunctionNode::logicalOr(bool a, bool b) {
    return (a || b);
}

/** Returns true if just 1 of arguments is true, otherwise returns false. */
bool LogicalFunctionNode::logicalXor(bool a, bool b) {
    return (a ^ b);
}

// ___________________________ LogicalExpressionTree ___________________________

/** Default constructor of LogicalExpressionTree object.\n
  * Sets root FunctionNode pointer to null address.
  */
LogicalExpressionTree::LogicalExpressionTree() {
    root = 0;
}

/** Constructor of LogicalExpressionTree object. Sets parent pointer to \a parent address.
  * \sa init
  */
LogicalExpressionTree::LogicalExpressionTree(const QString &exp, const QStringList &symbols,
                                             QVector<qint64> *vars, Selection *parent) {
    this->parent = parent;
    init(exp, symbols, vars);
}

/** Destructor of LogicalExpressionTree object. */
LogicalExpressionTree::~LogicalExpressionTree() {
    delete root;
}

/** Deletes old root node and create new expression tree.
  * \sa init
  */
void LogicalExpressionTree::create(const QString &exp, const QStringList &symbols,
                                   QVector<qint64> *vars) {
    delete root;
    init(exp, symbols, vars);
}

/** Creates expression tree based function paramethers.
  * \param exp      Text expression. If it's empty root will store FunctionNode.
  * \param symbols  Supported symbols of variables. Items are corresponding
  *                 \a vars items.
  * \param vars     Vector storing values of supported variables. Items are
  *                 corresponding \a symbols items.
  * \note If \a symbols and \a vars are different lenght or \a exp is invalid
  *       expression root will store null CompareFunctionNode.
  * \sa FunctionNode::solve CompareFunctionNode::solve
  */
void LogicalExpressionTree::init(const QString &exp, const QStringList &symbols,
                                 QVector<qint64> *vars) {
    if (exp.isEmpty()) {
        root = new FunctionNode();
        return;
    }
    if (symbols.length() != vars->size()) {
        root = new CompareFunctionNode();
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
        if (exp.contains(QRegExp("\\d"))) {
            bool ok;
            qint64 digit = exp.toLongLong(&ok);
            if (ok) {
                root = new CompareFunctionNode(&CompareFunctionNode::isEqual,
                                               new IntNode(&vars->first()),
                                               new IntNode(digit));
                return;
            }
        }
        root = new CompareFunctionNode();
        return;
    }
    QString expression = exp;
    expression.replace(QRegExp("\\s"),"");
    foreach (QString comparisonExp,
             expression.split(logicalOperatorRx, QString::SkipEmptyParts)) {
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
            root = new LogicalFunctionNode();
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
    bool isFileSizeTest(symbols == parent->fileSizeSymbols);
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
                    leftIntNode = new IntNode(const_cast<qint64*>(&vars->at(j)));
                    break;
                }
                else
                    leftIntNode = 0;
            }
        }
        else if (isFileSizeTest)
            leftIntNode = createFileSizeNode(valuesExpList.first());
        else
            leftIntNode = new IntNode(valuesExpList.first().toLongLong());
        if (valuesExpList.at(1).contains(symbolRx)) {
            for (int j=0; j<symbols.length(); j++) {
                if (symbols[j] == valuesExpList[1]) {
                    rigthIntNode = new IntNode(const_cast<qint64*>(&vars->at(j)));
                    break;
                }
                else
                    rigthIntNode = 0;
            }
        }
        else if (isFileSizeTest)
            rigthIntNode = createFileSizeNode(valuesExpList[1]);
        else
            rigthIntNode = new IntNode(valuesExpList.at(1).toLongLong());
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

/** Dynamically allocates IntNode object based \a valueStr string and returns
  * pointer to this object.\n
  * This function supports size multipliers: \em K and \em M. It's corresponding
  * file size units: \em KiB and \em MiB.
  */
IntNode * LogicalExpressionTree::createFileSizeNode(const QString &valueStr) {
    qint32 lastDigitPos = valueStr.lastIndexOf(QRegExp("\\d"));
    qint64 value = valueStr.left(lastDigitPos+1).toLongLong();
    if (lastDigitPos != valueStr.length()-1) {
        QString unit = valueStr.right(valueStr.length()-lastDigitPos);
        if (unit.contains('K',Qt::CaseInsensitive))
            value *= 1024;
        else if (unit.contains('M',Qt::CaseInsensitive))
            value *= 1048576;
    }
    return new IntNode(value);
}

/** Returns string corresponding \a rx contained in \a str.
  * \param[in]      str  Expression string.
  * \param[in]      c    First character of second part of \a str string.
  * \param[in]      rx   Regular expression.
  * \param[in,out]  from Position in \a str from it's begin searching string
  *                      compatible with \a rx regular expression. It'll write
  *                      here new value - position in \a str of first character
  *                      of returned string.
  */
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
