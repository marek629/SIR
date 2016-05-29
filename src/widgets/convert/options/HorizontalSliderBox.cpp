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

#include "widgets/convert/options/HorizontalSliderBox.hpp"

#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>


HorizontalSliderBox::HorizontalSliderBox(QWidget *parent) : QWidget(parent)
{
    slider = createSlider();
    spinBox = createSpinBox();
    layout = createLayout(slider, spinBox);
    connectSliderAndSpinBox();
}

HorizontalSliderBox::~HorizontalSliderBox()
{
    delete slider;
    delete spinBox;
    delete layout;
}

int HorizontalSliderBox::minimum() const
{
    return slider->minimum();
}

int HorizontalSliderBox::maximum() const
{
    return slider->maximum();
}

int HorizontalSliderBox::value() const
{
    return slider->value();
}

void HorizontalSliderBox::setRange(int min, int max)
{
    slider->setRange(min, max);
    spinBox->setRange(min, max);
}

void HorizontalSliderBox::setValue(int value)
{
    slider->setValue(value);
    spinBox->setValue(value);
}

QSlider *HorizontalSliderBox::createSlider()
{
    return new QSlider(Qt::Horizontal, this);
}

QSpinBox *HorizontalSliderBox::createSpinBox()
{
    return new QSpinBox(this);
}

QLayout *HorizontalSliderBox::createLayout(QSlider *slider, QSpinBox *spinBox)
{
    QLayout *result = new QHBoxLayout(this);
    result->addWidget(slider);
    result->addWidget(spinBox);
    return result;
}

void HorizontalSliderBox::connectSliderAndSpinBox()
{
    connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
    connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
}
