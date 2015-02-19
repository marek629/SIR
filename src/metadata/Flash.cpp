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
 * Program URL: http://marek629.github.io/SIR/
 */

#include "Flash.hpp"

using namespace MetadataUtils;

/** Returns translated string containing information about the lack of flash
  * function.
  */
QString Flash::noFlashFunction() {
    return tr("No flash function.");
}

/** Returns translated string containing information about flash fired.
  * \sa noFired
  */
QString Flash::fired() {
    return tr("Flash fired");
}

/** Returns translated string containing information about flash didn't fire.
  * \sa fired
  */
QString Flash::noFired() {
    return tr("Flash didn't fire");
}

/** Returns translated string containing information about flash fired in
  * compulsory mode.
  */
QString Flash::compulsoryMode() {
    return tr(", compulsory flash mode");
}

/** Returns translated string containing information about flash fired in
  * auto mode.
  */
QString Flash::autoMode() {
    return tr(", auto mode");
}

/** Returns translated string containing information about flash fired in
  * red-eye reduction mode. */
QString Flash::redEyeReduction() {
    return tr(", red-eye reduction");
}

/** Returns translated string containing information about detection of
  * returning strobe ligth.
  * \sa strobeReturnNotDetected
  */
QString Flash::strobeReturnDetected() {
    return tr(", strobe return light detected");
}

/** Returns translated string containing information about failed to
  * detection of returning strobe ligth.
  * \sa strobeReturnDetected
  */
QString Flash::strobeReturnNotDetected() {
    return tr(", strobe return light not detected");
}
