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

#include "raw/RawLoader.hpp"

#include <QImageReader>
#include <QProcess>

#include "raw/PaintDevice.hpp"


RawLoader::RawLoader(RawModel *rawModel, const QString &filePath)
    : RawToolbox(rawModel)
{
    this->filePath = filePath;
}

RawLoader::~RawLoader() {}

bool RawLoader::isRawImage()
{
    QString extension = fileExtension();
    if (regularImageFormatList().contains(extension)) {
        return false;
    }

    QProcess dcrawProcess;

    dcrawCommand = QString("%1 -i %2").arg(dcrawPath()).arg(filePath);
    dcrawProcess.start(dcrawCommand);

    dcrawProcess.waitForFinished(-1);

    return dcrawProcess.exitCode() == 0;
}

QString RawLoader::lastDcrawCommand() const
{
    return dcrawCommand;
}

PaintDevice *RawLoader::load()
{
    return (isRawImage()) ? loadFromRawFile() : loadFromNormalFile();
}

PaintDevice *RawLoader::loadFromRawFile()
{
    PaintDevice *paintDevice = createPaintDevice();
    QProcess process;

    dcrawCommand = QString("%1 %2 -c %3")
            .arg(dcrawPath())
            .arg(dcrawOptions())
            .arg(filePath);
    process.start(dcrawCommand);

    if (!process.waitForFinished(-1)) {
        return paintDevice;
    }

    const int exitCodeOk = 0;
    if (process.exitCode() == exitCodeOk) {
        paintDevice->loadFromData(process.readAll(), "PPM");
    }

    return paintDevice;
}

PaintDevice *RawLoader::loadFromNormalFile()
{
    PaintDevice *paintDevice = createPaintDevice();
    paintDevice->load(filePath);
    return paintDevice;
}

QStringList RawLoader::regularImageFormatList() const
{
    QStringList list;

    foreach(QByteArray format, QImageReader::supportedImageFormats()) {
        list << QString(format);
    }
    list << "jpg" << "JPG" << "JPEG" << "Jpg" << "Jpeg";

    return list;
}

QString RawLoader::fileExtension() const
{
    int index = filePath.size() - filePath.lastIndexOf(".") - 1;
    return filePath.right(index);
}
