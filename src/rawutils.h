/*
 * This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto
 * 2011-2012  Marek Jędryka
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
 * Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
 *                 Marek Jędryka   <jedryka89@gmail.com>
 * Program URL: http://sir.projet-libre.org/
 *
 */

#ifndef RAWUTILS_H
#define RAWUTILS_H

class QImage;
class QString;
class QStringList;
class QPixmap;

//! Raw image tools class.
class  RawUtils {

public:
    static bool isRaw(QString imagePath);
    static QImage *loadRawImage(QString imagePath);
    static QPixmap *loadRawPixmap(QString imagePath);
    static void createRawFilesList(QStringList &rawFormats);
    static bool isRawEnabled();

private:
    RawUtils(const RawUtils &);
    RawUtils &operator=(const RawUtils &);
    static QString readDcrawPath();
    static QStringList readDcrawOptions();
};

#endif // RAWUTILS_H
