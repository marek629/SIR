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
 * Program URL: http://marek629.github.io/SIR/
 */

#ifndef BASICRAWVIEWSCROLLAREA_HPP
#define BASICRAWVIEWSCROLLAREA_HPP

#include <QScrollArea>
#include "raw/RawView.hpp"

#include "ui_BasicRawScrollArea.h"


class BasicRawViewScrollArea : public QScrollArea, public RawView
{
    Q_OBJECT

public:
    explicit BasicRawViewScrollArea(QWidget *parent = 0);
    ~BasicRawViewScrollArea();

    QWidget *qWidget();

    bool isEnabledChecked() const;
    void setEnabledChecked(bool enabled);

    QString pathText() const;
    void setPathText(const QString &text);
    bool isPathTextEnabledInput() const;
    void setPathTextEnabledInput(bool inputEnabled);

    QString optionsText() const;
    void setOptionsText(const QString &text);
    bool isOptionsTextEnabledInput() const;
    void setOptionsTextEnabledInput(bool inputEnabled);

    void setHelpTextPlain(const QString &text);
    void setHelpWidgetEnabled(bool enabled);

    bool isButtonEnabledInput() const;
    void setButtonEnabledInput(bool inputEnabled);

    bool closeWindow();

private slots:
    void onBrowseButtonClick();
    void onEraseNoiseToggle(bool toggled);
    void onInterpolationPostProcessingToggle(bool toggled);
    void onRawEnabledChange(int state);

private:
    Ui::BasicRawScrollArea ui;

    int toggleSpinBox(bool enabled, QSpinBox *spinBox, int spinBoxEnabledValue);
};

#endif // BASICRAWVIEWSCROLLAREA_HPP
