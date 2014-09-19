/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2014  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://marek629.github.io/sir/
 */

#include <QImage>
#include <QDir>
#include <QtSvg/QSvgRenderer>
#include <QDebug>
#include <cmath>
#include <QPainter>
#include "convertthread.h"
#include "rawutils.h"
#include "widgets/messagebox.h"
#include "settings.h"
#include "svgmodifier.h"
#include "converteffects.h"

using namespace sir;

// setup static fields
SharedInformation * ConvertThread::shared = new SharedInformation();

// access method to static fields
/** Returns pointer to static SharedInformation object. */
SharedInformation * ConvertThread::sharedInfo() {
    return shared;
}

/** Default constructor.
  * \param parent parent object
  * \param tid thread ID
  */
ConvertThread::ConvertThread(QObject *parent, int tid) : QThread(parent) {
    this->tid = tid;
    work = true;
}

void ConvertThread::setAcceptWork(bool work) {
    this->work = work;
}

void ConvertThread::convertImage(const QString& name, const QString& extension,
                                 const QString& path) {
    imageData.clear();
    imageData << name << extension << path;
    if(!isRunning())
        start();
}

/** This is main function of thread.\n
  * Converts selected images to desired size, format and quality.
  */
void ConvertThread::run() {

    bool rawEnabled = RawUtils::isRawEnabled();

    while(work) {
        pd.imgData = this->imageData; // imageData change protection by convertImage()
        sizeComputed = 0;
        width = shared->width;
        height = shared->height;
        hasWidth = shared->hasWidth;
        hasHeight = shared->hasHeight;
        bool maintainAspect = shared->maintainAspect;
        rotate = shared->rotate;
        angle = shared->angle;

        if (shared->abort) {
            emit imageStatus(pd.imgData, tr("Cancelled"), Cancelled);
            getNextOrStop();
            continue;
        }

        emit imageStatus(pd.imgData, tr("Converting"), Converting);

        QString imageName = pd.imgData.at(0);
        QString originalFormat = pd.imgData.at(1);

        targetFilePath = shared->destFolder.absolutePath() + QDir::separator();
        if (!shared->prefix.isEmpty())
            targetFilePath += shared->prefix + "_";
        targetFilePath += imageName;
        if (!shared->suffix.isEmpty())
            targetFilePath += "_" + shared->suffix;
        targetFilePath += "." + shared->format;

        pd.imagePath = pd.imgData.at(2) + QDir::separator() + pd.imgData.at(0)
                     + "." + originalFormat;
        originalFormat = originalFormat.toLower();
        bool svgSource(originalFormat == "svg" || originalFormat == "svgz");

        QImage *image = 0;

        // load image data
        if(rawEnabled) {
            image = new QImage();
            if(RawUtils::isRaw(pd.imagePath))
                image = RawUtils::loadRawImage(pd.imagePath);
            else
                image->load(pd.imagePath);
        }
        else if (svgSource) {
            image = loadSvgImage();
            if (!image) {
                getNextOrStop();
                continue;
            }
        }
        else {
            image = new QImage();
            image->load(pd.imagePath);
        }

        if(image->isNull()) {
            //For some reason we where not able to open the image file
            emit imageStatus(pd.imgData, tr("Failed to open original image"),
                             Failed);
            delete image;
            //Ask for the next image and go to the beginning of the loop
            getNextOrStop();
            continue;
        }
#ifdef SIR_METADATA_SUPPORT
        // read metadata
        saveMetadata = false;
        if (shared->metadataEnabled) {
            saveMetadata = metadata.read(pd.imagePath, true, svgSource);
            int beta = MetadataUtils::Exif::rotationAngle(
                        metadata.exifStruct()->orientation);
            if (!saveMetadata)
                printError();
            // flip-flap width-height (px only)
            else if (angle == 0 && shared->sizeUnit != 1 && beta%90 == 0 && beta%180 != 0) {
                int temp = width;
                width = height;
                height = temp;
                bool tmp = hasWidth;
                hasWidth = hasHeight;
                hasHeight = tmp;
            }
            if (saveMetadata)
                saveMetadata = shared->saveMetadata;
        }
#endif // SIR_METADATA_SUPPORT
        // compute dest size in px
        if (sizeComputed == 0) { // false if converting from SVG file
                sizeComputed = computeSize(image,pd.imagePath);
            if (sizeComputed == 1) { // image saved
                delete image;
                getNextOrStop();
                continue;
            }
        }
        // ask enlarge
        if (sizeComputed == -3 || askEnlarge(*image,pd.imagePath) < 0) {
            delete image;
            getNextOrStop();
            continue;
        }
        // create null destination image object
        QImage destImg;
        // scale image
        if (hasWidth && hasHeight) {
            if (maintainAspect)
                destImg = image->scaled(width, height, Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);
            else
                destImg = image->scaled(width, height, Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation);
        }
        else if (hasWidth && !hasHeight)
            destImg = image->scaledToWidth(width, Qt::SmoothTransformation);
        else if (!hasWidth && hasHeight)
            destImg = image->scaledToHeight(height, Qt::SmoothTransformation);
        else if (!hasWidth && !hasHeight)
            destImg = *image;
        // paint effects
        destImg = paintEffects(&destImg);
        // rotate image and update thumbnail
        destImg = rotateImage(destImg);
#ifdef SIR_METADATA_SUPPORT
        updateThumbnail(destImg);
#endif // SIR_METADATA_SUPPORT
        // ask overwrite
        if ( QFile::exists( targetFilePath ) &&
             !(shared->overwriteAll || shared->abort || shared->noOverwriteAll)) {
            if (!(shared->overwriteAll || shared->abort || shared->noOverwriteAll)) {
                shared->mutex.lock();
                emit question(targetFilePath, Overwrite);
                shared->mutex.unlock();
            }
            if(shared->overwriteResult == QMessageBox::Yes ||
                    shared->overwriteResult == QMessageBox::YesToAll) {
                if (destImg.save(targetFilePath, 0, shared->quality)) {
#ifdef SIR_METADATA_SUPPORT
                    if (saveMetadata && !metadata.write(targetFilePath, destImg))
                        printError();
#endif // SIR_METADATA_SUPPORT
                    emit imageStatus(pd.imgData, tr("Converted"), Converted);
                }
                else
                    emit imageStatus(pd.imgData, tr("Failed to convert"), Failed);
            }
            else if (shared->overwriteResult == QMessageBox::Cancel)
                emit imageStatus(pd.imgData, tr("Cancelled"), Cancelled);
            else
                emit imageStatus(pd.imgData, tr("Skipped"), Skipped);
        }
        else if (shared->noOverwriteAll)
            emit imageStatus(pd.imgData, tr("Skipped"), Skipped);
        else if (shared->abort)
            emit imageStatus(pd.imgData, tr("Cancelled"), Cancelled);
        else { // when overwriteAll is true or file not exists
            if (destImg.save(targetFilePath, 0, shared->quality)) {
#ifdef SIR_METADATA_SUPPORT
                if (saveMetadata && !metadata.write(targetFilePath, destImg))
                    printError();
#endif // SIR_METADATA_SUPPORT
                emit imageStatus(pd.imgData, tr("Converted"), Converted);
            }
            else
                emit imageStatus(pd.imgData, tr("Failed to convert"), Failed);
        }
        delete image;
        getNextOrStop();
    }
}

