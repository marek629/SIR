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

#include "convert/model/TargetImage.hpp"


TargetImage::TargetImage() {}

TargetImage::TargetImage(const ImageFormat &imageFormat) : format(imageFormat) {}

ImageFormat TargetImage::imageFormat() const
{
    return format;
}

void TargetImage::setImageFormat(const ImageFormat &value)
{
    format = value;
}

bool TargetImage::isProgressiveWriteEnabled() const
{
    return progressiveWriteEnabled;
}

void TargetImage::setProgressiveWriteEnabled(bool value)
{
    progressiveWriteEnabled = value;
}

bool TargetImage::isOptimizedWriteEnabled() const
{
    return optimizedWriteEnabled;
}

void TargetImage::setOptimizedWriteEnabled(bool value)
{
    optimizedWriteEnabled = value;
}

int TargetImage::compression() const
{
    return compressionLevel;
}

void TargetImage::setCompression(int value)
{
    compressionLevel = value;
}

int TargetImage::quality() const
{
    return qualityLevel;
}

void TargetImage::setQuality(int value)
{
    qualityLevel = value;
}

unsigned int TargetImage::bytes() const
{
    return sizeBytes;
}

void TargetImage::setBytes(unsigned int value)
{
    sizeBytes = value;
}
