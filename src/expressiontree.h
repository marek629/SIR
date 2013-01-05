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

#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H

#include <QtGlobal>
#include <QVector>

class QChar;
class QStringList;
class QRegExp;
class Selection;

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
    /** Returns true if this IntNode object's storing constant value; otherwise
        returns false, ie. when this object's storing pointer to variable. */
    bool isConst() const { return (!valuePtr); }
    /** Returns 64-bit representation of storing value. \sa value32 */
    qint64 value() const { return (valuePtr) ? (*valuePtr) : valueInt; }
    /** Returns 32-bit representation of storing value. \sa value */
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
    virtual bool solve();
};

//! The compare function node of LogicalExpressionTree
class CompareFunctionNode : public FunctionNode {
public:
    /** Simply name for pointer to function taking 2 value of children IntNode nodes. */
    typedef bool(*fnPtr)(qint64,qint64);
    CompareFunctionNode(fnPtr fn = 0, IntNode *leftChild = 0, IntNode *rightChild = 0);
    virtual ~CompareFunctionNode();
    virtual bool solve();
    void setFunction(fnPtr);
    void setLeftChild(IntNode *);
    void setRightChild(IntNode *);
    static bool isEqual(qint64,qint64);
    static bool isUpper(qint64,qint64);
    static bool isLower(qint64,qint64);
    static const fnPtr fnArray[3];

private:
    fnPtr function;
    IntNode *child1;
    IntNode *child2;
};

//! The compare function node of LogicalExpressionTree
class LogicalFunctionNode : public FunctionNode {
public:
    /** Simply name for pointer to function taking 2 value of children FunctionNode
      * (or a inheriting class) nodes.
      */
    typedef bool(*fnPtr)(bool,bool);
    LogicalFunctionNode(fnPtr fn = 0, FunctionNode *leftChild = 0, FunctionNode *rightChild = 0);
    virtual ~LogicalFunctionNode();
    virtual bool solve();
    void setFunction(fnPtr);
    void setLeftChild(FunctionNode *);
    void setRightChild(FunctionNode *);
    static bool logicalAnd(bool,bool);
    static bool logicalOr(bool,bool);
    static bool logicalXor(bool,bool);
    static const fnPtr fnArray[3];

private:
    fnPtr function;
    FunctionNode *child1;
    FunctionNode *child2;
};

//! Binary tree of logical operations needed to solve image size expression.
class LogicalExpressionTree {
public:
    LogicalExpressionTree();
    LogicalExpressionTree(const QString &exp, const QStringList &symbols,
                          QVector<qint64> *vars, Selection *parent = 0);
    ~LogicalExpressionTree();
    FunctionNode *rootNode() const { return root; } /**< Returns pointer to root FunctionNode. */
    void create(const QString &exp, const QStringList &symbols, QVector<qint64> *vars);

private:
    // fields
    FunctionNode *root;
    Selection *parent;
    // methods
    void init(const QString &exp, const QStringList &symbols, QVector<qint64> *vars);
    QString rxString(const QString &str, QChar c, const QRegExp &rx, int *from = 0);
    IntNode *createFileSizeNode(const QString &valueStr);
};

#endif // EXPRESSIONTREE_H
