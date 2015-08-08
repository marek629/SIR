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
 * Program URL: http://marek629.github.io/SIR/
 */

#include "raw/RawToolbox.hpp"

#include "raw/RawModel.hpp"

#include <QProcess>


RawToolbox::RawToolbox(RawModel *rawModel)
{
    this->rawModel = rawModel;
}

QString RawToolbox::helpMessage()
{
    QProcess dcraw;
    dcraw.start(rawModel->getDcrawPath());

    if (!dcraw.waitForStarted())
        return QString();

    if (!dcraw.waitForFinished())
        return QString();

    return dcraw.readAll();
}

QStringList RawToolbox::fileFilters() const
{
    QStringList rawFormats;

    rawFormats.append(" *.3FR");
    rawFormats.append(" *.ARI");
    rawFormats.append(" *.ARW");
    rawFormats.append(" *.BAY");
    rawFormats.append(" *.CAP");
    rawFormats.append(" *.CR2");
    rawFormats.append(" *.CRW");
    rawFormats.append(" *.DCR");
    rawFormats.append(" *.DCS");
    rawFormats.append(" *.DNG");
    rawFormats.append(" *.DRF");
    rawFormats.append(" *.EIP");
    rawFormats.append(" *.ERF");
    rawFormats.append(" *.FFF");
    rawFormats.append(" *.IIQ");
    rawFormats.append(" *.K25");
    rawFormats.append(" *.KDC");
    rawFormats.append(" *.MEF");
    rawFormats.append(" *.MOS");
    rawFormats.append(" *.MRW");
    rawFormats.append(" *.NEF");
    rawFormats.append(" *.NRW");
    rawFormats.append(" *.ORF");
    rawFormats.append(" *.PEF");
    rawFormats.append(" *.PTX");
    rawFormats.append(" *.PXN");
    rawFormats.append(" *.R3D");
    rawFormats.append(" *.RAF");
    rawFormats.append(" *.RAW");
    rawFormats.append(" *.RW2");
    rawFormats.append(" *.RWL");
    rawFormats.append(" *.RWZ");
    rawFormats.append(" *.SR2");
    rawFormats.append(" *.SRF");
    rawFormats.append(" *.SRW");
    rawFormats.append(" *.X3F");

    return rawFormats;
}

QString RawToolbox::dcrawPath() const
{
    return rawModel->getDcrawPath();
}

bool RawToolbox::isRawSupportEnabled() const
{
    return rawModel->isEnabled();
}

QString RawToolbox::fileFilterString()
{
    QString result;

    if (isRawSupportEnabled()) {
        foreach (QString ext, fileFilters()) {
            result.append(ext);
        }
    }

    return result;
}
