#include "application.h"
#include <exiv2/error.hpp>

#include <QDebug>
#include <QString>
#include <QMessageBox>

bool Application::notify(QObject *receiver, QEvent *event)
{
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch (Exiv2::Error &e)
    {
        QString errorMessage = QString::fromUtf8(e.what()) + "\n"+
                tr("Error code: ")+QString::number(e.code());
        QMessageBox::critical(0, tr("Metadata error"), errorMessage);
    }
    catch (...)
    {
        qDebug("Unknown error!");
    }
}
