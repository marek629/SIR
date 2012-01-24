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

#include <QtGui>
#include "convertthread.h"
#include "defines.h"
#include "rawutils.h"

SharedInformation* ConvertThread::shared = new SharedInformation();

ConvertThread::ConvertThread(QObject *parent, int tid):QThread(parent) {

    this->tid = tid;
    work = true;
}

void ConvertThread::setMetadataEnabled(bool value) {
    shared->metadataEnabled = value;
}

void ConvertThread::setSaveMetadata(bool value) {
    shared->saveMetadata = value;
}

void ConvertThread::setRealRotate(bool rotate) {
    shared->realRotate = rotate;
}

void ConvertThread::setUpdateThumbnail(bool update) {
    shared->updateThumbnail = update;
}

void ConvertThread::setRotateThumbnail(bool rotate) {
    shared->rotateThumbnail = rotate;
}

void ConvertThread::setAcceptWork(bool work) {
    this->work = work;
}

void ConvertThread::setDesiredSize(int width, int height, bool percent,
                                   bool hasWidth, bool hasHeight,
                                   bool maintainAspect) {

    shared->width = width;
    shared->height = height;
    shared->hasWidth = hasWidth;
    shared->hasHeight = hasHeight;
    shared->maintainAspect = maintainAspect;
    if (percent)
        shared->sizeUnit = 1;
    else
        shared->sizeUnit = 0;
}

void ConvertThread::setDesiredSize(quint32 bytes) {
    shared->sizeBytes = bytes;
    shared->sizeUnit = 2;
}

void ConvertThread::setDesiredFormat(const QString& format) {
    shared->format = format;
    if (!MetadataUtils::Metadata::isWriteSupportedFormat(format)) {
        shared->saveMetadata = false;
        shared->realRotate = true;
        qWarning("Format \"%s\" haven't write metadata support",
                 format.toAscii().constData());
    }
    else
        shared->saveMetadata = QSettings("SIR").value("Metadata/saveMetadata",
                                                      true).toBool();
}

void ConvertThread::setDesiredRotation(bool rotate, double angle) {
    shared->rotate = rotate;
    int multipler = angle / 360;
    shared->angle = angle - multipler*360;
}

void ConvertThread::setDesiredFlip(int flip) {
    shared->flip = flip;
}

void ConvertThread::setQuality(int quality) {
    shared->quality = quality;
}

void ConvertThread::setDestPrefix(const QString& destPrefix) {
    shared->prefix = destPrefix;
}

void ConvertThread::setDestSuffix(const QString &destSuffix) {
    shared->suffix = destSuffix;
}

void ConvertThread::setDestFolder(const QDir& destFolder) {
    shared->destFolder = destFolder;
}

void ConvertThread::setOverwriteAll(bool overwriteAll) {
    QMutexLocker locker(&(shared->mutex));
    shared->overwriteAll = overwriteAll;
}

void ConvertThread::convertImage(const QString& name, const QString& extension,
                                 const QString& path) {
    imageData.clear();
    imageData << name << extension << path;
    if(!isRunning()) {
        start();
    }
}

void ConvertThread::confirmOverwrite(int result) {
    shared->overwriteResult = result;
    overwriteMutex.unlock();
    overwriteCondition.wakeOne();
}

void ConvertThread::confirmEnlarge(int result) {
    shared->enlargeResult = result;
    enlargeMutex.unlock();
    enlargeCondition.wakeOne();
}

void ConvertThread::confirmImage() {
    QMutexLocker locker(&imageMutex);
    imageCondition.wakeOne();
}

