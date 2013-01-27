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

#include <QPainter>
#include "converteffects.h"

/** Creates ConvertEffects object.
  * \sa setSharedInfo()
  */
ConvertEffects::ConvertEffects(SharedInformation *shared) {
    img = 0;
    setSharedInfo(shared);
}

/** Creates ConvertEffects object.
  * \sa setImage() setSharedInfo()
  */
ConvertEffects::ConvertEffects(QImage *image, SharedInformation *shared) {
    setImage(image);
    setSharedInfo(shared);
}

/** Destroys ConvertEffects object. */
ConvertEffects::~ConvertEffects() {}

/** Sets pointer to shared information object. */
void ConvertEffects::setSharedInfo(SharedInformation *shared) {
    this->shared = shared;
}

/** Returns pointer to shared information object. */
SharedInformation * ConvertEffects::sharedInfo() const {
    return this->shared;
}

/** Sets pointer to #img image object. */
void ConvertEffects::setImage(QImage *image) {
    img = image;
}

/** Returns pointer to #img image object. */
QImage * ConvertEffects::image() const {
    return img;
}

/** Adds frame to new image and returns this. */
QImage ConvertEffects::framedImage() {
    QImage result;
    if (!img || img->isNull())
        return result;
    int w2 = 2 * shared->frameWidth; // double frame width
    if (shared->frameAddAround) {
        QSize size = img->size();
        size += QSize(w2, w2);
        result = QImage(size, img->format());
    }
    else
        result = *img;
    QPainter painter(&result);
    QPen pen(Qt::SolidLine);
    if (shared->borderInsideWidth + shared->borderOutsideWidth < shared->frameWidth) {
        pen.setWidth(w2);
        pen.setColor(shared->frameColor);
        painter.setPen(pen);
        painter.drawRect(result.rect());
    }
    if (shared->borderOutsideWidth > 0) {
        pen.setWidth(2*shared->borderOutsideWidth);
        pen.setColor(shared->borderOutsideColor);
        painter.setPen(pen);
        painter.drawRect(result.rect());
    }
    if (shared->borderInsideWidth > 0) {
        pen.setWidth(shared->borderInsideWidth);
        pen.setColor(shared->borderInsideColor);
        painter.setPen(pen);
        int ih = shared->frameWidth - shared->borderInsideWidth * 0.5; // half of inside border
        int sub = w2 - ih + 1;
        painter.drawRect(ih, ih, result.width()-sub, result.height()-sub);
    }
    if (shared->frameAddAround)
        painter.drawImage(shared->frameWidth, shared->frameWidth, *img);
    return result;
}

/** Draws text on #img image.
  * \sa addImage() image() setImage()
  */
void ConvertEffects::addText() {
    if (!img) {
        qDebug("EffectPainter::addText(): image is not set!");
        return;
    }

    // reference point setup
    QPoint point = getTransformOriginPoint(shared->textPos, shared->textUnitPair);

    // text bounding rect setup
    QFontMetrics fontMetrics(shared->textFont, img);
    QRect rect = fontMetrics.boundingRect(shared->textString);
    const int dx = 5;
    const int dy = 1;
    rect.adjust(-dx, -dy, dx, dy);
    rect = getEffectBoundingRect(rect, point, shared->textPosModifier);

    QPainter painter(img);
    painter.setPen(shared->textColor);
    painter.setFont(shared->textFont);
    painter.setOpacity(shared->textOpacity);

    // rotate painter around center point of image
    this->rotate(&painter, point, shared->textRotation);

    // draw text
    painter.drawText(rect, Qt::AlignCenter, shared->textString);
    if (shared->textFrame) {
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.drawRect(rect);
    }
}

/** Draws image on #img image.
  * \sa addText() image() setImage()
  */
void ConvertEffects::addImage() {
    if (!img) {
        qDebug("EffectPainter::addImage(): image is not set!");
        return;
    }

    QPoint point = getTransformOriginPoint(shared->imagePos, shared->imageUnitPair);

    QRect rect = getEffectBoundingRect(shared->image.rect(), point,
                                       shared->imagePosModifier);

    QPainter painter(img);
    painter.setOpacity(shared->imageOpacity);
    painter.resetTransform();

    this->rotate(&painter, point, shared->imageRotation);

    painter.drawImage(rect, shared->image);
}

/** Rotates given \a painter around \a originPoint by \a angle in degrees. */
void ConvertEffects::rotate(QPainter *painter, const QPoint &originPoint, int angle) {
    QTransform t;
    t.translate(originPoint.x(), originPoint.y());
    t.rotate(angle);
    t.translate(-originPoint.x(), -originPoint.y());
    painter->setTransform(t);
}

/** Returns modified copy of \a position point.\n
  * The result of this function are modified by \a units pair items and img image.
  * \sa getEffectBoundingRect() image() setImage()
  */
QPoint ConvertEffects::getTransformOriginPoint(const QPoint &position,
                                              const PosUnitPair &units) {
    QPoint result(position);
    if (units.first == Percent)
        result.setX(0.01 * result.x() * img->width());
    if (units.second == Percent)
        result.setY(0.01 * result.y() * img->height());
    if (shared->textPos.x() < 0)
        result.setX(result.x() + img->width());
    if (shared->textPos.y() < 0)
        result.setY(result.y() + img->height());
    return result;
}

/** Returns moved copy of \a rect rectangle.\n
  * The result of this function are moved to \a pos point having regard to
  * \a modifier value.
  * \sa getTransformOriginPoint() image() setImage()
  */
QRect ConvertEffects::getEffectBoundingRect(const QRect &rect, const QPoint &pos,
                                           PosModifier modifier) {
    QRect result(rect);
    switch (modifier) {
    case Center:
        result.moveCenter(pos);
        break;
    case MiddleTopEdge:
        result.moveCenter(pos);
        result.moveTop(pos.y());
        break;
    case TopRightCorner:
        result.moveTopRight(pos);
        break;
    case MiddleRightEdge:
        result.moveCenter(pos);
        result.moveRight(pos.x());
        break;
    case BottomRightCorner:
        result.moveBottomRight(pos);
        break;
    case MiddleBottomEdge:
        result.moveCenter(pos);
        result.moveBottom(pos.y());
        break;
    case BottomLeftCorner:
        result.moveBottomLeft(pos);
        break;
    case MiddleLeftEdge:
        result.moveCenter(pos);
        result.moveLeft(pos.x());
        break;
    case TopLeftCorner:
    default:
        result.moveTo(pos);
        break;
    }
    return result;
}
