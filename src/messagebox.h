#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>

class MessageBox : public QMessageBox
{
    Q_OBJECT
public:
    explicit MessageBox(QWidget *parent = 0);
    static QMessageBox::StandardButton question(QWidget *parent, const QString &title, const QString &text);

signals:

public slots:

};

#endif // MESSAGEBOX_H
