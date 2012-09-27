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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Program URL: http://sir.projet-libre.org/
 */

#include <QKeyEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QMenu>
#include <QUrl>
#include "treewidget.h"

/** Default constructor. */
TreeWidget::TreeWidget(QWidget *parent) : QTreeWidget(parent) {
    QList<QString> itemList;
    setRootIsDecorated(false);
    setAlternatingRowColors(true);

    itemList.append(tr("Name"));
    itemList.append(tr("Ext"));
    itemList.append(tr("Path"));
    itemList.append(tr("Status"));
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setHeaderLabels(itemList);
    setUniformRowHeights(true);
    setSortingEnabled(true);
    sortItems(0,Qt::AscendingOrder);
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

/** Removes selected items when \a Delete key pressed by user. */
void TreeWidget::keyPressEvent( QKeyEvent *k ) {
    if(k->key() == Qt::Key_Delete) {
        for (int i = 0; i < this->topLevelItemCount(); i++) {
            if ((this->topLevelItem(i))->isSelected()) {
                this->takeTopLevelItem(i);
            }
        }
    }
}

/** Accepts proposed action. \sa dragMoveEvent */
void TreeWidget::dragEnterEvent(QDragEnterEvent *event) {
     event->acceptProposedAction();
}

/** Accepts proposed action. \sa dragEnterEvent */
void TreeWidget::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

/** Appends droped files or directories into this tree widget.\n
  * Emits changed() signal if any file added.
  */
void TreeWidget::dropEvent(QDropEvent *event) {

    QString fileName;
    QTreeWidgetItem *item;
    bool change = false;

    // image file filters
    static QString fileFilters;
    if (fileFilters.isEmpty()) {
        QStringList list;
        foreach(QByteArray format, QImageReader::supportedImageFormats())
            list.append(QString(format));
        fileFilters += "*.";
        fileFilters += list.join(" *.").toLower() + " *.jpg"+ " *.JPG";
        fileFilters += " *.JPEG *.Jpg *.Jpeg";
    }

    foreach (QUrl url, event->mimeData()->urls()) {
        fileName = url.path();
        QFileInfo info(fileName);

        if (!fileName.isEmpty() && info.exists()) {
            //Directory
            if (info.isDir()) {
                QDir sourceFolder(fileName,fileFilters);
                sourceFolder.setFilter( QDir::Files | QDir::NoSymLinks);

                QList<QFileInfo> list = sourceFolder.entryInfoList();
                QListIterator<QFileInfo> it(list);

                while ( it.hasNext() ) {
                    item = new QTreeWidgetItem(itemList(it.next()));
                    addTopLevelItem(item);
                    change = true;
                }
            }
            //File
            else {
                int comp = QString::compare("",QFileInfo(fileName).suffix());

                if((fileFilters.contains(QFileInfo(fileName).suffix()))
                    && (comp !=0)) {

                    item = new QTreeWidgetItem(itemList(info));
                    addTopLevelItem(item);
                    change = true;
                }
            }
        }
    }

    event->acceptProposedAction();

    if (change)
        emit changed();
}

QStringList TreeWidget::itemList(const QFileInfo &info) {
    QStringList result;
    // file name
    result += info.completeBaseName();
    // file expression
    result += info.suffix();
    // file path
    result += info.path();
    // convertion status
    result += tr("Not converted yet");
    return result;
}
