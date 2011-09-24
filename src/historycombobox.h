#ifndef HISTORYCOMBOBOX_H
#define HISTORYCOMBOBOX_H

#include <QComboBox>

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
                       int newMaxCount = -1);

private:
    void createActions();
    QLineEdit *line_edit;
    QIcon favIcon;
    QIcon unfavIcon;
    int listViewRow;
    QAction *favAction;
    QAction *rmAction;
    QAction *clearAction;
    QAction *surviveAction;

signals:

public slots:
    void prependText();
    void promoteItem(int);
    void favoriteAct();
    void removeAct();
    void clearAct();
    void surviveAct();
    void showMenu(const QPoint&);

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
