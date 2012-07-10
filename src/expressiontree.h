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

#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H

#include <QtGlobal>
#include <QVector>

class QChar;
class QStringList;
class QRegExp;

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

#endif // EXPRESSIONTREE_H
