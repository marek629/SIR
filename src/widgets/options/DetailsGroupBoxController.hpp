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

#ifndef DETAILSGROUPBOXCONTROLLER_H
#define DETAILSGROUPBOXCONTROLLER_H

#include "widgets/options/AbstractOptionsGroupBox.hpp"
#include "Settings.hpp"

class QCheckBox;
class DetailsGroupBoxView;

class DetailsGroupBoxController : public AbstractOptionsController {
    Q_OBJECT
    friend class DetailsGroupBoxTest;

public:
    explicit DetailsGroupBoxController(Settings::DetailsGroup *model,
                                       DetailsGroupBoxView *view,
                                       QObject *parent = 0);
    void loadSettings();
    void saveSettings();
    void checkExifFields(bool checked);
    void checkIptcFields(bool checked);
    void setCurrentTab(int tabId);
    void showAllFields();
    void hideAllFields();

    //! Tab metadata widget ID.
    enum TabId {
        Exif_tab,
        IPTC_tab
    };

private:
    Settings::DetailsGroup *model;
    DetailsGroupBoxView *view;
    QList<QCheckBox*> exifCheckBoxes;
    int exifSelectedFields;
    QList<QCheckBox*> iptcCheckBoxes;
    int iptcSelectedFields;

    void loadExifSettings();
    void loadIptcSettings();
    void saveExifSettings();
    void saveIptcSettings();
    void enableFieldButtons(const QList<QCheckBox *> &checkBoxList, int fieldCount);
};

#endif // DETAILSGROUPBOXCONTROLLER_H
