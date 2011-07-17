/*
* This file is part of Sir, an open-source cross-platform Image tool
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

#include "previewdialog.h"
#include <QGraphicsScene>
#include <QString>
#include <QPixmap>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include <QStringList>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QImageWriter>
#include <QKeyEvent>

#include "rawutils.h"
#include "metadatautils.h"

#define H 115
#define W 50


PreviewDialog::PreviewDialog(QWidget *parent, QStringList *images,
                             int currentImage):QDialog(parent) {

    setupUi(this);
    this->setWindowTitle(tr("SIR - Preview Images"));
    this->currentImage = currentImage;	
    this->images = images;
    this->imagePath = images->at(currentImage);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();
    this->setModal(true);

    rawEnabled = RawUtils::isRawEnabled();
    metadataEnabled = MetadataUtils::isEnabled();
    saveMetadata = MetadataUtils::isSave();
    initBar();
    createConnections();
    rotation = 0;

    if (metadataEnabled)
        metadata = new MetadataUtils();

    loadPixmap();

    scene = new QGraphicsScene();
    pix = scene->addPixmap(*image);
    view->setScene(scene);
    view->setCacheMode(QGraphicsView::CacheBackground);

    imageW = image->size().width();
    imageH = image->size().height();

    this->resize(imageW + W, imageH + H);
    statusBar->setText(imagePath);
    zoomFactor = 1.0;
    verifyImages();
    view->show();
}

PreviewDialog::~PreviewDialog() {
    delete scene;
    if (metadataEnabled)
        delete metadata;
}

void PreviewDialog::createConnections() {

    connect(quitButton, SIGNAL(clicked()), SLOT(close()));
    connect(zoomComboBox, SIGNAL(activated ( const QString &  )),
            SLOT(zoom(const QString & )));
    connect(rotateCwButton, SIGNAL(clicked ()), SLOT(rotatecw()));
    connect(rotateCcwButton, SIGNAL(clicked ()), SLOT(rotateccw()));
    connect(nextButton, SIGNAL(clicked ()), SLOT(nextImage()));
    connect(previousButton, SIGNAL(clicked ()), SLOT(previousImage()));
    connect(fullScreenButton, SIGNAL(clicked ()), SLOT(fullScreen()));
    connect(saveImageButton, SIGNAL(clicked ()), SLOT(save()));
    connect(saveAsButton, SIGNAL(clicked ()), SLOT(saveAs()));
    connect(printButton, SIGNAL(clicked ()), SLOT(print()));

}

void PreviewDialog::initBar() {
    for(int i = 100; i >= 10; i-=10) {		
        zoomComboBox->addItem(QString::number(i) + "%");
    }
    zoomComboBox->addItem(tr("Fit to window size"));

    comboLabel->setFixedSize(22,22);
    zoomComboBox->setFixedWidth(62);
    zoomComboBox->setEditable(true);	
    zoomComboBox->setToolTip(tr("Zoom image"));

    previousButton->setDefault(false);

    previousButton->setFixedSize(22,22);
    nextButton->setFixedSize(22,22);	

    previousButton->setToolTip(tr("Back to previous image"));
    nextButton->setToolTip(tr("Go to next image"));
    nextButton->setShortcut(Qt::Key_Right);
    previousButton->setShortcut(Qt::Key_Left);

    rotateCcwButton->setFixedSize(22,22);
    rotateCwButton->setFixedSize(22,22);

    rotateCcwButton->setToolTip(tr("Rotate counter clockwise"));
    rotateCwButton->setToolTip(tr("Rotate clockwise"));

    saveImageButton->setFixedSize(22,22);
    saveImageButton->setToolTip(tr("Save image"));
    saveImageButton->setShortcut(Qt::CTRL + Qt::Key_S);

    saveAsButton->setFixedSize(22,22);
    saveAsButton->setToolTip(tr("Save image as..."));

    quitButton->setFixedSize(80,22);
    quitButton->setToolTip(tr("Quit preview"));

    fullScreenButton->setFixedSize(22,22);
    fullScreenButton->setToolTip(tr("Fullscreen"));
    fullScreenButton->setShortcut(Qt::Key_F11);

    printButton->setFixedSize(22,22);	
    printButton->setToolTip(tr("Print current image..."));
    printButton->setShortcut(Qt::CTRL + Qt::Key_P);

}

void PreviewDialog::zoom( const QString & text ) {

    bool ok;
    QString aux(text);

    if (text == tr("Fit to window size")) {
        view->fitInView(pix,Qt::KeepAspectRatio);
        return;
    }

    if (aux.endsWith("%"))
        aux.chop(1);

    double dec = aux.toDouble(&ok)/100.;
    if (!ok) {
        QMessageBox::warning(
                this, "SIR",
                tr("Please enter a valid zoom factor." ));
        return;
    }
    zoomFactor = dec;

    view->resetTransform();
    view->scale(zoomFactor,zoomFactor);
    view->rotate(rotation);
}

void PreviewDialog::rotatecw( ) {
    rotation += 90;
    view->rotate(90);

    if(rotation == 360) {
        rotation = 0;
    }
}

void PreviewDialog::rotateccw( ) {
    rotation -= 90;
    view->rotate(-90);

    if(rotation == -360) {
        rotation = 0;
    }
}

void  PreviewDialog::nextImage( ) {

    view->resetMatrix();
    imagePath = images->at(++currentImage);
    scene->removeItem(pix);	
    delete pix;
    delete image;	

    loadPixmap();

    pix = scene->addPixmap(*image);
    view->setSceneRect(pix->boundingRect());
    imageW = image->size().width();
    imageH = image->size().height();

    if(zoomFactor != 1.0) {
        zoom(zoomComboBox->currentText());
    }
    
    statusBar->setText(imagePath);
    verifyImages();	
}

void  PreviewDialog::previousImage( ) {

    if(previousButton->hasFocus()) {	
        view->resetMatrix();
        imagePath = images->at(--currentImage);
        scene->removeItem(pix);
        delete pix;
        delete image;

        loadPixmap();

        pix = scene->addPixmap(*image);
        view->setSceneRect(pix->boundingRect());
        imageW = image->size().width();
        imageH = image->size().height();
        
        if(zoomFactor != 1.0) {
            zoom(zoomComboBox->currentText());
        }
        statusBar->setText(imagePath);;
        verifyImages();	
    }
}

void  PreviewDialog::verifyImages() {

    if(currentImage+2 > images->size()) { 
        nextButton->setEnabled(false);
    }
    else{
        nextButton->setEnabled(true);
    }

    if(currentImage == 0) {
        previousButton->setEnabled(false);
    }
    else {
        previousButton->setEnabled(true);	
    }

}

void PreviewDialog::fullScreen() {

    if(this->isFullScreen()) {
        this->showNormal();
    }
    else {
        this->showFullScreen();
    }

}

bool PreviewDialog::save() {

    bool ret;

    switch ( QMessageBox::question(
            this,
            tr("Overwrite File? -- SIR"),
            tr("A file called %1 already exists."
               "Do you want to overwrite it?").arg( imagePath),
            tr("&Yes"), tr("&No"), 0) ) {

            case 0:
                ret = saveFile(imagePath);
                if(ret) {
                    reloadImage(imagePath);
                }
            return ret;
            case 1:
                return false;
    }

    return false;
}

bool PreviewDialog::saveAs() {
    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList list;
    QString fileFilters;

    foreach(QByteArray format, imageFormats) {
        list.append(*new QString(format));
    }

    fileFilters = "*.";
    fileFilters += list.join(" *.").toLower() + " *.jpg"+ " *.JPG"+ " *.JPEG"
                   + " *.Jpg"+ " *.Jpeg"+ " *.PNG"+ " *.Png";

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save File"),
                                                    imagePath,fileFilters);
    
    if (fileName.isEmpty()) {
        return false;
    }

    bool ret = saveFile(fileName);
    
    if(ret) {
        reloadImage(fileName);
    }
    return ret;
}

bool PreviewDialog::saveFile(const QString &fileName) {
    int w = (int)(imageW*zoomFactor);
    int h = (int)(imageH*zoomFactor);

    qint16 orientation = 0;
    bool changedOrientation = false;
    if (metadataEnabled && saveMetadata) {
        orientation = metadata->exifOrientation();
        changedOrientation = (orientation==6 && rotation!=90)
                                || (orientation==8 && rotation!=-90);
        if (changedOrientation) {
            metadata->setExifOrientation(1);
            orientation = 1;
        }
    }
    if ( (rotation%180 != 0 || orientation==0) && changedOrientation ) {
        int aux = w;
        w = h;
        h = aux;
    }

    imageW = w;
    imageH = h;
    
    QPixmap destImg =  image->scaled(w, h,Qt::IgnoreAspectRatio,
                                     Qt::SmoothTransformation);

    if (destImg.save(fileName, 0 ,100)) {
        if (saveMetadata) {
            metadata->write(fileName);
        }
        statusBar->setText(tr("File saved"));
        if (!(orientation == 6 || orientation == 8))
            rotation = 0;
        return true;
    }
    else {
        statusBar->setText(tr("Failed to save image"));
        if (!(orientation == 6 || orientation == 8))
            rotation = 0;
        return false;
    }
}

void PreviewDialog::reloadImage(QString imageName) {
    view->resetMatrix();
    scene->removeItem(pix);	
    delete pix;
    delete image;
    rotation = 0;

    imagePath = imageName;	

    loadPixmap();

    pix = scene->addPixmap(*image);
    view->setSceneRect(pix->boundingRect());
    imageW = image->size().width();
    imageH = image->size().height();
    
    if(zoomFactor != 1.0) {
        zoom("100%");
        int index = zoomComboBox->findText("100%");
        zoomComboBox->setCurrentIndex(index);
    }
    if (metadataEnabled)
        view->repaint();
}

void PreviewDialog::loadPixmap() {
    image = new QPixmap();

    if(rawEnabled) {
        if(RawUtils::isRaw(imagePath)) {
            image = RawUtils::loadRawPixmap(imagePath);
        }
        else {
            image->load(imagePath);
        }
    }
    else {
        image->load(imagePath);
    }

    if (metadataEnabled) {
        metadata->read(imagePath);
        qint16 orientation = metadata->exifOrientation();
        if (orientation==6)
            rotatecw();
        else if (orientation==8)
            rotateccw();
    }
}

void PreviewDialog::print() {

    QPrinter printer;

    if (QPrintDialog(&printer).exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);
        scene->render(&painter);
    }

}
