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
#include <QPainter>
#include <QSvgRenderer>

#include "ConvertEffects.hpp"
#include "ConvertThread.hpp"
#include "sir_String.hpp"
#include "convert/model/ImageFileSize.hpp"
#include "convert/service/ImageChangeService.hpp"
#include "convert/service/ImageFileService.hpp"


using namespace sir;


BytesImageSizeStrategy::BytesImageSizeStrategy(int threadId, bool saveMetadataAllowed)
    : ImageSizeStrategy()
{
    this->threadId = threadId;
    this->saveMetadataAllowed = saveMetadataAllowed;
}

ImageSizeComputeResult BytesImageSizeStrategy::calculate(QSvgRenderer *renderer)
{
    targetImageModel.setSize(renderer->defaultSize());

    /* TODO: set to true ConvertThread::hasWidth and ConvertThread::hasHeight
     *       or modify TargetImage::hasWidth() and TargetImage::hasHeight() methods bodies
     */

    // TODO: set ConvertThread::shared.targetImage to targetImage field
    if (targetImageModel.imageFormat().isLinearFileSizeFormat()) {
        // TODO: set bytes value of targetImage field
        double destSize = countTargetFileSize(targetImageModel.bytes());
        // TODO: set size value of targetImage field
        double sourceSizeSqrt = sqrt(targetImageModel.size().width() * targetImageModel.size().height());
        double sourceWidthRatio = targetImageModel.size().width() / sourceSizeSqrt;
        double sourceHeightRatio = targetImageModel.size().height() / sourceSizeSqrt;
        destSize = sqrt(destSize);
        targetImageModel.setSize(QSize(sourceWidthRatio * destSize, sourceHeightRatio * destSize));
    }
    else {
        QString tempFilePath = temporaryFilePath();
        qint64 fileSize = QFile(path).size();
        QSize size = targetImageModel.size();
        double fileSizeRatio = (double) fileSize / targetImageModel.bytes();
        fileSizeRatio = sqrt(fileSizeRatio);
        QFile tempFile(tempFilePath);
        QPainter painter;
        for (uchar i=0; i<10 && (fileSizeRatio<0.97412 || fileSizeRatio>1.); i++) {
            resultSize.setWidth(size.width() / fileSizeRatio);
            resultSize.setHeight(size.height() / fileSizeRatio);
            QImage tempImage(resultSize, QImage::Format_ARGB32);
            fillImage(&tempImage);
            painter.begin(&tempImage);
            renderer->render(&painter);
            painter.end();

            ImageChangeService imageChangeService(tempImage);
            tempImage = paintEffects(&tempImage);
            double rotationAngle = imageChangeService.rotateImage(&this->targetImageModel);
            tempImage = imageChangeService.image();

            ImageFileService imageFileService(tempImage, targetImageModel);
            if (imageFileService.writeImage(tempImage)) {
    #ifdef SIR_METADATA_SUPPORT
                if (saveMetadataAllowed) {
                    // TODO: missing field metadata
                    metadata->write(tempFilePath, tempImage);
                    imageFileService.updateThumbnail(rotationAngle);
                }
    #endif // SIR_METADATA_SUPPORT
            } else {
                const char *filePathData = String(targetImageModel.filePath()).
                        toNativeStdString().data();
                qWarning("tid %d: Save temporary image file into %s failed",
                         threadId, filePathData);

                QStringList imageData = targetImageModel.imageDataStringList();
                // TODO: connect to the same slots as ConvertThread::imageStatus() signal or remove this signal
                emit imageStatus(imageData, tr("Failed to compute image size"),
                                 ConvertThread::Failed);
                resultState.setCanNotWriteTemporaryImageFile();
                return createResult();
            }
            fileSize = tempFile.size();
            size = tempImage.size();
            fileSizeRatio = (double) fileSize / targetImageModel.bytes();
            fileSizeRatio = sqrt(fileSizeRatio);
        }

        // ask overwrite
        char answer = questionManager->askOverwrite(&tempFile);
        if (answer < 0) {
            resultState.setNegativeOverwriteAnswer(answer);
            return createResult();
        }
    }
    resultState.setComputedForBytesSize();
    return createResult();
}

void BytesImageSizeStrategy::setMetadata(MetadataUtils::Metadata *value)
{
    metadata = value;
}

void BytesImageSizeStrategy::setQuestionManager(ConvertThread *value)
{
    questionManager = value;
}

double BytesImageSizeStrategy::countTargetFileSize(double fileSize)
{
    ImageFileSize imageFileSize(fileSize);
    return imageFileSize.bytesByFormat(targetImageModel.imageFormat());
}

QString BytesImageSizeStrategy::temporaryFilePath()
{
    return QDir::tempPath() + QDir::separator() +
            "sir_temp" + QString::number(threadId) +
            "." + targetImageModel.imageFormat().qString();
}

void BytesImageSizeStrategy::fillImage(QImage *img)
{
    if (targetImageModel.backgroundColor().isValid()) {
        img->fill(targetImageModel.backgroundColor().rgb());
    } else {
        if (targetImageModel.imageFormat().isTransparentSupportFormat()) {
            img->fill(Qt::transparent);
        } else {
            // in other formats tranparency isn't supported
            img->fill(Qt::white);
        }
    }
}

QImage BytesImageSizeStrategy::paintEffects(QImage *image)
{
    QImage destImg(*image);

    EffectsConfiguration effectsConfiguration = targetImageModel.effectsConfiguration();
    ConvertEffects effectPainter(&destImg, effectsConfiguration);

    if (effectsConfiguration.getHistogramOperation() > 0) {
        effectPainter.modifyHistogram();
    }
    if (effectsConfiguration.getFilterType() != NoFilter) {
        effectPainter.filtrate();
    }
    if (effectsConfiguration.getFrameWidth() > 0
            && effectsConfiguration.getFrameColor().isValid()) {
        destImg = effectPainter.framedImage();
        effectPainter.setImage(&destImg);
    }
    if (!effectsConfiguration.getImage().isNull()) {
        effectPainter.addImage();
    }
    if (!effectsConfiguration.getTextString().isEmpty()) {
        effectPainter.addText();
    }

    return destImg;
}
