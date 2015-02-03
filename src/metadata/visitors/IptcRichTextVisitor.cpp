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

#include "metadata/visitors/IptcRichTextVisitor.hpp"
#include "metadata/Iptc.hpp"
#include "metadata/structs/IptcStruct.hpp"
#include "optionsenums.h"

using namespace MetadataUtils;

IptcRichTextVisitor::IptcRichTextVisitor(int iptcPrint) {
    this->iptcPrint = iptcPrint;
}

QString IptcRichTextVisitor::visit(MetadataStruct *metadataStruct) {
    IptcStruct *iptcStruct = (IptcStruct *) metadataStruct;
    if (!Iptc::isVersionKnown())
        return QString();

    QString content;

    if (iptcPrint & DetailsOptions::ModelVersion)
        content += tr("Model version") + ": " +
                iptcStruct->modelVersion + htmlBr;

    if (iptcPrint & DetailsOptions::DateCreated)
        content += tr("Created date") + ": " +
                iptcStruct->dateCreated.toString(dateFormat) + htmlBr;
    if (iptcPrint & DetailsOptions::TimeCreated)
        content += tr("Created time") + ": " +
                iptcStruct->timeCreated.toString(timeFormat) + htmlBr;

    if (iptcPrint & DetailsOptions::DigitizedDate)
        content += tr("Digitized date") + ": " +
                iptcStruct->digitizationDate.toString(dateFormat) + htmlBr;
    if (iptcPrint & DetailsOptions::DigitizedTime)
        content += tr("Digitized time") + ": " +
                iptcStruct->digitizationTime.toString(timeFormat) + htmlBr;

    if (iptcPrint & DetailsOptions::Byline)
        content += tr("Author") + ": " + iptcStruct->byline + htmlBr;
    if (iptcPrint & DetailsOptions::CopyrightIptc)
        content += tr("Copyright") + ": " + iptcStruct->copyright + htmlBr;

    if (iptcPrint & DetailsOptions::ObjectName)
        content += tr("Object name") + ": " + iptcStruct->objectName + htmlBr;
    if (iptcPrint & DetailsOptions::Keywords)
        content += tr("Keywords") + ": " + iptcStruct->keywords + htmlBr;
    if (iptcPrint & DetailsOptions::Caption)
        content += tr("Description") + ": " + iptcStruct->caption + htmlBr;

    if (iptcPrint & DetailsOptions::CountryName)
        content += tr("Country") + ": " + iptcStruct->countryName + htmlBr;
    if (iptcPrint & DetailsOptions::City)
        content += tr("City") + ": " + iptcStruct->city + htmlBr;

    if (iptcPrint & DetailsOptions::EditStatus)
        content += tr("Edit status") + ": " + iptcStruct->editStatus + htmlBr;

    return content;
}

void IptcRichTextVisitor::setDateFormat(const QString &value) {
    dateFormat = value;
}

void IptcRichTextVisitor::setTimeFormat(const QString &value) {
    timeFormat = value;
}
