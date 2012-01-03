/*
    * This file is part of SIR, an open-source cross-platform Image tool
	* 2007  Rafael Sachetto
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
    * Program URL: http://sir.projet-libre.org/
	*
	*/

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
    QStringList *makeList();
    QString makeImagePath(QTreeWidgetItem *item);
    QTranslator *appTranslator;
    QMap<QString, int>  *statusList;
    int convertedImages;
    int numImages;
    QList<QTreeWidgetItem *> selectedItems;
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
	virtual void browseDestination();
	virtual void convert();
	virtual void addDir();
	virtual void removeAll();
	virtual void removeSelectedFromList();
	virtual void addFile();
	virtual void convertSelected();
	virtual void verify();
	virtual void showPreview(QTreeWidgetItem *item, int col);
    virtual void showMetadata();
	virtual void showMenu( const QPoint & point);
	virtual void verify(int status);
	virtual void about();
	virtual void setOptions();
	virtual void readSettings();
	virtual void updateTree();
	virtual void setImageStatus(const QStringList& imageData, const QString& status, int statusNum);
    virtual void query(const QString& targetFile, int tid, const QString& whatToDo);
	virtual void giveNextImage(int tid, bool onlySelected);
	virtual void setupThreads(int numThreads);
	virtual void closeOrCancel();
	virtual void updateInterface();
	virtual void setCanceled();
	virtual void stopConvertThreads();
    virtual void checkUpdates();
    virtual void showUpdateResult(QString *result, bool error);
    virtual void sendInstall();
    virtual void showSendInstallResult(QString *result, bool error);
    void previewAct();

private slots:
    void setSizeUnit(int index);
    void sizeChanged(const QString &value);
};

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
}

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
