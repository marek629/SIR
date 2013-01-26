#include <QDomDocument>
#include <QMessageBox>
#include "session.h"
#include "convertdialog.h"
#include "xmlstreamwriter.h"
#include "defines.h"
#include "metadata/string.h"

const QString falseString = "no";

/** Creates the Session object.
  * \param parent Parent convert dialog.
  */
Session::Session(ConvertDialog *parent) {
    convertDialog = parent;
    if (convertDialog) {
        sizeArea = convertDialog->sizeScrollArea;
        optionsArea = convertDialog->optionsScrollArea;
        effectsArea = convertDialog->effectsScrollArea;
        svgArea = convertDialog->svgScrollArea;
    }
    else {
        sizeArea = 0;
        optionsArea = 0;
        effectsArea = 0;
        svgArea = 0;
    }
}

/** Writes session to XML file in \a fileName path. */
void Session::save(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(convertDialog, tr("Session write error"),
                             tr("Session saving to %1 file failed.\n"
                                "Can't open the file.").arg(fileName) );
        return;
    }

    QString str; // working string

    XmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);
    writer.setCodec("UTF-8");
    writer.writeStartDocument();
    writer.writeStartElement("sir");
    writer.writeAttribute("version", VERSION);
    writer.writeStartElement("session");

    writer.writeStartElement("files");
    for (int i=0; i<convertDialog->filesTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = convertDialog->filesTreeWidget->topLevelItem(i);
        QString path = item->text(2);
        if (!path.endsWith(QDir::separator()))
            path += QDir::separator();
        path += item->text(0) + '.' + item->text(1);
        writer.writeStartElement("file");
        writer.writeCharacters(path);
        writer.writeEndElement(); // file
    }
    writer.writeEndElement(); // files

    writer.writeStartElement("target");
    writer.writeStartElement("dir");
    writer.writeCharacters(convertDialog->destFileEdit->text());
    writer.writeEndElement(); // dir
    writer.writeStartElement("prefix");
    writer.writeCharacters(convertDialog->destPrefixEdit->text());
    writer.writeEndElement(); // prefix
    writer.writeStartElement("suffix");
    writer.writeCharacters(convertDialog->destSuffixEdit->text());
    writer.writeEndElement(); // suffix
    writer.writeStartElement("format");
    writer.writeCharacters(convertDialog->targetFormatComboBox->currentText());
    writer.writeEndElement(); // format
    writer.writeEndElement(); // target

    writer.writeStartElement("size");
    writer.writeAttribute("keepaspect", sizeArea->maintainCheckBox->isChecked());
    writer.writeAttribute("width",  sizeArea->widthDoubleSpinBox->value());
    writer.writeAttribute("height", sizeArea->heightDoubleSpinBox->value());
    str = sizeArea->sizeUnitComboBox->currentText().split('(')[1].split(')')[0];
    writer.writeAttribute("unit", str);
    writer.writeStartElement("filesize");
    writer.writeCharacters(QString::number(sizeArea->fileSizeSpinBox->value())
                           + ' ' + sizeArea->fileSizeComboBox->currentText() );
    writer.writeEndElement(); // filesize
    writer.writeEndElement(); // size

    writer.writeStartElement("options");
    writer.writeStartElement("quality");
    writer.writeValue(optionsArea->qualitySpinBox->value());
    writer.writeEndElement(); // quality
    writer.writeStartElement("background");
    writer.writeAttribute("enabled", optionsArea->backgroundColorCheckBox->isChecked());
    writer.writeColorElement(optionsArea->backgroundColorFrame->color());
    writer.writeEndElement(); // background
    writer.writeStartElement("rotation");
    writer.writeAttribute("enabled", optionsArea->rotateCheckBox->isChecked());
    writer.writeCharacters(optionsArea->rotateLineEdit->text());
    writer.writeEndElement(); // rotation
    writer.writeStartElement("flip");
    writer.writeValue(optionsArea->flipComboBox->currentIndex());
    writer.writeEndElement(); // flip
    writer.writeEndElement(); // options

    writer.writeStartElement("effects");
    // add frame
    writer.writeStartElement("addframe");
    writer.writeAttribute("enabled", effectsArea->frameGroupBox->isChecked());
    writer.writeStartElement("frame");
    writer.writeAttribute("around", effectsArea->frameAroundRadioButton->isChecked());
    writer.writeAttribute("width", effectsArea->frameWidthSpinBox->value());
    writer.writeColorElement(effectsArea->frameColorFrame->color());
    writer.writeEndElement(); // frame
    writer.writeStartElement("insideborder");
    writer.writeAttribute("enabled", effectsArea->borderInsideGroupBox->isChecked());
    writer.writeAttribute("width", effectsArea->borderInsideSpinBox->value());
    writer.writeColorElement(effectsArea->borderInsideColorFrame->color());
    writer.writeEndElement(); // insideborder
    writer.writeStartElement("outsideborder");
    writer.writeAttribute("enabled", effectsArea->borderOutsideGroupBox->isChecked());
    writer.writeAttribute("width", effectsArea->borderOutsideSpinBox->value());
    writer.writeColorElement(effectsArea->borderOutsideColorFrame->color());
    writer.writeEndElement(); // outsideborder
    writer.writeEndElement(); // addframe
    // add text
    writer.writeStartElement("addtext");
    writer.writeAttribute("enabled", effectsArea->textGroupBox->isChecked());
    // TODO: add opacity for add text effect
    writer.writeStartElement("text");
    writer.writeAttribute("frame", effectsArea->textFrameCheckBox->isChecked());
    writer.writeCharacters(effectsArea->textLineEdit->text());
    writer.writeEndElement(); // text
    writer.writeStartElement("font");
    writer.writeAttribute("family",
                          effectsArea->textFontComboBox->currentFont().family() );
    writer.writeAttribute("size",
                          QString::number(effectsArea->textFontSizeSpinBox->value())
                          + ' '
                          + effectsArea->textFontSizeComboBox->currentText() );
    writer.writeAttribute("bold", effectsArea->textBoldPushButton->isChecked());
    writer.writeAttribute("italic", effectsArea->textItalicPushButton->isChecked());
    writer.writeAttribute("underline",
                          effectsArea->textUnderlinePushButton->isChecked() );
    writer.writeAttribute("strikeout",
                          effectsArea->textStrikeOutPushButton->isChecked() );
    writer.writeColorElement(effectsArea->textColorFrame->color());
    writer.writeEndElement(); // font
    writer.writeStartElement("pos");
    writer.writeAttribute("x", QString::number(effectsArea->textXSpinBox->value())
                               + ' ' + effectsArea->textXComboBox->currentText() );
    writer.writeAttribute("y", QString::number(effectsArea->textYSpinBox->value())
                               + ' ' + effectsArea->textYComboBox->currentText() );
    writer.writeAttribute("mod", effectsArea->textPositionComboBox->currentIndex());
    writer.writeAttribute("rotation", effectsArea->textRotationSpinBox->value());
    writer.writeEndElement(); // pos
    writer.writeEndElement(); // addtext
    // add image
    writer.writeStartElement("addimage");
    writer.writeAttribute("enabled", effectsArea->imageGroupBox->isChecked());
    writer.writeAttribute("opacity", effectsArea->imageOpacitySpinBox->value());
    writer.writeStartElement("image");
    writer.writeCharacters(effectsArea->imagePathLineEdit->text());
    writer.writeEndElement(); // image
    writer.writeStartElement("pos");
    writer.writeAttribute("x", QString::number(effectsArea->imageXSpinBox->value())
                               + ' ' + effectsArea->imageXComboBox->currentText() );
    writer.writeAttribute("y", QString::number(effectsArea->imageYSpinBox->value())
                               + ' ' + effectsArea->imageYComboBox->currentText() );
    writer.writeAttribute("mod", effectsArea->imagePositionComboBox->currentIndex());
    writer.writeAttribute("rotation", effectsArea->imageRotationSpinBox->value());
    writer.writeEndElement(); // pos
    writer.writeEndElement(); // addimage
    writer.writeEndElement(); // effects

    writer.writeStartElement("svg");
    writer.writeAttribute("save", svgArea->saveCheckBox->isChecked());
    writer.writeStartElement("remove");
    writer.writeAttribute("emptygroups", svgArea->removeGroupsCheckBox->isChecked());
    writer.writeStartElement("text");
    writer.writeAttribute("enabled", svgArea->removeTextCheckBox->isChecked());
    writer.writeCharacters(svgArea->removeTextLineEdit->text());
    writer.writeEndElement(); // text
    writer.writeEndElement(); // remove
    writer.writeEndElement(); // svg

    writer.writeEndElement(); // session
    writer.writeEndElement(); // sir
    writer.writeEndDocument();
}

