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

#include "rawgroupboxtest.h"

RawGroupBoxTest::RawGroupBoxTest() {}

void RawGroupBoxTest::initTestCase()
{

}

void RawGroupBoxTest::cleanupTestCase()
{

}

void RawGroupBoxTest::checkDcrawPath_emptyString() {
    QString input("");

    bool expected = false;
    bool result = groupBox.checkDcrawPath(input);

    QCOMPARE(result, expected);
}

void RawGroupBoxTest::checkDcrawPath_fileNotExists()
{

}

void RawGroupBoxTest::checkDcrawPath_fileNotExecutable()
{

}

void RawGroupBoxTest::checkDcrawPath_fileExecutable()
{

}
