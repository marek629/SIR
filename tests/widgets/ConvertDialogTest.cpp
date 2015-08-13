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

#include "ConvertDialogTest.hpp"

#include "fake/CommandLineAssistantFake.hpp"
#include "widgets/MessageBox.hpp"


ConvertDialogTest::ConvertDialogTest()
{
    MessageBox::enableTesting(true);

    CommandLineAssistantFake cmd;
    convertDialog = new ConvertDialog(0, QStringList(), &cmd);
}

ConvertDialogTest::~ConvertDialogTest()
{
    delete convertDialog;
}

void ConvertDialogTest::initTestCase() {}

void ConvertDialogTest::cleanupTestCase() {}

void ConvertDialogTest::convert_defaultPath()
{
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

    QCOMPARE(convertDialog->convertedImages, 0);
    QCOMPARE(convertDialog->numImages, 0);
}

void ConvertDialogTest::convert_histogram_stretch()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *histogramGroupBox = effectsScrollArea->histogramGroupBox;
    histogramGroupBox->setChecked(true);
    effectsScrollArea->stretchHistogramRadioButton->setChecked(true);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getHistogramOperation(), quint8(1));
}

void ConvertDialogTest::convert_histogram_equalize()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *histogramGroupBox = effectsScrollArea->histogramGroupBox;
    histogramGroupBox->setChecked(true);
    effectsScrollArea->equalizeHistogramRadioButton->setChecked(true);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getHistogramOperation(), quint8(2));
}

void ConvertDialogTest::convert_histogram_no()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *histogramGroupBox = effectsScrollArea->histogramGroupBox;
    histogramGroupBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getHistogramOperation(), quint8(0));
}

void ConvertDialogTest::convert_filter_colorBlackAndWhite()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *filterGroupBox = effectsScrollArea->filterGroupBox;
    filterGroupBox->setChecked(true);

    QRadioButton *filterColorRadioButton = effectsScrollArea->filterColorRadioButton;
    filterColorRadioButton->setChecked(true);

    int blackAndWhite = 0;
    QComboBox *filterTypeComboBox = effectsScrollArea->filterTypeComboBox;
    filterTypeComboBox->setCurrentIndex(blackAndWhite);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getFilterType(), int(BlackAndWhite));
    QCOMPARE(conf.getFilterBrush(), QBrush());
}

void ConvertDialogTest::convert_filter_colorSepia()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *filterGroupBox = effectsScrollArea->filterGroupBox;
    filterGroupBox->setChecked(true);

    QRadioButton *filterColorRadioButton = effectsScrollArea->filterColorRadioButton;
    filterColorRadioButton->setChecked(true);

    int sepia = 1;
    QComboBox *filterTypeComboBox = effectsScrollArea->filterTypeComboBox;
    filterTypeComboBox->setCurrentIndex(sepia);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getFilterType(), int(Sepia));
    QCOMPARE(conf.getFilterBrush(), QBrush());
}

void ConvertDialogTest::convert_filter_colorCustom()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *filterGroupBox = effectsScrollArea->filterGroupBox;
    filterGroupBox->setChecked(true);

    QRadioButton *filterColorRadioButton = effectsScrollArea->filterColorRadioButton;
    filterColorRadioButton->setChecked(true);

    int custom = 2;
    QComboBox *filterTypeComboBox = effectsScrollArea->filterTypeComboBox;
    filterTypeComboBox->setCurrentIndex(custom);

    QColor color = Qt::red;
    BrushFrame *filterBrushFrame = effectsScrollArea->filterBrushFrame;
    filterBrushFrame->setColor(color);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getFilterType(), int(CustomColor));
    QCOMPARE(conf.getFilterBrush(), QBrush(color));
}

void ConvertDialogTest::convert_filter_gradient()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *filterGroupBox = effectsScrollArea->filterGroupBox;
    filterGroupBox->setChecked(true);

    QRadioButton *filterGradientRadioButton = effectsScrollArea->filterGradientRadioButton;
    filterGradientRadioButton->setChecked(true);

    QRadialGradient gradient = QRadialGradient(QPoint(1, 2), 2.5);
    gradient.setColorAt(0.0, Qt::green);
    gradient.setColorAt(1.0, Qt::red);
    BrushFrame *filterBrushFrame = effectsScrollArea->filterBrushFrame;
    filterBrushFrame->setBrush(gradient);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getFilterType(), int(Gradient));
    QCOMPARE(conf.getFilterBrush(), QBrush(gradient));
}

