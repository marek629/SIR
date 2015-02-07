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

#include "widgets/convert/EffectsScrollArea.hpp"

#include <QFileDialog>
#include <QImageReader>
#include <QStringListModel>

#include "SharedInformation.hpp"


/** Creates the EffectsScrollArea object. Sets up GUI. */
EffectsScrollArea::EffectsScrollArea(QWidget *parent) : QScrollArea(parent) {
    setupUi(this);

    // create connections
    connect(filterColorRadioButton, SIGNAL(toggled(bool)),
            this, SLOT(filterToogled(bool)) );
    connect(filterTypeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(filterTypeChanged(int)) );
    connect(filterGradientWidget, SIGNAL(gradientChanged()),
            filterBrushFrame, SLOT(changeGradient()) );
    connect(imagePathPushButton, SIGNAL(clicked()), this, SLOT(browseImage()));
    connect(textXComboBox, SIGNAL(currentIndexChanged(QString)),
            textXSpinBox, SLOT(posUnitChanged(QString)) );
    connect(textYComboBox, SIGNAL(currentIndexChanged(QString)),
            textYSpinBox, SLOT(posUnitChanged(QString)) );
    connect(imageXComboBox, SIGNAL(currentIndexChanged(QString)),
            imageXSpinBox, SLOT(posUnitChanged(QString)) );
    connect(imageYComboBox, SIGNAL(currentIndexChanged(QString)),
            imageYSpinBox, SLOT(posUnitChanged(QString)) );

    filterBrushFrame->setColorDialogOptions(0);
    filterBrushFrame->setGradientType(QGradient::LinearGradient);
    filterBrushFrame->setGradientStops(filterGradientWidget->gradientStops());
    filterTypeComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    textColorFrame->setColor(Qt::black);

    // set combo box models
    setupFilterModels();
    filterToogled(filterColorRadioButton->isChecked());
    QAbstractItemModel *posUnitModel = textXComboBox->model();
    textYComboBox->setModel(posUnitModel);
    imagePositionComboBox->setModel(textPositionComboBox->model());
    imageXComboBox->setModel(posUnitModel);
    imageYComboBox->setModel(posUnitModel);

    // set range
    const QPair<int, int> posRange(-20000, 20000);
    textXSpinBox->setRange(posRange.first, posRange.second);
    textYSpinBox->setRange(posRange.first, posRange.second);
    imageXSpinBox->setRange(posRange.first, posRange.second);
    imageYSpinBox->setRange(posRange.first, posRange.second);
    imageRotationSpinBox->setRange(textRotationSpinBox->minimum(),
                                   textRotationSpinBox->maximum());

    // set icons
    textBoldPushButton->setIcon(QIcon::fromTheme("format-text-bold"));
    textItalicPushButton->setIcon(QIcon::fromTheme("format-text-italic"));
    textUnderlinePushButton->setIcon(QIcon::fromTheme("format-text-underline"));
    textStrikeOutPushButton->setIcon(QIcon::fromTheme("format-text-strikethrough"));
}

/** Deletes dynamicaly allocated fields. */
EffectsScrollArea::~EffectsScrollArea() {
    delete filterColorModel;
    delete filterGradientModel;
}

void EffectsScrollArea::retranslateStrings() {
    retranslateUi(this);
    filterGradientWidget->retranslateStrings();

    QStringListModel *model = 0;
    model = static_cast<QStringListModel*>(filterColorModel);
    model->setStringList(colorFilterStringList());
    model = static_cast<QStringListModel*>(filterGradientModel);
    model->setStringList(gradientFilterStringList());

    if (filterColorRadioButton->isChecked())
        filterTypeComboBox->setCurrentIndex(filterColorModelIndex);
    else
        filterTypeComboBox->setCurrentIndex(filterGradientModelIndex);
}

QStringList EffectsScrollArea::colorFilterStringList() {
    QStringList stringList;
    stringList << tr("Black & white") << tr("Sepia") << tr("Custom");
    return stringList;
}

QStringList EffectsScrollArea::gradientFilterStringList() {
    QStringList stringList;
    stringList << tr("Linear") << tr("Radial") << tr("Conical");
    return stringList;
}

SharedInformation *EffectsScrollArea::configureEffects(
        SharedInformation *sharedInformation) {
    sharedInformation = configureHistogram(sharedInformation);
    sharedInformation = configureFilter(sharedInformation);
    sharedInformation = configureAddFrame(sharedInformation);
    sharedInformation = configureAddText(sharedInformation);
    sharedInformation = configureAddImage(sharedInformation);
    return sharedInformation;
}

/** Sets up filters combo box models. */
void EffectsScrollArea::setupFilterModels() {
    filterColorModel = new QStringListModel(colorFilterStringList(), this);
    filterColorModelIndex = 0;
    filterGradientModel = new QStringListModel(gradientFilterStringList(), this);
    filterGradientModelIndex = 0;
}

