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
#include <QFileDialog>
#include "treewidget.h"
#include "convertdialog.h"
#include "defines.h"
#include "widgets/previewdialog.h"
#include "widgets/metadatadialog.h"

/** Default constructor. */
TreeWidget::TreeWidget(QWidget *parent) : QTreeWidget(parent) {
    // setup fields
    convertDialog = (ConvertDialog*)(parent->window());
    statusList = new QMap<QString,int>();
    // setup columns
    QList<QString> itemList;
    itemList.append(tr("Name"));
    itemList.append(tr("Ext"));
    itemList.append(tr("Path"));
    itemList.append(tr("Status"));
    setHeaderLabels(itemList);
    // setup widgets parameters
    setRootIsDecorated(false);
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setUniformRowHeights(true);
    setSortingEnabled(true);
    sortItems(0,Qt::AscendingOrder);
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    // setup actions & connections
    createActions();
    connect(this, SIGNAL(changed()), SLOT(updateTree()));
    connect(this, SIGNAL(customContextMenuRequested (QPoint)),
            this, SLOT(showMenu(QPoint)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this, SLOT(showPreview(QTreeWidgetItem *, int)));
}

/** Destructor. Dealocates memory. */
TreeWidget::~TreeWidget() {
    delete statusList;
    delete convertAction;
    delete removeAction;
    delete previewAction;
#ifdef SIR_METADATA_SUPPORT
    delete metadataAction;
#endif // SIR_METADATA_SUPPORT
}

/** Loads files into tree view from main() functions \a argv argument list. */
void TreeWidget::initList(const QStringList &argList) {

    QStringList::const_iterator it2 = argList.begin();
    QString fileName;
    QTreeWidgetItem *item;

    for ( ; it2 != argList.end(); ++it2 ) {
        fileName = *it2;
        fileName = QDir::convertSeparators(fileName);
        QFileInfo info(fileName);

        if (!fileName.isEmpty() && info.exists()) {
            //Directory
            if (info.isDir()) {
                QDir sourceFolder(fileName, convertDialog->fileFilters);
                sourceFolder.setFilter( QDir::Files | QDir::NoSymLinks);

                QList<QFileInfo> list = sourceFolder.entryInfoList();
                QListIterator<QFileInfo> it(list);
                QFileInfo fi;

                while ( it.hasNext() ) {
                    fi = it.next();
                    item = new QTreeWidgetItem(itemList(fi));
                    this->addTopLevelItem(item);
                    statusList->insert(fi.absoluteFilePath(), NOTCONVERTED);
                }
            }
            //File
            else {

                int comp = QString::compare("",QFileInfo(fileName).suffix());

                if((convertDialog->fileFilters.contains(QFileInfo(fileName).suffix()))
                    && (comp !=0)) {

                    item = new QTreeWidgetItem(itemList(info));
                    this->addTopLevelItem(item);
                    statusList->insert(info.absoluteFilePath(), NOTCONVERTED);
                }
            }
        }
    }

    if (topLevelItemCount() > 0) {
        convertDialog->convertButton->setEnabled(true);
        convertDialog->convertSelectedButton->setEnabled(true);
    }
    resizeColumnsToContents();
}

/** Add file button and action slot.
  * Load selected image files into tree widget and set state to
  * \em "Not \em converted \em yet".\n
  * This function remember last opened directory in the same session.
  * Default directory is home directory.
  * \sa addDir() loadFiles(const QStringList&)
  */
void TreeWidget::addFile() {
    QString aux = tr("Images") + "(" + convertDialog->fileFilters + ")";

    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            tr("Select one or more files to open"),
                            Settings::instance().settings.lastDir,
                            aux
                        );
    if (files.isEmpty())
        return;
    aux = files.first();
    if (!aux.isEmpty()) {
        Settings::instance().settings.lastDir =
                aux.left(aux.lastIndexOf(QDir::separator()));
        loadFiles(files);
    }
}

