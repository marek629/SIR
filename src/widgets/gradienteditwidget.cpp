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

#include <QInputDialog>
#include <QColorDialog>
#include "gradienteditwidget.h"

/** Creates the GradientEditWidget object.
  *
  * Sets up the user interface, ie. buttons and tree widget and creates connections.
  */
GradientEditWidget::GradientEditWidget(QWidget *parent) : QWidget(parent) {
    setupUi(this);

    setButtonIcons();
    disableButtons();
    setupTreeWidget(treeWidget);
    createConnections();
}

/** Returns list of GradientStops corresponding content of tree widget.
  * \sa setGradientStops()
  */
QGradientStops GradientEditWidget::gradientStops() const {
    QGradientStops result(treeWidget->topLevelItemCount());
    for (int i=0; i<result.size(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        QGradientStop stop(item->text(0).toDouble(), item->backgroundColor(1));
        result[i] = stop;
    }
    return result;
}

/** Sets content of tree widget corresponding \a stops list.
  * * \sa gradientStops()
  */
void GradientEditWidget::setGradientStops(const QGradientStops &stops) {
    treeWidget->clear();
    foreach (QGradientStop stop, stops) {
        QStringList list;
        list << QString::number(stop.first, 'f', 2) << "";
        QTreeWidgetItem *item = new QTreeWidgetItem(list);
        item->setBackgroundColor(1, stop.second);
        treeWidget->addTopLevelItem(item);
    }
    emit gradientChanged();
}

void GradientEditWidget::retranslateStrings() {
    retranslateUi(this);
}

/** Hides tree widget and related push buttons.
  * \sa show()
  */
void GradientEditWidget::hide() {
    treeWidget->hide();
    addPushButton->hide();
    deletePushButton->hide();
    moveUpPushButton->hide();
    moveDownPushButton->hide();
}

/** Shows tree widget and related push buttons.
  * \sa hide()
  */
void GradientEditWidget::show() {
    treeWidget->show();
    addPushButton->show();
    deletePushButton->show();
    moveUpPushButton->show();
    moveDownPushButton->show();
}

/** Reduces current selecion to first column of \a current item and modifies
  * push buttons related to tree widget.
  */
void GradientEditWidget::treeItemSelected(QTreeWidgetItem *current,
                                          QTreeWidgetItem *previous) {
    Q_UNUSED(previous)
    int currentIndex = treeWidget->indexOfTopLevelItem(current);
    if (currentIndex == treeWidget->topLevelItemCount()-1) {
        moveUpPushButton->setEnabled(true);
        moveDownPushButton->setEnabled(false);
        addPushButton->setEnabled(false);
    }
    else {
        addPushButton->setEnabled(true);
        if (currentIndex == 0) {
            moveUpPushButton->setEnabled(false);
            moveDownPushButton->setEnabled(true);
        }
        else {
            moveUpPushButton->setEnabled(true);
            moveDownPushButton->setEnabled(true);
        }
    }

    QModelIndex index = treeWidget->currentIndex();
    index = index.sibling(index.row(), 0);
    treeWidget->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
}

/** Gets from the user by dialog new value for specified \a item and \a column
  * of tree widget.
  */
void GradientEditWidget::editItem(QTreeWidgetItem *item, int column) {
    if (column == 0) {
        double oldValue = item->text(column).toDouble();
        double value = QInputDialog::getDouble(this, tr("Stop point"),
                                               tr("Type stop point value:"),
                                               oldValue, 0, 1, 2);
        if (value != oldValue) {
            item->setText(column, numberString(value));
            treeWidget->sortItems(column, Qt::AscendingOrder);
            emit gradientChanged();
        }
    }
    else {
        QColor color = QColorDialog::getColor(item->backgroundColor(column), this);
        if (color.isValid()) {
            item->setBackgroundColor(column, color);
            emit gradientChanged();
        }
    }
}

/** Adds new item to tree widget after current item of them. Sets up values of
  * the new item and updates user interface.
  * \sa deleteItem() editItem()
  */
void GradientEditWidget::addItem() {
    int index = treeWidget->indexOfTopLevelItem(treeWidget->currentItem());
    double currentValue = treeWidget->topLevelItem(index)->text(0).toDouble();
    index++;
    double nextValue = treeWidget->topLevelItem(index)->text(0).toDouble();
    QStringList list;
    list << numberString((nextValue - currentValue) / 2) << "";
    QTreeWidgetItem *item = new QTreeWidgetItem(list);
    treeWidget->insertTopLevelItem(index, item);
    editItem(item, 0);
    editItem(item, 1);
    deletePushButton->setEnabled(true);
}

/** Removes the current item of tree widget and updates user interface.
  * \sa addItem()
  */
void GradientEditWidget::deleteItem() {
    int index = treeWidget->indexOfTopLevelItem(treeWidget->currentItem());
    treeWidget->takeTopLevelItem(index);
    if (treeWidget->topLevelItemCount() < 3)
        deletePushButton->setEnabled(false);
}

/** Moves down the current item of tree widget and updates user interface.
  * \sa moveUpItem()
  */
void GradientEditWidget::moveDownItem() {
    const int column = 0;
    QTreeWidgetItem *item = treeWidget->currentItem();
    const int index = treeWidget->indexOfTopLevelItem(item) + 1;
    QTreeWidgetItem *nextItem = treeWidget->topLevelItem(index);
    const QString nextStop = nextItem->text(column);
    nextItem->setText(column, item->text(column));
    item->setText(column, nextStop);

    treeWidget->sortItems(column, Qt::AscendingOrder);

    if (index == treeWidget->topLevelItemCount()-1) {
        moveDownPushButton->setEnabled(false);
        addPushButton->setEnabled(false);
    }
    moveUpPushButton->setEnabled(true);
    emit gradientChanged();
}

/** Moves up the current item of tree widget and updates user interface.
  * \sa moveDownItem()
  */
void GradientEditWidget::moveUpItem() {
    const int column = 0;
    QTreeWidgetItem *item = treeWidget->currentItem();
    const int index = treeWidget->indexOfTopLevelItem(item) - 1;
    QTreeWidgetItem *previousItem = treeWidget->topLevelItem(index);
    const QString nextStop = previousItem->text(column);
    previousItem->setText(column, item->text(column));
    item->setText(column, nextStop);

    treeWidget->sortItems(column, Qt::AscendingOrder);

    if (index == 0)
        moveUpPushButton->setEnabled(false);
    moveDownPushButton->setEnabled(true);
    addPushButton->setEnabled(true);
    emit gradientChanged();
}

/** Returns string coresponding \a v double value needed by tree widget. */
QString GradientEditWidget::numberString(double v) {
    return QString::number(v, 'f', 2);
}

void GradientEditWidget::createConnections() {
    connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(treeItemSelected(QTreeWidgetItem*,QTreeWidgetItem*)) );
    connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(editItem(QTreeWidgetItem*,int)) );
    connect(addPushButton, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(deletePushButton, SIGNAL(clicked()), this, SLOT(deleteItem()));
    connect(moveUpPushButton, SIGNAL(clicked()), this, SLOT(moveUpItem()));
    connect(moveDownPushButton, SIGNAL(clicked()), this, SLOT(moveDownItem()));
}

void GradientEditWidget::setupTreeWidget(QTreeWidget *tree) {
    tree->setSelectionMode(QAbstractItemView::NoSelection);
    QStringList list;
    list << numberString(0.) << "";
    QTreeWidgetItem *item = new QTreeWidgetItem(list);
    item->setBackgroundColor(1, Qt::red);
    tree->addTopLevelItem(item);
    list[0] = numberString(1.);
    item = new QTreeWidgetItem(list);
    item->setBackgroundColor(1, Qt::yellow);
    tree->addTopLevelItem(item);
}

void GradientEditWidget::disableButtons() {
    addPushButton->setEnabled(false);
    deletePushButton->setEnabled(false);
    moveUpPushButton->setEnabled(false);
    moveDownPushButton->setEnabled(false);
}

void GradientEditWidget::setButtonIcons() {
    addPushButton->setIcon(QIcon::fromTheme("list-add"));
    deletePushButton->setIcon(QIcon::fromTheme("list-remove"));
    moveUpPushButton->setIcon(QIcon::fromTheme("go-up"));
    moveDownPushButton->setIcon(QIcon::fromTheme("go-down"));
}
