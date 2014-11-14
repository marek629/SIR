/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2014  Marek Jędryka   <jedryka89@gmail.com>
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

#include <QDomElement>
#include <QFile>
#include <QMessageBox>
#include "widgets/ConvertDialog.hpp"
#include "sir_string.h"
#include "xmlstreamwriter.h"
#include "version.h"
#include "effectscollector.h"

using namespace sir;

/** Creates the EffectsCollector object.
  * \param parent Parent convert dialog.
  */
EffectsCollector::EffectsCollector(ConvertDialog *parent) : XmlHelper(parent) {
    effectsArea = convertDialog->effectsScrollArea;
}

EffectsCollector::~EffectsCollector() {}

/** Writes collection of effects to XML file in \a fileName path.
  * \sa restore()
  */
void EffectsCollector::save(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(convertDialog, tr("Session write error"),
                             tr("Session saving to %1 file failed.\n"
                                "Can't open the file.").arg(fileName) );
        return;
    }

    Version version;

    XmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);
    writer.setCodec("UTF-8");
    writer.writeStartDocument();
    writer.writeStartElement("sir");
    writer.writeAttribute("version", version.version());

    write(&writer);

    writer.writeEndElement(); // sir
    writer.writeEndDocument();
}

/** Reads collection of effects from XML file in \a fileName path.
  * \sa save()
  */
void EffectsCollector::restore(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(convertDialog, tr("Effects collection read error"),
                             tr("Effects collection restoring from %1 file failed.\n"
                                "Can't open the file.").arg(fileName) );
        return;
    }

    QDomDocument document;
    if (!document.setContent(&file)) {
        QMessageBox::warning(convertDialog, tr("Effects collection read error"),
                             tr("Effects collection restoring from %1 file failed.\n"
                                "Can't parse the file.").arg(fileName) );
        return;
    }

    QDomElement sir = document.firstChildElement("sir");
    Version version;

    if (sir.isNull()
            || !version.isAtLeast(sir.attribute("version", version.version()))) {
        QMessageBox::warning(convertDialog, tr("Effects collection read error"),
                             tr("Effects collection restoring from %1 file failed.\n"
                                "Invalid file format.").arg(fileName) );
        return;
    }

    read(sir.firstChildElement("effects"));
}

/** Reads collection of effects from DOM \a element.
  * \return True if read succeed, otherwise false.
  * \sa write()
  */
bool EffectsCollector::read(const QDomElement &element) {
    bool result = false;
    if (element.isNull())
        return result;

    result = readHistogramEffect(element);
    result = readFilterEffect(element);
    result = readAddFrameEffect(element);
    result = readAddTextEffect(element);
    result = readAddImageEffect(element);

    return result;
}

/** Writes collection of effects to XML buffer used by \a writer.
  * \sa read()
  */
void EffectsCollector::write(XmlStreamWriter *writer) {
    writer->writeStartElement("effects");

    writeHistogramEffect(writer);
    writeFilterEffect(writer);
    writeAddFrameEffect(writer);
    writeAddTextEffect(writer);
    writeAddImageEffect(writer);

    writer->writeEndElement(); // effects
}

/** Reads gradient parameters from \a parentElement and sets the parameters in
  * filter brush frame.
  * \sa readGradientStops()
  */
void EffectsCollector::readGradients(const QDomElement &parentElement) {
    QDomElement elem = parentElement.firstChildElement("gradient");
    while (!elem.isNull()) {
        QString str = elem.attribute("type");
        if (str == "linear") {
            LinearGradientParams &lgp = effectsArea->filterBrushFrame->linearGradientParams_;
            lgp.start = readPointF(elem.firstChildElement("start"));
            lgp.finalStop = readPointF(elem.firstChildElement("finalstop"));
        }
        else if (str == "radial") {
            RadialGradientParams &rgp = effectsArea->filterBrushFrame->radialGradientParams_;
            rgp.radius = elem.attribute("radius").toDouble();
            rgp.center = readPointF(elem.firstChildElement("center"));
            rgp.focalPoint = readPointF(elem.firstChildElement("focalpoint"));
        }
        else if (str == "conical") {
            ConicalGradientParams &cgp = effectsArea->filterBrushFrame->conicalGradientParams_;
            cgp.angle = elem.attribute("angle").toDouble();
            cgp.center = readPointF(elem.firstChildElement("center"));
        }
        elem = elem.nextSiblingElement("gradient");
    }
}

