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
    EffectsConfiguration conf = sharedInformation->effectsConfiguration();

    EffectsConfiguration *confPtr = &conf;
    confPtr = configureHistogram(confPtr);
    confPtr = configureFilter(confPtr);
    confPtr = configureAddFrame(confPtr);
    confPtr = configureAddText(confPtr);
    confPtr = configureAddImage(confPtr);

    sharedInformation->setEffectsConfiguration(*confPtr);
    return sharedInformation;
}

/** Sets up filters combo box models. */
void EffectsScrollArea::setupFilterModels() {
    filterColorModel = new QStringListModel(colorFilterStringList(), this);
    filterColorModelIndex = 0;
    filterGradientModel = new QStringListModel(gradientFilterStringList(), this);
    filterGradientModelIndex = 0;
}

EffectsConfiguration *EffectsScrollArea::configureHistogram(EffectsConfiguration *conf) {
    if (histogramGroupBox->isChecked()) {
        if (stretchHistogramRadioButton->isChecked())
            conf->setHistogramOperation(1);
        else
            conf->setHistogramOperation(2);
    } else {
        conf->setHistogramOperation(0);
    }
    return conf;
}

EffectsConfiguration *EffectsScrollArea::configureFilter(EffectsConfiguration *conf) {
    if (filterGroupBox->isChecked()) {
        if (filterColorRadioButton->isChecked()) {
            conf->setFilterBrush(QBrush());
            switch (filterTypeComboBox->currentIndex()) {
            case 0:
                conf->setFilterType(BlackAndWhite);
                break;
            case 1:
                conf->setFilterType(Sepia);
                break;
            case 2:
                conf->setFilterType(CustomColor);
                conf->setFilterBrush(
                            QBrush(filterBrushFrame->color()));
                break;
            default:
                break;
            }
        } else {
            conf->setFilterType(Gradient);
            // TODO: strange entries count in code coverage
            conf->setFilterBrush(filterBrushFrame->brush());
        }
    } else {
        conf->setFilterType(NoFilter);
        conf->setFilterBrush(QBrush());
    }
    return conf;
}

EffectsConfiguration *EffectsScrollArea::configureAddFrame(EffectsConfiguration *conf) {
    if (frameGroupBox->isChecked()) {
        conf->setFrameAddAround(
                    frameAroundRadioButton->isChecked());
        conf->setFrameWidth(frameWidthSpinBox->value());
        conf->setFrameColor(frameColorFrame->color());
        if (borderOutsideGroupBox->isChecked()) {
            conf->setBorderOutsideWidth(
                        borderOutsideSpinBox->value());
            conf->setBorderOutsideColor(
                        borderOutsideColorFrame->color());
        } else {
            conf->setBorderOutsideWidth(-1);
            conf->setBorderOutsideColor(QColor());
        }
        if (borderInsideGroupBox->isChecked()) {
            conf->setBorderInsideWidth(
                        borderInsideSpinBox->value());
            conf->setBorderInsideColor(
                        borderInsideColorFrame->color());
        } else {
            conf->setBorderInsideWidth(-1);
            conf->setBorderInsideColor(QColor());
        }
    } else {
        conf->setFrameAddAround(false);
        conf->setFrameWidth(-1);
        conf->setFrameColor(QColor());
        conf->setBorderOutsideWidth(-1);
        conf->setBorderOutsideColor(QColor());
        conf->setBorderInsideWidth(-1);
        conf->setBorderInsideColor(QColor());
    }
    return conf;
}

EffectsConfiguration *EffectsScrollArea::configureAddText(EffectsConfiguration *conf) {
    if (textGroupBox->isChecked() &&
            !textLineEdit->text().isEmpty()) {
        conf->setTextString(textLineEdit->text());
        QFont font = textFontComboBox->currentFont();
        if (textFontSizeComboBox->currentIndex() == 0) // pt
            font.setPointSize(textFontSizeSpinBox->value());
        else
            font.setPixelSize(textFontSizeSpinBox->value());
        font.setBold(textBoldPushButton->isChecked());
        font.setItalic(textItalicPushButton->isChecked());
        font.setUnderline(textUnderlinePushButton->isChecked());
        font.setStrikeOut(textStrikeOutPushButton->isChecked());
        conf->setTextFont(font);
        conf->setTextColor(textColorFrame->color());
        conf->setTextOpacity(textOpacitySpinBox->value());
        conf->setTextPosModifier(
                    static_cast<PosModifier>(
                        textPositionComboBox->currentIndex() ));
        conf->setTextPos(QPoint(
                                          textXSpinBox->value(),
                                          textYSpinBox->value()));
        PosUnitPair posUnitPair = PosUnitPair(
                    static_cast<PosUnit>(textXComboBox->currentIndex()),
                    static_cast<PosUnit>(textYComboBox->currentIndex()));
        conf->setTextUnitPair(posUnitPair);
        conf->setTextFrame(textFrameCheckBox->isChecked());
        conf->setTextRotation(textRotationSpinBox->value());
    } else {
        conf->setTextString(QString());
        conf->setTextFont(QFont());
        conf->setTextColor(QColor());
        conf->setTextPosModifier(UndefinedPosModifier);
        conf->setTextPos(QPoint());
        conf->setTextUnitPair(
                    PosUnitPair(UndefinedUnit, UndefinedUnit));
        conf->setTextFrame(false);
        conf->setTextRotation(0);
    }
    return conf;
}

EffectsConfiguration *EffectsScrollArea::configureAddImage(EffectsConfiguration *conf) {
    if (imageGroupBox->isChecked()) {
        conf->setImage(QImage(imagePathLineEdit->text()));
        conf->setImageLoadError(
                    conf->getImage().isNull());
        conf->setImagePosModifier(
                    static_cast<PosModifier>(
                        imagePositionComboBox->currentIndex() ));
        conf->setImagePos(QPoint(
                                           imageXSpinBox->value(),
                                           imageYSpinBox->value()));
        PosUnitPair posUnitPair = PosUnitPair(
                    static_cast<PosUnit>(imageXComboBox->currentIndex()),
                    static_cast<PosUnit>(imageYComboBox->currentIndex()));
        conf->setImageUnitPair(posUnitPair);
        conf->setImageOpacity(imageOpacitySpinBox->value());
        conf->setImageRotation(imageRotationSpinBox->value());
    } else {
        conf->setImage(QImage());
        conf->setImageLoadError(false);
        conf->setImagePosModifier(UndefinedPosModifier);
        conf->setImagePos(QPoint());
        conf->setImageUnitPair(
                    PosUnitPair(UndefinedUnit, UndefinedUnit));
        conf->setImageRotation(0);
    }
    return conf;
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
