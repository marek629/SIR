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

#include "convert/algorithm/ImageSizeComputer.hpp"

#include <QSvgRenderer>

#include "convert/algorithm/BytesImageSizeStrategy.hpp"
#include "convert/algorithm/PercentImageSizeStrategy.hpp"
#include "convert/algorithm/PixelsImageSizeStrategy.hpp"


ImageSizeComputer::ImageSizeComputer(const QString &imagePath, int tid)
{
    path = imagePath;
    threadId = tid;
}

ImageSizeComputeResult ImageSizeComputer::calculate(QSvgRenderer *renderer, char sizeUnit)
{
    std::unique_ptr<ImageSizeStrategy> sizeStrategy = createSizeStrategy(sizeUnit);
    sizeStrategy->setFilePath(path);
    return sizeStrategy->calculate(renderer);
}

bool ImageSizeComputer::isSaveMetadataAllowed() const
{
    return saveMetadataAllowed;
}

void ImageSizeComputer::setSaveMetadataAllowed(bool value)
{
    saveMetadataAllowed = value;
}

std::unique_ptr<ImageSizeStrategy> ImageSizeComputer::createSizeStrategy(char sizeUnit)
{
    switch (sizeUnit) {
        case 1:
            return std::unique_ptr<ImageSizeStrategy>(new PercentImageSizeStrategy());
            break;
        case 2:
            return createBytesSizeStrategy();
            break;
        case 0:
        default:
            return std::unique_ptr<ImageSizeStrategy>(new PixelsImageSizeStrategy());
            break;
    }
}

std::unique_ptr<ImageSizeStrategy> ImageSizeComputer::createBytesSizeStrategy()
{
    BytesImageSizeStrategy *strategy = new BytesImageSizeStrategy(
                threadId, saveMetadataAllowed);
    return std::unique_ptr<ImageSizeStrategy>(strategy);
}
