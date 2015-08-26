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

#ifndef RAWGROUPBOXCONTROLLER_H
#define RAWGROUPBOXCONTROLLER_H

#include "widgets/options/AbstractOptionsGroupBox.hpp"

class RawController;
class RawModel;
class RawGroupBoxView;
class RawViewAdapter;


//! Raw group box controller class used in OptionsDialog dialog.
class RawGroupBoxController : public AbstractOptionsController
{
    Q_OBJECT

    friend class RawGroupBoxControllerTest;

public:
    explicit RawGroupBoxController(RawModel *model, RawGroupBoxView *view,
                                   QObject *parent = 0);
    ~RawGroupBoxController();

    void loadSettings();
    void saveSettings();

    void browseDcraw();
    void setRawStatus(int state);
    void setDcrawOptionsString(const QString &options);

private:
    RawModel *model;
    RawGroupBoxView *view;
    RawViewAdapter *viewAdapter;
    RawController *adaptedController;
};

#endif // RAWGROUPBOXCONTROLLER_H
