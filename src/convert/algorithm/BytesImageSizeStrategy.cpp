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

#include "convert/algorithm/BytesImageSizeStrategy.hpp"

#include <cmath>

#include <QDir>
#include <QSvgRenderer>

#include "sir_String.hpp"
#include "convert/model/ImageFileSize.hpp"
#include "convert/service/ImageChangeService.hpp"
#include "convert/service/ImageFileService.hpp"


BytesImageSizeStrategy::BytesImageSizeStrategy(int threadId) : ImageSizeStrategy()
{
    this->threadId = threadId;
}

void BytesImageSizeStrategy::calculate(QSvgRenderer *renderer)
{
    imageModel.setSize(renderer->defaultSize());

    // TODO: set to true ConvertThread::hasWidth and ConvertThread::hasHeight

    // TODO: set ConvertThread::shared.targetImage to targetImage field
    if (imageModel.imageFormat().isLinearFileSizeFormat()) {
        // TODO: set bytes value of targetImage field
        double destSize = countTargetFileSize(imageModel.bytes());
        // TODO: set size value of targetImage field
        double sourceSizeSqrt = sqrt(imageModel.size().width() * imageModel.size().height());
        double sourceWidthRatio = imageModel.size().width() / sourceSizeSqrt;
        double sourceHeightRatio = imageModel.size().height() / sourceSizeSqrt;
        destSize = sqrt(destSize);
        imageModel.setSize(QSize(sourceWidthRatio * destSize, sourceHeightRatio * destSize));
    }
    else {
        QString tempFilePath = temporaryFilePath();
        qint64 fileSize = QFile(path).size();
        QSize size = imageModel.size();
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
            // TODO: extract paintEffects(), rotateImage(), updateThumbnail() from ConvertThread class to somewhere else
            ImageChangeService imageChangeService(tempImage);
            tempImage = paintEffects(&tempImage);
//            tempImage = rotateImage(tempImage);
            angle = imageChangeService.rotateImage(angle);
            tempImage = imageChangeService.image();
    #ifdef SIR_METADATA_SUPPORT
            updateThumbnail(tempImage);
    #endif // SIR_METADATA_SUPPORT

            ImageFileService imageFileService;
            if (imageFileService.writeImage(tempImage, tempFilePath)) {
    #ifdef SIR_METADATA_SUPPORT
                if (saveMetadata)
                    // TODO: missing field metadata
                    metadata.write(tempFilePath, tempImage);
    #endif // SIR_METADATA_SUPPORT
            }
            else {
                qWarning("tid %d: Save temporary image file "
                         "into %s failed", threadId,
                         String(targetFilePath).
                            toNativeStdString().data());
                // TODO: emit to nowhere
                emit imageStatus(imageData, tr("Failed to compute image size"),
                                 Failed);
                resultState.setCanNotWriteTemporaryImageFile();
                return;// -4;
            }
            fileSize = tempFile.size();
            size = tempImage.size();
            fileSizeRatio = (double) fileSize / shared.sizeBytes;
            fileSizeRatio = sqrt(fileSizeRatio);
        }

        // TODO: call ConvertThread::askOverwrite() method
        // ask overwrite
        char answer = askOverwrite(&tempFile);
        if (answer < 0) {
            resultState.setNegativeOverwriteAnswer(answer);
            return;
        }
    }
    resultState.setComputedForBytesSize();
}

double BytesImageSizeStrategy::countTargetFileSize(double fileSize)
{
    ImageFileSize imageFileSize(fileSize);
    return imageFileSize.bytesByFormat(shared.targetImage.imageFormat());
}

QString BytesImageSizeStrategy::temporaryFilePath()
{
    return QDir::tempPath() + QDir::separator() +
            "sir_temp" + QString::number(threadId) +
            "." + shared.targetImage.imageFormat().qString();
}