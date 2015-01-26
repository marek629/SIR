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

#include <QPointF>
#include "XmlStreamWriter.hpp"

/** Creates the XmlStreamWriter object. */
XmlStreamWriter::XmlStreamWriter() : QXmlStreamWriter() {}

/** Creates the XmlStreamWriter object connected to \a device buffer. */
XmlStreamWriter::XmlStreamWriter(QIODevice *device) : QXmlStreamWriter(device) {}

/** Creates the XmlStreamWriter object connected to \a array buffer. */
XmlStreamWriter::XmlStreamWriter(QByteArray *array) : QXmlStreamWriter(array) {}

/** Creates the XmlStreamWriter object connected to \a string buffer. */
XmlStreamWriter::XmlStreamWriter(QString *string) : QXmlStreamWriter(string) {}

/** Writes current node attribute called \a qualifiedName and sets the attribute
  * value to \a value string.
  */
void XmlStreamWriter::writeAttribute(const QString &qualifiedName,
                                     const QString &value) {
    QXmlStreamWriter::writeAttribute(qualifiedName, value);
}

/** Writes current node attribute called \a qualifiedName and sets the attribute
  * value to \a value string.
  */
void XmlStreamWriter::writeAttribute(const QString &qualifiedName,
                                     const char *value) {
    QXmlStreamWriter::writeAttribute(qualifiedName, QString::fromStdString(value));
}

/** Writes current node attribute called \a qualifiedName and sets the attribute
  * value to string corresponding \a value: \e yes if true, otherwise \e no.
  */
void XmlStreamWriter::writeAttribute(const QString &qualifiedName, bool value) {
    QXmlStreamWriter::writeAttribute(qualifiedName, (value) ? "yes" : "no");
}

/** Writes current node attribute called \a qualifiedName and sets the attribute
  * value to \a value decimal string.
  */
void XmlStreamWriter::writeAttribute(const QString &qualifiedName, int value) {
    QXmlStreamWriter::writeAttribute(qualifiedName, QString::number(value));
}

/** Writes current node attribute called \a qualifiedName and sets the attribute
  * value to \a value decimal string.
  */
void XmlStreamWriter::writeAttribute(const QString &qualifiedName, double value) {
    QXmlStreamWriter::writeAttribute(qualifiedName, QString::number(value));
}

void XmlStreamWriter::writePointElement(const QString &qualifiedName, const QPointF &value) {
    writeStartElement(qualifiedName);
    writeAttribute("x", value.x());
    writeAttribute("y", value.y());
    writeEndElement();
}

/** Writes current node child node called \e color and sets attributes
  * \e r (red), \e g (green), \e b (blue) and \e a (alpha) values to \a color
  * components decimal values.
  */
void XmlStreamWriter::writeColorElement(const QColor &color) {
    writeStartElement("color");
    writeAttribute("r", color.red());
    writeAttribute("g", color.green());
    writeAttribute("b", color.blue());
    writeAttribute("a", color.alpha());
    writeEndElement();
}

/** Writes current node value as \a value decimal node text. */
void XmlStreamWriter::writeValue(int value) {
    QXmlStreamWriter::writeCharacters(QString::number(value));
}
