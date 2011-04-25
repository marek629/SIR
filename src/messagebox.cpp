#include "messagebox.h"

MessageBox::MessageBox(QWidget *parent) :
    QMessageBox(parent)
{
}

int MessageBox::question(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox box(parent);
    box.setWindowTitle(title);
    box.setText(text);
    box.setIcon(Question);
    QPushButton* yesButton = box.addButton(tr("&Yes"), QMessageBox::YesRole);
    QPushButton* noButton = box.addButton(tr("&No"), QMessageBox::NoRole);
    QPushButton* yesToAllButton = box.addButton(tr("Yes to &All"), QMessageBox::YesRole);
    QPushButton* noToAllButton = box.addButton(tr("N&o to All"), QMessageBox::NoRole);
    QPushButton* cancelButton = box.addButton(tr("&Cancel"), QMessageBox::RejectRole);
    box.exec();
    if (box.clickedButton() == (QAbstractButton*)yesButton)
        return Yes;
    if (box.clickedButton() == (QAbstractButton*)noButton)
        return No;
    if (box.clickedButton() == (QAbstractButton*)yesToAllButton)
        return YesToAll;
    if (box.clickedButton() == (QAbstractButton*)noToAllButton)
        return NoToAll;
    if (box.clickedButton() == (QAbstractButton*)cancelButton)
        return Cancel;
}
