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

#ifndef RAWGROUPBOXTEST_H
#define RAWGROUPBOXTEST_H

#include <QtTest/QTest>
#include "widgets/options/rawgroupbox.h"

class RawGroupBoxTest : public QObject {
    Q_OBJECT

public:
    RawGroupBoxTest();

private:
    RawGroupBox groupBox;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void checkDcrawPath_emptyString();
    void checkDcrawPath_fileNotExists();
    void checkDcrawPath_fileNotExecutable();
    void checkDcrawPath_fileExecutable();
};

#endif // RAWGROUPBOXTEST_H
