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
#include "effectsscrollarea.h"

/** Creates the EffectsScrollArea object. Sets up GUI. */
EffectsScrollArea::EffectsScrollArea(QWidget *parent) : QScrollArea(parent) {
    setupUi(this);

    // create connections
    connect(imagePathPushButton, SIGNAL(clicked()), this, SLOT(browseImage()));
    connect(textXComboBox, SIGNAL(currentIndexChanged(QString)),
            textXSpinBox, SLOT(posUnitChanged(QString)) );
    connect(textYComboBox, SIGNAL(currentIndexChanged(QString)),
            textYSpinBox, SLOT(posUnitChanged(QString)) );
    connect(imageXComboBox, SIGNAL(currentIndexChanged(QString)),
            imageXSpinBox, SLOT(posUnitChanged(QString)) );
    connect(imageYComboBox, SIGNAL(currentIndexChanged(QString)),
            imageYSpinBox, SLOT(posUnitChanged(QString)) );

    // set combo box models
    QAbstractItemModel *posUnitModel = textXComboBox->model();
    textYComboBox->setModel(posUnitModel);
    textColorFrame->setColor(Qt::black);
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
