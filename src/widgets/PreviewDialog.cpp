/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2015  Marek Jędryka   <jedryka89@gmail.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Program URL: http://marek629.github.io/SIR/
 */

#include <QGraphicsScene>
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
#include <QtSvg/QSvgRenderer>
#include <QDialogButtonBox>
#include "widgets/PreviewDialog.hpp"
#include "RawUtils.hpp"
#include "raw/RawPixmapLoader.hpp"

#define H 115
#define W 50

using namespace sir;

/** Default constructor.
  * \param parent Parent widget.
  * \param images List of image files path.
  * \param currentImage Index of current image on \a images list.
  */
PreviewDialog::PreviewDialog(QWidget *parent, QStringList *images,
                             int currentImage):QDialog(parent) {

    setupUi(this);
    this->setWindowTitle(tr("SIR - Preview Images"));
    this->setWindowFlags(Qt::Window);
    this->currentImage = currentImage;
    this->images = images;
    this->imagePath = images->at(currentImage);
    this->fileExt = imagePath.split('.').last().toLower().toLatin1();
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();
    this->setModal(true);

    rawEnabled = RawUtils::isRawEnabled();
#ifdef SIR_METADATA_SUPPORT
    metadataEnabled = isntTiffImage();
    saveMetadata = Settings::instance()->metadata.saveMetadata;
#endif // SIR_METADATA_SUPPORT
    initBar();
    createConnections();
    saveImageButton->setEnabled(
                    QImageWriter::supportedImageFormats().contains(fileExt) );
    rotation = 0;
    flip = MetadataUtils::None;

#ifdef SIR_METADATA_SUPPORT
    if (metadataEnabled)
        metadata = new MetadataUtils::Metadata();
#endif // SIR_METADATA_SUPPORT

    loadPixmap();

    scene = new QGraphicsScene();
    imageItem = addImageIntoScene();
    view->setScene(scene);
    view->setCacheMode(QGraphicsView::CacheBackground);

    imageW = imageItem->boundingRect().width();
    imageH = imageItem->boundingRect().height();

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

/** Destructor.\n
  * Dealocates dynamic allocated memory.
  */
PreviewDialog::~PreviewDialog() {
    delete scene;
    delete images;
#ifdef SIR_METADATA_SUPPORT
    if (metadataEnabled)
        delete metadata;
#endif // SIR_METADATA_SUPPORT
}

/** Connects signals emited by \em Actions toolbar for corresponding slots. */
void PreviewDialog::createConnections() {
    connect(quitButton, SIGNAL(clicked()), SLOT(close()));
    connect(zoomComboBox, SIGNAL(activated(QString)), SLOT(zoom(QString)));
    connect(rotateCwButton, SIGNAL(clicked()), SLOT(rotatecw()));
    connect(rotateCcwButton, SIGNAL(clicked()), SLOT(rotateccw()));
    connect(flipHButton, SIGNAL(clicked()), SLOT(flipHorizontal()));
    connect(flipVButton, SIGNAL(clicked()), SLOT(flipVertical()));
    connect(nextButton, SIGNAL(clicked()), SLOT(nextImage()));
    connect(previousButton, SIGNAL(clicked()), SLOT(previousImage()));
    connect(fullScreenButton, SIGNAL(clicked()), SLOT(fullScreen()));
    connect(saveImageButton, SIGNAL(clicked()), SLOT(save()));
    connect(saveAsButton, SIGNAL(clicked()), SLOT(saveAs()));
    connect(printButton, SIGNAL(clicked()), SLOT(print()));
}

/** Sets up \em Actions toolbar. */
void PreviewDialog::initBar() {
    QString imagesDirPath = QCoreApplication::applicationDirPath() +
                            "/../share/sir/images/";

    previousButton->setDefault(false);
    previousButton->setToolTip(tr("Back to previous image"));
    previousButton->setShortcut(Qt::Key_Left);
    QIcon icon = QIcon::fromTheme("go-previous",
                                  QIcon(imagesDirPath + "previous.png"));
    previousButton->setIcon(icon);

    nextButton->setToolTip(tr("Go to next image"));
    nextButton->setShortcut(Qt::Key_Right);
    icon = QIcon::fromTheme("go-next",
                            QIcon(imagesDirPath + "next.png"));
    nextButton->setIcon(icon);

    rotateCcwButton->setToolTip(tr("Rotate counter clockwise"));
    icon = QIcon::fromTheme("object-rotate-left",
                            QIcon(imagesDirPath + "rotate_ccw.png"));
    rotateCcwButton->setIcon(icon);

    rotateCwButton->setToolTip(tr("Rotate clockwise"));
    icon = QIcon::fromTheme("object-rotate-right",
                            QIcon(imagesDirPath + "rotate_cw.png"));
    rotateCwButton->setIcon(icon);

    icon = QIcon::fromTheme("object-flip-horizontal",
                            QIcon(imagesDirPath + "flip_horizontal.png"));
    flipHButton->setIcon(icon);

    icon = QIcon::fromTheme("object-flip-vertical",
                            QIcon(imagesDirPath + "flip_vertical.png"));
    flipVButton->setIcon(icon);

    comboLabel->setFixedSize(22,22);
    icon = QIcon::fromTheme("zoom-original",
                            QIcon(imagesDirPath + "zoom.png"));
    comboLabel->setPixmap(icon.pixmap(comboLabel->size()));

    for (int i=100; i>=10; i-=10)
        zoomComboBox->addItem(QString::number(i) + "%");
    zoomComboBox->addItem(tr("Fit to window size"));
    zoomComboBox->setEditable(true);
    zoomComboBox->setToolTip(tr("Zoom image"));

    fullScreenButton->setToolTip(tr("Fullscreen"));
    fullScreenButton->setShortcut(Qt::Key_F11);
    icon = QIcon::fromTheme("view-fullscreen",
                            QIcon(imagesDirPath + "window_fullscreen.png"));
    fullScreenButton->setIcon(icon);

    saveImageButton->setToolTip(tr("Save image"));
    saveImageButton->setShortcut(Qt::CTRL + Qt::Key_S);
    icon = QIcon::fromTheme("document-save",
                            QIcon(imagesDirPath + "filesave.png"));
    saveImageButton->setIcon(icon);

    saveAsButton->setToolTip(tr("Save image as..."));
    saveAsButton->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
    icon = QIcon::fromTheme("document-save-as",
                            QIcon(imagesDirPath + "filesaveas.png"));
    saveAsButton->setIcon(icon);

    printButton->setToolTip(tr("Print current image..."));
    printButton->setShortcut(Qt::CTRL + Qt::Key_P);
    icon = QIcon::fromTheme("document-print",
                            QIcon(imagesDirPath + "print.png"));
    printButton->setIcon(icon);

    quitButton->setToolTip(tr("Quit preview"));
    icon = QIcon::fromTheme("application-exit",
                            QIcon(imagesDirPath + "exit.png"));
    quitButton->setIcon(icon);
}

/** Zoom combo box slot.
  * \par
  * Zooms previewed image to typed perventage value as \a text. A valid value
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
            view->fitInView(imageItem,Qt::KeepAspectRatio);
            zoomFactor = (double) (view->width()-8) / imageW;
            view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else {
            view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            view->fitInView(imageItem,Qt::KeepAspectRatio);
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

/** Rotate clockwise button slot. Rotates clockwise viewing image. */
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

/** Rotate counterclockwise button slot. Rotates counterclockwise viewing image. */
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

/** Flip vertical button slot. Flips verticaly current image. */
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

/** Flip horizontal button slot. Flips horizontaly current image. */
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

/** Next image button slot. Shows next image. */
void  PreviewDialog::nextImage( ) {
    view->resetMatrix();

    imagePath = images->at(++currentImage);
    fileExt = imagePath.split('.').last().toLower().toLatin1();
    scene->removeItem(imageItem);
    delete imageItem;
    delete image;

    rotation = 0;
    flip = MetadataUtils::None;

#ifdef SIR_METADATA_SUPPORT
    metadataEnabled = isntTiffImage();
#endif // SIR_METADATA_SUPPORT

    saveImageButton->setEnabled(
                    QImageWriter::supportedImageFormats().contains(fileExt) );

    loadPixmap();

    imageItem = addImageIntoScene();
    view->setSceneRect(imageItem->boundingRect());
    imageW = imageItem->boundingRect().width();
    imageH = imageItem->boundingRect().height();

    if (zoomFactor != 1.0) {
        zoom(zoomComboBox->currentText());
    }

    statusBar->setText(imagePath);
    verifyImages();
}

/** Previous image button slot. Shows previous image. */
void  PreviewDialog::previousImage( ) {
    view->resetMatrix();

    imagePath = images->at(--currentImage);
    fileExt = imagePath.split('.').last().toLower().toLatin1();
    scene->removeItem(imageItem);
    delete imageItem;
    delete image;

    rotation = 0;
    flip = MetadataUtils::None;

#ifdef SIR_METADATA_SUPPORT
    metadataEnabled = isntTiffImage();
#endif // SIR_METADATA_SUPPORT

    saveImageButton->setEnabled(
                    QImageWriter::supportedImageFormats().contains(fileExt) );

    loadPixmap();

    imageItem = addImageIntoScene();
    view->setSceneRect(imageItem->boundingRect());
    imageW = imageItem->boundingRect().width();
    imageH = imageItem->boundingRect().height();

    if (zoomFactor != 1.0)
        zoom(zoomComboBox->currentText());

    statusBar->setText(imagePath);;
    verifyImages();
}

/** Menages enabled state of next and previous image buttons.\n
  * Disables next image button if current image is last of images list,
  * otherwise enables this button.\n
  * Disables previous image button if current image is first of images list,
  * otherwise enables this button.\n
  */
void  PreviewDialog::verifyImages() {
    if(currentImage+2 > images->size())
        nextButton->setEnabled(false);
    else
        nextButton->setEnabled(true);
    if(currentImage == 0)
        previousButton->setEnabled(false);
    else
        previousButton->setEnabled(true);
}

/** Full screen button slot. Shows this window in full screen or normal mode
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

/** Save button slot. Ask and overwrite if accepted current file. */
bool PreviewDialog::save() {
    bool ret;

    switch (QMessageBox::question(
                this,
                tr("Overwrite File? -- SIR"),
                tr("A file called %1 already exists."
                    "Do you want to overwrite it?").arg( imagePath),
                tr("&Yes"), tr("&No"), 0)) {
        case 0:
            ret = saveFile(imagePath);
            if (ret) {
                reloadImage(imagePath);
            }
            return ret;
        case 1:
            return false;
    }

    return false;
}

/** Save as button slot. Ask and overwrite if accepted current file. */
bool PreviewDialog::saveAs() {
    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList list;
    QString fileFilters;

    foreach(QByteArray format, imageFormats) {
        list.append(*new QString(format));
    }

    fileFilters = list.join(" *.").toLower() + " *.jpg"+ " *.JPG"+ " *.JPEG"
                   + " *.Jpg"+ " *.Jpeg"+ " *.PNG"+ " *.Png";

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save File"),
                                                   imagePath,fileFilters);
    if (fileName.isEmpty()) // when QFileDialog::getSaveFileName cancelled
        return false;
    if ( !QImageWriter::supportedImageFormats().contains(
                    fileName.split('.').last().toLower().toLatin1() ) ) {
        QDialog *dialog = new QDialog(this, Qt::Dialog);
        dialog->setWindowTitle(tr("Unsupported file format"));
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *label = new QLabel(tr("Write into %1 file is unsupported.\n"
                                      "Choose supported file extension:").
                                        arg(fileName), dialog);
        layout->addWidget(label);
        QComboBox *comboBox = new QComboBox(dialog);
        foreach (QByteArray format, imageFormats)
            comboBox->addItem(format.toLower());
        comboBox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        connect(comboBox, SIGNAL(currentIndexChanged(QString)),
                this, SLOT(setDestFileExt(QString)) );
        comboBox->setCurrentIndex(
                    comboBox->findText(Settings::instance()->settings.targetFormat));
        layout->addWidget(comboBox);
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                           QDialogButtonBox::Cancel,
                                                           Qt::Horizontal, dialog);
        buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
        connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
        layout->addWidget(buttonBox);
        dialog->setLayout(layout);
        if (dialog->exec() == QDialog::Rejected)
            return false;
        fileName = fileName.left(fileName.lastIndexOf('.')+1);
        fileName += destFileExtension;
    }
    bool ret = saveFile(fileName);
    if (ret)
        reloadImage(fileName);
    return ret;
}

