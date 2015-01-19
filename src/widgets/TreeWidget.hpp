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

#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTreeWidget>
#include "widgets/TreeWidgetHeader.hpp"
#include "sir_String.hpp"

class QKeyEvent;
class QDropEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QFileInfo;
class ConvertDialog;
class MetadataDialog;
class PreviewDialog;

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
    bool isColumnMember(int col, const QString &str);
    bool isColumnMember(int col, const QStringList &strList,
                        Qt::CaseSensitivity cs = Qt::CaseSensitive);
    void retranslateStrings();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
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
    QString fileFilters;
    // methods
    QStringList itemList(const QFileInfo &info);
    QStringList *makeList();
    QString makeImagePath(QTreeWidgetItem *item);
    QString imageSizeString(const sir::String &imagePath);
    void createActions();
    bool bringUrls(const QList<QUrl> &urls);
    void setupFilters();

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
    inline void resizeColumnsToContents();

signals:
    void changed(); /**< Indicates changes in this tree widget. \sa dropEvent */
    void loadingFilesStart(int totalQuantity);
    void loadingFilesTick(int partQuantity);
    void loadingFilesStop();
};

/** Resizes all columns of \a tree to their contents. */
void TreeWidget::resizeColumnsToContents() {
    for (int i=0; i<columnCount(); i++)
        this->resizeColumnToContents(i);
}

#endif // TREEWIDGET_H
