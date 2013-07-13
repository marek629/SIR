/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://sir.projet-libre.org/
 */

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
