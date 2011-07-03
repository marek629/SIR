#include "messagebox.h"
//#include "previewdialog.h"
//#include <QDir>

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
//    QPushButton* previewButton = box.addButton(tr("&Preview"), QMessageBox::HelpRole);
    QPushButton* yesButton = box.addButton(tr("&Yes"), QMessageBox::YesRole);
    QPushButton* noButton = box.addButton(tr("&No"), QMessageBox::NoRole);
    QPushButton* yesToAllButton = box.addButton(tr("Yes to &All"), QMessageBox::YesRole);
    QPushButton* noToAllButton = box.addButton(tr("N&o to All"), QMessageBox::NoRole);
    QPushButton* cancelButton = box.addButton(tr("&Cancel"), QMessageBox::RejectRole);
    box.exec();
//    if (box.clickedButton() == (QAbstractButton*)previewButton) {
//        qint16 origin = text.indexOf(QDir::separator());
//        qint16 end = text.lastIndexOf(QDir::separator(),origin);
//        end = text.indexOf(' ',end);
//#ifdef _WIN32
//        origin -= 2; // for device character on Windows systems
//#endif
//        QString filePath = text;
//        filePath.resize(end);
//        filePath.remove(0,origin);
//        QStringList *list = new QStringList(filePath);
//        PreviewDialog* previewForm = new PreviewDialog(parent, list);
//        previewForm->show();
//    }
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
