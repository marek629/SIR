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
 * Program URL: http://marek629.github.io/sir/
 */

#ifndef RGB_H
#define RGB_H

#include <QtGlobal>

class RgbF;

/** \brief Class for storage red, green and blue colors data for histogram
  *        operations and statistics.
  *
  * Rgb class was created for ConvertEffects class. Storages colors integer data.
  *
  * \sa RgbF
  */
class Rgb {
public:
    Rgb();
    Rgb(const Rgb &other);
    void normalize();
    Rgb & operator =(const RgbF &other);
    Rgb operator +(const Rgb &other);
    Rgb & operator +=(const Rgb &other);
    RgbF operator /(int div);
    int red;
    int green;
    int blue;
};

/** \brief Class for storage red, green and blue colors data for histogram
  *        operations and statistics.
  *
  * RgbF class was created for ConvertEffects class. Storages colors floating
  * point data.
  *
  * \sa Rgb
  */
class RgbF {
public:
    RgbF();
    RgbF(const RgbF &other);
    RgbF(const Rgb &other);
    void normalize();
    RgbF operator -(const RgbF &other);
    RgbF operator *(const RgbF &other);
    qreal red;
    qreal green;
    qreal blue;
};

RgbF operator -(int number, const RgbF &rgb);
RgbF operator /(int number, const RgbF &rgb);

#endif // RGB_H
