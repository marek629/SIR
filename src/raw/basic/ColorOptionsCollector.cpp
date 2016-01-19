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

#include "raw/basic/ColorOptionsCollector.hpp"

#include "raw/basic/BasicRawScrollAreaAdapter.hpp"


ColorOptionsCollector::ColorOptionsCollector(BasicOptionsCollector *collector,
                                             BasicRawScrollAreaAdapter *view)
    : CollectorDecorator(collector, view) {}

ColorOptionsCollector::~ColorOptionsCollector() {}

QString ColorOptionsCollector::optionsString() const
{
    QString result = decoratedCollector->optionsString();

    if (view->isWhiteBalanceFromCameraChecked()) {
        result = QString("%1 -w").arg(result);
    }

    if (view->isWhiteBalanceFromWholeImageChecked()) {
        result = QString("%1 -a").arg(result);
    }

    QChar matrixSign = view->isColorMatrixFromCameraChecked() ? '+' : '-';
    result = QString("%1 %2M").arg(result).arg(matrixSign);

    return result;
}

void ColorOptionsCollector::setOptions(const QString &string)
{
    decoratedCollector->setOptions(string);

    view->setWhiteBalanceFromCameraChecked(string.contains("-w"));

    view->setWhiteBalanceFromWholeImageChecked(string.contains("-a"));

    view->setColorMatrixFromCameraChecked(string.contains("+M"));
}
