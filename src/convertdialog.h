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
    /*! Default constuctor.
     * Sets up window with saved settings like window state, position
     * and convertion preferences.
     * \param parent Pointer to parent object.
     * \param args String containing aplication argv tables records
     *     separated by ** (double star-sign).
     * \sa readSettings
     */
    ConvertDialog(QWidget *parent = 0, QString args = 0);

    /*! Destructor.
     * Writes window state and position and dealocates dynamic allocated memory.
     */
    ~ConvertDialog();

    /*! Retranslates GUI.
     */
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
    /*! Save new window state and size in private fields.
     * This is overloaded QWidget's method.
     */
    virtual void changeEvent(QEvent *e);

public slots:
    /*! Browse destination directory button slot.
     * Choose destination directory in QFileDialog.
     * \par
     * Path from neighboring line edit is setting as origin directory in QFileDialog.
     * If the path is empty origin directory will be set to home path.
     */
    void browseDestination();

    /*! Convert all button slot.
     * Load all items from tree widget into list of images to convert and
     * call #convert() function.
     * \sa convertSelected
     */
    void convertAll();

    /*! Convert selected button slot.
     * Load selected items from tree widget into list of images to convert and
     * call #convert() function.
     * \sa convertAll
     */
    void convertSelected();

    /*! Add directory button and action slot.
     * Load all supported image files from choosed directory (non-recursive)
     * into tree widget and set state to \em "Not converted yet".\n
     * This function remember last opened directory in the same session.
     * Default directory is home directory.
     * \sa addFile
     */
    void addDir();

    /*! Add file button and action slot.
     * Load selected image files into tree widget and set state to
     * \em "Not \em converted \em yet".\n
     * This function remember last opened directory in the same session.
     * Default directory is home directory.
     * \sa addDir
     */
    void addFile();

    /*! Remove all button and action slot.
     * Remove all items of tree widget.
     * \sa removeSelectedFromList
     */
    void removeAll();

    /*! Remove selected button and action slot.
     * Remove selected items of tree widget.
     * \sa removeAll
     */
    void removeSelectedFromList();

    /*! Shows context menu.
     * Shows context menu for selected tree widgets item.
     * \param point Global position of context menu.
     * \sa showPreview showMetadata convertSelected removeSelectedFromList
     */
    void showMenu( const QPoint & point);

    /*! Preview action slot.
     * Show preview action from context menu slot providing usefull interface
     * for #showPreview slot.
     * \sa showMenu
     */
    void previewAct();

    /*! Show preview dialog.
     * Show preview dialog containig selected \a item image.\n
     * This slot is called when tree widgets \a item was double clicked.
     * \param item Pointer to selected tree widgets item.
     * \param col Is ignored, exists for signal-slot compatibility only.
     * \sa showMenu previewAct
     */
    void showPreview(QTreeWidgetItem *item, int col);

    /*! Metadata action slot.
     * Shows metadata dialog containg selected tree widgets item image metadata.
     * \sa showMenu showPreview
     */
    void showMetadata();

    /*! Rotate checkbox slot.
     * Disables/enables rotation angle line edit.
     * \param status Status of the checkbox.
     */
    void verifyRotate(int status);

    /*! Shows window containing information about SIR.
     */
    void about();

    /*! Writes window state and position and show options dialog.
     */
    void setOptions();

    /*! Reads settings and sets up window state, position and convertion preferences.
     */
    void readSettings();

    /*! Updates tree widget when it will change.
     */
    void updateTree();

    /*! Set converting status of image.
     * \param imageData List of strings containing path, image name and file extension.
     * \param status Status message.
     * \param statusNum Status code defined in src/defines.h.
     */
    void setImageStatus(const QStringList& imageData, const QString& status, int statusNum);

    /*! Ask for users agreement of typed action on file.
     * \param targetFile Asking file path.
     * \param tid Worker thread ID.
     * \param whatToDo Action on target file. Support for \em overwrite and \em enlarge only.
     */
    void query(const QString& targetFile, int tid, const QString& whatToDo);

    /*! Gives next image for worker thread legitimazed \a tid thread ID.
     */
    void giveNextImage(int tid);

    /*! Setup \a numThreads worker threads.
     */
    void setupThreads(int numThreads);

    /*! Cancel converting if converting runs; otherwise close window.
     */
    void closeOrCancel();

    /*! Update user interface after convering.
     */
    void updateInterface();

    /*! Set image status to \em Cancelled if the image isn't converted yet.
     */
    void setCanceled();

    /*! Terminate all worker threads.
     */
    void stopConvertThreads();

    /*! Check updates on SIR website.
     * \sa showUpdateResult
     */
    void checkUpdates();

    /*! Show result of checkUpdates() in message box.
     * \sa checkUpdates
     */
    void showUpdateResult(QString *result, bool error);

    /*! Notify SIR installation on project website if alread didn't notified;
     * otherwise show message box about alread sended.
     * \sa showSendInstallResult
     */
    void sendInstall();

    /*! Show message box about notified installation.
     * \sa sendInstall
     */
    void showSendInstallResult(QString *result, bool error);

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
