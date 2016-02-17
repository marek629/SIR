/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2016  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://marek629.github.io/SIR/
 */

#include "widgets/MessageBox.hpp"

#include <QAbstractButton>


bool MessageBox::testingEnabled = false;
int MessageBox::warningStandardButton = QMessageBox::Ok;


MessageBox::MessageBox(QMessageBox::Icon icon, const QString &title,
                       const QString &text, QMessageBox::StandardButtons buttons,
                       QWidget *parent)
    : QMessageBox(icon, title, text, buttons, parent) {}

void MessageBox::enableTesting(bool enabled) {
    MessageBox::testingEnabled = enabled;
}

int MessageBox::warning(QWidget *parent, const QString &title, const QString &text) {
    if (MessageBox::testingEnabled)
        return MessageBox::warningStandardButton;
    else
        return QMessageBox::warning(parent, title, text);
}

/** Shows question message containing \a text message and \a title titled window. */
int MessageBox::question(QWidget *parent, const QString &title, const QString &text)
{
    StandardButtons buttons = Yes | No | YesToAll | NoToAll | Cancel;
//    MessageBox messageBox = QMessageBox::question(parent, title, text, buttons, No);
    MessageBox messageBox(Question, title, text, buttons, parent);

    messageBox.button(Yes)->setText(tr("&Yes"));
    messageBox.button(No)->setText(tr("&No"));
    messageBox.button(YesToAll)->setText(tr("Yes to &All"));
    messageBox.button(NoToAll)->setText(tr("N&o to All"));
    messageBox.button(Cancel)->setText(tr("&Cancel"));

    return messageBox.exec();
}
