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

 #ifndef PREVIEWDIALOG_H
 #define PREVIEWDIALOG_H

#include "ui_previewdialog.h"
#include "metadatautils.h"
#include <QString>

class QGraphicsScene;
class QPixmap;
class QStringList;
class QGraphicsPixmapItem;
class QKeyEvent;

//! Preview image window.
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
    inline bool isntTiffImage();

    //Class Variables
    QGraphicsScene *scene;
    QString imagePath;
    QPixmap *image;
    double zoomFactor;
    int currentImage;
    int rotation;
    int flip;
    QStringList *images;
    QGraphicsPixmapItem *pix;
    int imageW;
    int imageH;
    bool rawEnabled;
    bool metadataEnabled;
    bool saveMetadata;
    MetadataUtils::Metadata* metadata;

public slots:

    //Slots
    void zoom(const QString &text);
    void rotatecw( );
    void rotateccw( );
    void flipVertical();
    void flipHorizontal();
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

bool PreviewDialog::isntTiffImage() {
    QString ext = imagePath.split('.').last().toLower();
    bool tiffMetadataDisabled = !(ext == "tif" || ext == "tiff");
    if (MetadataUtils::Metadata::isEnabled() && tiffMetadataDisabled) {
        metadata = new MetadataUtils::Metadata();
        return true;
    }
    return false;
}

#endif
