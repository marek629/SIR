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
 * Program URL: http://marek629.github.io/sir/
 */

#include "widgets/TimeComboBox.hpp"
#include <QLineEdit>
#include <QRegExp>

/** Default constructor. */
TimeComboBox::TimeComboBox(QWidget *parent) : QComboBox(parent) {}

void TimeComboBox::addEditedText() {
    addItem(lineEdit()->text());
}

/** Adds item containing typed \b text and \b userData into combo box.\n
  * If item with \b text already exists and duplicates disabled the item will
  * not be added.
  */
void TimeComboBox::addItem(const QString &text, const QVariant &userData) {
    int idx = findText(text);
    if ( idx != -1 && !duplicatesEnabled() )
        setCurrentIndex(idx);
    else { // add item to right index within combobox
        int i = validIndex(text);
        if (i > -1) {
            insertItem(i, text, userData);
            setCurrentIndex(i);
        }
    }
}

int TimeComboBox::validIndex(const QString &text) {
    if (text.isEmpty())
        return -2; // empty text code
    if (!text.contains(QRegExp("*[0-9]*", Qt::CaseSensitive, QRegExp::Wildcard)))
        return -3; // no digits detected code
    QStringList list = text.split(' ');
    if (list.last() == "s")
        list.removeLast();
    if (text.contains('/')) {
        QStringList strFraction = list.at(list.indexOf( QRegExp("*/*", Qt::CaseSensitive,
                                                             QRegExp::Wildcard))).split('/');
        float multiplier = 100.f / strFraction.last().toFloat();
        char numerator = strFraction.first().toInt() * multiplier;
        if (list.length() == 1) {
            double expTimeFraction = strFraction.first().toDouble() /
                    strFraction.last().toDouble();
            for (int i=1; ; i++) {
                QStringList strList = itemText(i).split(' ');
                strFraction = strList.first().split('/');
                double itemFraction = strFraction.first().toDouble() /
                        strFraction.last().toDouble();
                if (strList.length()==2 && itemFraction>expTimeFraction)
                    return i;
            }
        }
        else { // expTime > 1s
            QString intSpace = list.first() + " ";
            int i = findText(intSpace, Qt::MatchStartsWith);
            if (i == -1) {
                int x = intSpace.toInt();
                for (i=count()-1; itemText(i).split(' ').first().toInt() > x; i--) ;
            }
            for (; i<count(); i++) {
                QStringList strList = itemText(i).split(' ');
                if (strList.first() != list.first())
                    break;
                if (strList.length() == 2 && !strList.last().contains('/'))
                    continue;
                QStringList strList2 = strList.at(
                            strList.indexOf(QRegExp("*/*", Qt::CaseSensitive,
                                                    QRegExp::Wildcard)) ).split('/');
                multiplier = 100.f / strList2.last().toFloat();
                int frac = strList2.first().toFloat() * multiplier;
                if (frac > numerator)
                    break;
            }
            return i;
        }
    }
    else { // text without slash ('/')
        int expTimeInt = list.first().toInt();
        for (int i=count()-1; i>-1; i--) {
            int integer = itemText(i).split(' ').first().toInt();
            if (expTimeInt > integer)
                return i+1;
        }
    }
    return -111; // should never happen
}

/** Enables combo box's line edit if \b editable is true, otherwise disables line edit.\n
  * If line edit is enabled and focus off text of line edit will be added into combo box.
  */
void TimeComboBox::setEditable(bool editable) {
    QComboBox::setEditable(editable);
    if (editable)
        connect(lineEdit(), SIGNAL(editingFinished()), SLOT(addEditedText()));
    else
        disconnect(SLOT(addEditedText()));
}
