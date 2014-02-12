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

#include <QMouseEvent>
#include <QApplication>
#include "colorframe.h"

/** Creates ColorFrame object. */
ColorFrame::ColorFrame(QWidget *parent) : QFrame(parent) {
    setAutoFillBackground(true);
}

/** Returns current color. */
const QColor & ColorFrame::color() const {
    return palette().color(QPalette::Window);
}

/** Sets background color to \a color. */
void ColorFrame::setColor(const QColor &color) {
    QPalette palette(this->palette());
    palette.setColor(QPalette::Window, color);
    setPalette(palette);
    dialogFlags = QColorDialog::ShowAlphaChannel;
}

QColorDialog::ColorDialogOptions ColorFrame::colorDialogOptions() const {
    return dialogFlags;
}

void ColorFrame::setColorDialogOptions(QColorDialog::ColorDialogOptions options) {
    dialogFlags = options;
}

/** Means end of left button mouse click. If click finished on this object shows
  * color dialog and sets choosed color to current color; otherwise does nothing.
  * \sa color()
  */
void ColorFrame::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton && qApp->widgetAt(e->globalPos()) == this) {
        QColor color = QColorDialog::getColor(this->color(), this,
                                              tr("Choose Color"),
                                              dialogFlags);
        if (color.isValid())
            setColor(color);
    }
}
