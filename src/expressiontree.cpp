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

#include "expressiontree.h"
#include "selection.h"
#include <QStringList>
#include <QRegExp>

// ___________________________________ Node ___________________________________

Node::~Node() {}

// __________________________________ IntNode __________________________________

IntNode::IntNode(qint64 value) {
    init();
    setValue(value);
}

IntNode::IntNode(qint64 *value) {
    init();
    setValue(value);
}

IntNode::~IntNode() {}

void IntNode::setValue(qint64 *value) {
    valuePtr = value;
    valueInt = 0;
}

void IntNode::setValue(qint64 value) {
    if (!isConst())
        delete valuePtr;
    valuePtr = 0;
    valueInt = value;
}

void IntNode::init() {
    valuePtr = 0;
    valueInt = 0;
}

// _______________________________ FunctionNode _______________________________

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
    qint64 a = child1->value();
    qint64 b = child2->value();
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

bool CompareFunctionNode::isEqual(qint64 a, qint64 b) {
    return (a == b);
}

bool CompareFunctionNode::isUpper(qint64 a, qint64 b) {
    return (a > b);
}

bool CompareFunctionNode::isLower(qint64 a, qint64 b) {
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

LogicalExpressionTree::LogicalExpressionTree() {
    root = 0;
}

LogicalExpressionTree::LogicalExpressionTree(const QString &exp, const QStringList &symbols,
                                             QVector<qint64> *vars) {
    init(exp, symbols, vars);
}

LogicalExpressionTree::~LogicalExpressionTree() {
    delete root;
}

void LogicalExpressionTree::init(const QString &exp, const QStringList &symbols,
                                 QVector<qint64> *vars) {
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
        root = new CompareFunctionNode(0,0,0);
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
                    leftIntNode = new IntNode(const_cast<qint64*>(&vars->at(j)));
                    break;
                }
                else
                    leftIntNode = 0;
            }
        }
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