/** Sets destFileExtension value to \b ext. */
void PreviewDialog::setDestFileExt(const QString &ext) {
    destFileExtension = ext;
}

/** Save image file into typed \a fileName path. */
bool PreviewDialog::saveFile(const QString &fileName) {
    int w = (int)(imageW*zoomFactor);
    int h = (int)(imageH*zoomFactor);
#ifdef SIR_METADATA_SUPPORT
    if (flip == MetadataUtils::VerticalAndHorizontal) {
        flip = MetadataUtils::None;
        rotation += 180;
        if (rotation >= 360)
            rotation %= 360;
    }
#endif // SIR_METADATA_SUPPORT

    if (rotation == -270)
        rotation = 90;
    else if (rotation == 270)
        rotation = -90;
    else if (rotation == -180)
        rotation = 180;

    char orientation = 0;
    short rt = rotation;
#ifdef SIR_METADATA_SUPPORT
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
#endif // SIR_METADATA_SUPPORT
    if (orientation < 1 && rt%180 != 0) {
        int aux = w;
        w = h;
        h = aux;
    }

    imageW = w;
    imageH = h;

    QPixmap destImg(1,1);
    QString ext = fileName.split('.').last().toLower().toLatin1();
    if (ext == "png" || ext == "gif")
        destImg.fill(Qt::transparent);
    else
        destImg.fill(Qt::white);
    destImg = destImg.scaled(w,h);
    QPainter painter(&destImg);
    if (svgLoaded)
        svgImage->renderer()->render(&painter);
    else
        painter.drawPixmap(0,0, image->scaled(w,h,Qt::IgnoreAspectRatio,
                                              Qt::SmoothTransformation) );

    if (destImg.save(fileName, 0 ,100)) {
#ifdef SIR_METADATA_SUPPORT
        if (saveMetadata) {
            metadata->setExifData();
            metadata->write(fileName, destImg.toImage());
        }
#endif // SIR_METADATA_SUPPORT
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

/** Reloads current image after save.
  * \sa save saveAs
  */
void PreviewDialog::reloadImage(QString imageName) {
    view->resetMatrix();
    scene->removeItem(imageItem);
    delete imageItem;
    delete image;
    rotation = 0;
    flip = MetadataUtils::None;
    imagePath = imageName;
    fileExt = imagePath.split('.').last().toLower().toLatin1();
#ifdef SIR_METADATA_SUPPORT
    metadataEnabled = isntTiffImage();
#endif // SIR_METADATA_SUPPORT
    saveImageButton->setEnabled(
                    QImageWriter::supportedImageFormats().contains(fileExt) );

    loadPixmap();

    imageItem = addImageIntoScene();
    view->setSceneRect(imageItem->boundingRect());
    imageW = imageItem->boundingRect().width();
    imageH = imageItem->boundingRect().height();

    if(zoomFactor != 1.0) {
        zoom("100%");
        int index = zoomComboBox->findText("100%");
        zoomComboBox->setCurrentIndex(index);
    }
#ifdef SIR_METADATA_SUPPORT
    if (metadataEnabled)
        view->repaint();
#endif // SIR_METADATA_SUPPORT
}

/** Loads image's pixmap from imagePath file path into corresponding graphics
  * item object - QGraphicsPixmapItem* image or QGraphicsSvgItem* svgImage.\n
  * Its set svgLoaded variable to \a true if SVG image was loaded, otherwise
  * set false.
  */
void PreviewDialog::loadPixmap() {
    bool readSuccess;
    svgLoaded = false;

    // reading...
    if (rawEnabled) { // raw image
        RawPixmapLoader rawLoader = RawPixmapLoader(
                    &(Settings::instance()->raw), imagePath);
        image = rawLoader.load();
        readSuccess = (bool) image;
    }
    else if (fileExt == "svg" || fileExt == "svgz") { // SVG file
        svgImage = new QGraphicsSvgItem(imagePath);
        svgLoaded = true;
        readSuccess = (svgImage != 0);
    } else { // other image formats
        image = new QPixmap();
        readSuccess = image->load(imagePath);
    }

#ifdef SIR_METADATA_SUPPORT
    bool metadataReadError = false;
    if (readSuccess && metadataEnabled && !svgLoaded) {
        // reading metadata
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
#endif // SIR_METADATA_SUPPORT

    if (!readSuccess) {
        QString errorTitle = tr("Image file error");
        QString errorMessage = tr("Load image %1 failed").arg(imagePath);
        QMessageBox::critical(this, errorTitle, errorMessage);
    }
#ifdef SIR_METADATA_SUPPORT
    else if (metadataReadError) {
        MetadataUtils::Error *e = metadata->lastError();
        String str(e->message() +
                                  tr("\nError code: %1\nError message: %2").
                                     arg(e->code()).arg(e->what()) );
        qWarning() << tr("Metadata error!");
        qWarning() << str;
    }
#endif // SIR_METADATA_SUPPORT
}

/** Print button slot. Prints current image. */
void PreviewDialog::print() {
    QPrinter printer;
    if (QPrintDialog(&printer).exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);
        scene->render(&painter);
    }
}

/** When window resized fits image to window size if special value
  * \em "Fit to window size" of zoom combo box is activated.\n
  * This is overloaded QDialogs's method.
  */
void PreviewDialog::resizeEvent(QResizeEvent *) {
    QString currentZoomString = zoomComboBox->currentText();
    if (currentZoomString == tr("Fit to window size"))
        zoom(currentZoomString);
}