/** Adds directory button and action slot.
  * Load all supported image files from choosed directory (non-recursive)
  * into tree widget and set state to \em "Not converted yet".\n
  * This function remember last opened directory in the same session.
  * Default directory is home directory.
  * \sa addFile()
  */
void TreeWidget::addDir() {
    QString dirPath = QFileDialog::getExistingDirectory(
                       this,
                       tr("Choose a directory"),
                       Settings::instance().settings.lastDir,
                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    dirPath = QDir::convertSeparators(dirPath);
    if (dirPath.isEmpty())
        return;
    Settings::instance().settings.lastDir = dirPath;
    QDir sourceFolder(dirPath, convertDialog->fileFilters);
    sourceFolder.setFilter( QDir::Files | QDir::NoSymLinks);
    QList<QFileInfo> list = sourceFolder.entryInfoList();
    loadFiles(list);
}

/** Loads files into tree widget.
  * \param files Full paths list.
  */
void TreeWidget::loadFiles(const QStringList &files) {
    QStringList::const_iterator it = files.begin();
    QTreeWidgetItem *item;
    QString fileName;

    while ( it != files.end() ) {
        fileName = QDir::convertSeparators(*it);
        QFileInfo info(fileName);
        statusList->insert(info.absoluteFilePath(), NOTCONVERTED);

        item = new QTreeWidgetItem(itemList(info));
        this->addTopLevelItem(item);
        ++it;
        Settings::instance().settings.lastDir = info.path();
    }
    convertDialog->enableConvertButtons();
    resizeColumnsToContents();
}

/** Loads files into tree widget.
  * \param files File infos list.
  */
void TreeWidget::loadFiles(const QList<QFileInfo> &files) {
    QListIterator<QFileInfo> it(files);
    QFileInfo fi;
    QTreeWidgetItem *item;

    while ( it.hasNext() ) {
        fi = it.next();
        item = new QTreeWidgetItem(itemList(fi));
        this->addTopLevelItem(item);
        statusList->insert(fi.absoluteFilePath(), NOTCONVERTED);
    }
    if (!files.isEmpty()) {
        convertDialog->enableConvertButtons();
        resizeColumnsToContents();
    }
}

/** Remove all button and action slot. Removes all items of tree widget.
  * \sa removeSelectedFromList
  */
void TreeWidget::removeAll() {
    // clear list
    if (topLevelItemCount() > 0)
        this->clear();
    // update convert widgets
    convertDialog->enableConvertButtons(false);
    convertDialog->convertProgressBar->reset();
    statusList->clear();
}

/** Remove selected button and action slot.
  * Remove selected items of tree widget.
  * \sa removeAll
  */
void TreeWidget::removeSelectedFromList() {

    QTreeWidgetItem *item;
    QString fileName;

    for (int i=topLevelItemCount()-1; i>=0; i--)
    {

        if ((this->topLevelItem(i))->isSelected()) {
            item = this->takeTopLevelItem(i);

            fileName = item->text(2) + QDir::separator() +item->text(0) + ".";
            fileName += item->text(1);

            statusList->remove(fileName);
        }

    }

    if (this->topLevelItemCount() == 0) {
        convertDialog->convertButton->setEnabled(false);
        convertDialog->convertSelectedButton->setEnabled(false);
    }
}

/** Updates tree widget when it was changed. */
void TreeWidget::updateTree() {
    if (topLevelItemCount() > 0)
        convertDialog->enableConvertButtons(true);
    resizeColumnsToContents();
}

/** Shows context menu.
  * Shows context menu for selected tree widgets item.
  * \param point Global position of context menu.
  * \sa showPreview() showMetadata() convertSelected() removeSelectedFromList()
  */
void TreeWidget::showMenu(const QPoint & point) {

    treeMenuItem = itemAt(point);

    if (treeMenuItem) {
        QMenu contextMenu(this);
        contextMenu.addAction(previewAction);
#ifdef SIR_METADATA_SUPPORT
        contextMenu.addAction(metadataAction);
        QString ext = treeMenuItem->text(1).left(3).toUpper();
        if (ext == "SVG")
            metadataAction->setEnabled(false);
        else
            metadataAction->setEnabled(true);
#endif // SIR_METADATA_SUPPORT
        contextMenu.addSeparator();
        contextMenu.addAction(convertAction);
        contextMenu.addAction(removeAction);
        contextMenu.setDefaultAction(previewAction);
        contextMenu.exec(QCursor::pos());
    }
}

/** previewAction slot. \sa showPreview */
void TreeWidget::previewAct() {
    showPreview(treeMenuItem, 0);
}

/** Shows preview dialog containig selected \a item image.\n
  * This slot is called when tree widgets \a item was double clicked.
  * \param item Pointer to selected tree widgets item.
  * \param col Is ignored, exists for signal-slot compatibility only.
  * \sa showMenu() previewAct()
  */
void TreeWidget::showPreview(QTreeWidgetItem *item, int col) {

    Q_UNUSED(col);

    QString imagePath = makeImagePath(item);
    QStringList *list = makeList();
    int index = list->indexOf(imagePath);

    previewForm = new PreviewDialog(this, list, index);
    previewForm->show();
}

#ifdef SIR_METADATA_SUPPORT
/** Metadata action slot.
  * Shows metadata dialog containg selected tree widgets item image metadata.\n
  * This function is available if SIR_METADATA_SUPPORT is defined only.
  * \sa showMenu() showPreview()
  */
void TreeWidget::showMetadata() {
    QString imagePath = makeImagePath(treeMenuItem);
    QStringList *list = makeList();
    int index = list->indexOf(imagePath);

    metadataForm = new MetadataDialog(this, list, index);
    metadataForm->show();
}
#endif // SIR_METADATA_SUPPORT

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

/** Returns list strings describing file and (\a info based) convertion status. */
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

/** Creates (dynamic allocated) and returns pointer to new string list.
  * In the list is stored files path to images on tree view.
  * \sa makeImagePath()
  */
QStringList * TreeWidget::makeList() {
    int count = topLevelItemCount();
    QStringList *list = new QStringList();
    for (int i=0; i<count; i++)
        list->append(makeImagePath(topLevelItem(i)));
    return list;
}

/** Returns image file path corresponding to \b item.
  * \sa makeList()
  */
QString TreeWidget::makeImagePath(QTreeWidgetItem *item) {
    QString imagePath = item->text(2);
    if (!item->text(2).endsWith("/"))
        imagePath += QDir::separator();
    imagePath += item->text(0) + "." + item->text(1);
    return QDir::fromNativeSeparators(imagePath);
}

/** Creates actions and connects their signals to corresponding slots. */
void TreeWidget::createActions() {
    removeAction = new QAction(tr("Remove Selected"), this);
    removeAction->setStatusTip(tr("Remove selected images"));
    connect(removeAction, SIGNAL(triggered()),
            this, SLOT(removeSelectedFromList()));

    convertAction = new QAction(tr("Convert Selected"), this);
    convertAction->setStatusTip(tr("Convert selected images"));
    connect(convertAction, SIGNAL(triggered()),
            convertDialog, SLOT(convertSelected()));

    previewAction = new QAction(tr("Show Image"), this);
    previewAction->setStatusTip(tr("Show preview selected image"));
    connect(previewAction, SIGNAL(triggered()), this, SLOT(previewAct()));

#ifdef SIR_METADATA_SUPPORT
    metadataAction = new QAction(tr("Show Metadata"), this);
    metadataAction->setStatusTip(tr("Show metadata of selected image"));
    connect(metadataAction, SIGNAL(triggered()), this, SLOT(showMetadata()));
#endif // SIR_METADATA_SUPPORT
}
