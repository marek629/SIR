#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>

class MessageBox : public QMessageBox
{
    Q_OBJECT
public:
    explicit MessageBox(QWidget *parent = 0);
    static int question(QWidget *parent, const QString &title, const QString &text);
    enum Button
    {
        Yes=0,
        YesToAll=2,
        No=1,
        NoToAll=3,
        Cancel=4
    };

signals:

public slots:

};

#endif // MESSAGEBOX_H
