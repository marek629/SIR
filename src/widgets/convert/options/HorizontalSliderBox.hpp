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

#ifndef HORIZONTALSLIDERBOX_HPP
#define HORIZONTALSLIDERBOX_HPP

#include <QWidget>

class QSlider;
class QSpinBox;


/**
 * @brief The HorizontalSliderBox widget class
 *
 * Widget aggregated horizontal slider and integer-value spin box.
 */
class HorizontalSliderBox : public QWidget
{
public:
    explicit HorizontalSliderBox(QWidget *parent = NULL);
    ~HorizontalSliderBox();

    int minimum() const;
    int maximum() const;
    int value() const;

public slots:
    void setRange(int min, int max);
    void setValue(int value);

private:
    QSlider *createSlider();
    QSpinBox *createSpinBox();
    QLayout *createLayout(QSlider *slider, QSpinBox *spinBox);
    void connectSliderAndSpinBox();

    QSlider *slider;
    QSpinBox *spinBox;
    QLayout *layout;
};

#endif // HORIZONTALSLIDERBOX_HPP
