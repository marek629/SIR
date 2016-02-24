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

#include "image/QImageLoader.hpp"

#include "ConvertThread.hpp"
#include "SvgModifier.hpp"
#include "raw/RawImageLoader.hpp"

#include <QImageReader>
#include <QMessageBox>
#include <QPainter>
#include <QtSvg/QSvgRenderer>


QImageLoader::QImageLoader(RawModel *rawModel, ConvertThread *thread)
    : QObject()
{
    this->rawModel = rawModel;
    this->convertThread = thread;
}

QImage *QImageLoader::loadRawImage(const QString &imagePath)
{
    RawToolbox rawToolbox(rawModel);

    if (rawToolbox.isRawSupportEnabled()) {
        RawImageLoader rawLoader = RawImageLoader(rawModel, imagePath);
        return rawLoader.load();
    }

    return NULL;
}

QImage *QImageLoader::loadRegularImage(const QString &imagePath)
{
    QImage *image = NULL;

    QFileInfo imageFileInfo(imagePath);
    QString fileExtension = imageFileInfo.fileName().split('.').last();
    fileExtension = fileExtension.toLower();

    if (fileExtension == "png" || fileExtension == "gif") {
        QImage loadedImage;
        loadedImage.load(imagePath);
        image = new QImage(loadedImage.size(), loadedImage.format());
        convertThread->fillImage(image);
        QPainter painter(image);
        painter.drawImage(image->rect(), loadedImage);
    } else {
        image = new QImage();
        image->load(imagePath);
    }

    return image;
}

bool QImageLoader::isRegularImage(const QString &imagePath)
{
    QFileInfo imageFileInfo(imagePath);
    QString fileExtension = imageFileInfo.fileName().split('.').last();
    fileExtension = fileExtension.toLower();

    if (fileExtension == "svg" || fileExtension == "svgz") {
        return false;
    }

    QByteArray format = fileExtension.toLatin1();
    return QImageReader::supportedImageFormats().contains(format);
}

QImage *QImageLoader::loadSvgImage(const QString &imagePath)
{
    QSvgRenderer renderer;
    if (svgParameters.isSvgModifiersEnabled()) {
        SvgModifier modifier(imagePath);
        // modify SVG file
        if (!svgParameters.getSvgRemoveTextString().isNull())
            modifier.removeText(svgParameters.getSvgRemoveTextString());
        if (svgParameters.isSvgRemoveEmptyGroup())
            modifier.removeEmptyGroups();
        // save SVG file
        if (svgParameters.isSvgSave()) {
            QString targetFilePath = svgParameters.getTargetFilePath();
            QString svgTargetFileName = targetFilePath.left(
                        targetFilePath.lastIndexOf('.') + 1);
            svgTargetFileName += "svg";
            QFile file(svgTargetFileName);
            // ask overwrite
            if (file.exists()) {
                convertThread->askUser(svgTargetFileName,
                                       ConvertThread::Overwrite);
            }
            int overwriteDecision = convertThread->getUserAnswer(
                        ConvertThread::Overwrite);
            if (overwriteDecision == QMessageBox::Yes ||
                    overwriteDecision == QMessageBox::YesToAll) {
                if (!file.open(QIODevice::WriteOnly)) {
                    convertThread->setImageStatus(svgParameters.getImageData(),
                                                  tr("Failed to save new SVG file"),
                                                  ConvertThread::Failed);
                    return NULL;
                }
                file.write(modifier.content());
            }
        }
        // and load QByteArray buffer to renderer
        if (!renderer.load(modifier.content())) {
            convertThread->setImageStatus(svgParameters.getImageData(),
                                          tr("Failed to open changed SVG file"),
                                          ConvertThread::Failed);
            return NULL;
        }
    }
    else if (!renderer.load(imagePath)) {
        convertThread->setImageStatus(svgParameters.getImageData(),
                                      tr("Failed to open SVG file"),
                                      ConvertThread::Failed);
        return NULL;
    }
    svgParameters.setSizeComputed(convertThread->computeSize(&renderer, imagePath));
    if (svgParameters.getSizeComputed() == 2)
        return NULL;
    // keep aspect ratio
    if (svgParameters.isMaintainAspect()) {
        qreal w = svgParameters.getWidth();
        qreal h = svgParameters.getHeight();
        qreal targetRatio = w / h;
        QSizeF svgSize = renderer.defaultSize();
        qreal currentRatio = svgSize.width() / svgSize.height();
        if (currentRatio != targetRatio) {
            qreal diffRatio;
            if (currentRatio > targetRatio)
                diffRatio = w / svgSize.width();
            else
                diffRatio = h / svgSize.height();
            svgParameters.setWidth(diffRatio * svgSize.width());
            svgParameters.setHeight(diffRatio * svgSize.height());
        }
    }
    // create image
    QImage *img = new QImage(svgParameters.getWidth(), svgParameters.getHeight(),
                             QImage::Format_ARGB32);
    convertThread->fillImage(img);
    QPainter painter(img);
    renderer.render(&painter);
    // don't scale rendered image
    svgParameters.setHasWidth(false);
    svgParameters.setHasHeight(false);
    // finaly return the image pointer
    return img;
}

SvgParameters QImageLoader::getSvgParameters() const
{
    return svgParameters;
}

void QImageLoader::setSvgParameters(const SvgParameters &params)
{
    svgParameters = params;
}
