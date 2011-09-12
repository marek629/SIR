#ifndef HISTORYCOMBOBOX_H
#define HISTORYCOMBOBOX_H

#include <QComboBox>
#include "historyitemview.h"

template <typename T> class QList;

class HistoryComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit HistoryComboBox(QWidget *parent = 0);
    ~HistoryComboBox();
    bool removeFromEnd();
    void loadHistory(const QMap<QString,QVariant> &currentMap,
                     const QList<QVariant> &historyList,
                     int maxCount);
    void exportHistory(QMap<QString,QVariant> *currentMap,
                       QList<QVariant > *historyList,
                       int newMaxCount);

private:
    QLineEdit *line_edit;
    HistoryItemView *itemView;
    QIcon favIcon;
    QIcon unfavIcon;

signals:

public slots:
    void prependText();
    void promoteItem(int);
    void favoriteAct();
    void removeAct();
    void clearAct();
    void surviveAct();

protected:
    inline bool connectPromoteItem();
    inline bool disconnectPromoteItem();
};

bool HistoryComboBox::connectPromoteItem()
{
    return connect(this,SIGNAL(currentIndexChanged(int)),SLOT(promoteItem(int)));
}

bool HistoryComboBox::disconnectPromoteItem()
{
    return disconnect(this,SLOT(promoteItem(int)));
}

#endif // HISTORYCOMBOBOX_H