void ConvertDialogTest::convert_filter_no()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *filterGroupBox = effectsScrollArea->filterGroupBox;
    filterGroupBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getFilterType(), int(NoFilter));
    QCOMPARE(conf.getFilterBrush(), QBrush());
}

void ConvertDialogTest::convert_addFrame_withBorder()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *frameGroupBox = effectsScrollArea->frameGroupBox;
    frameGroupBox->setChecked(true);

    effectsScrollArea->frameAroundRadioButton->setChecked(true);
    effectsScrollArea->frameWidthSpinBox->setValue(20);
    effectsScrollArea->frameColorFrame->setColor(Qt::red);

    QGroupBox *borderOutsideGroupBox = effectsScrollArea->borderOutsideGroupBox;
    borderOutsideGroupBox->setChecked(true);
    effectsScrollArea->borderOutsideSpinBox->setValue(2);
    effectsScrollArea->borderOutsideColorFrame->setColor(Qt::green);

    QGroupBox *borderInsideGroupBox = effectsScrollArea->borderInsideGroupBox;
    borderInsideGroupBox->setChecked(true);
    effectsScrollArea->borderInsideSpinBox->setValue(3);
    effectsScrollArea->borderInsideColorFrame->setColor(Qt::blue);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getFrameAddAround(), true);
    QCOMPARE(conf.getFrameWidth(), 20);
    QCOMPARE(conf.getFrameColor(), QColor(Qt::red));
    QCOMPARE(conf.getBorderOutsideWidth(), 2);
    QCOMPARE(conf.getBorderOutsideColor(), QColor(Qt::green));
    QCOMPARE(conf.getBorderInsideWidth(), 3);
    QCOMPARE(conf.getBorderInsideColor(), QColor(Qt::blue));
}

void ConvertDialogTest::convert_addFrame_withoutBorder()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *frameGroupBox = effectsScrollArea->frameGroupBox;
    frameGroupBox->setChecked(true);
    QGroupBox *borderOutsideGroupBox = effectsScrollArea->borderOutsideGroupBox;
    borderOutsideGroupBox->setChecked(false);
    QGroupBox *borderInsideGroupBox = effectsScrollArea->borderInsideGroupBox;
    borderInsideGroupBox->setChecked(false);

    effectsScrollArea->frameAroundRadioButton->setChecked(true);
    effectsScrollArea->frameWidthSpinBox->setValue(20);

    QColor color = Qt::red;
    effectsScrollArea->frameColorFrame->setColor(color);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getFrameAddAround(), true);
    QCOMPARE(conf.getFrameWidth(), 20);
    QCOMPARE(conf.getFrameColor(), color);
    QCOMPARE(conf.getBorderOutsideWidth(), -1);
    QCOMPARE(conf.getBorderOutsideColor(), QColor());
    QCOMPARE(conf.getBorderInsideWidth(), -1);
    QCOMPARE(conf.getBorderInsideColor(), QColor());
}

void ConvertDialogTest::convert_addFrame_no()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *frameGroupBox = effectsScrollArea->frameGroupBox;
    frameGroupBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getFrameAddAround(), false);
    QCOMPARE(conf.getFrameWidth(), -1);
    QCOMPARE(conf.getFrameColor(), QColor());
    QCOMPARE(conf.getBorderOutsideWidth(), -1);
    QCOMPARE(conf.getBorderOutsideColor(), QColor());
    QCOMPARE(conf.getBorderInsideWidth(), -1);
    QCOMPARE(conf.getBorderInsideColor(), QColor());
}

void ConvertDialogTest::convert_addText_fontPt()
{
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
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getTextString(), testText);
    QCOMPARE(conf.getTextFont(), font);
    QCOMPARE(conf.getTextColor(), color);
    QCOMPARE(conf.getTextOpacity(), 0.5);
    QCOMPARE(conf.getTextPosModifier(), TopRightCorner);
    QCOMPARE(conf.getTextPos(), position);
    QCOMPARE(conf.getTextUnitPair(), PosUnitPair(Pixel, Pixel));
    QCOMPARE(conf.getTextFrame(), false);
    QCOMPARE(conf.getTextRotation(), 0);
}

void ConvertDialogTest::convert_addText_fontPx()
{
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
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getTextString(), testText);
    QCOMPARE(conf.getTextFont(), font);
    QCOMPARE(conf.getTextColor(), color);
    QCOMPARE(conf.getTextOpacity(), 0.5);
    QCOMPARE(conf.getTextPosModifier(), TopRightCorner);
    QCOMPARE(conf.getTextPos(), position);
    QCOMPARE(conf.getTextUnitPair(), PosUnitPair(Pixel, Pixel));
    QCOMPARE(conf.getTextFrame(), false);
    QCOMPARE(conf.getTextRotation(), 0);
}

