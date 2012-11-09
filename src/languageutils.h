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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
 *                 Marek Jędryka   <jedryka89@gmail.com>
 * Program URL: http://sir.projet-libre.org/
 *
 */

#ifndef LANGUAGEUTILS_H
#define LANGUAGEUTILS_H

#include <QString>
#include <QMap>

//! Struct containing user visible information about language.
struct LanguageInfo {
    QString niceName; /**< Language name. */
    QString flagFile; /**< National flag file name of language. */
};

//! Multi languages support tool class.
class LanguageUtils {
public:
    LanguageUtils();
    ~LanguageUtils();
    const LanguageInfo getLanguageInfo(const QString & language) const;
private:
    void readLanguages();
    /** Language information map. Key is compiled translation file name. */
    QMap<QString, LanguageInfo> *languageInfoMap;
};

#endif // LANGUAGEUTILS_H
