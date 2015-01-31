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

#include "ConvertDialogTest.hpp"
#include "fake/CommandLineAssistantFake.hpp"

ConvertDialogTest::ConvertDialogTest() {
    CommandLineAssistantFake cmd;
    convertDialog = new ConvertDialog(0, QStringList(), &cmd);
}

ConvertDialogTest::~ConvertDialogTest() {
    delete convertDialog;
}

void ConvertDialogTest::initTestCase() {}

void ConvertDialogTest::cleanupTestCase() {}

void ConvertDialogTest::convert_defaultPath() {
    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;

    QCOMPARE(convertDialog->convertedImages, 0);
    QCOMPARE(sharedInfo->format,
             convertDialog->targetFormatComboBox->currentText().toLower());
    QCOMPARE(sharedInfo->flip,
             convertDialog->optionsScrollArea->flipComboBox->currentIndex());
//    QCOMPARE(sharedInfo->rotate)
    QCOMPARE(sharedInfo->quality,
             convertDialog->optionsScrollArea->qualitySpinBox->value());
    QCOMPARE(sharedInfo->prefix, convertDialog->destPrefixEdit->text());
    QCOMPARE(sharedInfo->suffix, convertDialog->destSuffixEdit->text());
    QCOMPARE(sharedInfo->overwriteAll, false);
    QCOMPARE(sharedInfo->backgroundColor, QColor());
    QCOMPARE(sharedInfo->histogramOperation, quint8(0));
    QCOMPARE(sharedInfo->filterType, int(NoFilter));
    QCOMPARE(sharedInfo->filterBrush, QBrush());
    QCOMPARE(sharedInfo->frameAddAround, false);
    QCOMPARE(sharedInfo->frameWidth, -1);
    QCOMPARE(sharedInfo->frameColor, QColor());
    QCOMPARE(sharedInfo->borderOutsideWidth, -1);
    QCOMPARE(sharedInfo->borderOutsideColor, QColor());
    QCOMPARE(sharedInfo->borderInsideWidth, -1);
    QCOMPARE(sharedInfo->borderInsideColor, QColor());

    QCOMPARE(convertDialog->convertedImages, 0);
    QCOMPARE(convertDialog->numImages, 0);
}

void ConvertDialogTest::convert_addText_fontPt() {
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *textGroupBox = effectsScrollArea->textGroupBox;
    textGroupBox->setChecked(true);
    QLineEdit *textLineEdit = effectsScrollArea->textLineEdit;
    QString testText = "Test Text";
    textLineEdit->setText(testText);
    QComboBox *fontSizeComboBox = effectsScrollArea->textFontSizeComboBox;
    int pt = 0;
    fontSizeComboBox->setCurrentIndex(pt);

    QFont font = effectsScrollArea->textFontComboBox->currentFont();
    int fontSize = 20;
    font.setPointSize(fontSize);
    font.setBold(true);
    font.setItalic(true);
    font.setUnderline(true);
    font.setStrikeOut(true);

    effectsScrollArea->textFontSizeSpinBox->setValue(fontSize);
    effectsScrollArea->textBoldPushButton->setChecked(font.bold());
    effectsScrollArea->textItalicPushButton->setChecked(font.italic());
    effectsScrollArea->textUnderlinePushButton->setChecked(font.underline());
    effectsScrollArea->textStrikeOutPushButton->setChecked(font.strikeOut());

    QColor color = Qt::green;
    effectsScrollArea->textColorFrame->setColor(color);

    effectsScrollArea->textOpacitySpinBox->setValue(0.5);
    effectsScrollArea->textPositionComboBox->setCurrentIndex(TopRightCorner);

    QPoint position = QPoint(4, 3);
    effectsScrollArea->textXSpinBox->setValue(position.x());
    effectsScrollArea->textYSpinBox->setValue(position.y());

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    QCOMPARE(sharedInfo->textString, testText);
    QCOMPARE(sharedInfo->textFont, font);
    QCOMPARE(sharedInfo->textColor, color);
    QCOMPARE(sharedInfo->textOpacity, 0.5);
    QCOMPARE(sharedInfo->textPosModifier, TopRightCorner);
    QCOMPARE(sharedInfo->textPos, position);
    QCOMPARE(sharedInfo->textUnitPair, PosUnitPair(Pixel, Pixel));
    QCOMPARE(sharedInfo->textFrame, false);
    QCOMPARE(sharedInfo->textRotation, 0);
}

