/*
* This file is part of SIR, an open-source cross-platform Image tool
* 2007  Rafael Sachetto
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
* Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
* Program URL: http://sir.projet-libre.org/
*
*/

#include "messagebox.h"

MessageBox::MessageBox(QWidget *parent) :
    QMessageBox(parent)
{
}

int MessageBox::question(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox box(parent);
    box.setWindowTitle(title);
    box.setText(text);
    box.setIcon(Question);
    QPushButton* yesButton = box.addButton(tr("&Yes"), QMessageBox::YesRole);
    QPushButton* noButton = box.addButton(tr("&No"), QMessageBox::NoRole);
    QPushButton* yesToAllButton = box.addButton(tr("Yes to &All"), QMessageBox::YesRole);
    QPushButton* noToAllButton = box.addButton(tr("N&o to All"), QMessageBox::NoRole);
    QPushButton* cancelButton = box.addButton(tr("&Cancel"), QMessageBox::RejectRole);
    box.exec();
    if (box.clickedButton() == (QAbstractButton*)yesButton)
        return Yes;
    if (box.clickedButton() == (QAbstractButton*)noButton)
        return No;
    if (box.clickedButton() == (QAbstractButton*)yesToAllButton)
        return YesToAll;
    if (box.clickedButton() == (QAbstractButton*)noToAllButton)
        return NoToAll;
    if (box.clickedButton() == (QAbstractButton*)cancelButton)
        return Cancel;
}
