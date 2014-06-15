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
 * Program URL: http://marek629.github.io/sir/
 */

#include "versiontest.h"

VersionTest::VersionTest() : ver(VERSION) {}

void VersionTest::initTestCase() {
    QCOMPARE(ver.versionVector, ver.vector(ver.versionString));
}

void VersionTest::version() {
    QCOMPARE(ver.version(), QString(VERSION));
}

void VersionTest::vector_data() {
    QVector<int> vec;

    QTest::addColumn< QString >("string");
    QTest::addColumn< QVector<int> >("vector");

    vec.resize(2);
    vec[0] = 2;
    vec[1] = 5;
    QTest::newRow("2 numbers - stable") << "2.5" << vec;

    vec.resize(3);
    vec[0] = 2;
    vec[1] = 5;
    vec[2] = 1;
    QTest::newRow("3 numbers - stable") << "2.5.1" << vec;

    vec[0] = 2;
    vec[1] = 5;
    vec[2] = -2;
    QTest::newRow("2 numbers - alpha") << "2.5-alpha" << vec;

    vec[0] = 2;
    vec[1] = 5;
    vec[2] = -1;
    QTest::newRow("2 numbers - beta") << "2.5-beta" << vec;

    vec[0] = 2;
    vec[1] = 5;
    vec[2] = -3;
    QTest::newRow("2 numbers - development") << "2.5-dev" << vec;

    vec[0] = 2;
    vec[1] = 5;
    vec[2] = -4;
    QTest::newRow("2 numbers - unsupported unstable") << "2.5-a" << vec;
}

void VersionTest::vector() {
    QFETCH(QString, string);
    QFETCH(QVector<int>, vector);

    QCOMPARE(ver.vector(string), vector);
}

void VersionTest::isAtLeast_data() {
    QTest::addColumn< QString >("string");
    QTest::addColumn< bool >("boolean");

    QTest::newRow("next serries") << "3.4.5" << true;
    QTest::newRow("next serries origin") << "3.0" << true;
    QTest::newRow("more") << "2.5" << true;
    QTest::newRow("a bit more") << "2.3.1" << true;
    QTest::newRow("equal") << "2.3" << true;
    QTest::newRow("in fact equal") << "2.3.0" << true;
    QTest::newRow("almost equal") << "2.3.0-alpha" << false;
    QTest::newRow("less") << "2.2.2" << false;
    QTest::newRow("same serries origin") << "2" << false;
    QTest::newRow("previous serries") << "1.8.1" << false;
}

void VersionTest::isAtLeast() {
    QFETCH(QString, string);
    QFETCH(bool, boolean);

    QCOMPARE(ver.isAtLeast(string), boolean);
}

QTEST_APPLESS_MAIN(VersionTest)
#include "versiontest.moc"
