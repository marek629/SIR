#ifndef TIMECOMBOBOX_H
#define TIMECOMBOBOX_H

#include <QComboBox>

class TimeComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit TimeComboBox(QWidget *parent = 0);
    void addItem(const QString &text, const QVariant &userData = QVariant());

private:
    int rigthIndex(const QString &text);

signals:

public slots:

};

#endif // TIMECOMBOBOX_H
