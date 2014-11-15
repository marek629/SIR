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

#ifndef ABSTRACTOPTIONSGROUPBOX_H
#define ABSTRACTOPTIONSGROUPBOX_H

#include <QGroupBox>
#include "widgets/options/AbstractOptions.hpp"

/** \brief Abstract base class for all derived classes from AbstractOptions.
  *
  * New abstract class created for controllers.
  *
  * Old class is needed yet while refactoring to MVC.
  */
class AbstractOptionsController : public QObject, public AbstractOptions {
public:
    AbstractOptionsController(QObject *parent = 0);
    // pure virtual functions
    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
};

//! Abstract base class for all derived classes from QGroupBox and AbstractOptions.
class AbstractOptionsGroupBox : public QGroupBox, public AbstractOptions {
public:
    AbstractOptionsGroupBox(QWidget *parent = 0);
    // pure virtual functions
    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void setController(AbstractOptionsController *controller) = 0;
};

#endif // ABSTRACTOPTIONSGROUPBOX_H