SharedInformation *EffectsScrollArea::configureHistogram(
        SharedInformation *sharedInformation) {
    if (histogramGroupBox->isChecked()) {
        if (stretchHistogramRadioButton->isChecked())
            sharedInformation->setHistogramOperation(1);
        else
            sharedInformation->setHistogramOperation(2);
    } else {
        sharedInformation->setHistogramOperation(0);
    }
    return sharedInformation;
}

SharedInformation *EffectsScrollArea::configureFilter(
        SharedInformation *sharedInformation) {
    if (filterGroupBox->isChecked()) {
        if (filterColorRadioButton->isChecked()) {
            sharedInformation->setFilterBrush(QBrush());
            switch (filterTypeComboBox->currentIndex()) {
            case 0:
                sharedInformation->setFilterType(BlackAndWhite);
                break;
            case 1:
                sharedInformation->setFilterType(Sepia);
                break;
            case 2:
                sharedInformation->setFilterType(CustomColor);
                sharedInformation->setFilterBrush(
                            QBrush(filterBrushFrame->color()));
                break;
            default:
                break;
            }
        } else {
            sharedInformation->setFilterType(Gradient);
            // TODO: strange entries count in code coverage
            sharedInformation->setFilterBrush(filterBrushFrame->brush());
        }
    } else {
        sharedInformation->setFilterType(NoFilter);
        sharedInformation->setFilterBrush(QBrush());
    }
    return sharedInformation;
}

SharedInformation *EffectsScrollArea::configureAddFrame(
        SharedInformation *sharedInformation) {
    if (frameGroupBox->isChecked()) {
        sharedInformation->setFrameAddAround(
                    frameAroundRadioButton->isChecked());
        sharedInformation->setFrameWidth(frameWidthSpinBox->value());
        sharedInformation->setFrameColor(frameColorFrame->color());
        if (borderOutsideGroupBox->isChecked()) {
            sharedInformation->setBorderOutsideWidth(
                        borderOutsideSpinBox->value());
            sharedInformation->setBorderOutsideColor(
                        borderOutsideColorFrame->color());
        } else {
            sharedInformation->setBorderOutsideWidth(-1);
            sharedInformation->setBorderOutsideColor(QColor());
        }
        if (borderInsideGroupBox->isChecked()) {
            sharedInformation->setBorderInsideWidth(
                        borderInsideSpinBox->value());
            sharedInformation->setBorderInsideColor(
                        borderInsideColorFrame->color());
        } else {
            sharedInformation->setBorderInsideWidth(-1);
            sharedInformation->setBorderInsideColor(QColor());
        }
    } else {
        sharedInformation->setFrameAddAround(false);
        sharedInformation->setFrameWidth(-1);
        sharedInformation->setFrameColor(QColor());
        sharedInformation->setBorderOutsideWidth(-1);
        sharedInformation->setBorderOutsideColor(QColor());
        sharedInformation->setBorderInsideWidth(-1);
        sharedInformation->setBorderInsideColor(QColor());
    }
    return sharedInformation;
}

SharedInformation *EffectsScrollArea::configureAddText(
        SharedInformation *sharedInformation) {
    if (textGroupBox->isChecked() &&
            !textLineEdit->text().isEmpty()) {
        sharedInformation->setTextString(textLineEdit->text());
        QFont font = textFontComboBox->currentFont();
        if (textFontSizeComboBox->currentIndex() == 0) // pt
            font.setPointSize(textFontSizeSpinBox->value());
        else
            font.setPixelSize(textFontSizeSpinBox->value());
        font.setBold(textBoldPushButton->isChecked());
        font.setItalic(textItalicPushButton->isChecked());
        font.setUnderline(textUnderlinePushButton->isChecked());
        font.setStrikeOut(textStrikeOutPushButton->isChecked());
        sharedInformation->setTextFont(font);
        sharedInformation->setTextColor(textColorFrame->color());
        sharedInformation->setTextOpacity(textOpacitySpinBox->value());
        sharedInformation->setTextPosModifier(
                    static_cast<PosModifier>(
                        textPositionComboBox->currentIndex() ));
        sharedInformation->setTextPos(QPoint(
                                          textXSpinBox->value(),
                                          textYSpinBox->value()));
        PosUnitPair posUnitPair = PosUnitPair(
                    static_cast<PosUnit>(textXComboBox->currentIndex()),
                    static_cast<PosUnit>(textYComboBox->currentIndex()));
        sharedInformation->setTextUnitPair(posUnitPair);
        sharedInformation->setTextFrame(textFrameCheckBox->isChecked());
        sharedInformation->setTextRotation(textRotationSpinBox->value());
    } else {
        sharedInformation->setTextString(QString());
        sharedInformation->setTextFont(QFont());
        sharedInformation->setTextColor(QColor());
        sharedInformation->setTextPosModifier(UndefinedPosModifier);
        sharedInformation->setTextPos(QPoint());
        sharedInformation->setTextUnitPair(
                    PosUnitPair(UndefinedUnit, UndefinedUnit));
        sharedInformation->setTextFrame(false);
        sharedInformation->setTextRotation(0);
    }
    return sharedInformation;
}

