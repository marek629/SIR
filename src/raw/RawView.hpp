/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2016  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://marek629.github.io/SIR/
 */

#ifndef RAWVIEW_HPP
#define RAWVIEW_HPP

#include "raw/Retranslatable.hpp"

#include <QString>

class RawController;

class QWidget;


class RawView : public Retranslatable
{
public:
    RawView();
    virtual ~RawView();

    void loadSettings();
    void saveSettings();

    void setController(RawController *controller);

    virtual QWidget *qWidget();

    virtual bool isEnabledChecked() const = 0;
    virtual void setEnabledChecked(bool enabled) = 0;

    virtual QString pathText() const = 0;
    virtual void setPathText(const QString &text) = 0;
    virtual bool isPathTextEnabledInput() const = 0;
    virtual void setPathTextEnabledInput(bool inputEnabled) = 0;

    virtual QString optionsText() const = 0;
    virtual void setOptionsText(const QString &text) = 0;
    virtual bool isOptionsTextEnabledInput() const = 0;
    virtual void setOptionsTextEnabledInput(bool inputEnabled) = 0;

    virtual void setHelpTextPlain(const QString &text) = 0;
    virtual void setHelpWidgetEnabled(bool enabled) = 0;

    virtual bool isButtonEnabledInput() const = 0;
    virtual void setButtonEnabledInput(bool inputEnabled) = 0;

    virtual bool closeWindow() = 0;

protected:
    RawController *controller;
};

#endif // RAWVIEW_HPP
