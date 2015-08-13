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

#include "tests/raw/RawLoaderTest.hpp"

#include "raw/RawImageLoader.hpp"


RawLoaderTest::RawLoaderTest(QObject *parent) : QObject(parent) {}

RawModel RawLoaderTest::createModel()
{
    bool isEnabled = true;
    QString dcrawPath = "dcraw";
    QString dcrawOptions = "-q 0 -a";

    return RawModel(isEnabled, dcrawPath, dcrawOptions);
}

void RawLoaderTest::initTestCase() {}

void RawLoaderTest::cleanupTestCase() {}

void RawLoaderTest::test_isRawImage_dcrawCommand()
{
    RawModel model = createModel();
    QString fileName = "test_file.raw";
    RawLoader *loader = new RawImageLoader(&model, fileName);

    loader->isRawImage();

    QString expectedCommand = QString("%1 -i %2")
            .arg(model.getDcrawPath()).arg(fileName);
    QCOMPARE(loader->lastDcrawCommand(), expectedCommand);

    delete loader;
}

void RawLoaderTest::test_loadFromRawFile_dcrawCommand()
{
    RawModel model = createModel();
    QString fileName = "test_file.raw";
    RawLoader *loader = new RawImageLoader(&model, fileName);

    loader->loadFromRawFile();

    QString expectedCommand = QString("%1 %2 -c %3")
            .arg(model.getDcrawPath()).arg(model.getDcrawOptions()).arg(fileName);
    QCOMPARE(loader->lastDcrawCommand(), expectedCommand);

    delete loader;
}


QTEST_MAIN(RawLoaderTest)
#include "RawLoaderTest.moc"
