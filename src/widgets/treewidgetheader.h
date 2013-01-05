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

#ifndef TREEWIDGETHEADER_H
#define TREEWIDGETHEADER_H

#include <QHeaderView>

//! Header view of TreeWidget class with custom context menu.
class TreeWidgetHeader : public QHeaderView {
    Q_OBJECT

public:
    TreeWidgetHeader(QWidget *parent = 0);
    ~TreeWidgetHeader();
    void loadSettings();
    void saveSettings();

protected:
    virtual void showEvent(QShowEvent *event);

private:
    // fields
    QList<QAction*> actions;
    // methods
    void createActions();

private slots:
    void showMenu(const QPoint &point);
    void columnTriggered(bool visible);
};

#endif // TREEWIDGETHEADER_H
