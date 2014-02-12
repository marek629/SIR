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

#include <QFile>
#include <QList>
#include <QRegExp>
#include "svgmodifier.h"
#include "regexputils.h"

/** Creates the SvgModifier object. */
SvgModifier::SvgModifier() {}

/** Creates the SvgModifier object and loads SVG file data.
  * \sa load()
  */
SvgModifier::SvgModifier(const QString &fileName) {
    load(fileName);
}

/** Loads XML data from SVG file called \a fileName file path. */
bool SvgModifier::load(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!document.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();
    return true;
}

/** Deletes all SVG \b text nodes containig \a text parsed text including regular
  * expressions from loaded SVG file.
  * \sa load() removeEmptyGroups()
  */
void SvgModifier::removeText(const QString &text) {
    using namespace RegExpUtils;
    QList<QRegExp*> rxList;
    rxList << new QRegExp(text, Qt::CaseSensitive, QRegExp::FixedString)
           << new QRegExp(text, Qt::CaseSensitive, QRegExp::WildcardUnix)
           << new QRegExp(text, Qt::CaseSensitive, QRegExp::RegExp2);
    QDomNodeList nodeList = document.elementsByTagName("text");
    for (int i=0; i<nodeList.count(); ) {
        QDomNode node = nodeList.at(i);
        if (isCompatible(node.toElement().text(), rxList))
            node.parentNode().removeChild(node);
        // increment index if node wasn't removed only because nodeList is reduced
        // while node removing
        else
            i++;
    }
    clearPointerList(&rxList);
}

/** Deletes all empty SVG \b g nodes from loaded SVG file.
  * \sa load() removeText()
  */
void SvgModifier::removeEmptyGroups() {
    QDomNodeList nodeList = document.elementsByTagName("g");
    for (int i=0; i<nodeList.count(); ) {
        QDomNode node = nodeList.at(i);
        if (!node.hasChildNodes())
            node.parentNode().removeChild(node);
        // increment index if node wasn't removed only because nodeList is reduced
        // while node removing
        else
            i++;
    }
}
