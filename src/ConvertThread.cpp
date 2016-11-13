/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2016  Marek Jędryka   <jedryka89@gmail.com>
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

#include "ConvertThread.hpp"

#include "ConvertEffects.hpp"
#include "Settings.hpp"
#include "convert/model/ImageFileSize.hpp"
#include "convert/model/WriteImageFormat.hpp"
#include "convert/service/ImageFileService.hpp"
#include "image/QImageLoader.hpp"
#include "widgets/MessageBox.hpp"

#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QtSvg/QSvgRenderer>

// TODO: remove cmath header after ImageSizeStrategy class extraction
#include <cmath>


using namespace sir;


SharedInformation ConvertThread::shared = SharedInformation();


// access method to static fields
/** Returns pointer to static SharedInformation object. */
SharedInformation * ConvertThread::sharedInfo() {
    return &shared;
}

void ConvertThread::setSharedInfo(const SharedInformation &info)
{
    shared = SharedInformation(info);
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
void ConvertThread::run()
{
    while(work) {
        pd.imgData = this->imageData; // imageData change protection by convertImage()
        sizeComputed = 0;
        width = shared.width;
        height = shared.height;
        hasWidth = shared.hasWidth;
        hasHeight = shared.hasHeight;
        bool maintainAspect = shared.maintainAspect;
        rotate = shared.rotate;
        angle = shared.angle;

        if (shared.abort) {
            emit imageStatus(pd.imgData, tr("Cancelled"), Cancelled);
            getNextOrStop();
            continue;
        }

        emit imageStatus(pd.imgData, tr("Converting"), Converting);

        QString imageName = pd.imgData.at(0);
        QString originalFormat = pd.imgData.at(1);

        targetFilePath = shared.destFolder.absolutePath() + QDir::separator();
        if (!shared.prefix.isEmpty())
            targetFilePath += shared.prefix + "_";
        targetFilePath += imageName;
        if (!shared.suffix.isEmpty())
            targetFilePath += "_" + shared.suffix;
        targetFilePath += "." + shared.targetImage.imageFormat().qString();

        pd.imagePath = pd.imgData.at(2) + QDir::separator() + pd.imgData.at(0)
                     + "." + originalFormat;
        originalFormat = originalFormat.toLower();
        bool svgSource(originalFormat == "svg" || originalFormat == "svgz");

        QImage *image = loadImage(pd.imagePath, &shared.rawModel, svgSource);

        if (!image) {
            getNextOrStop();
            continue;
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
        if (shared.metadataEnabled) {
            saveMetadata = metadata.read(pd.imagePath, true, svgSource);
            int beta = MetadataUtils::Exif::rotationAngle(
                        metadata.exifStruct()->orientation);
            if (!saveMetadata)
                printError();
            // flip-flap width-height (px only)
            else if (angle == 0 && shared.sizeUnit != 1 && beta%90 == 0 && beta%180 != 0) {
                int temp = width;
                width = height;
                height = temp;
                bool tmp = hasWidth;
                hasWidth = hasHeight;
                hasHeight = tmp;
            }
            if (saveMetadata)
                saveMetadata = shared.saveMetadata;
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
             !(shared.overwriteAll || shared.abort || shared.noOverwriteAll)) {
            if (!(shared.overwriteAll || shared.abort || shared.noOverwriteAll)) {
                shared.mutex.lock();
                emit question(targetFilePath, Overwrite);
                shared.mutex.unlock();
            }
            if(shared.overwriteResult == QMessageBox::Yes ||
                    shared.overwriteResult == QMessageBox::YesToAll) {
                ImageFileService imageFileService;
                if (imageFileService.writeImage(destImg, targetFilePath)) {
#ifdef SIR_METADATA_SUPPORT
                    if (saveMetadata && !metadata.write(targetFilePath, destImg))
                        printError();
#endif // SIR_METADATA_SUPPORT
                    emit imageStatus(pd.imgData, tr("Converted"), Converted);
                }
                else
                    emit imageStatus(pd.imgData, tr("Failed to convert"), Failed);
            }
            else if (shared.overwriteResult == QMessageBox::Cancel)
                emit imageStatus(pd.imgData, tr("Cancelled"), Cancelled);
            else
                emit imageStatus(pd.imgData, tr("Skipped"), Skipped);
        }
        else if (shared.noOverwriteAll)
            emit imageStatus(pd.imgData, tr("Skipped"), Skipped);
        else if (shared.abort)
            emit imageStatus(pd.imgData, tr("Cancelled"), Cancelled);
        else { // when overwriteAll is true or file not exists
            ImageFileService imageFileService;
            if (imageFileService.writeImage(destImg, targetFilePath)) {
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

void ConvertThread::setImageStatus(const QStringList &imageData,
                                   const QString &message,
                                   ConvertThread::Status status)
{
    emit imageStatus(imageData, message, status);
}

void ConvertThread::askUser(const QString &targetFilePath,
                            ConvertThread::Question question)
{
    shared.mutex.lock();
    emit this->question(targetFilePath, question);
    shared.mutex.unlock();
}

int ConvertThread::getUserAnswer(ConvertThread::Question question) const
{
    switch (question) {
    case Overwrite:
        return shared.overwriteResult;
        break;
    case Enlarge:
        return shared.enlargeResult;
        break;
    default:
        return -1;
        break;
    }
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
    bool saveExifOrientation = false;
#ifdef SIR_METADATA_SUPPORT
    saveExifOrientation = !shared.realRotate;
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

            flip ^= shared.flip;

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
    if (!saveExifOrientation || shared.realRotate) {
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
    if (saveMetadata && shared.updateThumbnail) {
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
            if (shared.backgroundColor.isValid())
                thumbnail->fill(shared.backgroundColor.rgb());
            else
                thumbnail->fill(Qt::black);
            QPoint begin ( (w-tmpImg.width())/2, (h-tmpImg.height())/2 );
            for (int i=0, y=begin.y(); i<tmpImg.height(); i++, y++) {
                for (int j=0, x=begin.x(); j<tmpImg.width(); j++, x++)
                    thumbnail->setPixel(x, y, tmpImg.pixel(j,i));
            }
        }
        // rotate thumbnail
        if (shared.rotateThumbnail && !specialRotate) {
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
    if (shared.sizeUnit == 0) ; // px
    // compute size when it wasn't typed in pixels
    else if (shared.sizeUnit == 1) { // %
        width *= image->width() / 100.;
        height *= image->height() / 100.;
    }
    else if (shared.sizeUnit == 2) { // bytes
        width = image->width();
        height = image->height();
        hasWidth = true;
        hasHeight = true;
        if (shared.targetImage.imageFormat().isLinearFileSizeFormat()) {
            double destSize = countTargetFileSize(shared.sizeBytes);
            double sourceSizeSqrt = sqrt(width * height);
            double sourceWidthRatio = width / sourceSizeSqrt;
            double sourceHeightRatio = height / sourceSizeSqrt;
            destSize = sqrt(destSize);
            width = sourceWidthRatio * destSize;
            height = sourceHeightRatio * destSize;
        }
        else { // non-linear size relationship
            QString tempFilePath = temporaryFilePath(tid);
            QImage tempImage;
            qint64 fileSize = QFile(imagePath).size();
            QSize size = image->size();
            double fileSizeRatio = (double) fileSize / shared.sizeBytes;
            fileSizeRatio = sqrt(fileSizeRatio);
            QFile tempFile(tempFilePath);
            for (uchar i=0; i<10 && (fileSizeRatio<0.97412 || fileSizeRatio>1.); i++) {
                width = size.width() / fileSizeRatio;
                height = size.height() / fileSizeRatio;
                tempImage = image->scaled(width, height, Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation);
                tempImage = paintEffects(&tempImage);
                tempImage = rotateImage(tempImage);
#ifdef SIR_METADATA_SUPPORT
                updateThumbnail(tempImage);
#endif // SIR_METADATA_SUPPORT

                ImageFileService imageFileService;
                if (imageFileService.writeImage(tempImage, tempFilePath)) {
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
                fileSize = tempFile.size();
                size = tempImage.size();
                fileSizeRatio = (double) fileSize / shared.sizeBytes;
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

// TODO: move to QImageLoader class
/** This is overloaded function. It's version for SVG vector image.
  *
  * Sets required image size.
  * \return negative value when an error has occured
  * \return 0 when an unsupported SharedInformation::sizeUnit value was set
  * \return 1 when success (for 2 (\e bytes) value of SharedInformation::sizeUnit only)
  */
char ConvertThread::computeSize(QSvgRenderer *renderer, const QString &imagePath) {
    QSize defaultSize = renderer->defaultSize();
    if (shared.sizeUnit == 0) // px
    // compute size when it wasn't typed in pixels
        return 1;
    else if (shared.sizeUnit == 1) { // %
        width *= defaultSize.width() / 100.;
        height *= defaultSize.height() / 100.;
        return 1;
    }
    else if (shared.sizeUnit == 2) { // bytes
        width = defaultSize.width();
        height = defaultSize.height();
        hasWidth = true;
        hasHeight = true;
        if (shared.targetImage.imageFormat().isLinearFileSizeFormat()) {
            double destSize = countTargetFileSize(shared.sizeBytes);
            double sourceSizeSqrt = sqrt(width * height);
            double sourceWidthRatio = width / sourceSizeSqrt;
            double sourceHeightRatio = height / sourceSizeSqrt;
            destSize = sqrt(destSize);
            width = sourceWidthRatio * destSize;
            height = sourceHeightRatio * destSize;
        }
        else { // non-linear size relationship
            QString tempFilePath = temporaryFilePath(tid);
            qint64 fileSize = QFile(imagePath).size();
            QSize size = defaultSize;
            double fileSizeRatio = (double) fileSize / shared.sizeBytes;
            fileSizeRatio = sqrt(fileSizeRatio);
            QFile tempFile(tempFilePath);
            QPainter painter;
            for (uchar i=0; i<10 && (fileSizeRatio<0.97412 || fileSizeRatio>1.); i++) {
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

                ImageFileService imageFileService;
                if (imageFileService.writeImage(tempImage, tempFilePath)) {
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
                fileSize = tempFile.size();
                size = tempImage.size();
                fileSizeRatio = (double) fileSize / shared.sizeBytes;
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

double ConvertThread::countTargetFileSize(double fileSize)
{
    ImageFileSize imageFileSize(fileSize);
    return imageFileSize.bytesByFormat(shared.targetImage.imageFormat());
}

QString ConvertThread::temporaryFilePath(int threadId)
{
    return QDir::tempPath() + QDir::separator() +
            "sir_temp" + QString::number(threadId) +
            "." + shared.targetImage.imageFormat().qString();
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
        if (!(shared.enlargeAll || shared.noEnlargeAll || shared.abort)) {
            shared.mutex.lock();
            emit question(imagePath, Enlarge);
            shared.mutex.unlock();
        }
        if (shared.enlargeResult != QMessageBox::Yes &&
                shared.enlargeResult != QMessageBox::YesToAll) {
            if (shared.enlargeResult == QMessageBox::Cancel)
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
         !(shared.overwriteAll || shared.abort || shared.noOverwriteAll)) {
        if (!(shared.overwriteAll || shared.abort || shared.noOverwriteAll)) {
            shared.mutex.lock();
            emit question(targetFilePath, Overwrite);
            shared.mutex.unlock();
        }
        if(shared.overwriteResult == QMessageBox::Yes ||
                shared.overwriteResult == QMessageBox::YesToAll) {
            QFile::remove(targetFilePath);
            if (tempFile->copy(targetFilePath))
                emit imageStatus(imageData, tr("Converted"), Converted);
            else {
                emit imageStatus(imageData, tr("Failed to save"), Failed);
                return -1;
            }
        }
        else if (shared.overwriteResult == QMessageBox::Cancel)
            emit imageStatus(imageData, tr("Cancelled"), Cancelled);
        else
            emit imageStatus(imageData, tr("Skipped"), Skipped);
    }
    else if (shared.noOverwriteAll)
        emit imageStatus(imageData, tr("Skipped"), Skipped);
    else if (shared.abort)
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

QImage *ConvertThread::loadImage(const QString &imagePath, RawModel *rawModel,
                                 bool isSvgSource)
{
    QImage *image = NULL;

    QImageLoader loader(rawModel, this);

    if (loader.isRegularImage(imagePath)) {
        image = loader.loadRegularImage(imagePath);
    } else if (isSvgSource) {
        SvgParameters svgParams;
        svgParams.setSvgModifiersEnabled(shared.svgModifiersEnabled);
        svgParams.setSvgRemoveEmptyGroup(shared.svgRemoveEmptyGroup);
        svgParams.setSvgRemoveTextString(shared.svgRemoveTextString);
        svgParams.setSvgSave(shared.svgSave);
        svgParams.setMaintainAspect(shared.maintainAspect);
        svgParams.setImageData(pd.imgData);
        svgParams.setHeight(height);
        svgParams.setWidth(width);
        svgParams.setTargetFilePath(targetFilePath);
        loader.setSvgParameters(svgParams);

        image = loader.loadSvgImage(imagePath);

        svgParams = loader.getSvgParameters();
        height = svgParams.getHeight();
        width = svgParams.getWidth();
        hasHeight = svgParams.getHasHeight();
        hasWidth = svgParams.getHasWidth();
        sizeComputed = svgParams.getSizeComputed();
    } else {
        image = loader.loadRawImage(imagePath);
    }

    return image;
}

/**
 * \deprecated Moved to BytesImageSizeStrategy class.
 * \todo Safe delete.
 */
void ConvertThread::fillImage(QImage *img)
{
    if (shared.backgroundColor.isValid()) {
        img->fill(shared.backgroundColor.rgb());
    } else {
        if (shared.targetImage.imageFormat().isTransparentSupportFormat()) {
            img->fill(Qt::transparent);
        } else {
            // in other formats tranparency isn't supported
            img->fill(Qt::white);
        }
    }
}

/** Draws effects into new image.
  * \return New image object.
  */
QImage ConvertThread::paintEffects(QImage *image) {
    QImage destImg(*image);
    ConvertEffects effectPainter(&destImg, &shared);
    if (shared.effectsConfiguration().getHistogramOperation() > 0)
        effectPainter.modifyHistogram();
    if (shared.effectsConfiguration().getFilterType() != NoFilter)
        effectPainter.filtrate();
    if (shared.effectsConfiguration().getFrameWidth() > 0
            && shared.effectsConfiguration().getFrameColor().isValid()) {
        destImg = effectPainter.framedImage();
        effectPainter.setImage(&destImg);
    }
    if (!shared.effectsConfiguration().getImage().isNull())
        effectPainter.addImage();
    if (!shared.effectsConfiguration().getTextString().isEmpty())
        effectPainter.addText();
    return destImg;
}