/** Gets next image for converting or stops this thread. */
void ConvertThread::getNextOrStop() {
    static QMutex m;
    QMutexLocker locker(&m);
    emit getNextImage(this->tid);
}

#ifdef SIR_METADATA_SUPPORT
/** Prints metadata error message on standard error output. This function is
  * available if SIR_METADATA_SUPPORT is defined only.
  */
void ConvertThread::printError() {
    MetadataUtils::Error *error = metadata.lastError();
    qWarning() << "Metadata error within thread" << tid << '\n'
               << "    " << error->message() << '\n'
               << "    " << tr("Error code:") << error->code() << '\n'
               << "    " << error->what();
    delete error;
}
#endif // SIR_METADATA_SUPPORT

/** Rotates \a image object and returns new QImage object.
  * \return Rotated image if just rotated, without metadata manipulation.
  *         Otherwise returns a copy of \a image object.
  */
QImage ConvertThread::rotateImage(const QImage &image) {
    int alpha = (int)angle;
#ifdef SIR_METADATA_SUPPORT
    bool saveExifOrientation = !shared->realRotate;
#endif // SIR_METADATA_SUPPORT
    // rotate image
    if ((rotate && angle != 0.0) || saveExifOrientation) {
#ifdef SIR_METADATA_SUPPORT
        if (saveExifOrientation && (alpha!=angle || alpha%90!=0))
            saveExifOrientation = false;
        // don't rotate but save Exif orientation tag
        if (saveMetadata && saveExifOrientation) {
            int flip;
            alpha += MetadataUtils::Exif::rotationAngle(
                        metadata.exifStruct()->orientation, &flip );
            if (alpha == -360) {
                alpha = 0;
                flip = MetadataUtils::None;
            }

            flip ^= shared->flip;

            // normalization of values alpha and flip
            if (alpha == -270)
                alpha = 90;
            else if (alpha == 270)
                alpha = -90;
            else if (alpha == -180)
                alpha = 180;
            if (flip != MetadataUtils::None && (int)angle%180 != 0)
                flip ^= MetadataUtils::VerticalAndHorizontal;

            char orientation = MetadataUtils::Exif::getOrientation(alpha,flip);
            if (orientation < 1) { // really rotate when getOrientation() failed
                metadata.setExifDatum("Exif.Image.Orientation", 1);
                metadata.exifStruct()->orientation = 1;
                saveExifOrientation = false;
            }
            else {
                metadata.setExifDatum("Exif.Image.Orientation", orientation);
                metadata.exifStruct()->orientation = orientation;
            }
        }
#endif // SIR_METADATA_SUPPORT
    }
    // really rotate without saving Exif orientation tag
#ifdef SIR_METADATA_SUPPORT
    if (!saveExifOrientation || shared->realRotate) {
#endif // SIR_METADATA_SUPPORT
        // flip dimension variables
        if (alpha%90 == 0 && alpha%180 != 0) {
            int tmp = width;
            width = height;
            height = tmp;
            bool temp = hasWidth;
            hasWidth = hasHeight;
            hasHeight = temp;
        }
        // image tranformation matrix
        QTransform transform;
#ifdef SIR_METADATA_SUPPORT
        if (saveMetadata) {
            metadata.setExifDatum("Exif.Image.Orientation",1);
            int flip;
            angle += MetadataUtils::Exif::rotationAngle(
                        metadata.exifStruct()->orientation, &flip );
            if (flip == MetadataUtils::Vertical)
                transform.scale(1.0,-1.0);
            else if (flip == MetadataUtils::Horizontal)
                transform.scale(-1.0,1.0);
            else if (flip == MetadataUtils::VerticalAndHorizontal)
                angle += 360;
        }
#endif // SIR_METADATA_SUPPORT
        transform.rotate(angle);
        return image.transformed(transform, Qt::SmoothTransformation);
#ifdef SIR_METADATA_SUPPORT
    }
#endif // SIR_METADATA_SUPPORT
    return image;
}

