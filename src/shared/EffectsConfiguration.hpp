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

#ifndef EFFECTSCONFIGURATION_HPP
#define EFFECTSCONFIGURATION_HPP

#include <QBrush>
#include <QFont>

#include "shared/Enums.hpp"


class EffectsConfiguration {
public:
    EffectsConfiguration();

    quint8 getHistogramOperation() const;
    void setHistogramOperation(const quint8 &value);

    int getFilterType() const;
    void setFilterType(int value);

    QBrush getFilterBrush() const;
    void setFilterBrush(const QBrush &value);

    int getFrameWidth() const;
    void setFrameWidth(int value);

    QColor getFrameColor() const;
    void setFrameColor(const QColor &value);

    bool getFrameAddAround() const;
    void setFrameAddAround(bool value);

    int getBorderInsideWidth() const;
    void setBorderInsideWidth(int value);

    QColor getBorderInsideColor() const;
    void setBorderInsideColor(const QColor &value);

    int getBorderOutsideWidth() const;
    void setBorderOutsideWidth(int value);

    QColor getBorderOutsideColor() const;
    void setBorderOutsideColor(const QColor &value);

    QString getTextString() const;
    void setTextString(const QString &value);

    QFont getTextFont() const;
    void setTextFont(const QFont &value);

    QColor getTextColor() const;
    void setTextColor(const QColor &value);

    double getTextOpacity() const;
    void setTextOpacity(double value);

    PosModifier getTextPosModifier() const;
    void setTextPosModifier(const PosModifier &value);

    QPoint getTextPos() const;
    void setTextPos(const QPoint &value);

    PosUnitPair getTextUnitPair() const;
    void setTextUnitPair(const PosUnitPair &value);

    bool getTextFrame() const;
    void setTextFrame(bool value);

    int getTextRotation() const;
    void setTextRotation(int value);

    QImage getImage() const;
    void setImage(const QImage &value);

    bool getImageLoadError() const;
    void setImageLoadError(bool value);

    PosModifier getImagePosModifier() const;
    void setImagePosModifier(const PosModifier &value);

    QPoint getImagePos() const;
    void setImagePos(const QPoint &value);

    PosUnitPair getImageUnitPair() const;
    void setImageUnitPair(const PosUnitPair &value);

    double getImageOpacity() const;
    void setImageOpacity(double value);

    int getImageRotation() const;
    void setImageRotation(int value);


private:
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
    bool imageLoadError;
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
};

#endif // EFFECTSCONFIGURATION_HPP
