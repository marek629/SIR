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

void ConvertThread::setSaveMetadata(bool value) {
    ConvertThread::shared->saveMetadata = value;
}

ConvertThread::ConvertThread(QObject *parent, int tid):QThread(parent) {

    this->tid = tid;
    m_width = 0;
    m_height = 0;
    m_hasWidth = false;
    m_hasHeight = false;
    m_rotate = false;
    m_angle = 0.0;
    m_quality = 100;
    work = true;
}

void ConvertThread::setAcceptWork(bool work) {
    this->work = work;
}

void ConvertThread::setDesiredSize(int width, int height, bool hasWidth,
                                   bool hasHeight, bool maintainAspect) {

    m_width = width;
    m_height = height;
    m_hasWidth = hasWidth;
    m_hasHeight = hasHeight;
    m_maintainAspect = maintainAspect;
}


void ConvertThread::setDesiredFormat(const QString& format) {
    m_format = format;
}

void ConvertThread::setDesiredRotation(bool rotate, double angle) {
    m_rotate = rotate;
    m_angle = angle;
}

void ConvertThread::setQuality(int quality) {
    m_quality = quality;
}

void ConvertThread::setDestPrefix(const QString& destPrefix) {
    m_destPrefix = destPrefix;
}

void ConvertThread::setDestFolder(const QDir& destFolder) {
    m_destFolder = destFolder;
}

void ConvertThread::setOverwriteAll(bool overwriteAll) {
    QMutexLocker locker(&(ConvertThread::shared->mutex));
    ConvertThread::shared->overwriteAll = overwriteAll;
}

void ConvertThread::convertImage(const QString& name, const QString& extension,
                                 const QString& path, bool onlySelected) {

    this->onlySelected = onlySelected;
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

    int width = m_width;
    int height = m_height;
    bool hasWidth = m_hasWidth;
    bool hasHeight = m_hasHeight;
    bool maintainAspect = m_maintainAspect;
    QString format = m_format;
    bool rotate = m_rotate;
    double angle = m_angle;
    int quality = m_quality;	
    QString destPrefix = m_destPrefix;
    QDir destFolder = m_destFolder;
    bool rawEnabled = RawUtils::isRawEnabled();

    while(work) {
        if (shared->abort)
        {
            emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
            getNextOrStop();
            continue;
        }

        emit imageStatus(imageData, tr("Converting"), CONVERTING);

        QString imageName = imageData.at(0);
        QString originalFormat = imageData.at(1);

        QString targetFile;
        if (destPrefix.isEmpty()) {
                targetFile = destFolder.absolutePath() + QDir::separator()
                             + imageName + "." + format;
        }
        else {
            targetFile = destFolder.absolutePath() + QDir::separator() +
                         destPrefix + "_" + imageName + "." + format;
        }

        QString imagePath = imageData.at(2) + QDir::separator() +
                            imageData.at(0) + "." + originalFormat;


        QImage *image = new QImage();

        if(rawEnabled) {
            if(RawUtils::isRaw(imagePath)) {
                image = RawUtils::loadRawImage(imagePath);
            }
            else {
                image->load(imagePath);
            }
        }
        else {
            image->load(imagePath);
        }

        if(image->isNull()) {
            //For some reason we where not able to open the image file
            emit imageStatus(imageData, tr("Failed to open original image"),
                              FAILED);
            delete image;

            //Ask for the next image and go to the beginning of the loop
            getNextOrStop();
            continue;
        }

        if ( (hasWidth && image->width()<width && image->width()>=image->height()) ||
             (hasHeight && image->height()<height && image->width()<=image->height()) ) {

            if (!(ConvertThread::shared->enlargeAll ||
                  ConvertThread::shared->noEnlargeAll || ConvertThread::shared->abort)) {
                enlargeMutex.lock();
                emit question(imagePath,tid,"enlarge");
                enlargeCondition.wait(&enlargeMutex);
            }
            if (ConvertThread::shared->enlargeResult != 0 &&
                    ConvertThread::shared->enlargeResult != 2) {
                if (ConvertThread::shared->enlargeResult == 4)
                    emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
                else
                    emit imageStatus(imageData, tr("Skipped"), SKIPPED);
                delete image;
                getNextOrStop();
                continue;
            }
        }

        if (rotate && angle != 0.0) {
                QMatrix m;
                m.rotate( angle );
                *image = image->transformed( m,Qt::SmoothTransformation );
        }

        if (ConvertThread::shared->saveMetadata)
            metadata.read(imagePath);

        QImage destImg;
        
        if (hasWidth && hasHeight && !maintainAspect) {
            destImg = image->scaled(width, height, Qt::IgnoreAspectRatio,
                                    Qt::SmoothTransformation);
        }

        else if (hasWidth && hasHeight && maintainAspect) {
            destImg =  image->scaled(width, height, Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);
        }

        else if (hasWidth && !hasHeight) {
            destImg = image->scaledToWidth(width, Qt::SmoothTransformation);
        }
        else if (!hasWidth && hasHeight) {
            destImg = image->scaledToHeight(height, Qt::SmoothTransformation);
        }
        else if (!hasWidth && !hasHeight) {
            destImg = image->scaled(image->width(), image->height(),
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);
        }

        if ( QFile::exists( targetFile ) &&
             !(ConvertThread::shared->overwriteAll
               || ConvertThread::shared->abort || ConvertThread::shared->noOverwriteAll)) {

            if (!(ConvertThread::shared->overwriteAll
               || ConvertThread::shared->abort || ConvertThread::shared->noOverwriteAll)) {
                overwriteMutex.lock();
                emit question(targetFile,tid,"overwrite");
                overwriteCondition.wait(&overwriteMutex);
            }

            if(ConvertThread::shared->overwriteResult == 0
               || ConvertThread::shared->overwriteResult == 2) {

                if (destImg.save(targetFile, 0, quality)) {
                    if (ConvertThread::shared->saveMetadata)
                        metadata.write(targetFile, destImg);
                    emit imageStatus(imageData, tr("Converted"), CONVERTED);
                }
                else
                    emit imageStatus(imageData, tr("Failed to convert"), FAILED);
            }
            else if (ConvertThread::shared->overwriteResult == 4)
                emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
            else
                emit imageStatus(imageData, tr("Skipped"), SKIPPED);
        }
        else if (ConvertThread::shared->noOverwriteAll)
            emit imageStatus(imageData, tr("Skipped"), SKIPPED);
        else if (ConvertThread::shared->abort)
            emit imageStatus(imageData, tr("Cancelled"), CANCELLED);
        else { // when overwriteAll is true or file not exists
            if (destImg.save(targetFile, 0, quality)) {
                if (ConvertThread::shared->saveMetadata)
                    metadata.write(targetFile, destImg);
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
    emit getNextImage(this->tid, onlySelected);
    imageCondition.wait(&imageMutex);
    imageMutex.unlock();
}

