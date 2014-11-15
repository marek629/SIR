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

#include <QDomDocument>
#include <QMessageBox>
#include "Session.hpp"
#include "widgets/ConvertDialog.hpp"
#include "xmlstreamwriter.h"
#include "version.h"
#include "sir_string.h"
#include "ConvertSharedData.hpp"
#include "EffectsCollector.hpp"

using namespace sir;

/** Creates the Session object.
  * \param parent Parent convert dialog.
  */
Session::Session(ConvertDialog *parent) : XmlHelper(parent) {
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
    collector = new EffectsCollector(convertDialog);
}

Session::~Session() {}

/** Writes session to XML file in \a fileName path.
  * \sa restore()
  */
void Session::save(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(convertDialog, tr("Session write error"),
                             tr("Session saving to %1 file failed.\n"
                                "Can't open the file.").arg(fileName) );
        return;
    }

    Version version;
    QString str; // working string

    XmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);
    writer.setCodec("UTF-8");
    writer.writeStartDocument();
    writer.writeStartElement("sir");
    writer.writeAttribute("version", version.version());
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
    str = sizeArea->sizeUnitComboBox->currentText().split('(')[1].split(')')[0];
    writer.writeAttribute("unit", str);
    if (sizeArea->sizeUnitComboBox->currentIndex() == 0) {
        writer.writeStartElement("pixels");
        writer.writeAttribute("width",  sizeArea->widthDoubleSpinBox->value());
        writer.writeAttribute("height", sizeArea->heightDoubleSpinBox->value());
        writer.writeEndElement(); // pixels
        writer.writeStartElement("percent");
        writer.writeAttribute("width",  ConvertSharedData::instance()->sizeWidth);
        writer.writeAttribute("height", ConvertSharedData::instance()->sizeHeight);
        writer.writeEndElement(); // percent
    }
    else {
        writer.writeStartElement("percent");
        writer.writeAttribute("width",  sizeArea->widthDoubleSpinBox->value());
        writer.writeAttribute("height", sizeArea->heightDoubleSpinBox->value());
        writer.writeEndElement(); // percent
        writer.writeStartElement("pixels");
        writer.writeAttribute("width",  ConvertSharedData::instance()->sizeWidth);
        writer.writeAttribute("height", ConvertSharedData::instance()->sizeHeight);
        writer.writeEndElement(); // pixels
    }
    writer.writeStartElement("bytes");
    writer.writeCharacters(QString::number(sizeArea->fileSizeSpinBox->value())
                           + ' ' + sizeArea->fileSizeComboBox->currentText() );
    writer.writeEndElement(); // bytes
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

    collector->write(&writer);

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

/** Restores session from XML file in \a fileName path.
  * \sa save()
  */
void Session::restore(const QString &fileName) {
    QFile file(fileName);

    QFileInfo info = QFileInfo(file);
    convertDialog->sessionDir = info.absoluteDir();
    convertDialog->effectsDir = info.absoluteDir();

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
    Version version;
    bool fileInvalid(sir.isNull() ||
                     !version.isAtLeast(sir.attribute("version", version.version())) );
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
    String str;
    QStringList list;
    int x;

    convertDialog->filesTreeWidget->clear();
    elem = session.firstChildElement("files");
    el = elem.firstChildElement("file");
    while (!el.isNull()) {
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
        sizeArea->sizeUnitComboBox->setCurrentIndex(0);
        el = elem.firstChildElement("pixels");
        if (!el.isNull()) {
            sizeArea->widthDoubleSpinBox->setValue(el.attribute("width").toDouble());
            sizeArea->heightDoubleSpinBox->setValue(el.attribute("height").toDouble());
        }
        el = elem.firstChildElement("percent");
        if (!el.isNull()) {
            ConvertSharedData::instance()->sizeWidth = el.attribute("width").toDouble();
            ConvertSharedData::instance()->sizeHeight = el.attribute("height").toDouble();
        }
        el = elem.firstChildElement("filesize");
        if (!el.isNull()) {
            list = el.text().split(' ');
            sizeArea->fileSizeSpinBox->setValue(list[0].toDouble());
            sizeArea->fileSizeComboBox->setCurrentIndex(
                        sizeArea->fileSizeComboBox->findText(list[1]) );
        }
        x = sizeArea->sizeUnitComboBox->findText('(' + elem.attribute("unit") + ')',
                                                 Qt::MatchContains);
        sizeArea->sizeUnitComboBox->setCurrentIndex(x);
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
    collector->read(elem);

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