#ifdef SIR_METADATA_SUPPORT
/** Updates Exif thumnail after conversion and (if required) rotates this thumbnail.
  * New thumbnail will set as \a exifThumbnail in metadata object.\n
  * If setting thumbnail fails, this function will print error using printError()
  * function.\n
  * This function is available if SIR_METADATA_SUPPORT is defined only.
  */
void ConvertThread::updateThumbnail(const QImage &image) {
    // update thumbnail
    if (saveMetadata && shared->updateThumbnail) {
        MetadataUtils::ExifStruct *exifStruct = metadata.exifStruct();
        int w = exifStruct->thumbnailWidth.split(' ').first().toInt();
        int h = exifStruct->thumbnailHeight.split(' ').first().toInt();
        QImage tmpImg = image.scaled(w,h, Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation);
        bool specialRotate = (rotate && (int)angle%90 != 0);
        QImage *thumbnail = &exifStruct->thumbnailImage;
        if (specialRotate) {
            w = tmpImg.width();
            h = tmpImg.height();
            *thumbnail = tmpImg;
        }
        else {
            if (shared->backgroundColor.isValid())
                thumbnail->fill(shared->backgroundColor.rgb());
            else
                thumbnail->fill(Qt::black);
            QPoint begin ( (w-tmpImg.width())/2, (h-tmpImg.height())/2 );
            for (int i=0, y=begin.y(); i<tmpImg.height(); i++, y++) {
                for (int j=0, x=begin.x(); j<tmpImg.width(); j++, x++)
                    thumbnail->setPixel(x, y, tmpImg.pixel(j,i));
            }
        }
        // rotate thumbnail
        if (shared->rotateThumbnail && !specialRotate) {
            QTransform transform;
            int flip;
            transform.rotate(MetadataUtils::Exif::rotationAngle(
                                 exifStruct->orientation, &flip));
            if (flip == MetadataUtils::Vertical)
                transform.scale(1.0,-1.0);
            else if (flip == MetadataUtils::Horizontal)
                transform.scale(-1.0,1.0);
            *thumbnail = thumbnail->transformed(transform);
        }
        if (!metadata.setExifThumbnail(thumbnail,tid))
            printError();
    }
}
#endif // SIR_METADATA_SUPPORT

