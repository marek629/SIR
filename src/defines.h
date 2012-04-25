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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Program URL: http://sir.projet-libre.org/
 */

#include <QtGlobal>

#define VERSION "2.3"
#define NOTCONVERTED 1
#define CONVERTED 2
#define SKIPPED 3
#define FAILED 4
#define REMOVED 5
#define CONVERTING 6
#define CANCELLED 7

#ifndef QT_TRANSLATIONS_DIR
    #ifdef Q_OS_WIN32
        // Qt translations included in application.qrc file needed
        #define QT_TRANSLATIONS_DIR ":/translations/"
    #else
        #define QT_TRANSLATIONS_DIR "/usr/share/qt/translations/"
    #endif // Q_OS_WIN32
#endif // QT_TRANSLATIONS_DIR