/** Reads gradient stops from \a parentElement and sets the stops list in
  * filter gradient widget.
  * \sa readGradients()
  */
void EffectsCollector::readGradientStops(const QDomElement &parentElement) {
    QGradientStops stops;
    QDomElement elem = parentElement.firstChildElement("stop");
    while (!elem.isNull()) {
        QGradientStop stop;
        stop.first = elem.attribute("value").toDouble();
        stop.second = readColor(elem);
        stops += stop;
        elem = elem.nextSiblingElement("stop");
    }
    effectsArea->filterGradientWidget->setGradientStops(stops);
}

/** Reads \e Histogram effect from \a parentElement.
  * \return true if read succeed; otherwise false
  * \sa readFilterEffect() readAddFrameEffect() readAddTextEffect()
  *     readAddImageEffect()
  */
bool EffectsCollector::readHistogramEffect(const QDomElement &parentElement) {
    bool result = false;

    String str;
    QDomElement el = parentElement.firstChildElement("histogram");
    if (el.isNull())
        return result;

    result = true;
    str = el.attribute("enabled", falseString);
    effectsArea->histogramGroupBox->setChecked(str.toBool());
    if (el.attribute("operation") == "equalize")
        effectsArea->equalizeHistogramRadioButton->setChecked(true);
    else
        effectsArea->stretchHistogramRadioButton->setChecked(true);

    return result;
}

/** Reads \e Filter effect from \a parentElement.
  * \return true if read succeed; otherwise false
  * \sa readHistogramEffect() readAddFrameEffect() readAddTextEffect()
  *     readAddImageEffect()
  */
bool EffectsCollector::readFilterEffect(const QDomElement &parentElement) {
    bool result = false;

    String str;
    QDomElement el, e;
    QRadioButton *radioButton = effectsArea->filterColorRadioButton;

    el = parentElement.firstChildElement("filter");
    if (el.isNull())
        return result;

    result = true;
    str = el.attribute("enabled", falseString);
    effectsArea->filterGroupBox->setChecked(str.toBool());

    e = el.firstChildElement("colorfilter");
    if (!e.isNull()) {
        effectsArea->filterColorRadioButton->setChecked(true);
        effectsArea->filterTypeComboBox->setCurrentIndex(e.attribute("index").toInt());
        effectsArea->filterBrushFrame->setColor(readColor(e));
    }

    e = el.firstChildElement("gradientfilter");
    if (!e.isNull()) {
        str = e.attribute("enabled", falseString);
        if (str.toBool())
            radioButton = effectsArea->filterGradientRadioButton;
        effectsArea->filterGradientRadioButton->setChecked(true);
        readGradients(e);
        readGradientStops(e);
        int idx = e.attribute("index").toInt();
        if (idx > 0)
            effectsArea->filterTypeComboBox->setCurrentIndex(0);
        else
            effectsArea->filterTypeComboBox->setCurrentIndex(1);
        effectsArea->filterTypeComboBox->setCurrentIndex(idx);
    }

    radioButton->setChecked(true);

    return result;
}

/** Reads \em "Add Frame" effect from \a parentElement.
  * \return true if read succeed; otherwise false
  * \sa readHistogramEffect() readFilterEffect() readAddTextEffect()
  *     readAddImageEffect()
  */
