#ifndef XMLSTREAMWRITER_H
#define XMLSTREAMWRITER_H

#include <QXmlStreamWriter>
#include <QColor>

class QPointF;

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
    void writePointElement(const QString &qualifiedName, const QPointF &value);
    void writeColorElement(const QColor &color);
    void writeValue(int value);
};

#endif // XMLSTREAMWRITER_H
