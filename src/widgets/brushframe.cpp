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

#include <QMouseEvent>
#include <QPainter>
#include "brushframe.h"
#include "gradienteditwidget.h"

/** Creates BrushFrame object. */
BrushFrame::BrushFrame(QWidget *parent) : ColorFrame(parent) {
    brushEditable = true;
    mouseLeftButtonPressed = false;
    gradient = 0;
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

/** Returns current brush.
  * \sa setBrush()
  */
const QBrush &BrushFrame::brush() {
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

void BrushFrame::setGradientType(QGradient::Type type) {
    static LinearGradientParams lastLinearGradientParams;
    static RadialGradientParams lastRadialGradientParams;
    static ConicalGradientParams lastConicalGradientParams;

    QGradientStops stops;
    if (gradient) {
        stops = gradient->stops();
        switch (gradient->type()) {
        case QGradient::LinearGradient: {
            QLinearGradient *lg = static_cast<QLinearGradient*>(gradient);
            lastLinearGradientParams.start = lg->start();
            lastLinearGradientParams.finalStop = lg->finalStop();
            break;
        }
        case QGradient::RadialGradient: {
            QRadialGradient *rg = static_cast<QRadialGradient*>(gradient);
            lastRadialGradientParams.center = rg->center();
            lastRadialGradientParams.focalPoint = rg->focalPoint();
            lastRadialGradientParams.radius = rg->radius();
            break;
        }
        case QGradient::ConicalGradient: {
            QConicalGradient *cg = static_cast<QConicalGradient*>(gradient);
            lastConicalGradientParams.angle = cg->angle();
            lastConicalGradientParams.center = cg->center();
            break;
        }
        default:
            break;
        }
    }

    delete gradient;

    switch (type) {
    case QGradient::LinearGradient: {
        QLinearGradient *lg = new QLinearGradient();
        lg->setStart(lastLinearGradientParams.start);
        lg->setFinalStop(lastLinearGradientParams.finalStop);
        gradient = lg;
        break;
    }
    case QGradient::RadialGradient: {
        QRadialGradient *rg = new QRadialGradient();
        rg->setCenter(lastRadialGradientParams.center);
        rg->setRadius(lastRadialGradientParams.radius);
        rg->setFocalPoint(lastRadialGradientParams.focalPoint);
        gradient = rg;
        break;
    }
    case QGradient::ConicalGradient: {
        QConicalGradient *cg = new QConicalGradient();
        cg->setCenter(lastConicalGradientParams.center);
        cg->setAngle(lastConicalGradientParams.angle);
        gradient = cg;
        break;
    }
    default:
        gradient = 0;
        break;
    }

    if (gradient)
        setGradientStops(stops);
}

void BrushFrame::setGradientStops(const QGradientStops &stops) {
    Q_ASSERT(gradient);
    gradient->setStops(stops);
    setBrush(QBrush(*gradient));
    update();
}

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
    if (e->button() == Qt::LeftButton) {
        mouseLeftButtonPressed = true;
        originPoint = e->pos();
    }
}

/** Updates gradient customization if left mouse button was pressed.
  * \sa paintEvent() mousePressEvent() mouseReleaseEvent()
  */
void BrushFrame::mouseMoveEvent(QMouseEvent *e) {
    if (!mouseLeftButtonPressed || !gradient)
        return;

    endPoint = e->pos();

    switch (gradient->type()) {
    case QGradient::LinearGradient: {
        QLinearGradient *lg = static_cast<QLinearGradient*>(gradient);
        lg->setStart(originPoint);
        lg->setFinalStop(endPoint);
        break;
    }
    case QGradient::RadialGradient: {
        QRadialGradient *rg = static_cast<QRadialGradient*>(gradient);
        rg->setCenter(originPoint);
        rg->setFocalPoint(endPoint);
        rg->setRadius(qMin(width(), height()));
        break;
    }
    case QGradient::ConicalGradient: {
        QConicalGradient *cg = static_cast<QConicalGradient*>(gradient);
        QLineF line(originPoint, endPoint);
        cg->setCenter(originPoint);
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
