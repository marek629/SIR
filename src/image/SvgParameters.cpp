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

#include "image/SvgParameters.hpp"


SvgParameters::SvgParameters() {}

bool SvgParameters::isSvgModifiersEnabled() const
{
    return svgModifiersEnabled;
}

void SvgParameters::setSvgModifiersEnabled(bool value)
{
    svgModifiersEnabled = value;
}

QString SvgParameters::getSvgRemoveTextString() const
{
    return svgRemoveTextString;
}

void SvgParameters::setSvgRemoveTextString(const QString &value)
{
    svgRemoveTextString = value;
}

bool SvgParameters::isSvgRemoveEmptyGroup() const
{
    return svgRemoveEmptyGroup;
}

void SvgParameters::setSvgRemoveEmptyGroup(bool value)
{
    svgRemoveEmptyGroup = value;
}

bool SvgParameters::isMaintainAspect() const
{
    return maintainAspect;
}

void SvgParameters::setMaintainAspect(bool value)
{
    maintainAspect = value;
}

QStringList SvgParameters::getImageData() const
{
    return imageData;
}

void SvgParameters::setImageData(const QStringList &value)
{
    imageData = value;
}

int SvgParameters::getWidth() const
{
    return width;
}

void SvgParameters::setWidth(int value)
{
    width = value;
}

int SvgParameters::getHeight() const
{
    return height;
}

void SvgParameters::setHeight(int value)
{
    height = value;
}

bool SvgParameters::getHasWidth() const
{
    return hasWidth;
}

void SvgParameters::setHasWidth(bool value)
{
    hasWidth = value;
}

bool SvgParameters::getHasHeight() const
{
    return hasHeight;
}

void SvgParameters::setHasHeight(bool value)
{
    hasHeight = value;
}

qint8 SvgParameters::getSizeComputed() const
{
    return sizeComputed;
}

void SvgParameters::setSizeComputed(const qint8 &value)
{
    sizeComputed = value;
}
