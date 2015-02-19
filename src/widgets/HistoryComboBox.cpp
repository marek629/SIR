/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2015  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://marek629.github.io/SIR/
 */

#include <QMessageBox>
#include <QLineEdit>
#include <QList>
#include <QAbstractItemView>
#include <QAction>
#include <QMenu>
#include <QCoreApplication>
#include "widgets/HistoryComboBox.hpp"

/** Default constructor.\n
  * Constucts by default editable combo box which insert at top policy.
  */
HistoryComboBox::HistoryComboBox(QWidget *parent) : QComboBox(parent) {
    setInsertPolicy(QComboBox::InsertAtTop);
    setEditable(true);

    lineEdit()->disconnect(this);
    connect(lineEdit(),SIGNAL(editingFinished()),this,SLOT(prependText()));

    view()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(view(), SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showMenu(QPoint)));
    connectPromoteItem();

    favIcon = QIcon(QCoreApplication::applicationDirPath() +
                    "/../share/sir/images/favorite.png");
    unfavIcon = QIcon(QCoreApplication::applicationDirPath() +
                      "/../share/sir/images/unfavorite.png");
    createActions();
}

/** Destructor. */
HistoryComboBox::~HistoryComboBox() {
    delete favAction;
    delete rmAction;
    delete clearAction;
    delete surviveAction;
}

/** Creates and connects popup widgets menu actions to slots. */
void HistoryComboBox::createActions() {
    favAction = new QAction(this);
    connect(favAction, SIGNAL(triggered()), this, SLOT(favoriteAct()));

    rmAction = new QAction(tr("Remove item"), this);
    connect(rmAction, SIGNAL(triggered()), this, SLOT(removeAct()));

    clearAction = new QAction("Clear text history", this);
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clearAct()));

    surviveAction = new QAction("Survive only favorites", this);
    connect(surviveAction, SIGNAL(triggered()), this, SLOT(surviveAct()));
}

/** Insert at top text from this combo boxes line edit into this combo box
  * unless the text is a member of the combo box; otherwise item containg
  * the text will be moved to top of this combo box.
  * \note If this combo box is full of favorite items this method will show
  *       adequate information.
  * \sa promoteItem disconnectPromoteItem connectPromoteItem
  */
void HistoryComboBox::prependText() {
    int textIndex = findText(lineEdit()->text());
    if (lineEdit()->text() == itemText(0))
        return;
    else if (textIndex != -1) {
        setCurrentIndex(textIndex);
        promoteItem(textIndex);
    }
    else if ( (count()+1 <= maxCount()) || removeFromEnd() ) {
        disconnectPromoteItem();
        insertItem(0, unfavIcon, lineEdit()->text(), QVariant(false));
        setCurrentIndex(0);
        connectPromoteItem();
    }
    else
        QMessageBox::information(this, tr("Full text history - SIR"),
                                 tr("Text history memory is full of favorite items.\n" \
                                    "SIR can't automatically remove favorite item. " \
                                    "Do it manually.")
                                 );
}

/** Removes last unfavorite item from this combo box and returns remove success
  * code: if item removed returns true, otherwise false.\n
  * \note Favorite items can't be removed. If all items are favorites this method
  *       will return false.
  */
bool HistoryComboBox::removeFromEnd() {
    for (int i=count()-1; i>=0; i--) {
        if (!itemData(i).toBool()) {
            removeItem(i);
            return true;
        }
    }
    return false;
}

/** Shows custom context menu containig avalaible actions.
  * \sa favoriteAct removeAct clearAct surviveAct
  */
void HistoryComboBox::showMenu(const QPoint &point){
    listViewRow = view()->indexAt(point).row();

    if (itemData(listViewRow).toBool()) {
        favAction->setText(tr("Throw away from favorite"));
        favAction->setIcon(unfavIcon);
    }
    else {
        favAction->setText(tr("Mark as favorite"));
        favAction->setIcon(favIcon);
    }

    QMenu menu(this);
    menu.addAction(favAction);
    menu.addAction(rmAction);
    menu.addAction(surviveAction);
    menu.addAction(clearAction);

    menu.exec(QCursor::pos());
}

