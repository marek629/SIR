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

#include "languageutilstest.h"

LanguageUtilsTest::LanguageUtilsTest() {
    utils = LanguageUtils::instance();
}

void LanguageUtilsTest::initTestCase() {
    QVERIFY2(utils->fileNames().count() == 0,
             "Clean test environment is needed. "
             "Please move '../share' directory to another path.");
}

void LanguageUtilsTest::languageInfo_validKey() {
    QString qmFileName = "sir_pl.qm";
    LanguageInfo info;
    info.flagFile = "polish.png";
    info.niceName = "Polish";
    utils->languageInfoMap->insert(qmFileName, info);

    LanguageInfo result = utils->languageInfo(qmFileName);

    QCOMPARE(result.flagFile, info.flagFile);
    QCOMPARE(result.niceName, info.niceName);
}

void LanguageUtilsTest::languageInfo_invalidKey() {
    LanguageInfo result = utils->languageInfo("sir.qm");
    QString expectedString = "";

    QCOMPARE(result.flagFile, expectedString);
    QCOMPARE(result.niceName, expectedString);
}

void LanguageUtilsTest::languageInfo_tooDatailedKey() {
    LanguageInfo result = utils->languageInfo("sir_pl_PL.qm");
    LanguageInfo expected = utils->languageInfo("sir_pl.qm");

    QCOMPARE(result.flagFile, expected.flagFile);
    QCOMPARE(result.niceName, expected.niceName);
}

QTEST_MAIN(LanguageUtilsTest)
#include "languageutilstest.moc"
