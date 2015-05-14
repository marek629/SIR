/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2015  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://marek629.github.io/SIR/
 */

#include "raw/RawControllerTest.hpp"

#include "raw/RawController.hpp"
#include "raw/RawModelRuntime.hpp"
#include "raw/RawView.hpp"
#include "widgets/MessageBox.hpp"

#include <QDir>


RawControllerTest::RawControllerTest()
{
    MessageBox::enableTesting(true);

    RawModel *model = new RawModelRuntime(true, "/usr/bin/dcraw", "");
    view = new RawView();
    controller = new RawController(model, view, this);
}

RawControllerTest::~RawControllerTest()
{
    delete view;
    delete controller;
}

void RawControllerTest::initTestCase() {}

void RawControllerTest::cleanupTestCase() {}

void RawControllerTest::checkDcrawPath_emptyString()
{
    QString input("");

    bool expected = false;
    bool result = controller->checkDcrawPath(input);

    QCOMPARE(result, expected);
}

void RawControllerTest::checkDcrawPath_fileNotExists()
{
    QString input("/test_dcraw.bin");
    input.prepend(QDir::tempPath());

    QFile file(input);
    QVERIFY(!file.exists());

    bool expected = false;
    bool result = controller->checkDcrawPath(input);

    QCOMPARE(result, expected);
}

void RawControllerTest::checkDcrawPath_fileNotExecutable()
{
    QString input("/test_dcraw.bin");
    input.prepend(QDir::tempPath());

    QFile file(input);
    QVERIFY(!file.exists());
    file.open(QFile::ReadWrite);
    file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
    QVERIFY(file.exists());

    bool expected = false;
    bool result = controller->checkDcrawPath(input);

    QCOMPARE(result, expected);

    QVERIFY(file.remove());
}

void RawControllerTest::checkDcrawPath_fileExecutable()
{
    QString input("/test_dcraw.bin");
    input.prepend(QDir::tempPath());

    QFile file(input);
    QVERIFY(!file.exists());
    file.open(QFile::ReadWrite);
    file.setPermissions(QFile::ExeOwner);
    QVERIFY(file.exists());

    bool expected = true;
    bool result = controller->checkDcrawPath(input);

    QCOMPARE(result, expected);

    QVERIFY(file.remove());
}


QTEST_MAIN(RawControllerTest)
#include "RawControllerTest.moc"