bool EffectsCollector::readAddFrameEffect(const QDomElement &parentElement) {
    bool result = false;

    String str;
    QDomElement el, e;

    el = parentElement.firstChildElement("addframe");
    if (el.isNull())
        return result;

    result = true;
    str = el.attribute("enabled", falseString);
    effectsArea->frameGroupBox->setChecked(str.toBool());

    e = el.firstChildElement("frame");
    if (!e.isNull()) {
        str = e.attribute("around", falseString);
        if (str.toBool())
            effectsArea->frameAroundRadioButton->setChecked(true);
        else
            effectsArea->frameOverlayRadioButton->setChecked(true);
        effectsArea->frameWidthSpinBox->setValue(e.attribute("width").toInt());
        effectsArea->frameColorFrame->setColor(readColor(e));
    }

    e = el.firstChildElement("insideborder");
    if (!e.isNull()) {
        str = e.attribute("enabled", falseString);
        effectsArea->borderInsideGroupBox->setChecked(str.toBool());
        effectsArea->borderInsideSpinBox->setValue(
                    e.attribute("width").toInt() );
        effectsArea->borderInsideColorFrame->setColor(readColor(e));
    }

    e = el.firstChildElement("outsideborder");
    if (!e.isNull()) {
        str = e.attribute("enabled", falseString);
        effectsArea->borderOutsideGroupBox->setChecked(str.toBool());
        effectsArea->borderOutsideSpinBox->setValue(
                    e.attribute("width").toInt() );
        effectsArea->borderOutsideColorFrame->setColor(readColor(e));
    }

    return result;
}

/** Reads \em "Add Text" effect from \a parentElement.
  * \return true if read succeed; otherwise false
  * \sa readHistogramEffect() readFilterEffect() readAddFrameEffect()
  *     readAddImageEffect()
  */
bool EffectsCollector::readAddTextEffect(const QDomElement &parentElement) {
    bool result = false;

    String str;
    QStringList list;
    QDomElement el, e;

    el = parentElement.firstChildElement("addtext");
    if (el.isNull())
        return result;

    result = true;
    str = el.attribute("enabled", falseString);
    effectsArea->textGroupBox->setChecked(str.toBool());
    effectsArea->textOpacitySpinBox->setValue(el.attribute("opacity").toDouble());

    e = el.firstChildElement("text");
    if (!e.isNull()) {
        str = e.attribute("frame", falseString);
        effectsArea->textFrameCheckBox->setChecked(str.toBool());
        effectsArea->textLineEdit->setText(e.text());
    }

    e = el.firstChildElement("font");
    if (!e.isNull()) {
        effectsArea->textFontComboBox->setCurrentFont(
                    QFont(e.attribute("family")) );
        list = e.attribute("size").split(' ');
        effectsArea->textFontSizeSpinBox->setValue(list[0].toInt());
        effectsArea->textFontSizeComboBox->setCurrentIndex(
                    effectsArea->textFontSizeComboBox->findText(list[1]) );
        str = e.attribute("bold", falseString);
        effectsArea->textBoldPushButton->setChecked(str.toBool());
        str = e.attribute("italic", falseString);
        effectsArea->textItalicPushButton->setChecked(str.toBool());
        str = e.attribute("underline", falseString);
        effectsArea->textUnderlinePushButton->setChecked(str.toBool());
        str = e.attribute("strikeout", falseString);
        effectsArea->textStrikeOutPushButton->setChecked(str.toBool());
        effectsArea->textColorFrame->setColor(readColor(e));
    }

    e = el.firstChildElement("pos");
    if (!e.isNull()) {
        list = e.attribute("x").split(' ');
        effectsArea->textXSpinBox->setValue(list[0].toInt());
        effectsArea->textXComboBox->setCurrentIndex(
                    effectsArea->textXComboBox->findText(list[1]) );
        list = e.attribute("y").split(' ');
        effectsArea->textYSpinBox->setValue(list[0].toInt());
        effectsArea->textYComboBox->setCurrentIndex(
                    effectsArea->textYComboBox->findText(list[1]) );
    }
    effectsArea->textPositionComboBox->setCurrentIndex(e.attribute("mod").toInt());
    effectsArea->textRotationSpinBox->setValue(e.attribute("rotation").toInt());

    return result;
}

/** Reads \em "Add Image" effect from \a parentElement.
  * \return true if read succeed; otherwise false
  * \sa readHistogramEffect() readFilterEffect() readAddFrameEffect()
  *     readAddTextEffect()
  */
