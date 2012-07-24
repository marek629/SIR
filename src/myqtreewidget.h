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

 #ifndef MYQLISTWIDGET_H
 #define MYQLISTWIDGET_H

#include <QTreeWidget>

class QKeyEvent;
class QDropEvent;
class QDragEnterEvent;
class QDragMoveEvent;

//! QTreeWidget reimplementation for ConvertDialog with custom contex menu.
class myQTreeWidget : public QTreeWidget {
    Q_OBJECT
public:
    myQTreeWidget(QWidget *parent = 0);

protected:
    virtual void keyPressEvent( QKeyEvent *k );
    virtual void dropEvent(QDropEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);

signals:
    void changed(); /**< Indicates changes in this tree widget. \sa dropEvent */
};

#endif
