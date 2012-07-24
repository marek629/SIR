/*
 * This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto
 * 2011-2012  Marek Jędryka
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
 * Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
 *                 Marek Jędryka   <jedryka89@gmail.com>
 * Program URL: http://sir.projet-libre.org/
 *
 */

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>

//! Message box providing more buttons than QMessageBox.
class MessageBox : public QMessageBox {
    Q_OBJECT
public:
    explicit MessageBox(QWidget *parent = 0);
    static int question(QWidget *parent, const QString &title, const QString &text);
    enum Button {
        Yes=0,
        YesToAll=2,
        No=1,
        NoToAll=3,
        Cancel=4
    }; /**< Button codes enumeration. */
};

#endif // MESSAGEBOX_H
