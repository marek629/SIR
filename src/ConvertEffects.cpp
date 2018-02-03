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

#include <QPainter>
#include "ConvertEffects.hpp"

/** Creates ConvertEffects object.
  * \sa setSharedInfo()
  */
ConvertEffects::ConvertEffects(SharedInformation *shared) {
    img = 0;
    setSharedInfo(shared);
}

/** Creates ConvertEffects object.
  * \sa setImage() setSharedInfo()
  *
  * \deprecated Use 2-parametrized constructor (QImage*, const EffectsConfiguration &) instead.
  */
ConvertEffects::ConvertEffects(QImage *image, SharedInformation *shared)
{
    setImage(image);
    setSharedInfo(shared);
}

ConvertEffects::ConvertEffects(QImage *image, const EffectsConfiguration &effectsConfiguration)
{
    setImage(image);
    this->effectsConfiguration = effectsConfiguration;
}

/** Destroys ConvertEffects object. */
ConvertEffects::~ConvertEffects() {}

/** Sets pointer to shared information object. */
void ConvertEffects::setSharedInfo(SharedInformation *shared)
{
    this->shared = shared;
    this->effectsConfiguration = shared->effectsConfiguration();
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

void ConvertEffects::modifyHistogram() {
    switch (shared->effectsConfiguration().getHistogramOperation()) {
    case 1:
        stretchHistogram();
        break;
    case 2:
        equalizeHistogram();
        break;
    default:
        break;
    }
}

void ConvertEffects::stretchHistogram() {
    Q_ASSERT(img != NULL);
    Q_ASSERT(!img->isNull());

    const QPair<QColor, QColor> &range = colorRange();
    const qreal rMul = 255. / (range.second.red() - range.first.red());
    const qreal gMul = 255. / (range.second.green() - range.first.green());
    const qreal bMul = 255. / (range.second.blue() - range.first.blue());

    for (int x=0; x<img->width(); x++) {
        for (int y=0; y<img->height(); y++) {
            QRgb rgb = img->pixel(x,y);
            QColor c(rgb);
            c.setRed(rMul * (qRed(rgb) - range.first.red()) );
            c.setGreen(gMul * (qGreen(rgb) - range.first.green()) );
            c.setBlue(bMul * (qBlue(rgb) - range.first.blue()) );
            img->setPixel(x, y, c.rgb());
        }
    }
}

void ConvertEffects::equalizeHistogram() {
    Q_ASSERT(img != NULL);
    Q_ASSERT(!img->isNull());

    const QVector<Rgb> &LUT = lookUpTable();
    int i;

    for (int x=0; x<img->width(); x++) {
        for (int y=0; y<img->height(); y++) {
            QColor c(img->pixel(x,y));
            i = c.red();
            c.setRed(LUT[i].red);
            i = c.green();
            c.setGreen(LUT[i].green);
            i = c.blue();
            c.setBlue(LUT[i].blue);
            img->setPixel(x, y, c.rgb());
        }
    }
}

void ConvertEffects::filtrate()
{
    Q_ASSERT(img != NULL);
    Q_ASSERT(!img->isNull());

    switch (effectsConfiguration.getFilterType()) {
        case BlackAndWhite:
            for (int y=0; y<img->height(); y++) {
                for (int x=0; x<img->width(); x++) {
                    int gray = qGray(img->pixel(x,y));
                    img->setPixel(x, y, qRgb(gray, gray, gray));
                }
            }
            break;
        case Sepia:
            combine(QColor(112, 66, 20));
            break;
        case CustomColor:
            combine(effectsConfiguration.getFilterBrush().color());
            break;
        case Gradient:
            combine(effectsConfiguration.getFilterBrush());
            break;
        default:
            qDebug("ConvertEffects::filtate(): unexpected filter type occured");
            break;
    }
}

/** Adds frame to new image and returns this. */
QImage ConvertEffects::framedImage()
{
    Q_ASSERT(img != NULL);
    Q_ASSERT(!img->isNull());

    QImage result;
    if (!img || img->isNull())
        return result;
    int w2 = 2 * effectsConfiguration.getFrameWidth(); // double frame width
    if (effectsConfiguration.getFrameAddAround()) {
        QSize size = img->size();
        size += QSize(w2, w2);
        result = QImage(size, img->format());
    } else {
        result = *img;
    }

    QPainter painter(&result);
    QPen pen(Qt::SolidLine);
    if (effectsConfiguration.getBorderInsideWidth()
            + effectsConfiguration.getBorderOutsideWidth()
            < effectsConfiguration.getFrameWidth()) {
        pen.setWidth(w2);
        pen.setColor(effectsConfiguration.getFrameColor());
        painter.setPen(pen);
        painter.drawRect(result.rect());
    }
    if (effectsConfiguration.getBorderOutsideWidth() > 0) {
        pen.setWidth(2 * effectsConfiguration.getBorderOutsideWidth());
        pen.setColor(effectsConfiguration.getBorderOutsideColor());
        painter.setPen(pen);
        painter.drawRect(result.rect());
    }
    if (effectsConfiguration.getBorderInsideWidth() > 0) {
        pen.setWidth(effectsConfiguration.getBorderInsideWidth());
        pen.setColor(effectsConfiguration.getBorderInsideColor());
        painter.setPen(pen);
        int ih = effectsConfiguration.getFrameWidth()
                - effectsConfiguration.getBorderInsideWidth() * 0.5; // half of inside border
        int sub = w2 - ih + 1;
        painter.drawRect(ih, ih, result.width()-sub, result.height()-sub);
    }
    if (effectsConfiguration.getFrameAddAround()) {
        painter.drawImage(effectsConfiguration.getFrameWidth(),
                          effectsConfiguration.getFrameWidth(),
                          *img);
    }

    return result;
}

/** Draws text on #img image.
  * \sa addImage() image() setImage()
  */
void ConvertEffects::addText() {
    Q_ASSERT(img != NULL);
    Q_ASSERT(!img->isNull());

    // reference point setup
    QPoint point = getTransformOriginPoint(shared->effectsConfiguration().getTextPos(),
                                           shared->effectsConfiguration().getTextUnitPair());

    // text bounding rect setup
    QFontMetrics fontMetrics(shared->effectsConfiguration().getTextFont(), img);
    QRect rect = fontMetrics.boundingRect(shared->effectsConfiguration().getTextString());
    const int dx = 5;
    const int dy = 1;
    rect.adjust(-dx, -dy, dx, dy);
    rect = getEffectBoundingRect(rect, point, shared->effectsConfiguration().getTextPosModifier());

    QPainter painter(img);
    painter.setPen(shared->effectsConfiguration().getTextColor());
    painter.setFont(shared->effectsConfiguration().getTextFont());
    painter.setOpacity(shared->effectsConfiguration().getTextOpacity());

    // rotate painter around center point of image
    this->rotate(&painter, point, shared->effectsConfiguration().getTextRotation());

    // draw text
    painter.drawText(rect, Qt::AlignCenter, shared->effectsConfiguration().getTextString());
    if (shared->effectsConfiguration().getTextFrame()) {
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.drawRect(rect);
    }
}

/** Draws image on #img image.
  * \sa addText() image() setImage()
  */
void ConvertEffects::addImage() {
    Q_ASSERT(img != NULL);
    Q_ASSERT(!img->isNull());

    QPoint point = getTransformOriginPoint(effectsConfiguration.getImagePos(),
                                           effectsConfiguration.getImageUnitPair());

    QRect rect = getEffectBoundingRect(effectsConfiguration.getImage().rect(), point,
                                       effectsConfiguration.getImagePosModifier());

    QPainter painter(img);
    painter.setOpacity(effectsConfiguration.getImageOpacity());
    painter.resetTransform();

    this->rotate(&painter, point, effectsConfiguration.getImageRotation());

    painter.drawImage(rect, effectsConfiguration.getImage());
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
    if (position.x() < 0)
        result.setX(result.x() + img->width());
    if (position.y() < 0)
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

#ifdef SIR_TESTS
// for benchmark test only
void ConvertEffects::combineLoop(const QColor &color) {
    const int cr = color.red();
    const int cg = color.green();
    const int cb = color.blue();
    int max = qMax(cr, cg);
    max = qMax(max, cb);
    for (int y=0; y<img->height(); y++) {
        for (int x=0; x<img->width(); x++) {
            QRgb rgb = img->pixel(x,y);
            int r = cr * qRed(rgb) / max;
            int g = cg * qGreen(rgb) / max;
            int b = cb * qBlue(rgb) / max;
            img->setPixel(x, y, qRgb(r, g, b));
        }
    }
}
#endif // SIR_TESTS

void ConvertEffects::combine(const QColor &color) {
    QPainter painter(img);
    painter.setOpacity(0.5);
    painter.fillRect(img->rect(), color);
}

void ConvertEffects::combine(const QBrush &brush) {
    QPainter painter(img);
    painter.setOpacity(0.5);
    painter.fillRect(img->rect(), brush);
}

QPair<QColor, QColor> ConvertEffects::colorRange() {
    QPair<QColor, QColor> range(Qt::white, Qt::black);

    for (int x=0; x<img->width(); x++) {
        for (int y=0; y<img->height(); y++) {
            QRgb rgb = img->pixel(x,y);
            int v = qRed(rgb);
            if (v < range.first.red())
                range.first.setRed(v);
            else if (v > range.second.red())
                range.second.setRed(v);
            v = qGreen(rgb);
            if (v < range.first.green())
                range.first.setGreen(v);
            else if (v > range.second.green())
                range.second.setGreen(v);
            v = qBlue(rgb);
            if (v < range.first.blue())
                range.first.setBlue(v);
            else if (v > range.second.blue())
                range.second.setBlue(v);
        }
    }

    return range;
}

/** Create vector of image distribution function.
  * \return Distribution vector object.
  * \sa histogram() sumRgb()
  */
QVector<RgbF> ConvertEffects::distribution() {
    QVector<RgbF> D(256);
    const QVector<Rgb> &h = histogram();
    const int lp = img->width() * img->height();

    for (int n=0; n<D.size(); n++)
        D[n] = sumRgb(h, n) / lp;

    return D;
}

/** Counts pixel values.
  *
  * \p Data order
  * Index of vector is brightness and field of Rgb object is count pixel value
  * within a color channel.\n
  * For example h[2].red = 50 means red value 2 exist in image 50 times.
  *
  * \return Vector of count values.
  */
QVector<Rgb> ConvertEffects::histogram() {
    QVector<Rgb> h(256);

    for (int x=0; x<img->width(); x++) {
        for (int y=0; y<img->height(); y++) {
            QRgb rgb = img->pixel(x,y);
            int n = qRed(rgb);
            h[n].red++;
            n = qGreen(rgb);
            h[n].green++;
            n = qBlue(rgb);
            h[n].blue++;
        }
    }

    return h;
}

/** Sums first \a n values of \a h histogram.
  * \return Rgb object.
  */
Rgb ConvertEffects::sumRgb(const QVector<Rgb> &h, int n) {
    Rgb rgb;

    for (int u=0; u<n; u++)
        rgb += h[u];

    return rgb;
}

/** Create look up table (LUT) for histogram equalization.
  * \return Look up table vector object.
  * \sa distribution()
  */
QVector<Rgb> ConvertEffects::lookUpTable() {
    const int k = 256;
    QVector<Rgb> LUT(k);
    QVector<RgbF> D = distribution();
    RgbF D0 = D[0];

    // setting component colors of D0
    for (int n=1; D0.red == 0. && n<D.size(); n++)
        D0.red = D[n].red;
    for (int n=1; D0.green == 0. && n<D.size(); n++)
        D0.green = D[n].green;
    for (int n=1; D0.blue == 0. && n<D.size(); n++)
        D0.blue = D[n].blue;

    RgbF mul = (k - 1) / (1 - D0);
    for (int i=0; i<LUT.size(); i++) {
        LUT[i] = (D[i] - D0) * mul;
        LUT[i].normalize();
    }

    return LUT;
}
