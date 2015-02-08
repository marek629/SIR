/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2015  Marek Jędryka   <jedryka89@gmail.com>
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

#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <QPair>


/** Position modificators enumerator for \em "Add Text" and \em "Add Image"
  * effects. It's correlated position combo boxes in EffectsScrollArea.
  * \sa PosUnit
  */
enum PosModifier {
    TopLeftCorner,
    MiddleTopEdge,
    TopRightCorner,
    Center,
    MiddleRightEdge,
    BottomRightCorner,
    MiddleBottomEdge,
    BottomLeftCorner,
    MiddleLeftEdge,
    UndefinedPosModifier = -1
};

/** Position unit enumerator for \em "Add Text" and \em "Add Image" effects.
  * It's correlated position combo boxes in EffectsScrollArea.
  * \sa PosUnitPair PosModifier
  */
enum PosUnit {
    Pixel,
    Percent,
    UndefinedUnit = -1
};

enum Filter {
    NoFilter,
    BlackAndWhite,
    Sepia,
    CustomColor,
    Gradient
};

/** Pair of two PosUnit: first item is X coordinate, second is Y coordinate. */
typedef QPair <PosUnit, PosUnit> PosUnitPair;

#endif // ENUMS_HPP