SharedInformation *EffectsScrollArea::configureAddImage(
        SharedInformation *sharedInformation) {
    if (imageGroupBox->isChecked()) {
        sharedInformation->setImage(QImage(imagePathLineEdit->text()));
        sharedInformation->setImageLoadError(
                    sharedInformation->getImage().isNull());
        sharedInformation->setImagePosModifier(
                    static_cast<PosModifier>(
                        imagePositionComboBox->currentIndex() ));
        sharedInformation->setImagePos(QPoint(
                                           imageXSpinBox->value(),
                                           imageYSpinBox->value()));
        PosUnitPair posUnitPair = PosUnitPair(
                    static_cast<PosUnit>(imageXComboBox->currentIndex()),
                    static_cast<PosUnit>(imageYComboBox->currentIndex()));
        sharedInformation->setImageUnitPair(posUnitPair);
        sharedInformation->setImageOpacity(imageOpacitySpinBox->value());
        sharedInformation->setImageRotation(imageRotationSpinBox->value());
    } else {
        sharedInformation->setImage(QImage());
        sharedInformation->setImageLoadError(false);
        sharedInformation->setImagePosModifier(UndefinedPosModifier);
        sharedInformation->setImagePos(QPoint());
        sharedInformation->setImageUnitPair(
                    PosUnitPair(UndefinedUnit, UndefinedUnit));
        sharedInformation->setImageRotation(0);
    }
    return sharedInformation;
}

/** Shows widgets for color filter if \a colorToogled is true. Otherwise shows
  * widgets for gradient filter.
  */
void EffectsScrollArea::filterToogled(bool colorToogled) {
    if (colorToogled) {
        filterBrush = filterBrushFrame->brush();
        filterGradientModelIndex = filterTypeComboBox->currentIndex();
        filterTypeLabel->setText(tr("Color"));
        if (filterGradientModelIndex < 0)
            filterGradientModelIndex = 0;
        filterTypeComboBox->setModel(filterColorModel);
        filterTypeComboBox->setCurrentIndex(filterColorModelIndex);
        filterGradientWidget->hide();
        filterBrushFrame->setBrushEditable(false);
        filterBrushFrame->setColor(filterColor);
    }
    else {
        filterColor = filterBrushFrame->color();
        filterColorModelIndex = filterTypeComboBox->currentIndex();
        filterTypeLabel->setText(tr("Gradient"));
        if (filterColorModelIndex < 0)
            filterColorModelIndex = 0;
        filterTypeComboBox->setModel(filterGradientModel);
        filterTypeComboBox->setCurrentIndex(filterGradientModelIndex);
        filterGradientWidget->show();
        filterBrushFrame->setBrushEditable(true);
        filterBrushFrame->setBrush(filterBrush);
    }
}

void EffectsScrollArea::filterTypeChanged(int type) {
    if (filterColorRadioButton->isChecked()) {
        // show color filters widgets
        switch (type) {
        case 0:
        case 1:
            filterBrushFrame->hide();
            break;
        case 2:
        default:
            filterBrushFrame->show();
            break;
        }
    }
    else {
        // show gradient filters widgets
        filterBrushFrame->show();

        switch (type) {
        case 0: {
            filterBrushFrame->setGradientType(QGradient::LinearGradient);
            break;
        }
        case 1: {
            filterBrushFrame->setGradientType(QGradient::RadialGradient);
            break;
        }
        case 2: {
            filterBrushFrame->setGradientType(QGradient::ConicalGradient);
            break;
        }
        default:
            qDebug("EffectsScrollArea::filterTypeChanged(): unsupported type %d", type);
            return;
        }
    }
}

/** Browses image file to open and sets the new path to image path line edit. */
void EffectsScrollArea::browseImage() {
    QString path = imagePathLineEdit->text();
    QFileInfo oldImageFileInfo(path);
    if (oldImageFileInfo.exists())
        path = oldImageFileInfo.absoluteDir().absolutePath();
    else
        path = QDir::homePath();
    QString formats;
    foreach (QByteArray format, QImageReader::supportedImageFormats())
        formats += "*." + format + ' ';
    formats.resize(formats.size()-1);

    path = QFileDialog::getOpenFileName(this, tr("Choose Image File"), path,
                                        tr("Images")+" ("+formats+")");
    if (!path.isEmpty())
        imagePathLineEdit->setText(path);
}
