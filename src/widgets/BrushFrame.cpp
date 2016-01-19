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

#include "widgets/BrushFrame.hpp"

#include "widgets/GradientEditWidget.hpp"

#include <QMouseEvent>
#include <QPainter>


BrushFrame::BrushFrame(QWidget *parent) : ColorFrame(parent)
{
    brushEditable = true;
    mouseLeftButtonPressed = false;
    gradient = 0;

    setupLinearGradient(&linearGradientParams_);
    setupRadialGradient(&radialGradientParams_);
    setupConicalGradient(&conicalGradientParams_);
}

BrushFrame::~BrushFrame()
{
    delete gradient;
}

/** Returns true if gradient edition is allowed, otherwise false.
  * \sa setBrushEditable()
  */
bool BrushFrame::isBrushEditable() const {
    return brushEditable;
}

/** Allows gradient edition if \a editable is true, otherwise disallows.
  * \sa setBrushEditable()
  */
void BrushFrame::setBrushEditable(bool editable) {
    brushEditable = editable;
}

const QColor &BrushFrame::color() const {
    return this->brush().color();
}

void BrushFrame::setColor(const QColor &color) {
    setBrush(QBrush(color));
}

/** Returns current brush.
  * \sa setBrush()
  */
const QBrush &BrushFrame::brush() const {
    return palette().brush(QPalette::Window);
}

/** Sets brush to \a brush.
  * \sa brush()
  */
void BrushFrame::setBrush(const QBrush &brush) {
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Window, brush);
    setPalette(palette);
}

/** Sets type of brush gradient to \a type value. */
void BrushFrame::setGradientType(QGradient::Type type) {
    QGradientStops stops;
    if (gradient)
        stops = gradient->stops();

    delete gradient;

    switch (type) {
    case QGradient::LinearGradient: {
        QLinearGradient *lg = new QLinearGradient();
        lg->setStart(linearGradientParams_.start);
        lg->setFinalStop(linearGradientParams_.finalStop);
        gradient = lg;
        break;
    }
    case QGradient::RadialGradient: {
        QRadialGradient *rg = new QRadialGradient();
        rg->setCenter(radialGradientParams_.center);
        rg->setRadius(radialGradientParams_.radius);
        rg->setFocalPoint(radialGradientParams_.focalPoint);
        gradient = rg;
        break;
    }
    case QGradient::ConicalGradient: {
        QConicalGradient *cg = new QConicalGradient();
        cg->setCenter(conicalGradientParams_.center);
        cg->setAngle(conicalGradientParams_.angle);
        gradient = cg;
        break;
    }
    default:
        gradient = 0;
        break;
    }

    if (gradient) {
        gradient->setCoordinateMode(QGradient::ObjectBoundingMode);
        setGradientStops(stops);
    }
}

/** Sets stops of brush gradient to \a stops content. */
void BrushFrame::setGradientStops(const QGradientStops &stops) {
    Q_ASSERT(gradient);
    gradient->setStops(stops);
    setBrush(QBrush(*gradient));
    update();
}

/** Returns parameters of linear gradient.
  * \sa radialGradientParams() conicalGradientParams()
  */
LinearGradientParams BrushFrame::linearGradientParams() const {
    return linearGradientParams_;
}

/** Returns parameters of radial gradient.
  * \sa linearGradientParams() conicalGradientParams()
  */
RadialGradientParams BrushFrame::radialGradientParams() const {
    return radialGradientParams_;
}

/** Returns parameters of conical gradient.
  * \sa linearGradientParams() radialGradientParams()
  */
ConicalGradientParams BrushFrame::conicalGradientParams() const {
    return conicalGradientParams_;
}

/** Changes stops of current brush gradient.
  *
  * \note This function works as slot for signal received from
  *       GradientEditWidget only.
  */
void BrushFrame::changeGradient() {
    Q_ASSERT(sender());
    if (!gradient)
        return;
    GradientEditWidget *editor = dynamic_cast<GradientEditWidget*>(sender());
    setGradientStops(editor->gradientStops());
}

/** Starts gradient customization if left mouse button was pressed.
  * \sa mouseMoveEvent() mouseReleaseEvent()
  */