/** This is overloaded function. It's version for \e normal, raster image.
  *
  * Sets required image size.
  * \return negative value when an error has occured
  * \return 0 when an unsupported SharedInformation::sizeUnit value was set
  * \return 1 when success (for 2 (\e bytes) value of SharedInformation::sizeUnit only)
  */
char ConvertThread::computeSize(const QImage *image, const QString &imagePath) {
    if (shared->sizeUnit == 0) ; // px
    // compute size when it wasn't typed in pixels
    else if (shared->sizeUnit == 1) { // %
        width *= image->width() / 100.;
        height *= image->height() / 100.;
    }
    else if (shared->sizeUnit == 2) { // bytes
        width = image->width();
        height = image->height();
        hasWidth = true;
        hasHeight = true;
        double destSize = shared->sizeBytes;
        if (isLinearFileSizeFormat(&destSize)) {
            double sourceSizeSqrt = sqrt(width * height);
            double sourceWidthRatio = width / sourceSizeSqrt;
            double sourceHeightRatio = height / sourceSizeSqrt;
            destSize = sqrt(destSize);
            width = sourceWidthRatio * destSize;
            height = sourceHeightRatio * destSize;
        }
        else { // non-linear size relationship
            QString tempFilePath = QDir::tempPath() + QDir::separator() +
                    "sir_temp" + QString::number(tid) + "." + shared->format;
            QImage tempImage;
            qint64 fileSize = QFile(imagePath).size();
            QSize size = image->size();
            double fileSizeRatio = (double) fileSize / shared->sizeBytes;
            fileSizeRatio = sqrt(fileSizeRatio);
            QFile tempFile(tempFilePath);
            for (uchar i=0; i<10 && (fileSizeRatio<0.97412 || fileSizeRatio>1.); i++) {
                tempFile.open(QIODevice::WriteOnly);
                tempFile.seek(0);
                width = size.width() / fileSizeRatio;
                height = size.height() / fileSizeRatio;
                tempImage = image->scaled(width, height, Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation);
                tempImage = paintEffects(&tempImage);
                tempImage = rotateImage(tempImage);
#ifdef SIR_METADATA_SUPPORT
                updateThumbnail(tempImage);
#endif // SIR_METADATA_SUPPORT
                if (tempImage.save(&tempFile, 0, shared->quality)) {
#ifdef SIR_METADATA_SUPPORT
                    if (saveMetadata)
                        metadata.write(tempFilePath, tempImage);
#endif // SIR_METADATA_SUPPORT
                }
                else {
                    qWarning("tid %d: Save temporary image file "
                             "into %s failed", tid,
                             String(targetFilePath).toNativeStdString().data() );
                    emit imageStatus(imageData, tr("Failed to compute image size"),
                                     Failed);
                    return -4;
                }
                tempFile.close();
                fileSize = tempFile.size();
                size = tempImage.size();
                fileSizeRatio = (double) fileSize / shared->sizeBytes;
                fileSizeRatio = sqrt(fileSizeRatio);
            }
            // ask enlarge
            if (askEnlarge(*image,imagePath) < 0)
                return -3;
            // ask overwrite
            char answer = askOverwrite(&tempFile);
            if (answer < 0)
                return answer;
        }
        return 1;
    }
    return 0;
}

