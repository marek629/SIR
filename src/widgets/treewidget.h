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

#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTreeWidget>
#include "treewidgetheader.h"

class QKeyEvent;
class QDropEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QFileInfo;
class ConvertDialog;
class MetadataDialog;
class PreviewDialog;
namespace MetadataUtils {
class String;
}

/** \brief TreeWidget widgets column indexes enumerator.
  * \sa TreeWidget::columnsNames()
  */
enum TreeWidgetColumns {
    NameColumn,
    ExtColumn,
    PathColumn,
    ImageSizeColumn,
    FileSizeColumn,
    StatusColumn
};

//! QTreeWidget reimplementation for ConvertDialog with custom contex menu.
class TreeWidget : public QTreeWidget {
    Q_OBJECT
    friend class ConvertDialog;
    friend class Selection;

public:
    TreeWidget(QWidget *parent = 0);
    ~TreeWidget();
    void initList(const QStringList &argList);
    QSize imageSize(QTreeWidgetItem *item);
    QStringList columnsNames();
    TreeWidgetHeader *header() { return (TreeWidgetHeader*)QTreeWidget::header(); }
    void setHeader(TreeWidgetHeader *header) { QTreeWidget::setHeader(header); }

protected:
    virtual void keyPressEvent( QKeyEvent *k );
    virtual void dropEvent(QDropEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);

private:
    // fields
    ConvertDialog *convertDialog;
    QMap<QString, int> *statusList;
    QAction *removeAction;
    QAction *convertAction;
    PreviewDialog  *previewForm;
    QAction *previewAction;
#ifdef SIR_METADATA_SUPPORT
    MetadataDialog *metadataForm;
    QAction *metadataAction;
#endif // SIR_METADATA_SUPPORT
    QTreeWidgetItem *treeMenuItem;
    // methods
    QStringList itemList(const QFileInfo &info);
    QStringList *makeList();
    QString makeImagePath(QTreeWidgetItem *item);
    QString imageSizeString(const MetadataUtils::String &imagePath);
    void createActions();

public slots:
    void addDir();
    void addFile();
    void loadFile(const QString &file);
    void loadFiles(const QStringList &files);
    void loadFiles(const QList<QFileInfo> &files);
    void removeAll();
    void removeSelectedFromList();
    void updateTree();
    // contex menu
    void showMenu( const QPoint & point);
    void previewAct();
    void showPreview(QTreeWidgetItem *item, int col);
#ifdef SIR_METADATA_SUPPORT
    void showMetadata();
#endif // SIR_METADATA_SUPPORT
    void retranslateStrings();
    inline void resizeColumnsToContents();

signals:
    void changed(); /**< Indicates changes in this tree widget. \sa dropEvent */
};

/** Resizes all columns of \a tree to their contents. */
void TreeWidget::resizeColumnsToContents() {
    for (int i=0; i<columnCount(); i++)
        this->resizeColumnToContents(i);
}

#endif // TREEWIDGET_H