void BrushFrame::mousePressEvent(QMouseEvent *e) {
    if (!brushEditable)
        return;

    if (e->button() == Qt::LeftButton) {
        mouseLeftButtonPressed = true;
        originPoint = e->pos();
        originPointF = originPoint;
        originPointF.setX(originPointF.x()/width());
        originPointF.setY(originPointF.y()/height());
    }
}

/** Updates gradient customization if left mouse button was pressed.
  * \sa paintEvent() mousePressEvent() mouseReleaseEvent()
  */
void BrushFrame::mouseMoveEvent(QMouseEvent *e) {
    if (!brushEditable || !mouseLeftButtonPressed || !gradient)
        return;

    endPoint = e->pos();
    endPointF = endPoint;
    endPointF.setX(endPointF.x()/width());
    endPointF.setY(endPointF.y()/height());

    switch (gradient->type()) {
    case QGradient::LinearGradient: {
        QLinearGradient *lg = static_cast<QLinearGradient*>(gradient);
        lg->setStart(originPointF);
        lg->setFinalStop(endPointF);
        break;
    }
    case QGradient::RadialGradient: {
        QRadialGradient *rg = static_cast<QRadialGradient*>(gradient);
        rg->setCenter(originPointF);
        rg->setFocalPoint(endPointF);
        rg->setRadius(0.8);
        break;
    }
    case QGradient::ConicalGradient: {
        QConicalGradient *cg = static_cast<QConicalGradient*>(gradient);
        QLineF line(originPointF, endPointF);
        cg->setCenter(originPointF);
        cg->setAngle(line.angle());
        break;
    }
    default:
        break;
    }

    setBrush(QBrush(*gradient));
    update();
}

/** Ends gradient customization if left mouse button was pressed.
  * \sa mouseMoveEvent() mousePressEvent()
  */
void BrushFrame::mouseReleaseEvent(QMouseEvent *e) {
    if (!brushEditable) {
        ColorFrame::mouseReleaseEvent(e);
        return;
    }

    if (e->button() == Qt::LeftButton) {
        mouseLeftButtonPressed = false;
        updateGradientParams();
        update();
    }
}

/** Paints live preview of gradient customization.
  * \sa mouseMoveEvent()
  */
void BrushFrame::paintEvent(QPaintEvent *e) {
    ColorFrame::paintEvent(e);
    if (!brushEditable || !mouseLeftButtonPressed)
        return;

    // drawing mouse radius vector
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    // draw origin cross
    const int d = 3;
    painter.drawLine(originPoint.x(), originPoint.y()-d, originPoint.x(), originPoint.y()+d);
    painter.drawLine(originPoint.x()-d, originPoint.y(), originPoint.x()+d, originPoint.y());
    // draw radius vector
    painter.drawLine(originPoint, endPoint);
}

/** Updates internal cache parameters of current brush gradient. */
void BrushFrame::updateGradientParams() {
    if (!gradient)
        return;

    switch (gradient->type()) {
    case QGradient::LinearGradient: {
        QLinearGradient *lg = static_cast<QLinearGradient*>(gradient);
        linearGradientParams_.start = lg->start();
        linearGradientParams_.finalStop = lg->finalStop();
        break;
    }
    case QGradient::RadialGradient: {
        QRadialGradient *rg = static_cast<QRadialGradient*>(gradient);
        radialGradientParams_.center = rg->center();
        radialGradientParams_.focalPoint = rg->focalPoint();
        radialGradientParams_.radius = rg->radius();
        break;
    }
    case QGradient::ConicalGradient: {
        QConicalGradient *cg = static_cast<QConicalGradient*>(gradient);
        conicalGradientParams_.angle = cg->angle();
        conicalGradientParams_.center = cg->center();
        break;
    }
    default:
        break;
    }
}

void BrushFrame::setupLinearGradient(LinearGradientParams *params) {
    params->finalStop.setX(1.0);
}

void BrushFrame::setupRadialGradient(RadialGradientParams *params) {
    params->center.setX(0.5);
    params->center.setY(0.5);
    params->focalPoint.setX(0.5);
    params->focalPoint.setY(0.5);
    params->radius = 0.5;
}

void BrushFrame::setupConicalGradient(ConicalGradientParams *params) {
    params->angle = 0.;
    params->center.setX(0.5);
    params->center.setY(0.5);
}
