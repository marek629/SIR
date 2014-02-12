/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2014  Marek Jędryka   <jedryka89@gmail.com>
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

#ifndef XMLHELPER_H
#define XMLHELPER_H

#include <QColor>
#include <QString>
#include <QPointF>

class QDomElement;
class ConvertDialog;

/** \brief Abstract class helping read/write operations on XML files.
  * \sa XmlStreamWriter
  */
class XmlHelper {
public:
    XmlHelper(ConvertDialog *parent = 0);
    virtual ~XmlHelper();
    virtual void save(const QString &fileName) = 0;
    virtual void restore(const QString &fileName) = 0;

protected:
    // fields
    static const QString falseString; /**< False string defined as \e no. */
    ConvertDialog *convertDialog; /**< Parent convert dialog. */
    // methods
    QColor readColor(const QDomElement &parentElement);
    QPointF readPointF(const QDomElement &pointElement);
};

#endif // XMLHELPER_H