/** Restores session from XML file in \a fileName path. */
void Session::restore(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(convertDialog, tr("Session read error"),
                             tr("Session restoring from %1 file failed.\n"
                                "Can't open the file.").arg(fileName) );
        return;
    }
    QDomDocument document;
    if (!document.setContent(&file)) {
        QMessageBox::warning(convertDialog, tr("Session read error"),
                             tr("Session restoring from %1 file failed.\n"
                                "Can't parse the file.").arg(fileName) );
        return;
    }
    QDomElement sir = document.firstChildElement("sir");
    bool fileInvalid(sir.isNull() || sir.attribute("version", VERSION) != VERSION);
    QDomElement session = sir.firstChildElement("session");
    if (!fileInvalid)
        fileInvalid = (session.isNull() || !session.hasChildNodes());
    if (fileInvalid) {
        QMessageBox::warning(convertDialog, tr("Session read error"),
                             tr("Session restoring from %1 file failed.\n"
                                "Invalid file format.").arg(fileName) );
        return;
    }

    QDomElement elem, el, e; // working DOM element variables
    MetadataUtils::String str;
    QStringList list;

    elem = session.firstChildElement("files");
    el = elem.firstChildElement("file");
    while (el.isNull()) {
        convertDialog->filesTreeWidget->loadFile(el.text());
        el = el.nextSiblingElement("file");
    }

    elem = session.firstChildElement("target");
    if (!elem.isNull()) {
        el = elem.firstChildElement("dir");
        if (!el.isNull())
            convertDialog->destFileEdit->setText(el.text());
        el = elem.firstChildElement("prefix");
        if (!el.isNull())
            convertDialog->destPrefixEdit->setText(el.text());
        el = elem.firstChildElement("suffix");
        if (!el.isNull())
            convertDialog->destSuffixEdit->setText(el.text());
        el = elem.firstChildElement("format");
        if (!el.isNull())
            convertDialog->targetFormatComboBox->setCurrentIndex(
                        convertDialog->targetFormatComboBox->findText(el.text()) );
    }

    elem = session.firstChildElement("size");
    if (!elem.isNull()) {
        str = elem.attribute("keepaspect", falseString);
        sizeArea->maintainCheckBox->setChecked(str.toBool());
        sizeArea->widthDoubleSpinBox->setValue(elem.attribute("width").toDouble());
        sizeArea->heightDoubleSpinBox->setValue(elem.attribute("height").toDouble());
        sizeArea->sizeUnitComboBox->setCurrentIndex(
                    sizeArea->sizeUnitComboBox->findText(elem.attribute("unit")) );
        el = elem.firstChildElement("filesize");
        if (!el.isNull()) {
            list = el.text().split(' ');
            sizeArea->fileSizeSpinBox->setValue(list[0].toDouble());
            sizeArea->fileSizeComboBox->setCurrentIndex(
                        sizeArea->fileSizeComboBox->findText(list[1]) );
        }
    }

    elem = session.firstChildElement("options");
    if (!elem.isNull()) {
        el = elem.firstChildElement("quality");
        if (!el.isNull())
            optionsArea->qualitySpinBox->setValue(el.text().toInt());
        el = elem.firstChildElement("background");
        if (!el.isNull()) {
            str = el.attribute("enabled", falseString);
            optionsArea->backgroundColorCheckBox->setChecked(str.toBool());
            optionsArea->backgroundColorFrame->setColor(readColor(el));
        }
        el = elem.firstChildElement("rotation");
        if (!el.isNull()) {
            str = el.attribute("enabled", falseString);
            optionsArea->rotateCheckBox->setChecked(str.toBool());
            optionsArea->rotateLineEdit->setText(el.text());
        }
        el = elem.firstChildElement("flip");
        if (!el.isNull())
            optionsArea->flipComboBox->setCurrentIndex(el.text().toInt());
    }

    elem = session.firstChildElement("effects");
    if (!elem.isNull()) {
        el = elem.firstChildElement("addframe");
        if (!el.isNull()) {
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
        }
        el = elem.firstChildElement("addtext");
        if (!el.isNull()) {
            str = el.attribute("enabled", falseString);
            effectsArea->textGroupBox->setChecked(str.toBool());
            // TODO: add opacity for add text effect
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
                effectsArea->textPositionComboBox->setCurrentIndex(
                            e.attribute("mod").toInt() );
                effectsArea->textRotationSpinBox->setValue(
                            e.attribute("rotation").toInt() );
            }
        }
        el = elem.firstChildElement("addimage");
        if (!el.isNull()) {
            str = el.attribute("enabled", falseString);
            effectsArea->imageGroupBox->setChecked(str.toBool());
            effectsArea->imageOpacitySpinBox->setValue(
                        el.attribute("opacity").toDouble() );
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
        }
    }

    elem = session.firstChildElement("svg");
    if (!elem.isNull()) {
        str = elem.attribute("save");
        svgArea->saveCheckBox->setChecked(str.toBool());
        el = elem.firstChildElement("remove");
        if (!el.isNull()) {
            str = el.attribute("emptygroups", falseString);
            svgArea->removeGroupsCheckBox->setChecked(str.toBool());
            e = el.firstChildElement("text");
            if (!e.isNull()) {
                str = e.attribute("enabled", falseString);
                svgArea->removeTextCheckBox->setChecked(str.toBool());
                svgArea->removeTextLineEdit->setText(e.text());
            }
        }
    }
}

/** Reads color data from \e color child node of \a parentElement node.
  * \return Read color object.
  */
QColor Session::readColor(const QDomElement &parentElement) {
    QColor result;
    QDomElement e = parentElement.firstChildElement("color");
    if (e.isNull())
        return result;
    result.setRed(  e.attribute("r").toInt());
    result.setGreen(e.attribute("g").toInt());
    result.setBlue( e.attribute("b").toInt());
    result.setAlpha(e.attribute("a").toInt());
    return result;
}
