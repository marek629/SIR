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
 * Program URL: http://marek629.github.io/sir/
 */

#ifndef METADATADIALOG_H
#define METADATADIALOG_H

#include "ui_MetadataDialog.h"

namespace MetadataUtils {
    class Metadata;
    struct ExifStruct;
    struct IptcStruct;
}
class QStringList;
class QString;

//! Same metadata fields viever and editor window, already supports Exif.
class MetadataDialog : public QDialog, public Ui::MetadataDialog {
    Q_OBJECT
public:
    explicit MetadataDialog(QWidget *parent = 0, QStringList *images = 0,
                            int currentImage = 0);
    ~MetadataDialog();

private:
    // methods
    void resetStructs();
    void setupValues();
    void readFile();
    void createConnections();
    void saveHistory();
    void setupInputWidgets();
    void setIcons();
    // fields
    MetadataUtils::Metadata *metadata;
    MetadataUtils::ExifStruct *exifStruct;
    MetadataUtils::IptcStruct *iptcStruct;
    QStringList *images;
    QString imagePath;
    int currentImage;

private slots:
    void saveChanges();
    void deleteMetadata();
    void previousImage();
    void nextImage();
};

#endif // METADATADIALOG_H
