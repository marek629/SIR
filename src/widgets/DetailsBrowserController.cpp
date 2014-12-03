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

#include "widgets/DetailsBrowserController.hpp"
#include "widgets/DetailsBrowserView.hpp"
#include "widgets/ConvertDialog.hpp"
#include "optionsenums.h"
#include "ConvertSharedData.hpp"
#include "DetailsThumbnail.hpp"

#ifdef SIR_METADATA_SUPPORT
#include "metadata/visitors/RichTextVisitor.hpp"
#include "metadata/visitors/ExifRichTextVisitor.hpp"
#include "metadata/visitors/IptcRichTextVisitor.hpp"
#endif // SIR_METADATA_SUPPORT

using namespace MetadataUtils;
using namespace sir;

DetailsBrowserController::DetailsBrowserController(TreeWidget *model,
                                                   DetailsBrowserView *view,
                                                   QObject *parent)
    : QObject(parent) {
    this->model = model;
    connect(this->model, SIGNAL(itemSelectionChanged()), this, SLOT(showDetails()));

    this->view = view;
    this->view->setController(this);
    this->view->setReadOnly(true);

    convertDialog = (ConvertDialog*)this->view->window();
}

/** Adds detailed information (including thumbnail) about \a item.
  * \param item Files tree widget item.
  * \param index Serial number of thumbnail for \a item.
  * \sa addMetadataToContent()
  */
void DetailsBrowserController::addItem(QTreeWidgetItem *item, int index) {
    DetailsThumbnail thumb(item, index, view->usableWidth());

    htmlContent += "<center><img src=\"" + thumb.filePath() + "\"";
    if (thumb.size().width() > view->usableWidth())
        htmlContent += " width=\"" + QString::number(view->usableWidth()) + "\"";
    htmlContent += "/></center>" + RichTextVisitor::htmlBr;

    htmlContent += thumb.sourceFilePath() + RichTextVisitor::htmlBr;

    if (thumb.isRenderedFromSVG())
        htmlContent += tr("Default image size: ");
    else
        htmlContent += tr("Image size: ");
    QSize imageSize = thumb.sourceImageSize();
    htmlContent += QString::number(imageSize.width()) + "x"
            + QString::number(imageSize.height()) + " px"
            + RichTextVisitor::htmlBr;

    htmlContent += tr("File size: ");
    htmlContent += convertDialog->fileSizeString(thumb.sourceFileSize())
            + RichTextVisitor::htmlBr;

#ifdef SIR_METADATA_SUPPORT
    if (thumb.isReadFromMetadataThumbnail())
        htmlContent += addMetadataToContent(thumb.exifStructRef(),
                                            thumb.iptcStructRef());
#endif // SIR_METADATA_SUPPORT
}

/** Shows file details or few files summary.\n
  * This function is called when selection items changed in tree view list.\n
  * When horizontal spliters widgets containing this details widget width equal
  * 0 this function will do nothing.
  * \sa addItem()
  */
void DetailsBrowserController::showDetails() {
    if (convertDialog->horizontalSplitter->widget(1)->width() == 0)
        return;

    view->clear();
    htmlContent.clear();

    selectedFiles = convertDialog->filesTreeWidget->selectedItems();

    if (selectedFiles.length() <= 0) {
        view->setText(tr("Select image to show this one details."));
        return;
    }

    if (selectedFiles.length() == 1) {
        addItem(selectedFiles.first());
    }
    else { // many files summary
        int i = 0;
        int lastItemIndex = selectedFiles.length() - 1;
        foreach (QTreeWidgetItem *item, selectedFiles) {
            addItem(item, i);
            if (i < lastItemIndex)
                htmlContent += RichTextVisitor::htmlHr;
            i++;
        }
    }

    view->setHtml(RichTextVisitor::htmlOrigin + htmlContent
                  + RichTextVisitor::htmlEnd);
}

#ifdef SIR_METADATA_SUPPORT
/** Returns metadata information string using data stored in
  * exifStruct and iptcStruct structs and exifPhoto, exifImage,
  * exifAuthor, exifCamera and iptcPrint enumeration fields.
  *
  * This function is available if SIR_METADATA_SUPPORT is defined.
  *
  * \sa DetailsOptions addItem()
  */
QString DetailsBrowserController::addMetadataToContent(
        const MetadataUtils::ExifStruct &exifStruct,
        const MetadataUtils::IptcStruct &iptcStruct) {
    QString content;

    content += exifContent(exifStruct);
    content += iptcContent(iptcStruct);

    return content;
}

QString DetailsBrowserController::exifContent(
        const MetadataUtils::ExifStruct &exifStruct) {
    const ConvertSharedData &csd = convertDialog->convertSharedData();

    ExifPrintSettings exifPrint = loadExifPrintSettings();
    ExifRichTextVisitor visitor(exifPrint.author, exifPrint.camera,
                                exifPrint.image, exifPrint.photo);
    visitor.setDateTimeFormat(csd.dateTimeFormat);

    return visitor.visit(const_cast<MetadataUtils::ExifStruct *>(&exifStruct));
}

QString DetailsBrowserController::iptcContent(
        const MetadataUtils::IptcStruct &iptcStruct) {
    const ConvertSharedData &csd = convertDialog->convertSharedData();

    int iptcPrint = loadIptcPrintSettings();
    IptcRichTextVisitor visitor(iptcPrint);
    visitor.setDateFormat(csd.dateFormat);
    visitor.setTimeFormat(csd.timeFormat);

    return visitor.visit(const_cast<MetadataUtils::IptcStruct *>(&iptcStruct));
}

ExifPrintSettings DetailsBrowserController::loadExifPrintSettings() {
    ExifPrintSettings result;
    result.loadSettings();
    return result;
}

int DetailsBrowserController::loadIptcPrintSettings() {
    Settings *s = Settings::instance();
    if (s->metadata.enabled) {
        return s->details.iptc;
    }
    else {
        return 0;
    }
}
#endif // SIR_METADATA_SUPPORT
