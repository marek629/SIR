#include "xmlstreamwriter.h"

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
