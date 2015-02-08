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

#include "EffectsConfiguration.hpp"

EffectsConfiguration::EffectsConfiguration() {}

quint8 EffectsConfiguration::getHistogramOperation() const {
    return histogramOperation;
}

void EffectsConfiguration::setHistogramOperation(const quint8 &value) {
    histogramOperation = value;
}

int EffectsConfiguration::getFilterType() const {
    return filterType;
}

void EffectsConfiguration::setFilterType(int value) {
    filterType = value;
}

QBrush EffectsConfiguration::getFilterBrush() const {
    return filterBrush;
}

void EffectsConfiguration::setFilterBrush(const QBrush &value) {
    filterBrush = value;
}

int EffectsConfiguration::getFrameWidth() const {
    return frameWidth;
}

void EffectsConfiguration::setFrameWidth(int value) {
    frameWidth = value;
}

QColor EffectsConfiguration::getFrameColor() const {
    return frameColor;
}

void EffectsConfiguration::setFrameColor(const QColor &value) {
    frameColor = value;
}

bool EffectsConfiguration::getFrameAddAround() const {
    return frameAddAround;
}

void EffectsConfiguration::setFrameAddAround(bool value) {
    frameAddAround = value;
}

int EffectsConfiguration::getBorderInsideWidth() const {
    return borderInsideWidth;
}

void EffectsConfiguration::setBorderInsideWidth(int value) {
    borderInsideWidth = value;
}

QColor EffectsConfiguration::getBorderInsideColor() const {
    return borderInsideColor;
}

void EffectsConfiguration::setBorderInsideColor(const QColor &value) {
    borderInsideColor = value;
}

int EffectsConfiguration::getBorderOutsideWidth() const {
    return borderOutsideWidth;
}

void EffectsConfiguration::setBorderOutsideWidth(int value) {
    borderOutsideWidth = value;
}

QColor EffectsConfiguration::getBorderOutsideColor() const {
    return borderOutsideColor;
}

void EffectsConfiguration::setBorderOutsideColor(const QColor &value) {
    borderOutsideColor = value;
}

QString EffectsConfiguration::getTextString() const {
    return textString;
}

void EffectsConfiguration::setTextString(const QString &value) {
    textString = value;
}

QFont EffectsConfiguration::getTextFont() const {
    return textFont;
}

void EffectsConfiguration::setTextFont(const QFont &value) {
    textFont = value;
}

QColor EffectsConfiguration::getTextColor() const {
    return textColor;
}

void EffectsConfiguration::setTextColor(const QColor &value) {
    textColor = value;
}

double EffectsConfiguration::getTextOpacity() const {
    return textOpacity;
}

void EffectsConfiguration::setTextOpacity(double value) {
    textOpacity = value;
}

PosModifier EffectsConfiguration::getTextPosModifier() const {
    return textPosModifier;
}

void EffectsConfiguration::setTextPosModifier(const PosModifier &value) {
    textPosModifier = value;
}

QPoint EffectsConfiguration::getTextPos() const {
    return textPos;
}

void EffectsConfiguration::setTextPos(const QPoint &value) {
    textPos = value;
}
PosUnitPair EffectsConfiguration::getTextUnitPair() const {
    return textUnitPair;
}

void EffectsConfiguration::setTextUnitPair(const PosUnitPair &value) {
    textUnitPair = value;
}

bool EffectsConfiguration::getTextFrame() const {
    return textFrame;
}

void EffectsConfiguration::setTextFrame(bool value) {
    textFrame = value;
}
int EffectsConfiguration::getTextRotation() const {
    return textRotation;
}

void EffectsConfiguration::setTextRotation(int value) {
    textRotation = value;
}

QImage EffectsConfiguration::getImage() const {
    return image;
}

void EffectsConfiguration::setImage(const QImage &value) {
    image = value;
}
bool EffectsConfiguration::getImageLoadError() const {
    return imageLoadError;
}

void EffectsConfiguration::setImageLoadError(bool value) {
    imageLoadError = value;
}

PosModifier EffectsConfiguration::getImagePosModifier() const {
    return imagePosModifier;
}

void EffectsConfiguration::setImagePosModifier(const PosModifier &value) {
    imagePosModifier = value;
}

QPoint EffectsConfiguration::getImagePos() const {
    return imagePos;
}

void EffectsConfiguration::setImagePos(const QPoint &value) {
    imagePos = value;
}

PosUnitPair EffectsConfiguration::getImageUnitPair() const {
    return imageUnitPair;
}

void EffectsConfiguration::setImageUnitPair(const PosUnitPair &value) {
    imageUnitPair = value;
}

double EffectsConfiguration::getImageOpacity() const {
    return imageOpacity;
}

void EffectsConfiguration::setImageOpacity(double value) {
    imageOpacity = value;
}

int EffectsConfiguration::getImageRotation() const {
    return imageRotation;
}

void EffectsConfiguration::setImageRotation(int value) {
    imageRotation = value;
}
