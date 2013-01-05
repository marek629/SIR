/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://sir.projet-libre.org/
 */

#ifndef GENERALGROUPBOX_H
#define GENERALGROUPBOX_H

#include "ui_generalgroupbox.h"
#include "widgets/options/abstractoptions.h"

class LanguageUtils;

//! General group box class used in OptionsDialog dialog.
class GeneralGroupBox : public AbstractOptionsGroupBox, public Ui::GeneralGroupBox {
    Q_OBJECT
    friend class OptionsDialog;

public:
    explicit GeneralGroupBox(QWidget *parent = 0);
    ~GeneralGroupBox();
    static quint8 detectCoresCount();
    void loadSettings();
    void saveSettings();

private slots:
    void browseDestination();
    void respondCoresSpinBox(bool checked);

private:
    // fields
    /** Count of threads created for convertion. It's mean count of images
      * converted at the same moment.
      */
    quint8 coresCount;
    /** Maximum count of threads created for convertion.
      * \sa coresCount
      */
    static quint8 maxCoresCount;
    LanguageUtils * languages;
    QMap<QString, QString> * fileToNiceName;
    // methods
    void createLanguageMenu();
};

#endif // GENERALGROUPBOX_H
