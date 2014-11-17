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

#include "Rgb.hpp"

Rgb::Rgb() {
    red = 0;
    green = 0;
    blue = 0;
}

Rgb::Rgb(const Rgb &other) {
    red = other.red;
    green = other.green;
    blue = other.blue;
}

void Rgb::normalize(int min, int max) {
    if (red < min)
        red = min;
    else if (red > max)
        red = max;

    if (green < min)
        green = min;
    else if (green > max)
        green = max;

    if (blue < min)
        blue = min;
    else if (blue > max)
        blue = max;
}

Rgb &Rgb::operator =(const RgbF &other) {
    red = int(other.red + 0.5);
    green = int(other.green + 0.5);
    blue = int(other.blue + 0.5);

    return *this;
}

Rgb Rgb::operator +(const Rgb &other) {
    Rgb result(*this);

    result.red += other.red;
    result.green += other.green;
    result.blue += other.blue;

    return result;
}

Rgb &Rgb::operator +=(const Rgb &other) {
    red += other.red;
    green += other.green;
    blue += other.blue;

    return *this;
}

RgbF Rgb::operator /(int div) {
    double d(div);
    RgbF result(*this);

    result.red /= d;
    result.green /= d;
    result.blue /= d;

    return result;
}


RgbF operator -(int number, const RgbF &rgb) {
    RgbF result;

    result.red = number - rgb.red;
    result.green = number - rgb.green;
    result.blue = number - rgb.blue;

    return result;
}


RgbF operator /(int number, const RgbF &rgb) {
    RgbF result;

    result.red = number / rgb.red;
    result.green = number / rgb.green;
    result.blue = number / rgb.blue;

    return result;
}


RgbF::RgbF() {
    red = 0.;
    green = 0.;
    blue = 0.;
}

RgbF::RgbF(const RgbF &other) {
    red = other.red;
    green = other.green;
    blue = other.blue;
}

RgbF::RgbF(const Rgb &other) {
    red = other.red;
    green = other.green;
    blue = other.blue;
}

void RgbF::normalize(qreal min, qreal max) {
    if (red < min)
        red = min;
    else if (red > max)
        red = max;

    if (green < min)
        green = min;
    else if (green > max)
        green = max;

    if (blue < min)
        blue = min;
    else if (blue > max)
        blue = max;
}

RgbF RgbF::operator -(const RgbF &other) {
    RgbF result(*this);

    result.red -= other.red;
    result.green -= other.green;
    result.blue -= other.blue;

    return result;
}

RgbF RgbF::operator *(const RgbF &other) {
    RgbF result(*this);

    result.red *= other.red;
    result.green *= other.green;
    result.blue *= other.blue;

    return result;
}
