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

#ifndef SVGPARAMETERS_HPP
#define SVGPARAMETERS_HPP

#include <QStringList>


class SvgParameters
{
public:
    SvgParameters();

    bool isSvgModifiersEnabled() const;
    void setSvgModifiersEnabled(bool value);
    QString getSvgRemoveTextString() const;
    void setSvgRemoveTextString(const QString &value);
    bool isSvgRemoveEmptyGroup() const;
    void setSvgRemoveEmptyGroup(bool value);
    bool isSvgSave() const;
    void setSvgSave(bool value);

    bool isMaintainAspect() const;
    void setMaintainAspect(bool value);

    QStringList getImageData() const;
    void setImageData(const QStringList &value);

    int getWidth() const;
    void setWidth(int value);
    int getHeight() const;
    void setHeight(int value);
    bool getHasWidth() const;
    void setHasWidth(bool value);
    bool getHasHeight() const;
    void setHasHeight(bool value);

    qint8 getSizeComputed() const;
    void setSizeComputed(const qint8 &value);

    QString getTargetFilePath() const;
    void setTargetFilePath(const QString &value);

private:
    bool svgModifiersEnabled;
    QString svgRemoveTextString;
    bool svgRemoveEmptyGroup;
    bool svgSave;

    bool maintainAspect;

    QStringList imageData;

    int width;
    int height;
    bool hasWidth;
    bool hasHeight;

    qint8 sizeComputed;

    QString targetFilePath;
};

#endif // SVGPARAMETERS_HPP
