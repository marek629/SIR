/*
 * This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto
 * 2011-2012  Marek Jędryka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
 *                 Marek Jędryka   <jedryka89@gmail.com>
 * Program URL: http://sir.projet-libre.org/
 *
 */

#ifndef CONVERTDIALOG_H
#define CONVERTDIALOG_H

#include "ui_convertdialog.h"
#include "convertthread.h"
#include <QSettings>
#include <QQueue>

class QString;
class QDropEvent;
class QPicture;
class QStringList;
class QDir;
class QMessageBox;
class QFileDialog;
class QImageWriter;
class QMenu;
class QCompleter;
class myQTreeWidget;
class PreviewDialog;
class QTranslator;
class NetworkUtils;
class QPoint;
class QSize;
class MetadataDialog;

//! Main window class provides images convertion dialog.
class ConvertDialog : public QMainWindow, public Ui::ConvertDialog {

    Q_OBJECT

public:
    ConvertDialog(QWidget *parent = 0, QString args = 0);
    ~ConvertDialog();
    void retranslateStrings();

private:
    struct QueryData {
        QString filePath;
        int tid;
    };
    QQueue<QueryData> overwriteQueue;
    QQueue<QueryData> enlargeQueue;
    void initList();
    void init();
    void createConnections();
    inline void connectSizeLinesEdit();
    inline void disconnectSizeLinesEdit();
    void createActions();
    void createRawFilesList();
    void questionOverwrite(QueryData data);
    void questionEnlarge(QueryData data);
    inline void writeWindowProperties();
    inline void resetAnswers();
    QStringList *makeList();
    QString makeImagePath(QTreeWidgetItem *item);
    void convert();

    QList<ConvertThread*> convertThreads;
    QString args;
    QStringList argsList;
    QImage *image;
    QString targetFile;
    QString fileFilters;
    QStringList rawFormats;
    QString lastDir;
    PreviewDialog  *previewForm;
    MetadataDialog *metadataForm;
    quint8 numThreads;
    QTranslator *appTranslator;
    QMap<QString, int>  *statusList;
    int convertedImages;
    int numImages;
    QList<QTreeWidgetItem *> itemsToConvert;
    bool converting;
    bool rawEnabled;
    bool alreadSent;
    NetworkUtils *net;
    QPoint windowPossition;
    QSize windowSize;
    QAction *removeAction;
    QAction *convertAction;
    QAction *previewAction;
    QAction *metadataAction;
    QTreeWidgetItem *treeMenuItem;
    QString sizeWidthString;
    QString sizeHeightString;

protected:
    virtual void changeEvent(QEvent *e);

public slots:
    void browseDestination();
    void convertAll();
    void convertSelected();
    void addDir();
    void addFile();
    void removeAll();
    void removeSelectedFromList();
    void showMenu( const QPoint & point);
    void previewAct();
    void showPreview(QTreeWidgetItem *item, int col);
    void showMetadata();
    void verifyRotate(int status);
    void about();
    void setOptions();
    void readSettings();
    void updateTree();
    void setImageStatus(const QStringList& imageData, const QString& status, int statusNum);
    void query(const QString& targetFile, int tid, const QString& whatToDo);
    void giveNextImage(int tid);
    void setupThreads(int numThreads);
    void closeOrCancel();
    void updateInterface();
    void setCanceled();
    void stopConvertThreads();
    void checkUpdates();
    void showUpdateResult(QString *result, bool error);
    void sendInstall();
    void showSendInstallResult(QString *result, bool error);

private slots:
    void setSizeUnit(int index);
    void sizeChanged(const QString &value);
};

/** Saves window maximized status, possition on screen and size and last
  * choosed directory by the user in settings file.\n
  * If window is maximized this function will save last possition and size
  * of normal size mode (before maximizing).
  */
void ConvertDialog::writeWindowProperties() {
    QSettings settings("SIR");
    settings.beginGroup("MainWindow");
    if (this->isMaximized()) {
        settings.setValue("maximized",true);
        settings.setValue("possition",windowPossition);
        settings.setValue("size",windowSize);
    }
    else {
        settings.setValue("maximized",false);
        settings.setValue("possition",this->pos());
        settings.setValue("size",this->size());
    }
    settings.endGroup();
    settings.beginGroup("Settings");
    settings.setValue("lastDir", lastDir);
    settings.endGroup();
}

/** Resets user ansers about overwrite file, enlarge image and abort convertion
  * variables. This function is useful when convertion is starting for reset
  * user-anser data after last convertion.
  */
void ConvertDialog::resetAnswers() {
    ConvertThread::shared->overwriteResult = 1;
    ConvertThread::shared->overwriteAll = false;
    ConvertThread::shared->noOverwriteAll = false;
    ConvertThread::shared->abort = false;
    ConvertThread::shared->enlargeResult = 1;
    ConvertThread::shared->enlargeAll = false;
    ConvertThread::shared->noEnlargeAll = false;
}

void ConvertDialog::connectSizeLinesEdit() {
    connect(widthLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(sizeChanged(QString)));
    connect(heightLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(sizeChanged(QString)));
}

void ConvertDialog::disconnectSizeLinesEdit() {
    widthLineEdit->disconnect(this,SLOT(sizeChanged(QString)));
    heightLineEdit->disconnect(this, SLOT(sizeChanged(QString)));
}

#endif // CONVERTDIALOG_H
