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

#ifndef EXIFRICHTEXTVISITOR_HPP
#define EXIFRICHTEXTVISITOR_HPP

#include "visitors/RichTextVisitor.hpp"

#include "metadata/Exif.hpp"
#include "metadata/structs/ExifStruct.hpp"


namespace MetadataUtils {
class ExifRichTextVisitor : public RichTextVisitor {
    Q_DECLARE_TR_FUNCTIONS(ExifRichTextVisitor)

public:
    ExifRichTextVisitor(int exifAuthor, int exifCamera,
                        int exifImage, int exifPhoto);
    void visit(Visitable *visitable);
    QString richTextString() const;
    void setDateTimeFormat(const QString &value);

private:
    QString richText;
    int exifAuthor;
    int exifCamera;
    int exifImage;
    int exifPhoto;
    QString dateTimeFormat;

    QString authorContent(const MetadataUtils::ExifStruct &exifStruct);
    QString cameraContent(const MetadataUtils::ExifStruct &exifStruct);
    QString imageContent(const MetadataUtils::ExifStruct &exifStruct);
    QString photoContent(const MetadataUtils::ExifStruct &exifStruct);
};
}

#endif // EXIFRICHTEXTVISITOR_HPP
