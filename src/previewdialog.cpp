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
#include <QDesktopWidget>

#include <QDebug>

#include "rawutils.h"
#include "metadatautils.h"

#define H 115
#define W 50

/*! Default constructor.
 * \param parent Parent widget.
 * \param images List of image files path.
 * \param currentImage Index of current image on \a images list.
 */
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
    metadataEnabled = isntTiffImage();
    saveMetadata = MetadataUtils::Metadata::isSave();
    initBar();
    createConnections();
    rotation = 0;
    flip = MetadataUtils::None;

    if (metadataEnabled)
        metadata = new MetadataUtils::Metadata();

    loadPixmap();

    scene = new QGraphicsScene();
    pix = scene->addPixmap(*image);
    view->setScene(scene);
    view->setCacheMode(QGraphicsView::CacheBackground);

    imageW = image->size().width();
    imageH = image->size().height();

    QSize desktopSize = QApplication::desktop()->size();
    desktopSize -= QSize(8,8);
    int w = imageW + W;
    int h = imageH + H;
    if (desktopSize.width()>=w || desktopSize.height()>=h)
        this->resize(w,h);
    else
        this->showMaximized();

    statusBar->setText(imagePath);
    zoomFactor = 1.0;
    verifyImages();
    view->show();
}

/*! Destructor.
 * \par
 * Dealocates dynamic allocated memory.
 */
PreviewDialog::~PreviewDialog() {
    delete scene;
    delete images;
    if (metadataEnabled)
        delete metadata;
}

