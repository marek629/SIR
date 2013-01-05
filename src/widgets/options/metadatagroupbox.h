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

#ifndef METADATAGROUPBOX_H
#define METADATAGROUPBOX_H

#include "ui_metadatagroupbox.h"
#include "widgets/options/abstractoptions.h"

//! Metadata group box class used in OptionsDialog dialog.
class MetadataGroupBox : public AbstractOptionsGroupBox, public Ui::MetadataGroupBox {
    Q_OBJECT

public:
    explicit MetadataGroupBox(QWidget *parent = 0);
    ~MetadataGroupBox();
    void loadSettings();
    void saveSettings();

private slots:
    void enableMetadata(bool checked);
    void saveMetadata(bool save);
    void updateThumbnail(bool update);

private:
    // fields
    QRegExpValidator* validator;
};

#endif // METADATAGROUPBOX_H
