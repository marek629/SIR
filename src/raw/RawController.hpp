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

#ifndef RAWCONTROLLER_HPP
#define RAWCONTROLLER_HPP

#include <QCoreApplication>

class RawModel;
class RawView;


class RawController
{
    Q_DECLARE_TR_FUNCTIONS(RawController)

    friend class RawControllerTest;

public:
    RawController(RawModel *model, RawView *view);
    void loadSettings();
    void saveSettings();
    void browseDcraw();
    void setRawStatus(int state);
    void setDcrawOptionsString(const QString &options);
    void setModel(RawModel *model);

private:
    RawModel *model;
    RawView *view;
    bool checkDcrawPath(const QString &fileName);
    void setDcrawPath(const QString &fileName);
};

#endif // RAWCONTROLLER_HPP
