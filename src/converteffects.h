/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://sir.projet-libre.org/
 */

#ifndef EFFECTPAINTER_H
#define EFFECTPAINTER_H

#include "sharedinformation.h"

class ConvertEffects {
public:
    ConvertEffects(SharedInformation *shared = 0);
    ConvertEffects(QImage *image, SharedInformation *shared = 0);
    ~ConvertEffects();
    void setSharedInfo(SharedInformation *shared);
    SharedInformation *sharedInfo() const;
    void setImage(QImage *image);
    QImage *image() const;
    QImage framedImage();
    void addText();
    void addImage();

private:
    // fields
    QImage *img; /**< Image for drawing effects. */
    SharedInformation *shared; /**< Convert shared information. */
    // methods
    void rotate(QPainter *painter, const QPoint &originPoint, int angle);
    QPoint getTransformOriginPoint(const QPoint &position, const PosUnitPair &units);
    QRect getEffectBoundingRect(const QRect &rect, const QPoint &pos,
                                PosModifier modifier);
};

#endif // EFFECTPAINTER_H
