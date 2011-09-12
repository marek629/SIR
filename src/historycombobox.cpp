#include "historycombobox.h"
#include <QMessageBox>
#include <QLineEdit>
#include <QList>

#include <QDebug>

HistoryComboBox::HistoryComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setInsertPolicy(QComboBox::InsertAtTop);
    setEditable(true);
    line_edit = lineEdit();
    line_edit->disconnect(this);
    connect(line_edit,SIGNAL(returnPressed()),this,SLOT(prependText()));
    connectPromoteItem();
    itemView = new HistoryItemView(this);
    setView((QAbstractItemView*)itemView);
    favIcon = QIcon(":/images/favorite.png");
    unfavIcon = QIcon(":/images/unfavorite.png");
}

HistoryComboBox::~HistoryComboBox()
{
    delete itemView;
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
                                 tr("Text history memory is full of favorite items.\nSIR can't automatically remove favorite item. Do it manually.")
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
    int i = itemView->contexIndex();
    bool b = (! itemData(i).toBool() );
    setItemData(i,b);
    if (b)
        setItemIcon(i,favIcon);
    else
        setItemIcon(i,unfavIcon);
}

void HistoryComboBox::removeAct()
{
    removeItem(itemView->contexIndex());
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
    text = currentMap.keys().first();
    value = currentMap.value(text,false).toBool();
    if (value)
        icon = favIcon;
    else
        icon = unfavIcon;
    insertItem(0,icon,text,QVariant(value));
    setCurrentIndex(0);

    connectPromoteItem();
}

void HistoryComboBox::exportHistory(QMap<QString, QVariant> *currentMap,
                                    QList< QVariant > *historyList,
                                    int newMaxCount)
{
    currentMap->clear();
    currentMap->insert( currentText(), itemData(currentIndex()).toBool() );

    int skip = maxCount() - newMaxCount;
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