void ConvertThread::run() {

    bool rawEnabled = RawUtils::isRawEnabled();

    while(work) {
        width = shared->width;
        height = shared->height;
        hasWidth = shared->hasWidth;
        hasHeight = shared->hasHeight;
        bool maintainAspect = shared->maintainAspect;
        rotate = shared->rotate;
        angle = shared->angle;

        if (shared->abort) {
            emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
            getNextOrStop();
            continue;
        }

        emit imageStatus(imageData, tr("Converting"), CONVERTING);

        QString imageName = imageData.at(0);
        QString originalFormat = imageData.at(1);

        targetFilePath = shared->destFolder.absolutePath() + QDir::separator();
        if (!shared->prefix.isEmpty())
            targetFilePath += shared->prefix + "_";
        targetFilePath += imageName;
        if (!shared->suffix.isEmpty())
            targetFilePath += "_" + shared->suffix;
        targetFilePath += "." + shared->format;

        QString imagePath = imageData.at(2) + QDir::separator() +
                            imageData.at(0) + "." + originalFormat;


        QImage *image = new QImage();

        if(rawEnabled) {
            if(RawUtils::isRaw(imagePath))
                image = RawUtils::loadRawImage(imagePath);
            else
                image->load(imagePath);
        }
        else
            image->load(imagePath);

        if(image->isNull()) {
            //For some reason we where not able to open the image file
            emit imageStatus(imageData, tr("Failed to open original image"),
                              FAILED);
            delete image;

            //Ask for the next image and go to the beginning of the loop
            getNextOrStop();
            continue;
        }

        // read metadata
        saveMetadata = false;
        if (shared->metadataEnabled) {
            saveMetadata = metadata.read(imagePath,true);
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

        // compute dest size in px
        if (computeSize(image,imagePath) == 1) { // image saved
            delete image;
            getNextOrStop();
            continue;
        }

        // ask enlarge
        if ( (hasWidth && image->width()<width && image->width()>=image->height()) ||
             (hasHeight && image->height()<height && image->width()<=image->height()) ) {
            if (!(shared->enlargeAll ||
                  shared->noEnlargeAll || shared->abort)) {
                enlargeMutex.lock();
                emit question(imagePath,tid,"enlarge");
                enlargeCondition.wait(&enlargeMutex);
            }
            if (shared->enlargeResult != 0 && shared->enlargeResult != 2) {
                if (shared->enlargeResult == 4)
                    emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
                else
                    emit imageStatus(imageData, tr("Skipped"), SKIPPED);
                delete image;
                getNextOrStop();
                continue;
            }
        }

        // rotate image and update thumbnail
        rotateImage(image);
        updateThumbnail(image);

        QImage destImg;

        if (hasWidth && hasHeight && !maintainAspect)
            destImg = image->scaled(width, height, Qt::IgnoreAspectRatio,
                                    Qt::SmoothTransformation);
        else if (hasWidth && hasHeight && maintainAspect)
            destImg =  image->scaled(width, height, Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);
        else if (hasWidth && !hasHeight)
            destImg = image->scaledToWidth(width, Qt::SmoothTransformation);
        else if (!hasWidth && hasHeight)
            destImg = image->scaledToHeight(height, Qt::SmoothTransformation);
        else if (!hasWidth && !hasHeight)
            destImg = *image;

        // ask overwrite
        if ( QFile::exists( targetFilePath ) &&
             !(shared->overwriteAll || shared->abort || shared->noOverwriteAll)) {
            if (!(shared->overwriteAll
               || shared->abort || shared->noOverwriteAll)) {
                overwriteMutex.lock();
                emit question(targetFilePath,tid,"overwrite");
                overwriteCondition.wait(&overwriteMutex);
            }
            if(shared->overwriteResult == 0 || shared->overwriteResult == 2) {
                if (destImg.save(targetFilePath, 0, shared->quality)) {
                    if (saveMetadata)
                        metadata.write(targetFilePath, destImg);
                    emit imageStatus(imageData, tr("Converted"), CONVERTED);
                }
                else
                    emit imageStatus(imageData, tr("Failed to convert"), FAILED);
            }
            else if (shared->overwriteResult == 4)
                emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
            else
                emit imageStatus(imageData, tr("Skipped"), SKIPPED);
        }
        else if (shared->noOverwriteAll)
            emit imageStatus(imageData, tr("Skipped"), SKIPPED);
        else if (shared->abort)
            emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
        else { // when overwriteAll is true or file not exists
            if (destImg.save(targetFilePath, 0, shared->quality)) {
                if (saveMetadata)
                    metadata.write(targetFilePath, destImg);
                emit imageStatus(imageData, tr("Converted"), CONVERTED);
            }
            else
                emit imageStatus(imageData, tr("Failed to convert"), FAILED);
        }
        delete image;
        getNextOrStop();
    }
}

void ConvertThread::getNextOrStop() {
    imageMutex.lock();
    emit getNextImage(this->tid);
    imageCondition.wait(&imageMutex);
    imageMutex.unlock();
}

void ConvertThread::printError() {
    MetadataUtils::Error *error = metadata.lastError();
    qWarning() << "tid:" << tid << "/n"
               << "    " << error->message() << "/n"
               << "    " << tr("Error code:") << error->code() << "/n"
               << "    " << error->what();
}

void ConvertThread::rotateImage(QImage *image) {

    bool saveExifOrientation = !shared->realRotate;
    // rotate image
    if (rotate && angle != 0.0) {
        int alpha = (int)angle;
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
                metadata.setExifDatum("Exif.Image.Orientation",1);
                saveExifOrientation = false;
            }
            else
                metadata.setExifDatum("Exif.Image.Orientation",orientation);
        }
    }
    // really rotate without saving Exif orientation tag
    if (!saveExifOrientation || shared->realRotate) {
        QTransform transform;
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
        transform.rotate(angle);
        *image = image->transformed(transform, Qt::SmoothTransformation);
    }
}

