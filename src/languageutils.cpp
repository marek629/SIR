/*
* This file is part of SIR, an open-source cross-platform Image tool
* 2007  Rafael Sachetto
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
* Program URL: http://sir.projet-libre.org/
*
*/

#include "languageutils.h"
#include <QStringList>
#include <QDebug>
#include <QFile>

LanguageUtils::LanguageUtils() {

    this->readLanguages();

}

LanguageUtils::~LanguageUtils() {

    delete languageInfoMap;

}

void LanguageUtils::readLanguages() {

    /*
        Here we read and create a map to hold the informations about
        Language names
    */

    languageInfoMap = new QMap<QString, LanguageInfo>;

    QFile translationsFile(":translations/translation_info.txt");

    if (!translationsFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&translationsFile);

    //We have to initialize en language by hand

    LanguageInfo info;
    info.niceName = "English";
    info.flagFile = "english.png";

    languageInfoMap->insert("sir_en_US.qm", info);

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

const LanguageInfo LanguageUtils::getLanguageInfo(const QString & language) const {
    return languageInfoMap->value(language);
}
