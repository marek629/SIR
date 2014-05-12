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
 * Program URL: http://sir.projet-libre.org/
 */

#ifndef LANGUAGEUTILS_H
#define LANGUAGEUTILS_H

#include <QStringList>
#include <QMap>

class QTranslator;

//! Struct containing user visible information about language.
struct LanguageInfo {
    QString niceName; /**< Language name. */
    QString flagFile; /**< National flag file name of language. */
};

//! Multi languages support tool class.
class LanguageUtils {
    friend class LanguageUtilsTest;

public:
    static LanguageUtils *instance();
    const LanguageInfo languageInfo(const QString &qmFile) const;
    QString fileName(const QString &lang) const;
    /** Returns list of compiled translation file names (*.qm).
      * \sa languageInfoMap
      */
    QStringList fileNames() const { return languageInfoMap->keys(); }
    QTranslator *qtTranslator; /**< Translator of standard Qt objects. */
    QTranslator *appTranslator; /**< Translator of SIR objects. */

private:
    LanguageUtils();
    ~LanguageUtils();
    void readLanguages();
    /** Language information map. Key is compiled translation file name. */
    QMap<QString, LanguageInfo> *languageInfoMap;
};

#endif // LANGUAGEUTILS_H
