#ifndef TIMECOMBOBOX_H
#define TIMECOMBOBOX_H

#include <QComboBox>

class TimeComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit TimeComboBox(QWidget *parent = 0);
    void addItem(const QString &text, const QVariant &userData = QVariant());
    void setEditable(bool editable);

private:
    int validIndex(const QString &text);

private slots:
    void addEditedText();
};

#endif // TIMECOMBOBOX_H