void ConvertDialogTest::convert_addText_textIsEmpty()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *textGroupBox = effectsScrollArea->textGroupBox;
    textGroupBox->setChecked(true);
    QLineEdit *textLineEdit = effectsScrollArea->textLineEdit;
    textLineEdit->setText("");

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getTextString(), QString());
    QCOMPARE(conf.getTextFont(), QFont());
    QCOMPARE(conf.getTextColor(), QColor());
    QCOMPARE(conf.getTextPosModifier(), UndefinedPosModifier);
    QCOMPARE(conf.getTextPos(), QPoint());
    QCOMPARE(conf.getTextUnitPair(), PosUnitPair(UndefinedUnit, UndefinedUnit));
    QCOMPARE(conf.getTextFrame(), false);
    QCOMPARE(conf.getTextRotation(), 0);
}

void ConvertDialogTest::convert_addText_no()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *textGroupBox = effectsScrollArea->textGroupBox;
    textGroupBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getTextString(), QString());
    QCOMPARE(conf.getTextFont(), QFont());
    QCOMPARE(conf.getTextColor(), QColor());
    QCOMPARE(conf.getTextPosModifier(), UndefinedPosModifier);
    QCOMPARE(conf.getTextPos(), QPoint());
    QCOMPARE(conf.getTextUnitPair(), PosUnitPair(UndefinedUnit, UndefinedUnit));
    QCOMPARE(conf.getTextFrame(), false);
    QCOMPARE(conf.getTextRotation(), 0);
}

void ConvertDialogTest::convert_addImage_imageNotNull()
{
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
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getImage(), image);
    QCOMPARE(conf.getImageLoadError(), false);
    QCOMPARE(conf.getImagePosModifier(), MiddleTopEdge);
    QCOMPARE(conf.getImagePos(), imagePoint);
    QCOMPARE(conf.getImageUnitPair(), PosUnitPair(Percent, Percent));
    QCOMPARE(conf.getImageOpacity(), 0.5);
    QCOMPARE(conf.getImageRotation(), 40);
}

void ConvertDialogTest::convert_addImage_no()
{
    EffectsScrollArea *effectsScrollArea = convertDialog->effectsScrollArea;
    QGroupBox *imageGroupBox = effectsScrollArea->imageGroupBox;
    imageGroupBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    EffectsConfiguration conf = sharedInfo->effectsConfiguration();
    QCOMPARE(conf.getImage(), QImage());
    QCOMPARE(conf.getImageLoadError(), false);
    QCOMPARE(conf.getImagePosModifier(), UndefinedPosModifier);
    QCOMPARE(conf.getImagePos(), QPoint());
    QCOMPARE(conf.getImageUnitPair(), PosUnitPair(UndefinedUnit, UndefinedUnit));
    QCOMPARE(conf.getImageRotation(), 0);
}

void ConvertDialogTest::convert_svg_removeText()
{
    SvgScrollArea *svgScrollArea = convertDialog->svgScrollArea;
    QString textToRemove = "text to remove";
    svgScrollArea->removeTextCheckBox->setChecked(true);
    svgScrollArea->removeTextLineEdit->setText(textToRemove);
    svgScrollArea->saveCheckBox->setChecked(false);
    svgScrollArea->removeGroupsCheckBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    QCOMPARE(sharedInfo->svgRemoveText(), textToRemove);
    QCOMPARE(sharedInfo->svgRemoveEmptyGroup, false);
    QCOMPARE(sharedInfo->svgSave, false);
    QCOMPARE(sharedInfo->svgModifiersEnabled, true);
}

void ConvertDialogTest::convert_svg_doNotRemoveText()
{
    SvgScrollArea *svgScrollArea = convertDialog->svgScrollArea;
    QString textToRemove = "text to remove";
    svgScrollArea->removeTextCheckBox->setChecked(false);
    svgScrollArea->removeTextLineEdit->setText(textToRemove);
    svgScrollArea->saveCheckBox->setChecked(false);
    svgScrollArea->removeGroupsCheckBox->setChecked(false);

    convertDialog->convert();

    SharedInformation *sharedInfo = convertDialog->sharedInfo;
    QCOMPARE(sharedInfo->svgRemoveText(), QString());
    QCOMPARE(sharedInfo->svgRemoveEmptyGroup, false);
    QCOMPARE(sharedInfo->svgSave, false);
    QCOMPARE(sharedInfo->svgModifiersEnabled, false);
}

QTEST_MAIN(ConvertDialogTest)
#include "ConvertDialogTest.moc"