/** This is overloaded function. It's version for SVG vector image.
  *
  * Sets required image size.
  * \return negative value when an error has occured
  * \return 0 when an unsupported SharedInformation::sizeUnit value was set
  * \return 1 when success (for 2 (\e bytes) value of SharedInformation::sizeUnit only)
  */
char ConvertThread::computeSize(QSvgRenderer *renderer, const QString &imagePath) {
    QSize defaultSize = renderer->defaultSize();
    if (shared->sizeUnit == 0) // px
    // compute size when it wasn't typed in pixels
        return 1;
    else if (shared->sizeUnit == 1) { // %
        width *= defaultSize.width() / 100.;
        height *= defaultSize.height() / 100.;
        return 1;
    }
    else if (shared->sizeUnit == 2) { // bytes
        width = defaultSize.width();
        height = defaultSize.height();
        hasWidth = true;
        hasHeight = true;
        double destSize = shared->sizeBytes;
        if (isLinearFileSizeFormat(&destSize)) {
            double sourceSizeSqrt = sqrt(width * height);
            double sourceWidthRatio = width / sourceSizeSqrt;
            double sourceHeightRatio = height / sourceSizeSqrt;
            destSize = sqrt(destSize);
            width = sourceWidthRatio * destSize;
            height = sourceHeightRatio * destSize;
        }
        else { // non-linear size relationship
            QString tempFilePath = QDir::tempPath() + QDir::separator() +
                    "sir_temp" + QString::number(tid) + "." + shared->format;
            qint64 fileSize = QFile(imagePath).size();
            QSize size = defaultSize;
            double fileSizeRatio = (double) fileSize / shared->sizeBytes;
            fileSizeRatio = sqrt(fileSizeRatio);
            QFile tempFile(tempFilePath);
            QPainter painter;
            for (uchar i=0; i<10 && (fileSizeRatio<0.97412 || fileSizeRatio>1.); i++) {
                tempFile.open(QIODevice::WriteOnly);
                tempFile.seek(0);
                width = size.width() / fileSizeRatio;
                height = size.height() / fileSizeRatio;
                QImage tempImage(width, height, QImage::Format_ARGB32);
                fillImage(&tempImage);
                painter.begin(&tempImage);
                renderer->render(&painter);
                painter.end();
                tempImage = paintEffects(&tempImage);
                tempImage = rotateImage(tempImage);
#ifdef SIR_METADATA_SUPPORT
                updateThumbnail(tempImage);
#endif // SIR_METADATA_SUPPORT
                if (tempImage.save(&tempFile, 0, shared->quality)) {
#ifdef SIR_METADATA_SUPPORT
                    if (saveMetadata)
                        metadata.write(tempFilePath, tempImage);
#endif // SIR_METADATA_SUPPORT
                }
                else {
                    qWarning("tid %d: Save temporary image file "
                             "into %s failed", tid,
                             String(targetFilePath).
                                toNativeStdString().data());
                    emit imageStatus(imageData, tr("Failed to compute image size"),
                                     Failed);
                    return -4;
                }
                tempFile.close();
                fileSize = tempFile.size();
                size = tempImage.size();
                fileSizeRatio = (double) fileSize / shared->sizeBytes;
                fileSizeRatio = sqrt(fileSizeRatio);
            }
            // ask overwrite
            char answer = askOverwrite(&tempFile);
            if (answer < 0)
                return answer;
        }
        return 2;
    }
    return 0;
}