void ConvertDialogTest::convert_addText_fontPx() {
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *textGroupBox = effectsScrollArea->textGroupBox;
    textGroupBox->setChecked(true);
    QLineEdit *textLineEdit = effectsScrollArea->textLineEdit;
    QString testText = "Test Text";
    textLineEdit->setText(testText);
    QComboBox *fontSizeComboBox = effectsScrollArea->textFontSizeComboBox;
    int px = 1;
    fontSizeComboBox->setCurrentIndex(px);

    QFont font = effectsScrollArea->textFontComboBox->currentFont();
    int fontSize = 20;
    font.setPixelSize(fontSize);
    font.setBold(true);
    font.setItalic(true);
    font.setUnderline(true);
    font.setStrikeOut(true);

    effectsScrollArea->textFontSizeSpinBox->setValue(fontSize);
    effectsScrollArea->textBoldPushButton->setChecked(font.bold());
    effectsScrollArea->textItalicPushButton->setChecked(font.italic());
    effectsScrollArea->textUnderlinePushButton->setChecked(font.underline());
    effectsScrollArea->textStrikeOutPushButton->setChecked(font.strikeOut());

    QColor color = Qt::green;
    effectsScrollArea->textColorFrame->setColor(color);

    effectsScrollArea->textOpacitySpinBox->setValue(0.5);
    effectsScrollArea->textPositionComboBox->setCurrentIndex(TopRightCorner);

    QPoint position = QPoint(4, 3);
    effectsScrollArea->textXSpinBox->setValue(position.x());
    effectsScrollArea->textYSpinBox->setValue(position.y());

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    QCOMPARE(sharedInfo->textString, testText);
    QCOMPARE(sharedInfo->textFont, font);
    QCOMPARE(sharedInfo->textColor, color);
    QCOMPARE(sharedInfo->textOpacity, 0.5);
    QCOMPARE(sharedInfo->textPosModifier, TopRightCorner);
    QCOMPARE(sharedInfo->textPos, position);
    QCOMPARE(sharedInfo->textUnitPair, PosUnitPair(Pixel, Pixel));
    QCOMPARE(sharedInfo->textFrame, false);
    QCOMPARE(sharedInfo->textRotation, 0);
}

void ConvertDialogTest::convert_addText_textIsEmpty() {
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *textGroupBox = effectsScrollArea->textGroupBox;
    textGroupBox->setChecked(true);
    QLineEdit *textLineEdit = effectsScrollArea->textLineEdit;
    textLineEdit->setText("");

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    QCOMPARE(sharedInfo->textString, QString());
    QCOMPARE(sharedInfo->textFont, QFont());
    QCOMPARE(sharedInfo->textColor, QColor());
    QCOMPARE(sharedInfo->textPosModifier, UndefinedPosModifier);
    QCOMPARE(sharedInfo->textPos, QPoint());
    QCOMPARE(sharedInfo->textUnitPair, PosUnitPair(UndefinedUnit, UndefinedUnit));
    QCOMPARE(sharedInfo->textFrame, false);
    QCOMPARE(sharedInfo->textRotation, 0);
}

void ConvertDialogTest::convert_addText_no() {
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *textGroupBox = effectsScrollArea->textGroupBox;
    textGroupBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    QCOMPARE(sharedInfo->textString, QString());
    QCOMPARE(sharedInfo->textFont, QFont());
    QCOMPARE(sharedInfo->textColor, QColor());
    QCOMPARE(sharedInfo->textPosModifier, UndefinedPosModifier);
    QCOMPARE(sharedInfo->textPos, QPoint());
    QCOMPARE(sharedInfo->textUnitPair, PosUnitPair(UndefinedUnit, UndefinedUnit));
    QCOMPARE(sharedInfo->textFrame, false);
    QCOMPARE(sharedInfo->textRotation, 0);
}

