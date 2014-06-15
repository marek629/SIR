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

#ifndef SHAREDINFORMATION_H
#define SHAREDINFORMATION_H

#include <QMutex>
#include <QString>
#include <QDir>
#include <QColor>
#include <QFont>
#include <QPair>
#include <QImage>
#include <QBrush>

/** Position modificators enumerator for \em "Add Text" and \em "Add Image"
  * effects. It's correlated position combo boxes in EffectsScrollArea.
  * \sa PosUnit
  */
enum PosModifier {
    TopLeftCorner,
    MiddleTopEdge,
    TopRightCorner,
    Center,
    MiddleRightEdge,
    BottomRightCorner,
    MiddleBottomEdge,
    BottomLeftCorner,
    MiddleLeftEdge,
    UndefinedPosModifier = -1
};

/** Position unit enumerator for \em "Add Text" and \em "Add Image" effects.
  * It's correlated position combo boxes in EffectsScrollArea.
  * \sa PosUnitPair PosModifier
  */
enum PosUnit {
    Pixel,
    Percent,
    UndefinedUnit = -1
};

enum Filter {
    NoFilter,
    BlackAndWhite,
    Sepia,
    CustomColor,
    Gradient
};

/** Pair of two PosUnit: first item is X coordinate, second is Y coordinate. */
typedef QPair <PosUnit, PosUnit> PosUnitPair;

