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

#include "metadata/visitors/IptcRichTextVisitor.hpp"

#include <typeinfo>

#include "optionsenums.h"
#include "metadata/Iptc.hpp"
#include "metadata/structs/IptcStruct.hpp"


using namespace MetadataUtils;

IptcRichTextVisitor::IptcRichTextVisitor(int iptcPrint) {
    this->iptcPrint = iptcPrint;
}

void IptcRichTextVisitor::visit(Visitable *visitable) {
    Q_ASSERT(typeid(*visitable) == typeid(IptcStruct));

    richText = QString();

    IptcStruct *iptcStruct = static_cast<IptcStruct *>(visitable);
    if (!Iptc::isVersionKnown())
        return;

    if (iptcPrint & DetailsOptions::ModelVersion)
        richText += tr("Model version") + ": " +
                iptcStruct->modelVersion + htmlBr;

    if (iptcPrint & DetailsOptions::DateCreated)
        richText += tr("Created date") + ": " +
                iptcStruct->dateCreated.toString(dateFormat) + htmlBr;
    if (iptcPrint & DetailsOptions::TimeCreated)
        richText += tr("Created time") + ": " +
                iptcStruct->timeCreated.toString(timeFormat) + htmlBr;

    if (iptcPrint & DetailsOptions::DigitizedDate)
        richText += tr("Digitized date") + ": " +
                iptcStruct->digitizationDate.toString(dateFormat) + htmlBr;
    if (iptcPrint & DetailsOptions::DigitizedTime)
        richText += tr("Digitized time") + ": " +
                iptcStruct->digitizationTime.toString(timeFormat) + htmlBr;

    if (iptcPrint & DetailsOptions::Byline)
        richText += tr("Author") + ": " + iptcStruct->byline + htmlBr;
    if (iptcPrint & DetailsOptions::CopyrightIptc)
        richText += tr("Copyright") + ": " + iptcStruct->copyright + htmlBr;

    if (iptcPrint & DetailsOptions::ObjectName)
        richText += tr("Object name") + ": " + iptcStruct->objectName + htmlBr;
    if (iptcPrint & DetailsOptions::Keywords)
        richText += tr("Keywords") + ": " + iptcStruct->keywords + htmlBr;
    if (iptcPrint & DetailsOptions::Caption)
        richText += tr("Description") + ": " + iptcStruct->caption + htmlBr;

    if (iptcPrint & DetailsOptions::CountryName)
        richText += tr("Country") + ": " + iptcStruct->countryName + htmlBr;
    if (iptcPrint & DetailsOptions::City)
        richText += tr("City") + ": " + iptcStruct->city + htmlBr;

    if (iptcPrint & DetailsOptions::EditStatus)
        richText += tr("Edit status") + ": " + iptcStruct->editStatus + htmlBr;
}

QString IptcRichTextVisitor::richTextString() const {
    return richText;
}

void IptcRichTextVisitor::setDateFormat(const QString &value) {
    dateFormat = value;
}

void IptcRichTextVisitor::setTimeFormat(const QString &value) {
    timeFormat = value;
}