bool EffectsCollector::readAddImageEffect(const QDomElement &parentElement) {
    bool result = false;

    String str;
    QStringList list;
    QDomElement el, e;

    el = parentElement.firstChildElement("addimage");
    if (el.isNull())
        return result;

    result = true;
    str = el.attribute("enabled", falseString);
    effectsArea->imageGroupBox->setChecked(str.toBool());
    effectsArea->imageOpacitySpinBox->setValue(el.attribute("opacity").toDouble());

    e = el.firstChildElement("image");
    if (!e.isNull())
        effectsArea->imagePathLineEdit->setText(e.text());

    e = el.firstChildElement("pos");
    if (!e.isNull()) {
        list = e.attribute("x").split(' ');
        effectsArea->imageXSpinBox->setValue(list[0].toInt());
        effectsArea->imageXComboBox->setCurrentIndex(
                    effectsArea->imageXComboBox->findText(list[1]) );
        list = e.attribute("y").split(' ');
        effectsArea->imageYSpinBox->setValue(list[0].toInt());
        effectsArea->imageYComboBox->setCurrentIndex(
                    effectsArea->imageYComboBox->findText(list[1]) );
        effectsArea->imagePositionComboBox->setCurrentIndex(
                    e.attribute("mod").toInt() );
        effectsArea->imageRotationSpinBox->setValue(
                    e.attribute("rotation").toInt() );
    }

    return result;
}

void EffectsCollector::writeHistogramEffect(XmlStreamWriter *writer) {
    writer->writeStartElement("histogram");
    writer->writeAttribute("enabled", effectsArea->histogramGroupBox->isChecked());
    if (effectsArea->stretchHistogramRadioButton->isChecked())
        writer->writeAttribute("operation", "stretch");
    else
        writer->writeAttribute("operation", "equalize");
    writer->writeEndElement(); // histogram
}

void EffectsCollector::writeFilterEffect(XmlStreamWriter *writer) {
    writer->writeStartElement("filter");

    writer->writeAttribute("enabled", effectsArea->filterGroupBox->isChecked());
    writer->writeStartElement("colorfilter");
    writer->writeAttribute("enabled", effectsArea->filterColorRadioButton->isChecked());
    if (effectsArea->filterColorRadioButton->isChecked()) {
        writer->writeAttribute("index", effectsArea->filterTypeComboBox->currentIndex());
        writer->writeColorElement(effectsArea->filterBrushFrame->color());
    }
    else {
        writer->writeAttribute("index", effectsArea->filterColorModelIndex);
        writer->writeColorElement(effectsArea->filterColor);
    }
    writer->writeEndElement(); // colorfilter

    writer->writeStartElement("gradientfilter");
    writer->writeAttribute("enabled", effectsArea->filterGradientRadioButton->isChecked());
    writer->writeAttribute("index", effectsArea->filterTypeComboBox->currentIndex());

    LinearGradientParams lgp = effectsArea->filterBrushFrame->linearGradientParams();
    writer->writeStartElement("gradient");
    writer->writeAttribute("type", "linear");
    writer->writePointElement("start", lgp.start);
    writer->writePointElement("finalstop", lgp.finalStop);
    writer->writeEndElement(); // gradient

    RadialGradientParams rgp = effectsArea->filterBrushFrame->radialGradientParams();
    writer->writeStartElement("gradient");
    writer->writeAttribute("type", "radial");
    writer->writeAttribute("radius", rgp.radius);
    writer->writePointElement("center", rgp.center);
    writer->writePointElement("focalpoint", rgp.focalPoint);
    writer->writeEndElement(); // gradient

    ConicalGradientParams cgp = effectsArea->filterBrushFrame->conicalGradientParams();
    writer->writeStartElement("gradient");
    writer->writeAttribute("type", "conical");
    writer->writeAttribute("angle", cgp.angle);
    writer->writePointElement("center", cgp.center);
    writer->writeEndElement(); // gradient

    foreach (QGradientStop stop, effectsArea->filterGradientWidget->gradientStops()) {
        writer->writeStartElement("stop");
        writer->writeAttribute("value", stop.first);
        writer->writeColorElement(stop.second);
        writer->writeEndElement(); // stop
    }

    writer->writeEndElement(); // gradientfilter

    writer->writeEndElement(); // filter
}

