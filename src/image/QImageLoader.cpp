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
#include "raw/RawImageLoader.hpp"

#include <QImageReader>
#include <QPainter>


QImageLoader::QImageLoader(RawModel *rawModel, ConvertThread *thread)
{
    this->rawModel = rawModel;
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
