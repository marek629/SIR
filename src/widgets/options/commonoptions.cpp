/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
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

#include "commonoptions.h"
#include "settings.h"

/** Default constructor.\n
  * Sets default value of fields of common options object.
  */
CommonOptions::CommonOptions() {
    targetDirPath_ = QDir::homePath();
    maxHistoryCount_ = Settings::instance()->settings.maxHistoryCount;
}

CommonOptions * CommonOptions::instance() {
    static CommonOptions *object = 0;
    if (!object && Settings::instance())
        object = new CommonOptions();
    return object;
}

QString CommonOptions::targetDirPath() const {
    return targetDirPath_;
}

void CommonOptions::setTargetDirPath(const QString &path) {
    targetDirPath_ = (path.isEmpty()) ? QDir::homePath() : path;
}

int CommonOptions::maxHistoryCount() const {
    return maxHistoryCount_;
}

void CommonOptions::setMaxHistoryCount(int v) {
    maxHistoryCount_ = v;
}
