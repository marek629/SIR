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

#ifndef RAWGROUPBOXCONTROLLER_H
#define RAWGROUPBOXCONTROLLER_H

#include "abstractoptionsgroupbox.h"
#include "settings.h"

class RawGroupBoxView;

//! Raw group box controller class used in OptionsDialog dialog.
class RawGroupBoxController : public AbstractOptionsController {
    Q_OBJECT
    friend class RawGroupBoxControllerTest;

public:
    explicit RawGroupBoxController(Settings::RawGroup *model, RawGroupBoxView *view,
                                   QObject *parent = 0);
    void loadSettings();
    void saveSettings();
    void browseDcraw();
    void setRawStatus(int state);

signals:
    void ok(); /**< Indicates write settings success. */

private:
    Settings::RawGroup *model;
    RawGroupBoxView *view;
    bool checkDcrawPath(const QString &fileName);
};

#endif // RAWGROUPBOXCONTROLLER_H