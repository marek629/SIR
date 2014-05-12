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

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>
#include <QTranslator>
#include "languageutils.h"
#include "settings.h"

/** Returns singledon instance pointer.
  * \sa LanguageUtils()
  */
LanguageUtils * LanguageUtils::instance() {
    static LanguageUtils *object = 0;
    if (!object && QCoreApplication::instance())
        object = new LanguageUtils();
    return object;
}

/** Default constructor.\n
  * Reads informations about languages and loads into languageInfoMap language
  * information map.
  * \sa LanguageInfo readLanguages()
  */
LanguageUtils::LanguageUtils() {
    qtTranslator = new QTranslator;
    appTranslator = new QTranslator;
    this->readLanguages();
}

/** Deallocates memory. */
LanguageUtils::~LanguageUtils() {
    delete qtTranslator;
    delete appTranslator;
    delete languageInfoMap;
}

/** Reads translation info file and creates languageInfoMap language information
  * map.
  * \sa languageInfoMap
  */
void LanguageUtils::readLanguages() {

    /*
        Here we read and create a map to hold the informations about
        Language names
    */

    languageInfoMap = new QMap<QString, LanguageInfo>;

    QFile translationsFile(QCoreApplication::applicationDirPath() +
                           "/../share/sir/translations/translation_info.csv");

    if (!translationsFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&translationsFile);

    while (!in.atEnd()) {

        QString line = in.readLine();

        /*Each line has the format:
          translation_file,Language Name,flag file
        */
        QStringList languageData = line.split(",");

        LanguageInfo info;
        info.niceName = languageData[1];
        info.flagFile = languageData[2];

        languageInfoMap->insert(languageData[0], info);

    }
}

/** Returns LanguageInfo about typed \a qmFile name. */
const LanguageInfo LanguageUtils::languageInfo(const QString &qmFile) const {
    LanguageInfo info = languageInfoMap->value(qmFile);

    if (info.niceName.isEmpty()) {
        QStringList list = qmFile.split('_');
        if (list.length() == 3) {
            QString fileName = list[0] + '_' + list[1] + ".qm";
            info = languageInfoMap->value(fileName);
        }
    }

    return info;
}

/** Returns name of file corresponding \a lang language symbol, i.e. en for
  * english.\n
  * If \a lang is null string returns translation file name from Settings.
  * \sa languageInfoMap
  */
QString LanguageUtils::fileName(const QString &lang) const {
    if (lang.isNull())
        return Settings::instance()->settings.languageFileName;
    QStringList qmFiles = fileNames();
    QStringList translations = qmFiles.filter(lang);
    if (translations.isEmpty())
        translations = qmFiles.filter(lang.section('_',0,0));
    if (translations.isEmpty()) {
        qDebug() << QString("Translation file for %1 language symbol not found.\n"
                            "Loading english translation.").arg(lang);
        return "sir_en.qm";
    }
    return translations[0];
}
