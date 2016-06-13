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

#include "ConvertDialogTest.hpp"

#include "Settings.hpp"
#include "fake/CommandLineAssistantFake.hpp"
#include "widgets/MessageBox.hpp"


ConvertDialogTest::ConvertDialogTest()
{
    MessageBox::enableTesting(true);

    CommandLineAssistantFake cmd;
    convertDialog = new ConvertDialog(0, QStringList(), &cmd);

    Settings *settings = Settings::instance();
    settings->readSettings();
    backupLastDir = settings->settings.lastDir;
}

ConvertDialogTest::~ConvertDialogTest()
{
    delete convertDialog;

    Settings *settings = Settings::instance();
    settings->settings.lastDir = backupLastDir;
    settings->writeSettings();
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
    QCOMPARE(sharedInfo->targetImage.quality(),
             convertDialog->optionsScrollArea->qualitySliderBox->value());
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

void ConvertDialogTest::test_closeOrCancel_close_saveSettings_lastDir_data()
{
    QString lastDirPattern = QDir::homePath() + QDir::separator()
            + "%1" + QDir::separator();

    QTest::addColumn<QString>("loadLastDir");
    QTest::addColumn<QString>("writeLastDir");

    QTest::newRow("same old and new last directory")
            << lastDirPattern.arg("dir1") << lastDirPattern.arg("dir1");
    QTest::newRow("diffrent old and new last directory")
            << lastDirPattern.arg("dir1") << lastDirPattern.arg("dir2");
}

void ConvertDialogTest::test_closeOrCancel_close_saveSettings_lastDir()
{
    QFETCH(QString, loadLastDir);
    QFETCH(QString, writeLastDir);

    Settings *settings = Settings::instance();

    settings->settings.lastDir = loadLastDir;
    settings->writeSettings();

    settings->settings.lastDir = writeLastDir;

    convertDialog->converting = false;
    convertDialog->closeOrCancel();

    settings->readSettings();
    QCOMPARE(settings->settings.lastDir, writeLastDir);
}

void ConvertDialogTest::test_closeOrCancel_close_saveSettings_isMainWindowMaximized_data()
{
    QTest::addColumn<bool>("loadMaximized");
    QTest::addColumn<bool>("writeMaximized");

    QTest::newRow("is not maximized at all")
            << false << false;
    QTest::newRow("is not maximized on start, is maximized on close")
            << false << true;
    QTest::newRow("is maximized at all")
            << true << true;
    QTest::newRow("is maximized on start, is not maximized on close")
            << true << false;
}

void ConvertDialogTest::test_closeOrCancel_close_saveSettings_isMainWindowMaximized()
{
    QFETCH(bool, loadMaximized);
    QFETCH(bool, writeMaximized);

    Settings *settings = Settings::instance();

    settings->mainWindow.maximized = loadMaximized;
    settings->writeSettings();

    if (writeMaximized) {
        convertDialog->window()->showMaximized();
    }
    else {
        convertDialog->window()->showNormal();
    }

    convertDialog->converting = false;
    convertDialog->closeOrCancel();

    settings->readSettings();
    QCOMPARE(settings->mainWindow.maximized, writeMaximized);
}

void ConvertDialogTest::test_closeOrCancel_close_saveSettings_mainWindowPosition_data()
{
    QPoint topLeftCornerPosition(0, 0);
    QPoint testPosition1(20, 30);
    QPoint testPosition2(35, 100);

    QTest::addColumn<QPoint>("loadPosition");
    QTest::addColumn<QPoint>("writePosition");

    QTest::newRow("top left corner at all")
            << topLeftCornerPosition << topLeftCornerPosition;
    QTest::newRow("top left corner was moved bottom-right")
            << topLeftCornerPosition << testPosition1;
    QTest::newRow("initial position was moved to top-left")
            << testPosition2 << testPosition1;
    QTest::newRow("initial position was moved to bottom-right")
            << testPosition1 << testPosition2;
    QTest::newRow("initial position was not changed")
            << testPosition1 << testPosition1;
    QTest::newRow("initial position was changed top left corner")
            << testPosition1 << topLeftCornerPosition;
}

void ConvertDialogTest::test_closeOrCancel_close_saveSettings_mainWindowPosition()
{
    QFETCH(QPoint, loadPosition);
    QFETCH(QPoint, writePosition);

    Settings *settings = Settings::instance();

    settings->mainWindow.possition = loadPosition;
    settings->writeSettings();

    convertDialog->move(writePosition);

    convertDialog->converting = false;
    convertDialog->closeOrCancel();

    settings->readSettings();
    QCOMPARE(settings->mainWindow.possition, writePosition);
}

void ConvertDialogTest::test_closeOrCancel_close_saveSettings_mainWindowSplitters_data()
{
    QTest::addColumn<int>("loadHorizontalSplitterFirstWidth");
    QTest::addColumn<int>("writeHorizontalSplitterFirstWidth");
    QTest::addColumn<int>("loadVericalSplitterFirstWidth");
    QTest::addColumn<int>("writeVerticalSplitterFirstWidth");

    QTest::newRow("no move")
            << 100 << 100 << 100 << 100;
    QTest::newRow("horizontal splitter was moved")
            << 100 << 120 << 100 << 100;
    QTest::newRow("vertical splitter was moved")
            << 100 << 100 << 100 << 120;
    QTest::newRow("horizontal and vertical splitters was moved")
            << 100 << 120 << 100 << 120;
}

void ConvertDialogTest::test_closeOrCancel_close_saveSettings_mainWindowSplitters()
{
    QFETCH(int, loadHorizontalSplitterFirstWidth);
    QFETCH(int, writeHorizontalSplitterFirstWidth);
    QFETCH(int, loadVericalSplitterFirstWidth);
    QFETCH(int, writeVerticalSplitterFirstWidth);

    Settings *settings = Settings::instance();

    QList<int> horizontalSizes;
    horizontalSizes << loadHorizontalSplitterFirstWidth;
    QSplitter *horizontalSplitter = convertDialog->horizontalSplitter;
    horizontalSplitter->setSizes(horizontalSizes);

    QList<int> verticalSizes;
    verticalSizes << loadVericalSplitterFirstWidth;
    QSplitter *verticalSplitter = convertDialog->verticalSplitter;
    verticalSplitter->setSizes(verticalSizes);

    settings->mainWindow.horizontalSplitter = horizontalSplitter->saveState();
    settings->mainWindow.verticalSplitter = verticalSplitter->saveState();
    settings->writeSettings();

    horizontalSizes.clear();
    horizontalSizes << writeHorizontalSplitterFirstWidth;
    horizontalSplitter->setSizes(horizontalSizes);

    verticalSizes.clear();
    verticalSizes << writeVerticalSplitterFirstWidth;
    verticalSplitter->setSizes(verticalSizes);

    convertDialog->converting = false;
    convertDialog->closeOrCancel();

    settings->readSettings();
    QCOMPARE(settings->mainWindow.horizontalSplitter, horizontalSplitter->saveState());
    QCOMPARE(settings->mainWindow.verticalSplitter, verticalSplitter->saveState());
}

void ConvertDialogTest::test_closeOrCancel_close_saveSettings_mainWindowSize_data()
{
    QTest::addColumn<QSize>("loadSize");
    QTest::addColumn<QSize>("writeSize");

    QTest::newRow("is not changed")
            << QSize(800, 600) << QSize(800, 600);
    QTest::newRow("is resized up")
            << QSize(800, 600) << QSize(1000, 700);
    QTest::newRow("is resized down")
            << QSize(800, 700) << QSize(650, 600);
    QTest::newRow("is resized width down")
            << QSize(800, 600) << QSize(700, 600);
}

void ConvertDialogTest::test_closeOrCancel_close_saveSettings_mainWindowSize()
{
    QFETCH(QSize, loadSize);
    QFETCH(QSize, writeSize);

    Settings *settings = Settings::instance();

    settings->mainWindow.size = loadSize;
    settings->writeSettings();

    convertDialog->resize(writeSize);

    convertDialog->converting = false;
    convertDialog->closeOrCancel();

    settings->readSettings();
    QCOMPARE(settings->mainWindow.size, writeSize);
}

QTEST_MAIN(ConvertDialogTest)
#include "ConvertDialogTest.moc"
