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

#ifndef DETAILSBROWSERCONTROLLER_H
#define DETAILSBROWSERCONTROLLER_H

#include <QObject>

#ifdef SIR_METADATA_SUPPORT
#include "metadata/structs/ExifPrintSettings.hpp"
#endif // SIR_METADATA_SUPPORT

class QTreeWidgetItem;
class ConvertDialog;
class TreeWidget;
class DetailsBrowserView;
#ifdef SIR_METADATA_SUPPORT
namespace MetadataUtils {
struct ExifStruct;
struct IptcStruct;
}
#endif // SIR_METADATA_SUPPORT

//! Image file details browser controller class.
class DetailsBrowserController : public QObject {
    Q_OBJECT

public:
    explicit DetailsBrowserController(TreeWidget *model, DetailsBrowserView *view, QObject *parent = 0);
    void addItem(QTreeWidgetItem *item, int index = 0);

public slots:
    void showDetails();

signals:
    void loadingDetailsStart();
    void loadingDetailsStop();

private:
    TreeWidget *model;
    DetailsBrowserView *view;
    ConvertDialog *convertDialog;
    QList<QTreeWidgetItem*> selectedFiles;
    QString htmlContent;

#ifdef SIR_METADATA_SUPPORT
    QString addMetadataToContent(MetadataUtils::ExifStruct *exifStruct,
                                 MetadataUtils::IptcStruct *iptcStruct);
    QString exifContent(MetadataUtils::ExifStruct *exifStruct);
    QString iptcContent(MetadataUtils::IptcStruct *iptcStruct);
    ExifPrintSettings loadExifPrintSettings();
    int loadIptcPrintSettings();
#endif // SIR_METADATA_SUPPORT
};

#endif // DETAILSBROWSERCONTROLLER_H