//! ConvertThread threads shared information.
class SharedInformation {
    friend class ConvertThread;
    friend class ConvertDialog;
    friend class ConvertEffects;
    friend class ConvertEffectsTest;

public:
    // constructor
    SharedInformation();
    // methods
    void setDesiredSize(int width, int height, bool percent = false,
                        bool widthSet = false, bool heightSet = false,
                        bool keepAspect = true);
    void setDesiredSize(quint32 bytes);
    void setDesiredFormat(const QString& format);
    void setDesiredRotation(bool rotate, double angle = 0.0);
    void setDesiredFlip(int flip);
    void setQuality(int quality);
    void setDestPrefix(const QString& destPrefix);
    void setDestSuffix(const QString& destSuffix);
    void setDestFolder(const QDir& destFolder);
    void setOverwriteAll(bool overwriteAll = false);
#ifdef SIR_METADATA_SUPPORT
    void setMetadataEnabled(bool value);
    void setSaveMetadata(bool value);
    void setRealRotate(bool rotate);
    void setUpdateThumbnail(bool update);
    void setRotateThumbnail(bool rotate);
#endif // SIR_METADATA_SUPPORT

private:
    // fields
    // image data
    // destinated size
    int width; /**< Width of target image in pixels. */
    int height; /**< Height of target image in pixels. */
    bool hasWidth; /**< Width given indicator. */
    bool hasHeight; /**< Height given indicator. */
    bool maintainAspect; /**< Keep/Ignore aspect ratio indicator. */
    quint32 sizeBytes; /**< Size of target image in bytes. */
    /** Size unit code based on size unit combo box index into ConvertDialog. */
    char sizeUnit;
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
    // destinated effects
    /** Custom background color of image.
      * If custom background color is disabled it's invalid QColor object.
      */
    QColor backgroundColor;
    quint8 histogramOperation; // 0 (none), 1 (stretch), 2 (equalize)
    // filter
    int filterType;
    QBrush filterBrush;
    // add frame
    /** Frame width for \em "Add Frame" effect.
      * \sa #frameColor
      */
    int frameWidth;
    /** Frame color for \em "Add Frame" effect.
      * \sa #frameWidth
      */
    QColor frameColor;
    /** A flag for \em "Add Frame" effect.
      *
      * True means the frame will be added around the converting image and it
      * will be extended in coclusion.
      *
      * False means the frame will be painted on the current converting image.
      */
    bool frameAddAround;
    /** Width of border painded on inside edge of the \em "Add Frame" effects frame.
      * \sa #borderInsideColor
      */
    int borderInsideWidth;
    /** Color of border painded on inside edge of the \em "Add Frame" effects frame.
      * \sa #borderInsideWidth
      */
    QColor borderInsideColor;
    /** Width of border painded on outside edge of the \em "Add Frame" effects frame.
      * \sa #borderOutsideColor
      */
    int borderOutsideWidth;
    /** Color of border painded on outside edge of the \em "Add Frame" effects frame.
      * \sa #borderOutsideWidth
      */
    QColor borderOutsideColor;
    // add text
    /** Text for \em "Add Text" effect.
      * \sa #textFont #textColor
      */
    QString textString;
    /** Font for \em "Add Text" effect.
      * \sa #textString #textColor
      */
    QFont textFont;
    /** Font color for \em "Add Text" effect.
      * \sa #textString #textFont
      */
    QColor textColor;
    /** Text position modificator for \em "Add Text" effect.
      * \sa #textPos
      */
    double textOpacity; /**< Text opacity for \em "Add Text" effect. */
    PosModifier textPosModifier;
    /** Text position for \em "Add Text" effect.
      * \sa #textPosModifier #textUnitPair
      */
    QPoint textPos;
    /** \link #textPos Text position \endlink units for \em "Add Text" effect.
      * \sa #textPos
      */
    PosUnitPair textUnitPair;
    /** Drawing text flag for \em "Add Text" effect.
      *
      * True means a rectangle will be drawn around the text.
      */
    bool textFrame;
    /** Text rotation angle in degree for \em "Add Text" effect. */
    int textRotation;
    // add image
    QImage image; /**< Image for \em "Add Image" effect. */
    /** Image position modificator for \em "Add Image" effect.
      * \sa #imagePos
      */
    PosModifier imagePosModifier;
    /** Image position for \em "Add Image" effect.
      * \sa #imagePosModifier #imageUnitPair
      */
    QPoint imagePos;
    /** \link #imagePos Text position \endlink units for \em "Add Image" effect.
      * \sa #imagePos
      */
    PosUnitPair imageUnitPair;
    double imageOpacity; /**< Image opacity for \em "Add Image" effect. */
    /** Image rotation angle in degree for \em "Add Image" effect. */
    int imageRotation;
    // SVG modifiers
    bool svgModifiersEnabled; /**< Set it true if you want modify SVG file. */
    /** Text value of SVG \e text nodes to delete. Plain text and regular
      * expression are supported.
      */
    QString svgRemoveText;
    bool svgRemoveEmptyGroup; /**< Enables removing all empty SVG \e g nodes. */
    bool svgSave; /**< Enables saving changed SVG file in SVG format also. */
#ifdef SIR_METADATA_SUPPORT
    // metadata settings
    bool metadataEnabled; /**< Indicates that metadata support enabled. */
    bool saveMetadata; /**< Saving metadata into target image indicator. */
    /** Real rotation indicator - not save into \em Exif.Image.Orientation field. */
    bool realRotate;
    bool updateThumbnail; /**< Update thumbnail of target image indicator. */
    bool rotateThumbnail; /**< Rotate thumbnail of target image indicator. */
#endif // SIR_METADATA_SUPPORT
    // thread synchronization data
    // mutexes
    /** Mutual exclusion object using for worker threads synchronization. */
    QMutex mutex;
    // user conversation data
    // cancel
    bool abort; /**< Abort indicator. */
    // overwrite
    bool overwriteAll; /**< Overwrite all conflicting files indicator. */
    bool noOverwriteAll; /**< No overwrite all conflicting files indicator. */
    /** Message box containing question about overwriting file result code. */
    int overwriteResult;
    // enlarge
    bool enlargeAll; /**< Enlarge all conflicting files indicator. */
    bool noEnlargeAll; /**< No enlarge all conflicting files indicator. */
    /** Message box containing question about enlarging image result code. */
    int enlargeResult;
};

#endif // SHAREDINFORMATION_H
