/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2012  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://sir.projet-libre.org/
 */

#ifndef SHAREDINFORMATION_H
#define SHAREDINFORMATION_H

#include <QMutex>
#include <QString>
#include <QDir>

class ConvertThread;

//! ConvertThread threads shared information.
class SharedInformation {

public:
    // methods
    SharedInformation();

    // fields
    // image data
    // destinated size
    int width; /**< Width of target image in pixels. */
    int height; /**< Height of target image in pixels. */
    bool hasWidth; /**< Width given indicator. */
    bool hasHeight; /**< Height given indicator. */
    bool maintainAspect; /**< Keep/Ignore aspect ratio indicator. */
    quint32 sizeBytes; /**< Size of target image in bytes. */
    char sizeUnit; /**< Size unit code based on size unit combo box index into ConvertDialog. */
    // destinated image file parameters
    QDir destFolder; /**< Destination directory. */
    QString prefix; /**< Target file prefix. */
    QString suffix; /**< Target file suffix. */
    QString format; /**< Target file format. */
    int quality;  /**< Target image quality in range between 1 to 100. */
    // destinated orientation
    bool rotate; /**< Rotation indicator. */
    double angle; /**< Rotation angle. */
    int flip; /**< MetadataUtils::Flip code. */
    // metadata settings
    bool metadataEnabled; /**< Indicates that metadata support enabled. */
    bool saveMetadata; /**< Saving metadata into target image indicator. */
    bool realRotate; /**< Real rotation indicator - not save into \em Exif.Image.Orientation field. */
    bool updateThumbnail; /**< Update thumbnail of target image indicator. */
    bool rotateThumbnail; /**< Rotate thumbnail of target image indicator. */

    // thread synchronization data
    QMutex mutex; /**< Mutual exclusion object using for worker threads synchronization. */
    bool abort; /**< Abort indicator. */
    // overwrite
    bool overwriteAll; /**< Overwrite all conflicting files indicator. */
    bool noOverwriteAll; /**< No overwrite all conflicting files indicator. */
    int overwriteResult; /**< Message box containing question about overwriting file result code. */
    // enlarge
    bool enlargeAll; /**< Enlarge all conflicting files indicator. */
    bool noEnlargeAll; /**< No enlarge all conflicting files indicator. */
    int enlargeResult; /**< Message box containing question about enlarging image result code. */
};

#endif // SHAREDINFORMATION_H
