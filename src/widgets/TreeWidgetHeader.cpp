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

#include <QMenu>
#include <QShowEvent>
#include "widgets/TreeWidgetHeader.hpp"
#include "widgets/TreeWidget.hpp"
#include "Settings.hpp"
#include "optionsenums.h"

/** Default constructor.
  *
  * Creates horizontal header view object.
  */
TreeWidgetHeader::TreeWidgetHeader(QWidget *parent)
    : QHeaderView(Qt::Horizontal, parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setStretchLastSection(true);
    setDefaultAlignment(Qt::AlignLeft);
    createActions();
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
}

/** Updates settings variables.
  * \sa saveSettings()
  */
TreeWidgetHeader::~TreeWidgetHeader() {
    saveSettings();
}

/** Loads settings of tree widget header.
  *
  * Hides unwanted columns if parent window is visible, otherwise do nothing.
  *
  * \note setSectionHidden() method doesn't work before ConvertDialog parent
  *       window was showed, so this method brakes if said situation occurred
  * \sa showEvent() saveSettings()
  */
void TreeWidgetHeader::loadSettings() {
    if (window()->isHidden())
        return;
    int hex = Settings::instance()->treeWidget.columns;
    setSectionHidden(NameColumn,      !(hex & TreeWidgetOptions::NameColumn));
    setSectionHidden(ExtColumn,       !(hex & TreeWidgetOptions::ExtColumn));
    setSectionHidden(PathColumn,      !(hex & TreeWidgetOptions::PathColumn));
    setSectionHidden(ImageSizeColumn, !(hex & TreeWidgetOptions::ImageSizeColumn));
    setSectionHidden(FileSizeColumn,  !(hex & TreeWidgetOptions::FileSizeColumn));
    setSectionHidden(StatusColumn,    !(hex & TreeWidgetOptions::StatusColumn));
    for (int i=0; i<actions.count(); i++)
        actions[i]->setChecked(!isSectionHidden(i));
}

/** Sets new values of settings stored in memory.
  * \sa loadSettings() Settings::writeSettings()
  */
void TreeWidgetHeader::saveSettings() {
    int hex = 0;
    if (actions[NameColumn]->isChecked())
        hex |= TreeWidgetOptions::NameColumn;
    if (actions[ExtColumn]->isChecked())
        hex |= TreeWidgetOptions::ExtColumn;
    if (actions[PathColumn]->isChecked())
        hex |= TreeWidgetOptions::PathColumn;
    if (actions[ImageSizeColumn]->isChecked())
        hex |= TreeWidgetOptions::ImageSizeColumn;
    if (actions[FileSizeColumn]->isChecked())
        hex |= TreeWidgetOptions::FileSizeColumn;
    if (actions[StatusColumn]->isChecked())
            hex |= TreeWidgetOptions::StatusColumn;
    Settings::instance()->treeWidget.columns = hex;
}

/** Loads settings if \a event is QEvent::Show (tree widget just was showed).
  * \sa loadSettings()
  */
void TreeWidgetHeader::showEvent(QShowEvent *event) {
    if (event->type() == QEvent::Show)
        loadSettings();
}

/** Creates actions of custom menu. */
void TreeWidgetHeader::createActions() {
    TreeWidget *treeWidget = (TreeWidget*)parentWidget();
    QStringList names = treeWidget->columnsNames();
    for (int i=0; i<names.count(); i++) {
        QAction *action = new QAction(names[i], this);
        action->setCheckable(true);
        action->setChecked(!isSectionHidden(i));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(columnTriggered(bool)));
        actions += action;
    }
}

/** Shows menu where the user can choose witch columns should be visible. */
void TreeWidgetHeader::showContextMenu(const QPoint &point) {
    Q_UNUSED(point)
    QMenu contextMenu(this);
    contextMenu.addActions(actions);
    contextMenu.exec(QCursor::pos());
}

/** Flips visible status of tree widget column. */
void TreeWidgetHeader::columnTriggered(bool visible) {
    int idx = actions.indexOf((QAction*)sender());
    setSectionHidden(idx, !visible);
}