void ConvertDialogTest::convert_addImage_imageNotNull() {
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *imageGroupBox = effectsScrollArea->imageGroupBox;
    imageGroupBox->setChecked(true);

    QString filePath = QDir::tempPath() + QDir::separator()
            + "sir_test_convert_addImage_imageNotNull.bmp";
    effectsScrollArea->imagePathLineEdit->setText(filePath);
    effectsScrollArea->imagePositionComboBox->setCurrentIndex(MiddleTopEdge);
    QPoint imagePoint(1, 2);
    effectsScrollArea->imageXSpinBox->setValue(imagePoint.x());
    effectsScrollArea->imageYSpinBox->setValue(imagePoint.y());
    effectsScrollArea->imageXComboBox->setCurrentIndex(Percent);
    effectsScrollArea->imageYComboBox->setCurrentIndex(Percent);
    effectsScrollArea->imageOpacitySpinBox->setValue(0.5);
    effectsScrollArea->imageRotationSpinBox->setValue(40);

    QImage image(2, 3, QImage::Format_RGB32);
    image.fill(Qt::green);
    QVERIFY(image.save(filePath));

    convertDialog->convert();

    QFile file(filePath);
    QVERIFY(file.remove());

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    QCOMPARE(sharedInfo->image, image);
    QCOMPARE(sharedInfo->imageLoadError, false);
    QCOMPARE(sharedInfo->imagePosModifier, MiddleTopEdge);
    QCOMPARE(sharedInfo->imagePos, imagePoint);
    QCOMPARE(sharedInfo->imageUnitPair, PosUnitPair(Percent, Percent));
    QCOMPARE(sharedInfo->imageOpacity, 0.5);
    QCOMPARE(sharedInfo->imageRotation, 40);
}

void ConvertDialogTest::convert_addImage_no() {
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *imageGroupBox = effectsScrollArea->imageGroupBox;
    imageGroupBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    QCOMPARE(sharedInfo->image, QImage());
    QCOMPARE(sharedInfo->imageLoadError, false);
    QCOMPARE(sharedInfo->imagePosModifier, UndefinedPosModifier);
    QCOMPARE(sharedInfo->imagePos, QPoint());
    QCOMPARE(sharedInfo->imageUnitPair, PosUnitPair(UndefinedUnit, UndefinedUnit));
    QCOMPARE(sharedInfo->imageRotation, 0);
}

void ConvertDialogTest::convert_svg_removeText() {
    SvgScrollArea *svgScrollArea = convertDialog->svgScrollArea;
    QString textToRemove = "text to remove";
    svgScrollArea->removeTextCheckBox->setChecked(true);
    svgScrollArea->removeTextLineEdit->setText(textToRemove);
    svgScrollArea->saveCheckBox->setChecked(false);
    svgScrollArea->removeGroupsCheckBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    QCOMPARE(sharedInfo->svgRemoveText, textToRemove);
    QCOMPARE(sharedInfo->svgRemoveEmptyGroup, false);
    QCOMPARE(sharedInfo->svgSave, false);
    QCOMPARE(sharedInfo->svgModifiersEnabled, true);
}

void ConvertDialogTest::convert_svg_doNotRemoveText() {
    SvgScrollArea *svgScrollArea = convertDialog->svgScrollArea;
    QString textToRemove = "text to remove";
    svgScrollArea->removeTextCheckBox->setChecked(false);
    svgScrollArea->removeTextLineEdit->setText(textToRemove);
    svgScrollArea->saveCheckBox->setChecked(false);
    svgScrollArea->removeGroupsCheckBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    QCOMPARE(sharedInfo->svgRemoveText, QString());
    QCOMPARE(sharedInfo->svgRemoveEmptyGroup, false);
    QCOMPARE(sharedInfo->svgSave, false);
    QCOMPARE(sharedInfo->svgModifiersEnabled, false);
}

QTEST_MAIN(ConvertDialogTest)
#include "ConvertDialogTest.moc"
