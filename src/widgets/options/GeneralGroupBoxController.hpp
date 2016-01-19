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

#ifndef GENERALGROUPBOXCONTROLLER_H
#define GENERALGROUPBOXCONTROLLER_H

#include "widgets/options/AbstractOptionsGroupBox.hpp"
#include "Settings.hpp"

class GeneralGroupBoxView;

class GeneralGroupBoxController : public AbstractOptionsController {
    Q_OBJECT

public:
    explicit GeneralGroupBoxController(Settings::SettingsGroup *settingsModel,
                                       Settings::SizeGroup *sizeModel,
                                       GeneralGroupBoxView *view,
                                       QObject *parent = 0);
    void loadSettings();
    void saveSettings();

    static quint8 detectCoresCount();
    static quint8 maxCoresCount();
    quint8 coresCount();
    void setCoresCount(quint8 cores);

private:
    Settings::SettingsGroup *modelSettings;
    Settings::SizeGroup *modelSize;
    GeneralGroupBoxView *view;

    /** Count of threads created for convertion. It's mean count of images
      * converted at the same moment.
      */
    quint8 coresCount_;
    /** Maximum count of threads created for convertion.
      * \sa coresCount
      */
    static quint8 maxCoresCount_;

    void loadSettingsSettings();
    void loadSizeSettings();

    void saveSettingsSettings();
    void saveSizeSettings();
};

#endif // GENERALGROUPBOXCONTROLLER_H
