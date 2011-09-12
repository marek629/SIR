#ifndef HISTORYITEMVIEW_H
#define HISTORYITEMVIEW_H

#include <QListView>

class HistoryComboBox;

class HistoryItemView : public QListView
{
    Q_OBJECT
public:
    explicit HistoryItemView(QWidget *parent = 0);
    ~HistoryItemView();
    int contexIndex() const { return contex_index; }

protected:
    virtual void contextMenuEvent(QContextMenuEvent *);

private:
    int contex_index;
    HistoryComboBox *comboBox;
    QAction *favAction;
    QAction *rmAction;
    QAction *clearAction;
    QAction *surviveAction;

signals:

public slots:

};

#endif // HISTORYITEMVIEW_H