void PreviewDialog::createConnections() {

    connect(quitButton, SIGNAL(clicked()), SLOT(close()));
    connect(zoomComboBox, SIGNAL(activated(QString)), SLOT(zoom(QString)));
    connect(rotateCwButton, SIGNAL(clicked ()), SLOT(rotatecw()));
    connect(rotateCcwButton, SIGNAL(clicked ()), SLOT(rotateccw()));
    connect(flipHButton, SIGNAL(clicked()), SLOT(flipHorizontal()));
    connect(flipVButton, SIGNAL(clicked()), SLOT(flipVertical()));
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

/*! Zoom combo box slot.
 * \par
 * Zoom previewed image to typed perventage value as \a text. A valid value
 * should be decimal value and percent-sign without space, for example \em 100%.\n
 * A special value \em "Fit to window size" (translatable) is supported.
 */
void PreviewDialog::zoom(const QString &text) {

    bool ok;
    QString aux(text);

    if (text == tr("Fit to window size")) {
        double verticalRatio;
        double horizontalRatio;
        if (rotation%180 == 0) {
            verticalRatio = (double) view->width() / imageW;
            horizontalRatio = (double) view->height() / imageH;
        } else {
            verticalRatio = (double) view->width() / imageH;
            horizontalRatio = (double) view->height() / imageW;
        }
        if (verticalRatio < horizontalRatio) {
            view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            view->fitInView(pix,Qt::KeepAspectRatio);
            zoomFactor = (double) (view->width()-8) / imageW;
            view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else {
            view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            view->fitInView(pix,Qt::KeepAspectRatio);
            zoomFactor = (double) (view->height()-8) / imageH;
            view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
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

/*! Rotate clockwise button slot. Rotates clockwise viewing image.
 */
void PreviewDialog::rotatecw( ) {
    if (flip == MetadataUtils::None || rotation == 180 || rotation == -180) {
        rotation += 90;
        view->rotate(90);
        if(rotation == 270)
            rotation = -90;
    }
    else  if (rotation == 0) {
        rotation = -90;
        view->rotate(rotation);
    }
    else {
        rotation = 0;
        view->rotate(-90);
    }
}

/*! Rotate counterclockwise button slot. Rotates counterclockwise viewing image.
 */
void PreviewDialog::rotateccw( ) {
    if (flip == MetadataUtils::None || rotation == 180 || rotation == -180) {
        rotation -= 90;
        view->rotate(-90);
        if(rotation == -270)
            rotation = 90;
    }
    else  if (rotation == 0) {
        rotation = 90;
        view->rotate(rotation);
    }
    else {
        rotation = 0;
        view->rotate(90);
    }
}

/*! Flip vertical button slot. Flips verticaly current image.
 */
void PreviewDialog::flipVertical() {
    flip ^= MetadataUtils::Vertical;
    if (flip == MetadataUtils::VerticalAndHorizontal) {
        flip = MetadataUtils::None;
        if (rotation%180 != 0)
            view->scale(1.0,-1.0);
        else
            view->scale(-1.0, 1.0);
        rotation += 180;
        view->rotate(180);
        if (rotation == 270)
            rotation = -90;
        else if (rotation == 360)
            rotation = 0;
        return;
    }
    if (rotation%180 != 0)
        view->scale(-1.0,1.0);
    else
        view->scale(1.0, -1.0);
}

/*! Flip horizontal button slot. Flips horizontaly current image.
 */
void PreviewDialog::flipHorizontal() {
    using namespace MetadataUtils;
    flip ^= Horizontal;
    if (flip == VerticalAndHorizontal) {
        flip = None;
        if (rotation%180 != 0)
            view->scale(-1.0,1.0);
        else
            view->scale(1.0, -1.0);
        rotation += 180;
        view->rotate(180);
        if (rotation == 270)
            rotation = -90;
        else if (rotation == 360)
            rotation = 0;
        return;
    }
    if (rotation%180 != 0)
        view->scale(1.0,-1.0);
    else
        view->scale(-1.0, 1.0);
}

/*! Next image button slot. Shows next image.
 */
void  PreviewDialog::nextImage( ) {

    view->resetMatrix();
    imagePath = images->at(++currentImage);
    scene->removeItem(pix);
    delete pix;
    delete image;
    rotation = 0;
    flip = MetadataUtils::None;
    metadataEnabled = isntTiffImage();

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

/*! Previous image button slot. Shows previous image.
 */
void  PreviewDialog::previousImage( ) {

    if(previousButton->hasFocus()) {
        view->resetMatrix();
        imagePath = images->at(--currentImage);
        scene->removeItem(pix);
        delete pix;
        delete image;
        rotation = 0;
        flip = MetadataUtils::None;
        metadataEnabled = isntTiffImage();

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

/*! Full screen button slot. Shows this window in full screen or normal mode
 * depending on current window state.
 */
void PreviewDialog::fullScreen() {
    static bool maximized = false;
    if(this->isFullScreen()) {
        this->showNormal();
        if (maximized)
            this->showMaximized();
    }
    else {
        maximized = this->isMaximized();
        this->showFullScreen();
    }
}

/*! Save button slot. Ask and overwrite if accepted current file.
 */
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

/*! Save as button slot. Ask and overwrite if accepted current file.
 */
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

/*! Save image file into typed \a fileName path.
 */
bool PreviewDialog::saveFile(const QString &fileName) {
    int w = (int)(imageW*zoomFactor);
    int h = (int)(imageH*zoomFactor);

    if (flip == MetadataUtils::VerticalAndHorizontal) {
        flip = MetadataUtils::None;
        rotation += 180;
        if (rotation >= 360)
            rotation %= 360;
    }

    if (rotation == -270)
        rotation = 90;
    else if (rotation == 270)
        rotation = -90;
    else if (rotation == -180)
        rotation = 180;

    char orientation = 0;
    short rt = rotation;
    int fl = flip;
    if (metadataEnabled && saveMetadata) {
        orientation = metadata->exifStruct()->orientation;
        switch (orientation) {
        case 1:
            break;
        case 2:
            fl ^= MetadataUtils::Horizontal;
            break;
        case 3:
            rt -= 180;
            break;
        case 4:
            fl ^= MetadataUtils::Vertical;
            break;
        case 5:
            rt -= 90;
            fl ^= MetadataUtils::Horizontal;
            break;
        case 6:
            rt -= 90;
            break;
        case 7:
            rt += 90;
            fl ^= MetadataUtils::Horizontal;
            break;
        case 8:
            rt += 90;
            break;
        default:
            qWarning("PreviewDialog::loadPixmap(): Invalid orientation value");
            orientation = 1;
            break;
        }
        if (rt!=0  || fl!=MetadataUtils::None || orientation == 1) {
            orientation = MetadataUtils::Exif::getOrientation(rotation,flip);
            if (orientation < 1)
                orientation = 1;
            metadata->exifStruct()->orientation = orientation;
        }
        else
            orientation = 0;
    }
    if (orientation < 1 && rt%180 != 0) {
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
            metadata->setExifData();
            metadata->write(fileName, destImg.toImage());
        }
        statusBar->setText(tr("File saved"));
        return true;
    }
    else {
        statusBar->setText(tr("Failed to save image"));
        if (orientation < 1) {
            rotation = 0;
            flip = MetadataUtils::None;
        }
        return false;
    }
}

void PreviewDialog::reloadImage(QString imageName) {
    view->resetMatrix();
    scene->removeItem(pix);
    delete pix;
    delete image;
    rotation = 0;
    flip = MetadataUtils::None;
    imagePath = imageName;
    metadataEnabled = isntTiffImage();

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
    bool readSuccess;

    if(rawEnabled) {
        if(RawUtils::isRaw(imagePath)) {
            image = RawUtils::loadRawPixmap(imagePath);
            readSuccess = image;
        }
        else {
            readSuccess = image->load(imagePath);
        }
    }
    else {
        readSuccess = image->load(imagePath);
    }

    bool metadataReadError = false;
    if (readSuccess && metadataEnabled) {
        metadataReadError = !metadata->read(imagePath,true);
        if (!metadataReadError) {
            char orientation = metadata->exifStruct()->orientation;
            flip = MetadataUtils::None;
            switch (orientation) {
            case 1:
                rotation = 0;
                break;
            case 2:
                rotation = 0;
                flipHorizontal();
                break;
            case 3:
                rotation = 180;
                view->rotate(rotation);
                break;
            case 4:
                flipVertical();
                break;
            case 5:
                rotation = 90;
                view->rotate(rotation);
                flipHorizontal();
                break;
            case 6:
                rotation = 90;
                view->rotate(rotation);
                break;
            case 7:
                rotation = -90;
                view->rotate(rotation);
                flipHorizontal();
                break;
            case 8:
                rotation = -90;
                view->rotate(rotation);
                break;
            default:
                qWarning("PreviewDialog::loadPixmap(): Invalid orientation value");
                orientation = 1;
                rotation = 0;
                break;
            }
        }
    }

    if (!readSuccess) {
        QString errorTitle = tr("Image file error");
        QString errorMessage = tr("Load image %1 failed").arg(imagePath);
        QMessageBox::critical(this, errorTitle, errorMessage);
    }
    else if (metadataReadError) {
        MetadataUtils::Error *e = metadata->lastError();
        MetadataUtils::String str(e->message() +
                                  tr("\nError code: %1\nError message: %2").
                                     arg(e->code()).arg(e->what()) );
        qWarning(str.toNativeStdString().c_str());
    }
}

/*! Print button slot. Prints current image.
 */
void PreviewDialog::print() {
    QPrinter printer;
    if (QPrintDialog(&printer).exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);
        scene->render(&painter);
    }
}

/*! When window resized fits image to window size if special value \em "Fit to window size"
 * of zoom combo box is activated.
 * \par
 * This is overloaded QDialogs's method.
 */
void PreviewDialog::resizeEvent(QResizeEvent *) {
    QString currentZoomString = zoomComboBox->currentText();
    if (currentZoomString == tr("Fit to window size"))
        zoom(currentZoomString);
}
