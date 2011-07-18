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

 #ifndef PREVIEWDIALOG_H
 #define PREVIEWDIALOG_H

#include "ui_previewdialog.h"

class QGraphicsScene;
class QString;
class QPixmap;
class QStringList;
class QGraphicsPixmapItem;
class QKeyEvent;
class MetadataUtils;

class PreviewDialog : public QDialog, public Ui::PreviewDialog {
    Q_OBJECT
public:
    PreviewDialog(QWidget *parent = 0, QStringList *images = 0,
                  int currentImage = 0);
    ~PreviewDialog();
     
private:
    //Methods
	void initBar();
    void createConnections();
    void verifyImages();
    void reloadImage(QString imagePath);
    void loadPixmap();

    //Class Variables
    QDesktopWidget* desktop;
    QGraphicsScene *scene;
    QString imagePath;
    QPixmap *image;
    double zoomFactor;
	int currentImage;
	int rotation;
	QStringList *images;
	QGraphicsPixmapItem *pix;
	int imageW;
	int imageH;
    bool rawEnabled;
    bool metadataEnabled;
    bool saveMetadata;
    MetadataUtils* metadata;

public slots:

    //Slots
	void zoom( const QString & text );
    void rotatecw( );
    void rotateccw( );
    void nextImage();
    void previousImage();
    void fullScreen();
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    void print();

protected:
    virtual void resizeEvent(QResizeEvent *);
};
#endif
