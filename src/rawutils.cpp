/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto
 * 2011-2012  Marek Jędryka
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
 * Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
 *                 Marek Jędryka   <jedryka89@gmail.com>
 * Program URL: http://sir.projet-libre.org/
 */

#include "rawutils.h"
#include <QImage>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QImageReader>
#include <QSettings>
#include <QPixmap>

/** Returns true if \b imagePath is path to raw image, otherwise returns false. */
bool RawUtils::isRaw(QString imagePath) {

    QList<QByteArray> imageFormats = QImageReader::supportedImageFormats();
    QStringList list;
    QString dcrawPath = readDcrawPath();

    foreach(QByteArray format, imageFormats)
    {
        list.append(*new QString(format));
    }

    list.append("jpg");
    list.append("JPG");
    list.append("JPEG");
    list.append("Jpg");
    list.append("Jpeg");

    //Getting the file extension
    int aux = imagePath.size()-imagePath.lastIndexOf(".")-1;
    QString ext = imagePath.right(aux);

    if(list.contains(ext)) {
        //Is not a raw image file. Its a regular image.
        return false;
    }

    //Its not a regular image file, using dcraw to identify it!
    QProcess *p = new QProcess();

    p->start(dcrawPath + " -i " + imagePath);
    p->waitForFinished(-1);

    bool dc_success = !(p->exitCode());

    delete p;
    return dc_success;

}

/** Loads raw image from file in \b imagePath path and returns pointer to this image.
  * \note If loading image failed this function will return null pointer.
  * \sa loadRawPixmap
  */
QImage *RawUtils::loadRawImage(QString imagePath) {

    //The error handling is up to the caller
    //This function returns a NULL image if
    //loadFromData fails to load the image

    QProcess *p = new QProcess();
    QImage *image = new QImage();
    QString dcrawPath = readDcrawPath();

    p->start(dcrawPath + " -c " + imagePath);
    p->waitForFinished(-1);

    //exitCode is 0 if dcraw was able to identify a raw image
    //and 1 otherwise
    bool dc_success = !(p->exitCode());

    if(dc_success) {
        image->loadFromData(p->readAll(), "PPM");
    }

    delete p;
    return image;
}

/** Loads raw image from file in \b imagePath path and returns pointer to this pixmap.
  * \note If loading image failed this function will return null pointer.
  * \sa loadRawImage
  */
QPixmap *RawUtils::loadRawPixmap(QString imagePath) {

    //The error handling is up to the caller
    //This function returns a NULL image if
    //loadFromData fails to load the image

    QProcess *p = new QProcess();
    QPixmap *image = new QPixmap();
    QString dcrawPath = readDcrawPath();

    p->start(dcrawPath + " -c " + imagePath);
    p->waitForFinished(-1);
    bool dc_success = !(p->exitCode());

    //exitCode is 0 if dcraw was able to identify a raw image
    //and 1 otherwise
    if(dc_success) {
        image->loadFromData(p->readAll(), "PPM");
    }

    delete p;
    return image;
}

/** Returns true if raw support is enabled, otherwise returns false. */
bool RawUtils::isRawEnabled() {
    QSettings settings("SIR");
    settings.beginGroup("Raw");
    bool rawEnabled = settings.value("raw", false).toBool();
    settings.endGroup();
    return rawEnabled;
}

QString RawUtils::readDcrawPath() {
    QSettings settings("SIR");
    settings.beginGroup("Raw");
    QString dcraw = settings.value("dcrawPath", "").toString();
    settings.endGroup();
    return dcraw;
}

QStringList RawUtils::readDcrawOptions() {
    QSettings settings("SIR");
    settings.beginGroup("Raw");
    QStringList options = settings.value("dcrawOptions", "").toString().split(" ");
    settings.endGroup();
    return options;
}

/** Creates list of supported image file extensions into \b rawFormats. */
void RawUtils::createRawFilesList(QStringList &rawFormats) {
    rawFormats.clear();
    rawFormats.append(" *.3fr");
    rawFormats.append(" *.arw");
    rawFormats.append(" *.arw");
    rawFormats.append(" *.bay");
    rawFormats.append(" *.cap");
    rawFormats.append(" *.cr2");
    rawFormats.append(" *.crw");
    rawFormats.append(" *.dcr");
    rawFormats.append(" *.dcs");
    rawFormats.append(" *.drf");
    rawFormats.append(" *.eip");
    rawFormats.append(" *.erf");
    rawFormats.append(" *.fff");
    rawFormats.append(" *.iiq");
    rawFormats.append(" *.k25");
    rawFormats.append(" *.kdc");
    rawFormats.append(" *.mef");
    rawFormats.append(" *.mos");
    rawFormats.append(" *.mrw");
    rawFormats.append(" *.nef");
    rawFormats.append(" *.nrw");
    rawFormats.append(" *.orf");
    rawFormats.append(" *.pef");
    rawFormats.append(" *.ptx");
    rawFormats.append(" *.pxn");
    rawFormats.append(" *.r3d");
    rawFormats.append(" *.raf");
    rawFormats.append(" *.raw");
    rawFormats.append(" *.rw2");
    rawFormats.append(" *.rwl");
    rawFormats.append(" *.rwz");
    rawFormats.append(" *.sr2");
    rawFormats.append(" *.srf");
    rawFormats.append(" *.tif");
    rawFormats.append(" *.x3f");
    rawFormats.append(" *.3FR");
    rawFormats.append(" *.ARW");
    rawFormats.append(" *.ARW");
    rawFormats.append(" *.BAY");
    rawFormats.append(" *.CAP");
    rawFormats.append(" *.CR2");
    rawFormats.append(" *.CRW");
    rawFormats.append(" *.DCR");
    rawFormats.append(" *.DCS");
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
    rawFormats.append(" *.TIF");
    rawFormats.append(" *.X3F");
}
