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

#ifndef EFFECTSCOLLECTOR_H
#define EFFECTSCOLLECTOR_H

#include "xmlhelper.h"

class EffectsScrollArea;
class XmlStreamWriter;

class EffectsCollector : public XmlHelper {
    Q_DECLARE_TR_FUNCTIONS(EffectsCollector);

public:
    EffectsCollector(ConvertDialog *parent = 0);
    // TODO: testing read() and write() and use save() and restore() methods
    // TODO: add read code for "filter" effect
    // TODO: split read() and write() functions to minor methods
    void save(const QString &fileName);
    void restore(const QString &fileName);
    bool read(const QDomElement &element);
    void write(XmlStreamWriter *writer);

private:
    EffectsScrollArea *effectsArea;
};

#endif // EFFECTSCOLLECTOR_H
