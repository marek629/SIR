/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
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

#include <QFileDialog>
#include <QImageReader>
#include <QStringListModel>
#include "effectsscrollarea.h"

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

/** Sets up filters combo box models. */
void EffectsScrollArea::setupFilterModels() {
    QStringList stringList;
    stringList << tr("Black & white") << tr("Sepia") << tr("Custom");
    filterColorModel = new QStringListModel(stringList, this);
    filterColorModelIndex = 0;
    stringList.clear();
    stringList << tr("Linear") << tr("Radial") << tr("Conical");
    filterGradientModel = new QStringListModel(stringList, this);
    filterGradientModelIndex = 0;
}

/** Shows widgets for color filter if \a colorToogled is true. Otherwise shows
  * widgets for gradient filter.
  */
void EffectsScrollArea::filterToogled(bool colorToogled) {
    if (colorToogled) {
        filterTypeLabel->setText(tr("Color"));
        filterGradientModelIndex = filterTypeComboBox->currentIndex();
        if (filterGradientModelIndex < 0)
            filterGradientModelIndex = 0;
        filterTypeComboBox->setModel(filterColorModel);
        filterTypeComboBox->setCurrentIndex(filterColorModelIndex);
        filterGradientWidget->hide();
        filterBrushFrame->setBrushEditable(false);
        filterBrush = filterBrushFrame->brush();
        filterBrushFrame->setColor(filterColor);
    }
    else {
        filterTypeLabel->setText(tr("Gradient"));
        filterColorModelIndex = filterTypeComboBox->currentIndex();
        if (filterColorModelIndex < 0)
            filterColorModelIndex = 0;
        filterTypeComboBox->setModel(filterGradientModel);
        filterTypeComboBox->setCurrentIndex(filterGradientModelIndex);
        filterGradientWidget->show();
        filterBrushFrame->setBrushEditable(true);
        filterColor = filterBrushFrame->color();
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