/** Returns true if desired file format is corresponding file size to image size
  * as linear function, otherwise returns false.\n
  * Following file formats are linear size: BMP, PPM, ICO, TIFF and XBM.
  */
bool ConvertThread::isLinearFileSizeFormat(double *destSize) {
    bool linearSize = false;
    if (shared->format == "bmp") {
        *destSize -= 54;
        *destSize /= 3;
        linearSize = true;
    }
    else if (shared->format == "ppm") {
        *destSize -= 17;
        *destSize /= 3;
        linearSize = true;
    }
    else if (shared->format == "ico") {
        *destSize -= 1422;
        *destSize /= 4;
        linearSize = true;
    }
    else if (shared->format == "tif" || shared->format == "tiff") {
        *destSize -= 14308;
        *destSize /= 4;
        linearSize = true;
    }
    else if (shared->format == "xbm") {
        *destSize -= 60;
        *destSize /= 0.65;
        linearSize = true;
    }
    return linearSize;
}

/** Asks the user in message box if enlarge image by emiting question() signal.\n
  * \return -1 when the user didn't answered \em yes\n
  * \return 0  when the user answered \em yes\n
  * \return 1  when enlarge of image isn't necessary
  * \sa askOverwrite() question()
  */
char ConvertThread::askEnlarge(const QImage &image, const QString &imagePath) {
    if ( (image.width()<width && image.width()>=image.height()) ||
         (image.height()<height && image.width()<=image.height()) ) {
        if (!(shared->enlargeAll || shared->noEnlargeAll || shared->abort)) {
            shared->mutex.lock();
            emit question(imagePath, Enlarge);
            shared->mutex.unlock();
        }
        if (shared->enlargeResult != QMessageBox::Yes &&
                shared->enlargeResult != QMessageBox::YesToAll) {
            if (shared->enlargeResult == QMessageBox::Cancel)
                emit imageStatus(imageData, tr("Cancelled"), Cancelled);
            else
                emit imageStatus(imageData, tr("Skipped"), Skipped);
            return -1;
        }
        return 0;
    }
    return 1;
}

/** Asks the user in message box if overwrite file by emiting question() signal.\n
  * Returns negative value if overwriting failed, otherwise returns 0.
  * \note This function was created for SVG images.
  * \sa askEnlarge() question()
  */
char ConvertThread::askOverwrite(QFile *tempFile) {
    if ( QFile::exists( targetFilePath ) &&
         !(shared->overwriteAll || shared->abort || shared->noOverwriteAll)) {
        if (!(shared->overwriteAll || shared->abort || shared->noOverwriteAll)) {
            shared->mutex.lock();
            emit question(targetFilePath, Overwrite);
            shared->mutex.unlock();
        }
        if(shared->overwriteResult == QMessageBox::Yes ||
                shared->overwriteResult == QMessageBox::YesToAll) {
            QFile::remove(targetFilePath);
            if (tempFile->copy(targetFilePath))
                emit imageStatus(imageData, tr("Converted"), Converted);
            else {
                emit imageStatus(imageData, tr("Failed to save"), Failed);
                return -1;
            }
        }
        else if (shared->overwriteResult == QMessageBox::Cancel)
            emit imageStatus(imageData, tr("Cancelled"), Cancelled);
        else
            emit imageStatus(imageData, tr("Skipped"), Skipped);
    }
    else if (shared->noOverwriteAll)
        emit imageStatus(imageData, tr("Skipped"), Skipped);
    else if (shared->abort)
        emit imageStatus(imageData, tr("Cancelled"), Cancelled);
    else { // when overwriteAll is true or file not exists
        QFile::remove(targetFilePath);
        if (tempFile->copy(targetFilePath))
            emit imageStatus(imageData, tr("Converted"), Converted);
        else {
            emit imageStatus(imageData, tr("Failed to save"), Failed);
            return -2;
        }
    }
    return 0;
}