void ConvertThread::updateThumbnail(const QImage *image) {

    // update thumbnail
    if (saveMetadata && shared->updateThumbnail) {
        MetadataUtils::ExifStruct *exifStruct = metadata.exifStruct();
        int w = exifStruct->thumbnailWidth.split(' ').first().toInt();
        int h = exifStruct->thumbnailHeight.split(' ').first().toInt();
        QImage tmpImg = image->scaled(w,h, Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation);
        bool specialRotate = (rotate && (int)angle%90 != 0);
        QImage *thumbnail = &exifStruct->thumbnailImage;
        if (specialRotate) {
            w = tmpImg.width();
            h = tmpImg.height();
            *thumbnail = tmpImg;
        }
        else {
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
        bool linearSize = false;
        if (shared->format == "bmp") {
            destSize -= 54;
            destSize /= 3;
            linearSize = true;
        }
        else if (shared->format == "ppm") {
            destSize -= 17;
            destSize /= 3;
            linearSize = true;
        }
        else if (shared->format == "ico") {
            destSize -= 1422;
            destSize /= 4;
            linearSize = true;
        }
        else if (shared->format == "tif" || shared->format == "tiff") {
            destSize -= 14308;
            destSize /= 4;
            linearSize = true;
        }
        else if (shared->format == "xbm") {
            destSize -= 60;
            destSize /= 0.65;
            linearSize = true;
        }
        if (linearSize) {
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
            quint32 fileSize = QFile(imagePath).size();
            QSize size = image->size();
            double fileSizeRatio = (double) fileSize / shared->sizeBytes;
            fileSizeRatio = sqrt(fileSizeRatio);
            QFile tempFile(tempFilePath);
            tempFile.open(QIODevice::WriteOnly);
            for (uchar i=0; (fileSizeRatio<0.99 || fileSizeRatio>1.) && i<20; i++) {
                width = size.width() / fileSizeRatio;
                height = size.height() / fileSizeRatio;
                tempImage = image->scaled(width, height, Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);
                rotateImage(&tempImage);
                updateThumbnail(&tempImage);
                if (tempImage.save(&tempFile, 0, shared->quality)) {
                    if (saveMetadata)
                        metadata.write(tempFilePath, tempImage);
                }
                else {
                    qWarning("tid %d: Save temporary image file "
                             "into %s failed", tid,
                             MetadataUtils::String(targetFilePath).toNativeStdString().data());
                    emit imageStatus(imageData, tr("Failed to compute image size"),
                                     FAILED);
                    return false;
                }
                fileSize = tempFile.size();
                size = tempImage.size();
                fileSizeRatio = (double) fileSize / shared->sizeBytes;
                fileSizeRatio = sqrt(fileSizeRatio);
                tempFile.close();
            }
            // ask enlarge
            if ( (hasWidth && image->width()<width && image->width()>=image->height()) ||
                 (hasHeight && image->height()<height && image->width()<=image->height()) ) {
                if (!(shared->enlargeAll || shared->noEnlargeAll || shared->abort)) {
                    enlargeMutex.lock();
                    emit question(imagePath,tid,"enlarge");
                    enlargeCondition.wait(&enlargeMutex);
                }
                if (shared->enlargeResult != 0 && shared->enlargeResult != 2) {
                    if (shared->enlargeResult == 4)
                        emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
                    else
                        emit imageStatus(imageData, tr("Skipped"), SKIPPED);
                }
            }
            // ask overwrite
            if ( QFile::exists( targetFilePath ) &&
                 !(shared->overwriteAll || shared->abort
                   || shared->noOverwriteAll)) {
                if (!(shared->overwriteAll || shared->abort
                      || shared->noOverwriteAll)) {
                    overwriteMutex.lock();
                    emit question(targetFilePath,tid,"overwrite");
                    overwriteCondition.wait(&overwriteMutex);
                }
                if(shared->overwriteResult == 0 || shared->overwriteResult == 2) {
                    QFile::remove(targetFilePath);
                    if (tempFile.copy(targetFilePath))
                        emit imageStatus(imageData, tr("Converted"), CONVERTED);
                    else {
                        emit imageStatus(imageData, tr("Failed to save"), FAILED);
                        return -1;
                    }
                }
                else if (shared->overwriteResult == 4)
                    emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
                else
                    emit imageStatus(imageData, tr("Skipped"), SKIPPED);
            }
            else if (shared->noOverwriteAll)
                emit imageStatus(imageData, tr("Skipped"), SKIPPED);
            else if (shared->abort)
                emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
            else { // when overwriteAll is true or file not exists
                QFile::remove(targetFilePath);
                if (tempFile.copy(targetFilePath))
                    emit imageStatus(imageData, tr("Converted"), CONVERTED);
                else {
                    emit imageStatus(imageData, tr("Failed to save"), FAILED);
                    return -2;
                }
            }
        }
        return 1;
    }
    return 0;
}
