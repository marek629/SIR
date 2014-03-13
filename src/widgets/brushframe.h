/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2014  Marek Jędryka   <jedryka89@gmail.com>
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

#ifndef BRUSHFRAME_H
#define BRUSHFRAME_H

#include "colorframe.h"

struct LinearGradientParams {
    QPointF start;
    QPointF finalStop;
};

struct RadialGradientParams {
    QPointF center;
    QPointF focalPoint;
    qreal radius;
};

struct ConicalGradientParams {
    qreal angle;
    QPointF center;
};

/** \brief The empty frame class providing easy way to direction customization
  * of gradient.
  *
  * This class provides functionality inherited from ColorFrame,
  * if brushEditable is false.
  *
  * Additional functionality is enabled if brushEditable is true. Then this class
  * features drawing gradient supporting gradient direction customization.
  * Access to customized gradient is available using brush() method.
  *
  * \sa setBrushEditable()
  */
class BrushFrame : public ColorFrame {
    friend class EffectsCollector;
    Q_OBJECT

public:
    explicit BrushFrame(QWidget * parent = 0);
    bool isBrushEditable() const;
    void setBrushEditable(bool editable);
    const QColor &color() const;
    void setColor(const QColor &color);
    const QBrush &brush() const;
    void setBrush(const QBrush &brush);
    void setGradientType(QGradient::Type type);
    void setGradientStops(const QGradientStops &stops);
    LinearGradientParams linearGradientParams() const;
    RadialGradientParams radialGradientParams() const;
    ConicalGradientParams conicalGradientParams() const;

public slots:
    void changeGradient();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

private:
    // fields
    /** Allows gradient customization.
      * \sa isBrushEditable() setBrushEditable()
      */
    bool brushEditable;
    /** Allows changing direction of gradient by mouse moving. */
    bool mouseLeftButtonPressed;
    /** Start point for gradient customization. */
    QPoint originPoint;
    QPointF originPointF;
    /** End point for gradient customization. */
    QPoint endPoint;
    QPointF endPointF;
    QGradient *gradient;
    LinearGradientParams linearGradientParams_;
    RadialGradientParams radialGradientParams_;
    ConicalGradientParams conicalGradientParams_;
    // methods
    void updateGradientParams();
    void setupLinearGradient(LinearGradientParams *params);
    void setupRadialGradient(RadialGradientParams *params);
    void setupConicalGradient(ConicalGradientParams *params);
};

#endif // BRUSHFRAME_H