/** Fills image with custom background color if is valid or transparent
  * (if target file format supports transparency); otherwise with white.
  * \sa SharedInformation::backgroundColor SharedInformation::format
  */
void ConvertThread::fillImage(QImage *img) {
    if (shared->backgroundColor.isValid())
        img->fill(shared->backgroundColor.rgb());
    else if (shared->format == "gif" || shared->format == "png")
        img->fill(Qt::transparent);
    else // in other formats tranparency isn't supported
        img->fill(Qt::white);
}

/** Loads and modifies SVG file if needed. Renders SVG data to \a image object.
  * \return Pointer to rendered image if succeed. Otherwise null pointer.
  */
QImage * ConvertThread::loadSvgImage() {
    QSvgRenderer renderer;
    if (shared->svgModifiersEnabled) {
        SvgModifier modifier(pd.imagePath);
        // modify SVG file
        if (!shared->svgRemoveText.isNull())
            modifier.removeText(shared->svgRemoveText);
        if (shared->svgRemoveEmptyGroup)
            modifier.removeEmptyGroups();
        // save SVG file
        if (shared->svgSave) {
            QString svgTargetFileName =
                    targetFilePath.left(targetFilePath.lastIndexOf('.')+1) + "svg";
            QFile file(svgTargetFileName);
            // ask overwrite
            if (file.exists()) {
                shared->mutex.lock();
                emit question(svgTargetFileName, Overwrite);
                shared->mutex.unlock();
            }
            if (shared->overwriteResult == QMessageBox::Yes ||
                    shared->overwriteResult == QMessageBox::YesToAll) {
                if (!file.open(QIODevice::WriteOnly)) {
                    emit imageStatus(pd.imgData, tr("Failed to save new SVG file"),
                                     Failed);
                    return NULL;
                }
                file.write(modifier.content());
            }
        }
        // and load QByteArray buffer to renderer
        if (!renderer.load(modifier.content())) {
            emit imageStatus(pd.imgData, tr("Failed to open changed SVG file"),
                             Failed);
            return NULL;
        }
    }
    else if (!renderer.load(pd.imagePath)) {
        emit imageStatus(pd.imgData, tr("Failed to open SVG file"), Failed);
        return NULL;
    }
    sizeComputed = computeSize(&renderer, pd.imagePath);
    if (sizeComputed == 2)
        return NULL;
    // keep aspect ratio
    if (shared->maintainAspect) {
        qreal w = width;
        qreal h = height;
        qreal targetRatio = w / h;
        QSizeF svgSize = renderer.defaultSize();
        qreal currentRatio = svgSize.width() / svgSize.height();
        if (currentRatio != targetRatio) {
            qreal diffRatio;
            if (currentRatio > targetRatio)
                diffRatio = w / svgSize.width();
            else
                diffRatio = h / svgSize.height();
            width = diffRatio * svgSize.width();
            height = diffRatio * svgSize.height();
        }
    }
    // create image
    QImage *img = new QImage(width, height, QImage::Format_ARGB32);
    fillImage(img);
    QPainter painter(img);
    renderer.render(&painter);
    // don't scale rendered image
    hasWidth = false;
    hasHeight = false;
    // finaly return the image pointer
    return img;
}

/** Draws effects into new image.
  * \return New image object.
  */
QImage ConvertThread::paintEffects(QImage *image) {
    QImage destImg(*image);
    ConvertEffects effectPainter(&destImg, shared);
    if (shared->histogramOperation > 0)
        effectPainter.modifyHistogram();
    if (shared->filterType != NoFilter)
        effectPainter.filtrate();
    if (shared->frameWidth > 0 && shared->frameColor.isValid()) {
        destImg = effectPainter.framedImage();
        effectPainter.setImage(&destImg);
    }
    if (!shared->image.isNull())
        effectPainter.addImage();
    if (!shared->textString.isEmpty())
        effectPainter.addText();
    return destImg;
}
