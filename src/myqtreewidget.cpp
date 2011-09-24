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

#include <myqtreewidget.h>
#include <QKeyEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDir>
#include <QFileInfo>
#include <QImageWriter>
#include <QMenu>

myQTreeWidget::myQTreeWidget(QWidget *parent):QTreeWidget(parent) {
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

void myQTreeWidget::keyPressEvent( QKeyEvent *k ) {
	
    if(k->key() == Qt::Key_Delete) {
        for (int i = 0; i < this->topLevelItemCount(); i++) {
            if ((this->topLevelItem(i))->isSelected()) {
                this->takeTopLevelItem(i);
            }	
        }
    }
}

void myQTreeWidget::dragEnterEvent(QDragEnterEvent *event) {
     event->acceptProposedAction();
}

void myQTreeWidget::dragMoveEvent(QDragMoveEvent *event) {
     event->acceptProposedAction();
}

void myQTreeWidget::dropEvent(QDropEvent *event) {

    const QMimeData *mimeData = event->mimeData();
    QString fileName = mimeData->text();
    QTreeWidgetItem *item;
    QStringList argsList;
    bool change = false;	
    argsList = fileName.split("\n");
	
    QStringList::Iterator it2 = argsList.begin();
	
    for ( ; it2 != argsList.end(); ++it2 ) {
        fileName = *it2;		
        fileName = fileName.section("/",2);		
	
        QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
        QStringList list;
	
        foreach(QByteArray format, imageFormats) {
            list.append(*new QString(format));
        }
	
        QString fileFilters = "*.";
        fileFilters += list.join(" *.").toLower() + " *.jpg"+ " *.JPG";
        fileFilters += " *.JPEG *.Jpg *.Jpeg";
			
	
        if (!fileName.isEmpty() && QFileInfo(fileName).exists()) {
            //Directory
            if (QFileInfo(fileName).isDir()) {
                QDir sourceFolder(fileName,fileFilters);
                sourceFolder.setFilter( QDir::Files | QDir::NoSymLinks);
	
                QList<QFileInfo> list = sourceFolder.entryInfoList();
                QListIterator<QFileInfo> it(list);
                QFileInfo fi;
	
                while ( it.hasNext() ) {
                    fi = it.next();
                    QList<QString> itemList;
                    itemList.append(fi.completeBaseName());
                    itemList.append(fi.suffix());
                    itemList.append(fi.path());
                    itemList.append(tr("Not converted yet"));
                    item = new QTreeWidgetItem(itemList);
                    addTopLevelItem(item);
                    change = true;
                }
            }
            //File
            else {  
                int comp = QString::compare("",QFileInfo(fileName).suffix());         	 

                if((fileFilters.contains(QFileInfo(fileName).suffix()))
                    && (comp !=0)) {

                    QList<QString> itemList;
                    itemList.append(QFileInfo(fileName).completeBaseName());
                    itemList.append(QFileInfo(fileName).suffix());
                    itemList.append(QFileInfo(fileName).path());
                    itemList.append(tr("Not converted yet"));
                    item = new QTreeWidgetItem(itemList);
                    addTopLevelItem(item);
                    change = true;
                }
            }
        }
    }

    event->acceptProposedAction();
	
    if(change) {
        emit changed();
    }
		
}
