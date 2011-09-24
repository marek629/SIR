#include "historycombobox.h"
#include <QMessageBox>
#include <QLineEdit>
#include <QList>
#include <QAbstractItemView>
#include <QAction>
#include <QMenu>

#include <QDebug>

HistoryComboBox::HistoryComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setInsertPolicy(QComboBox::InsertAtTop);
    setEditable(true);

    line_edit = lineEdit();
    line_edit->disconnect(this);
    connect(line_edit,SIGNAL(returnPressed()),this,SLOT(prependText()));

    view()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(view(), SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showMenu(QPoint)));
    connectPromoteItem();

    favIcon = QIcon(":/images/favorite.png");
    unfavIcon = QIcon(":/images/unfavorite.png");
    createActions();
}

HistoryComboBox::~HistoryComboBox()
{
    delete favAction;
    delete rmAction;
    delete clearAction;
    delete surviveAction;
}

void HistoryComboBox::createActions()
{
    favAction = new QAction(this);
    connect(favAction, SIGNAL(triggered()), this, SLOT(favoriteAct()));

    rmAction = new QAction(tr("Remove item"), this);
    connect(rmAction, SIGNAL(triggered()), this, SLOT(removeAct()));

    clearAction = new QAction("Clear text history", this);
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clearAct()));

    surviveAction = new QAction("Survive only favorites", this);
    connect(surviveAction, SIGNAL(triggered()), this, SLOT(surviveAct()));
}

void HistoryComboBox::prependText()
{
    int textIndex = findText(line_edit->text());
    if (line_edit->text() == itemText(0))
        return;
    else if (textIndex != -1)
    {
        setCurrentIndex(textIndex);
        promoteItem(textIndex);
    }
    else if ( (count()+1 <= maxCount()) || removeFromEnd() )
    {
        disconnectPromoteItem();
        insertItem(0, unfavIcon, line_edit->text(), QVariant(false));
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

bool HistoryComboBox::removeFromEnd()
{
    int i = count()-1;
    while (i>=0)
    {
        if (!itemData(i).toBool())
        {
            removeItem(i);
            return true;
        }
        i--;
    }
    return false;
}

void HistoryComboBox::showMenu(const QPoint &point)
{
    listViewRow = view()->indexAt(point).row();

    if (itemData(listViewRow).toBool())
    {
        favAction->setText(tr("Throw away from favorite"));
        favAction->setIcon(unfavIcon);
    }
    else
    {
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

void HistoryComboBox::promoteItem(int i)
{
    if (currentIndex() > 0)
    {
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

void HistoryComboBox::favoriteAct()
{
    bool b = (! itemData(listViewRow).toBool() );
    setItemData(listViewRow,b);
    if (b)
        setItemIcon(listViewRow,favIcon);
    else
        setItemIcon(listViewRow,unfavIcon);
}

void HistoryComboBox::removeAct()
{
    removeItem(listViewRow);
    hidePopup();
    if (count() > 0)
        showPopup();
}

void HistoryComboBox::clearAct()
{
    QString text = line_edit->text();
    clear();
    line_edit->setText(text);
    hidePopup();
}

void HistoryComboBox::surviveAct()
{
    for (int i=count()-1; i>=0; i--)
    {
        if (!itemData(i).toBool())
            removeItem(i);
    }
    hidePopup();
    if (count() > 0)
        showPopup();
}

void HistoryComboBox::loadHistory(const QMap<QString, QVariant> &currentMap,
                                  const QList< QVariant > &historyList,
                                  int maxCount)
{
    setMaxCount(maxCount);
    disconnectPromoteItem();

    QString text;
    bool value;
    QIcon icon;
    for (int i=0; i<historyList.count(); i++)
    {
        text = historyList[i].toMap().keys().first();
        value = historyList[i].toMap().value(text,false).toBool();
        if (value)
            icon = favIcon;
        else
            icon = unfavIcon;
        insertItem(i,icon,text,QVariant(value));
    }
    if (!currentMap.isEmpty())
    {
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

void HistoryComboBox::exportHistory(QMap<QString, QVariant> *currentMap,
                                    QList< QVariant > *historyList,
                                    int newMaxCount)
{
    currentMap->clear();
    currentMap->insert( currentText(), itemData(currentIndex()).toBool() );

    int skip;
    if (newMaxCount == -1)
        skip = 0;
    else
        skip = maxCount() - newMaxCount;

    for (int i=maxCount()-1; i>=0 && skip>0; i--)
    {
        if (!itemData(i).toBool())
        {
            removeItem(i);
            skip--;
        }
    }
    historyList->clear();
    int lastIndex = count() - skip;
    for (int i=1; i<lastIndex; i++)
    {
        QMap<QString,QVariant> map;
        map.insert(itemText(i), itemData(i));
        historyList->append(QVariant(map));
    }
}