void EffectsCollector::writeAddFrameEffect(XmlStreamWriter *writer) {
    writer->writeStartElement("addframe");
    writer->writeAttribute("enabled", effectsArea->frameGroupBox->isChecked());
    writer->writeStartElement("frame");
    writer->writeAttribute("around", effectsArea->frameAroundRadioButton->isChecked());
    writer->writeAttribute("width", effectsArea->frameWidthSpinBox->value());
    writer->writeColorElement(effectsArea->frameColorFrame->color());
    writer->writeEndElement(); // frame
    writer->writeStartElement("insideborder");
    writer->writeAttribute("enabled", effectsArea->borderInsideGroupBox->isChecked());
    writer->writeAttribute("width", effectsArea->borderInsideSpinBox->value());
    writer->writeColorElement(effectsArea->borderInsideColorFrame->color());
    writer->writeEndElement(); // insideborder
    writer->writeStartElement("outsideborder");
    writer->writeAttribute("enabled", effectsArea->borderOutsideGroupBox->isChecked());
    writer->writeAttribute("width", effectsArea->borderOutsideSpinBox->value());
    writer->writeColorElement(effectsArea->borderOutsideColorFrame->color());
    writer->writeEndElement(); // outsideborder
    writer->writeEndElement(); // addframe
}

void EffectsCollector::writeAddTextEffect(XmlStreamWriter *writer) {
    writer->writeStartElement("addtext");
    writer->writeAttribute("enabled", effectsArea->textGroupBox->isChecked());
    writer->writeAttribute("opacity", effectsArea->textOpacitySpinBox->value());
    writer->writeStartElement("text");
    writer->writeAttribute("frame", effectsArea->textFrameCheckBox->isChecked());
    writer->writeCharacters(effectsArea->textLineEdit->text());
    writer->writeEndElement(); // text
    writer->writeStartElement("font");
    writer->writeAttribute("family",
                          effectsArea->textFontComboBox->currentFont().family() );
    writer->writeAttribute("size",
                          QString::number(effectsArea->textFontSizeSpinBox->value())
                          + ' '
                          + effectsArea->textFontSizeComboBox->currentText() );
    writer->writeAttribute("bold", effectsArea->textBoldPushButton->isChecked());
    writer->writeAttribute("italic", effectsArea->textItalicPushButton->isChecked());
    writer->writeAttribute("underline",
                          effectsArea->textUnderlinePushButton->isChecked() );
    writer->writeAttribute("strikeout",
                          effectsArea->textStrikeOutPushButton->isChecked() );
    writer->writeColorElement(effectsArea->textColorFrame->color());
    writer->writeEndElement(); // font
    writer->writeStartElement("pos");
    writer->writeAttribute("x", QString::number(effectsArea->textXSpinBox->value())
                               + ' ' + effectsArea->textXComboBox->currentText() );
    writer->writeAttribute("y", QString::number(effectsArea->textYSpinBox->value())
                               + ' ' + effectsArea->textYComboBox->currentText() );
    writer->writeAttribute("mod", effectsArea->textPositionComboBox->currentIndex());
    writer->writeAttribute("rotation", effectsArea->textRotationSpinBox->value());
    writer->writeEndElement(); // pos
    writer->writeEndElement(); // addtext
}

void EffectsCollector::writeAddImageEffect(XmlStreamWriter *writer) {
    writer->writeStartElement("addimage");
    writer->writeAttribute("enabled", effectsArea->imageGroupBox->isChecked());
    writer->writeAttribute("opacity", effectsArea->imageOpacitySpinBox->value());
    writer->writeStartElement("image");
    writer->writeCharacters(effectsArea->imagePathLineEdit->text());
    writer->writeEndElement(); // image
    writer->writeStartElement("pos");
    writer->writeAttribute("x", QString::number(effectsArea->imageXSpinBox->value())
                               + ' ' + effectsArea->imageXComboBox->currentText() );
    writer->writeAttribute("y", QString::number(effectsArea->imageYSpinBox->value())
                               + ' ' + effectsArea->imageYComboBox->currentText() );
    writer->writeAttribute("mod", effectsArea->imagePositionComboBox->currentIndex());
    writer->writeAttribute("rotation", effectsArea->imageRotationSpinBox->value());
    writer->writeEndElement(); // pos
    writer->writeEndElement(); // addimage
}
