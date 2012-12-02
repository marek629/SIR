/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2012  Marek Jędryka   <jedryka89@gmail.com>
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

#ifndef CONVERTSETTINGS_H
#define CONVERTSETTINGS_H

#include <QString>

//! Shared data class of ConvertDialog dialog.
class ConvertSharedData {
public:
    // singleton instance
    static ConvertSharedData *instance();
    // fields
    QString fileFilters;
    float sizeWidth;
    float sizeHeight;
    QString dateFormat;
    QString timeFormat;
    QString dateTimeFormat;

private:
    ConvertSharedData();
};

#endif // CONVERTSETTINGS_H
