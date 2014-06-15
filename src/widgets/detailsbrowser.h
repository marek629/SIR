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

#ifndef DETAILSBROWSER_H
#define DETAILSBROWSER_H

#include <QTextEdit>

class QTreeWidgetItem;
class ConvertDialog;
#ifdef SIR_METADATA_SUPPORT
namespace MetadataUtils {
struct ExifStruct;
struct IptcStruct;
}
#endif // SIR_METADATA_SUPPORT

//! Image file details browser.
class DetailsBrowser : public QTextEdit {
    Q_OBJECT
    Q_PROPERTY(int usableWidth READ usableWidth)

public:
    explicit DetailsBrowser(QWidget *parent = 0);
    int usableWidth() const { return usableWidth_; }
    void addItem(QTreeWidgetItem *item, int index = 0);

public slots:
    void showDetails();

private:
    // properties
    int usableWidth_;
    // fields
    ConvertDialog *convertDialog;
    QList<QTreeWidgetItem*> selectedFiles;
    QString htmlContent;
#ifdef SIR_METADATA_SUPPORT
    int exifAuthor;
    int exifCamera;
    int exifPhoto;
    int exifImage;
    int iptcPrint;
    MetadataUtils::ExifStruct *exifStruct;
    MetadataUtils::IptcStruct *iptcStruct;
#endif // SIR_METADATA_SUPPORT
    // methods
    void loadSettings();
#ifdef SIR_METADATA_SUPPORT
    void addMetadataToContent();
#endif // SIR_METADATA_SUPPORT

protected:
    void resizeEvent(QResizeEvent *e);
};

#endif // DETAILSBROWSER_H
