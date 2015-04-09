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

#include "RawPixmapLoader.hpp"

#include <QProcess>


RawPixmapLoader::RawPixmapLoader(Settings::RawGroup *rawSettings)
    : RawLoader(rawSettings) {}

QPixmap *RawPixmapLoader::load(const QString &filePath) {
    // TODO: make filePath to be a field of the class
    return (isRawImage(filePath))
            ? loadFromRawFile(filePath)
            : loadFromNormalFile(filePath);
}

QPixmap *RawPixmapLoader::loadFromRawFile(const QString &filePath) {
    QPixmap *pixmap = new QPixmap();
    QProcess process;

    process.start(dcrawPath() + " -c " + filePath);

    // exitCode is 0 if dcraw was able to identify a raw image and 1 otherwise
    if (process.waitForFinished(-1) && process.exitCode() == 0) {
        pixmap->loadFromData(process.readAll(), "PPM");
    }

    return pixmap;
}

QPixmap *RawPixmapLoader::loadFromNormalFile(const QString &filePath) {
    QPixmap *pixmap = new QPixmap();
    pixmap->load(filePath);
    return pixmap;
}
