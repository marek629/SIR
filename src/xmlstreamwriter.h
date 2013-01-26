#ifndef XMLSTREAMWRITER_H
#define XMLSTREAMWRITER_H

#include <QXmlStreamWriter>
#include <QColor>

//! XML writer for Session class.
class XmlStreamWriter : public QXmlStreamWriter {
public:
    XmlStreamWriter();
    XmlStreamWriter(QIODevice *device);
    XmlStreamWriter(QByteArray *array);
    XmlStreamWriter(QString *string);
    void writeAttribute(const QString &qualifiedName, const QString &value);
    void writeAttribute(const QString &qualifiedName, const char *value);
    void writeAttribute(const QString &qualifiedName, bool value);
    void writeAttribute(const QString &qualifiedName, int value);
    void writeAttribute(const QString &qualifiedName, double value);
    void writeColorElement(const QColor &color);
    void writeValue(int value);
};

#endif // XMLSTREAMWRITER_H