/** Moves item corresponding with \a i index to top of combo box.
  * \sa prependText disconnectPromoteItem connectPromoteItem
  */
void HistoryComboBox::promoteItem(int i){
    if (currentIndex() > 0) {
        disconnectPromoteItem();

        QString text = itemText(i);
        QVariant data = itemData(i);
        QIcon icon = itemIcon(i);
        removeItem(i);
        insertItem(0,icon,text,data);
        setCurrentIndex(0);

        connectPromoteItem();
    }
}

/** Inverts favorite-status value and ajusts star icon.
  * \sa showMenu removeAct clearAct surviveAct
  */
void HistoryComboBox::favoriteAct() {
    bool b = (! itemData(listViewRow).toBool() );
    setItemData(listViewRow,b);
    if (b)
        setItemIcon(listViewRow,favIcon);
    else
        setItemIcon(listViewRow,unfavIcon);
}

/** Removes item pointed by mouse cursor.
  * \sa showMenu favoriteAct clearAct surviveAct
  */
void HistoryComboBox::removeAct() {
    removeItem(listViewRow);
    hidePopup();
    if (count() > 0)
        showPopup();
}

/** Clears combo box.
  * \sa showMenu favoriteAct removeAct surviveAct
  */
void HistoryComboBox::clearAct() {
    QString text = lineEdit()->text();
    clear();
    lineEdit()->setText(text);
    hidePopup();
}

/** Removes all unfavorite item from combo box.
  * \sa showMenu favoriteAct removeAct clearAct
  */
void HistoryComboBox::surviveAct() {
    for (int i=count()-1; i>=0; i--) {
        if (!itemData(i).toBool())
            removeItem(i);
    }
    hidePopup();
    if (count() > 0)
        showPopup();
}

/** Imports list of items from \a historyList and \a currentMap (as current item)
  * into this combo box and sets maximum count of combo box to \a maxCount.
  * \sa exportHistory
  */
void HistoryComboBox::importHistory(const QMap<QString, QVariant> &currentMap,
                                    const QList< QVariant > &historyList,
                                    int maxCount) {
    setMaxCount(maxCount);
    disconnectPromoteItem();

    QString text;
    bool value;
    QIcon icon;
    for (int i=0; i<historyList.count(); i++) {
        text = historyList[i].toMap().keys().first();
        value = historyList[i].toMap().value(text,false).toBool();
        if (value)
            icon = favIcon;
        else
            icon = unfavIcon;
        insertItem(i,icon,text,QVariant(value));
    }
    if (!currentMap.isEmpty()) {
        text = currentMap.keys().first();
        value = currentMap.value(text,false).toBool();
        if (value)
            icon = favIcon;
        else
            icon = unfavIcon;
        insertItem(0,icon,text,QVariant(value));
        setCurrentIndex(0);
    }
    else
        setCurrentIndex(-1);

    connectPromoteItem();
}

/** Exports current item of combo box into \a currentMap and other up to
  * \a newMaxCount items into \a historyList.
  * \note \a currentMap and \a historyList is cleared before export.
  * \sa importHistory
  */
void HistoryComboBox::exportHistory(QMap<QString, QVariant> *currentMap,
                                    QList<QVariant> *historyList,
                                    int newMaxCount) {
    currentMap->clear();
    if (!currentText().isEmpty())
        currentMap->insert(currentText(), itemData(currentIndex()));

    int skip;
    if (newMaxCount == -1)
        skip = 0;
    else
        skip = maxCount() - newMaxCount;

    for (int i=maxCount()-1; i>=0 && skip>0; i--) {
        if (!itemData(i).toBool()) {
            removeItem(i);
            skip--;
        }
    }
    historyList->clear();
    int lastIndex = count() - skip;
    for (int i=1; i<lastIndex; i++) {
        QMap<QString,QVariant> map;
        map.insert(itemText(i), itemData(i));
        historyList->append(QVariant(map));
    }
}
