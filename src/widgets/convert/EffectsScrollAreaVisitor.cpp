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

#include "widgets/convert/EffectsScrollAreaVisitor.hpp"

#include <typeinfo>

#include "widgets/convert/EffectsScrollArea.hpp"


void EffectsScrollAreaVisitor::visit(Visitable *visitable) {
    Q_ASSERT(typeid(*visitable) == typeid(EffectsScrollArea));

    EffectsScrollArea *effectsScrollArea =
            static_cast<EffectsScrollArea *>(visitable);

    configureHistogram(effectsScrollArea);
    configureFilter(effectsScrollArea);
    configureAddFrame(effectsScrollArea);
    configureAddText(effectsScrollArea);
    configureAddImage(effectsScrollArea);
}

EffectsConfiguration EffectsScrollAreaVisitor::effectsConfiguration() const {
    return conf;
}

void EffectsScrollAreaVisitor::configureHistogram(EffectsScrollArea *area) {
    if (area->histogramGroupBox->isChecked()) {
        if (area->stretchHistogramRadioButton->isChecked())
            conf.setHistogramOperation(1);
        else
            conf.setHistogramOperation(2);
    } else {
        conf.setHistogramOperation(0);
    }
}

void EffectsScrollAreaVisitor::configureFilter(EffectsScrollArea *area) {
    if (area->filterGroupBox->isChecked()) {
        if (area->filterColorRadioButton->isChecked()) {
            conf.setFilterBrush(QBrush());
            switch (area->filterTypeComboBox->currentIndex()) {
            case 0:
                conf.setFilterType(BlackAndWhite);
                break;
            case 1:
                conf.setFilterType(Sepia);
                break;
            case 2:
                conf.setFilterType(CustomColor);
                conf.setFilterBrush(
                            QBrush(area->filterBrushFrame->color()));
                break;
            default:
                break;
            }
        } else {
            conf.setFilterType(Gradient);
            // TODO: strange entries count in code coverage
            conf.setFilterBrush(area->filterBrushFrame->brush());
        }
    } else {
        conf.setFilterType(NoFilter);
        conf.setFilterBrush(QBrush());
    }
}

void EffectsScrollAreaVisitor::configureAddFrame(EffectsScrollArea *area) {
    if (area->frameGroupBox->isChecked()) {
        conf.setFrameAddAround(
                    area->frameAroundRadioButton->isChecked());
        conf.setFrameWidth(area->frameWidthSpinBox->value());
        conf.setFrameColor(area->frameColorFrame->color());
        if (area->borderOutsideGroupBox->isChecked()) {
            conf.setBorderOutsideWidth(
                        area->borderOutsideSpinBox->value());
            conf.setBorderOutsideColor(
                        area->borderOutsideColorFrame->color());
        } else {
            conf.setBorderOutsideWidth(-1);
            conf.setBorderOutsideColor(QColor());
        }
        if (area->borderInsideGroupBox->isChecked()) {
            conf.setBorderInsideWidth(
                        area->borderInsideSpinBox->value());
            conf.setBorderInsideColor(
                        area->borderInsideColorFrame->color());
        } else {
            conf.setBorderInsideWidth(-1);
            conf.setBorderInsideColor(QColor());
        }
    } else {
        conf.setFrameAddAround(false);
        conf.setFrameWidth(-1);
        conf.setFrameColor(QColor());
        conf.setBorderOutsideWidth(-1);
        conf.setBorderOutsideColor(QColor());
        conf.setBorderInsideWidth(-1);
        conf.setBorderInsideColor(QColor());
    }
}

void EffectsScrollAreaVisitor::configureAddText(EffectsScrollArea *area) {
    if (area->textGroupBox->isChecked() &&
            !area->textLineEdit->text().isEmpty()) {
        conf.setTextString(area->textLineEdit->text());
        QFont font = area->textFontComboBox->currentFont();
        if (area->textFontSizeComboBox->currentIndex() == 0) // pt
            font.setPointSize(area->textFontSizeSpinBox->value());
        else
            font.setPixelSize(area->textFontSizeSpinBox->value());
        font.setBold(area->textBoldPushButton->isChecked());
        font.setItalic(area->textItalicPushButton->isChecked());
        font.setUnderline(area->textUnderlinePushButton->isChecked());
        font.setStrikeOut(area->textStrikeOutPushButton->isChecked());
        conf.setTextFont(font);
        conf.setTextColor(area->textColorFrame->color());
        conf.setTextOpacity(area->textOpacitySpinBox->value());
        conf.setTextPosModifier(
                    static_cast<PosModifier>(
                        area->textPositionComboBox->currentIndex() ));
        conf.setTextPos(QPoint(
                                          area->textXSpinBox->value(),
                                          area->textYSpinBox->value()));
        PosUnitPair posUnitPair = PosUnitPair(
                    static_cast<PosUnit>(area->textXComboBox->currentIndex()),
                    static_cast<PosUnit>(area->textYComboBox->currentIndex()));
        conf.setTextUnitPair(posUnitPair);
        conf.setTextFrame(area->textFrameCheckBox->isChecked());
        conf.setTextRotation(area->textRotationSpinBox->value());
    } else {
        conf.setTextString(QString());
        conf.setTextFont(QFont());
        conf.setTextColor(QColor());
        conf.setTextPosModifier(UndefinedPosModifier);
        conf.setTextPos(QPoint());
        conf.setTextUnitPair(
                    PosUnitPair(UndefinedUnit, UndefinedUnit));
        conf.setTextFrame(false);
        conf.setTextRotation(0);
    }
}

void EffectsScrollAreaVisitor::configureAddImage(EffectsScrollArea *area) {
    if (area->imageGroupBox->isChecked()) {
        conf.setImage(QImage(area->imagePathLineEdit->text()));
        conf.setImageLoadError(
                    conf.getImage().isNull());
        conf.setImagePosModifier(
                    static_cast<PosModifier>(
                        area->imagePositionComboBox->currentIndex() ));
        conf.setImagePos(QPoint(
                                           area->imageXSpinBox->value(),
                                           area->imageYSpinBox->value()));
        PosUnitPair posUnitPair = PosUnitPair(
                    static_cast<PosUnit>(area->imageXComboBox->currentIndex()),
                    static_cast<PosUnit>(area->imageYComboBox->currentIndex()));
        conf.setImageUnitPair(posUnitPair);
        conf.setImageOpacity(area->imageOpacitySpinBox->value());
        conf.setImageRotation(area->imageRotationSpinBox->value());
    } else {
        conf.setImage(QImage());
        conf.setImageLoadError(false);
        conf.setImagePosModifier(UndefinedPosModifier);
        conf.setImagePos(QPoint());
        conf.setImageUnitPair(
                    PosUnitPair(UndefinedUnit, UndefinedUnit));
        conf.setImageRotation(0);
    }
}
