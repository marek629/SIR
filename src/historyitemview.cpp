#include "historycombobox.h"
#include <QMenu>
#include <QContextMenuEvent>

HistoryItemView::HistoryItemView(QWidget *parent) :
    QListView(parent)
{
    comboBox = (HistoryComboBox*) parent;

    favAction = new QAction(this);
    connect(favAction, SIGNAL(triggered()), comboBox, SLOT(favoriteAct()));

    rmAction = new QAction(tr("Remove item"), this);
    connect(rmAction, SIGNAL(triggered()), comboBox, SLOT(removeAct()));

    clearAction = new QAction("Clear text history", this);
    connect(clearAction, SIGNAL(triggered()), comboBox, SLOT(clearAct()));

    surviveAction = new QAction("Survive only favorites", this);
    connect(surviveAction, SIGNAL(triggered()), comboBox, SLOT(surviveAct()));
}

HistoryItemView::~HistoryItemView()
{
    delete favAction;
    delete rmAction;
    delete clearAction;
    delete surviveAction;
}

void HistoryItemView::contextMenuEvent(QContextMenuEvent *e)
{
    contex_index = indexAt( mapFromGlobal( e->globalPos() ) ).row();

    if (comboBox->itemData(contex_index).toBool())
    {
        favAction->setText(tr("Throw away from favorite"));
        favAction->setIcon(QIcon(":/images/unfavorite.png"));
    }
    else
    {
        favAction->setText(tr("Mark as favorite"));
        favAction->setIcon(QIcon(":/images/favorite.png"));
    }

    QMenu menu(this);
    menu.addAction(favAction);
    menu.addAction(rmAction);
    menu.addAction(surviveAction);
    menu.addAction(clearAction);

    menu.exec(e->globalPos());
}
