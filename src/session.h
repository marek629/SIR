#ifndef SESSION_H
#define SESSION_H

#include <QDomElement>
#include <QColor>
#include <QCoreApplication>

class ConvertDialog;
class SizeScrollArea;
class OptionsScrollArea;
class EffectsScrollArea;
class SvgScrollArea;

//! Session files reader and writer class.
class Session {
    Q_DECLARE_TR_FUNCTIONS(Session)

public:
    explicit Session(ConvertDialog *parent = 0);
    void save(const QString &fileName);
    void restore(const QString &fileName);

private:
    // fields
    /** Parent convert dialog.
      * \sa sizeArea optionsArea effectsArea svgArea
      */
    ConvertDialog *convertDialog;
    SizeScrollArea *sizeArea; /**< Size scroll area in convertDialog. */
    OptionsScrollArea *optionsArea; /**< Options scroll area in convertDialog. */
    EffectsScrollArea *effectsArea; /**< Effects scroll area in convertDialog. */
    SvgScrollArea *svgArea; /**< SVG scroll area in convertDialog. */
    // methods
    QColor readColor(const QDomElement &parentElement);
};

#endif // SESSION_H
