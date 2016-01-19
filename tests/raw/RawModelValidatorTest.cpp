/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2016  Marek Jędryka   <jedryka89@gmail.com>
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

#include "tests/raw/RawModelValidatorTest.hpp"

#include "raw/RawModel.hpp"
#include "raw/RawModelValidator.hpp"


RawModelValidatorTest::RawModelValidatorTest(QObject *parent) : QObject(parent) {}

RawModelValidatorTest::~RawModelValidatorTest() {}

void RawModelValidatorTest::initTestCase() {}

void RawModelValidatorTest::cleanupTestCase() {}

void RawModelValidatorTest::test_isValidModel_disabled()
{
    bool isEnabled = false;
    QString dcrawPath = "nevermind";
    QString dcrawOptions = "nevermind";

    const RawModel &model = RawModel(isEnabled, dcrawPath, dcrawOptions);

    QVERIFY(model.isValid() == true);
}

void RawModelValidatorTest::test_isValidModel_enabled_emptyDcrawPath()
{
    bool isEnabled = true;
    QString dcrawPath = "";
    QString dcrawOptions = "nevermind";

    const RawModel &model = RawModel(isEnabled, dcrawPath, dcrawOptions);

    QVERIFY(model.isValid() == false);
}

void RawModelValidatorTest::test_isValidModel_enabled_nullDcrawPath()
{
    bool isEnabled = true;
    QString dcrawPath;
    QString dcrawOptions = "nevermind";

    const RawModel &model = RawModel(isEnabled, dcrawPath, dcrawOptions);

    QVERIFY(model.isValid() == false);
}

void RawModelValidatorTest::test_isValidModel_enabled_filledDcrawPath()
{
    bool isEnabled = true;
    QString dcrawPath = "/test/path";
    QString dcrawOptions = "nevermind";

    const RawModel &model = RawModel(isEnabled, dcrawPath, dcrawOptions);

    QVERIFY(model.isValid() == true);
}

void RawModelValidatorTest::test_isValidModel_enabled_emptyDcrawOptions()
{
    bool isEnabled = true;
    QString dcrawPath = "nevermind";
    QString dcrawOptions = "";

    const RawModel &model = RawModel(isEnabled, dcrawPath, dcrawOptions);

    QVERIFY(model.isValid() == true);
}

void RawModelValidatorTest::test_isValidModel_enabled_nullDcrawOptions()
{
    bool isEnabled = true;
    QString dcrawPath = "nevermind";
    QString dcrawOptions;

    const RawModel &model = RawModel(isEnabled, dcrawPath, dcrawOptions);

    QVERIFY(model.isValid() == true);
}

void RawModelValidatorTest::test_isValidModel_enabled_filledDcrawOptions()
{
    bool isEnabled = true;
    QString dcrawPath = "nevermind";
    QString dcrawOptions = "--test-options";

    const RawModel &model = RawModel(isEnabled, dcrawPath, dcrawOptions);

    QVERIFY(model.isValid() == true);
}


QTEST_MAIN(RawModelValidatorTest)
#include "RawModelValidatorTest.moc"
