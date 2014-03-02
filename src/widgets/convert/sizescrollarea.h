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

#ifndef SIZESCROLLAREA_H
#define SIZESCROLLAREA_H

#include "ui_sizescrollarea.h"

//! Image size options scroll area used in ConvertDialog
class SizeScrollArea : public QScrollArea, public Ui::SizeScrollArea {
    Q_OBJECT

public:
    explicit SizeScrollArea(QWidget *parent = 0);
    void retranslateStrings();

private:
    // fields
    double aspectRatio;
    // methods
    inline void connectSizeLinesEdit();
    inline void disconnectSizeLinesEdit();

public slots:
    void setSizeUnit(int index);

private slots:
    void sizeChanged(double value);
    void maintainCheckBoxChecked(bool keepAspect);
};

/** Connects width and height lines edit's textChanged() signal to sizeChanged()
  * slot.
  * \sa disconnectSizeLinesEdit()
  */
void SizeScrollArea::connectSizeLinesEdit() {
    connect(widthDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(sizeChanged(double)));
    connect(heightDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(sizeChanged(double)));
}

/** Disconnects sizeChanged() slot.
  * \sa connectSizeLinesEdit()
  */
void SizeScrollArea::disconnectSizeLinesEdit() {
    widthDoubleSpinBox->disconnect(this, SLOT(sizeChanged(double)));
    heightDoubleSpinBox->disconnect(this, SLOT(sizeChanged(double)));
}

#endif // SIZESCROLLAREA_H
