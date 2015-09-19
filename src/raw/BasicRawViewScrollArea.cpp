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

#include "raw/BasicRawViewScrollArea.hpp"

#include "raw/RawController.hpp"
#include "raw/basic/InterpolationOptionsCollector.hpp"


BasicRawViewScrollArea::BasicRawViewScrollArea(QWidget *parent)
    : QScrollArea(parent), RawView()
{
    ui.setupUi(this);

    connect(ui.rawCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(onRawEnabledChange(int)));
}

BasicRawViewScrollArea::~BasicRawViewScrollArea() {}

QWidget *BasicRawViewScrollArea::qWidget()
{
    return ui.scrollAreaWidgetContents;
}

bool BasicRawViewScrollArea::isEnabledChecked() const
{
    return ui.rawCheckBox->isChecked();
}

void BasicRawViewScrollArea::setEnabledChecked(bool enabled)
{
    ui.rawCheckBox->setChecked(enabled);
}

QString BasicRawViewScrollArea::pathText() const
{
    return ui.dcrawLineEdit->text();
}

void BasicRawViewScrollArea::setPathText(const QString &text)
{
    ui.dcrawLineEdit->setText(text);
}

void BasicRawViewScrollArea::setPathTextEnabledInput(bool inputEnabled)
{
    ui.dcrawLineEdit->setEnabled(inputEnabled);
}

QString BasicRawViewScrollArea::optionsText() const
{
    Ui::BasicRawScrollArea *uiPointer = const_cast<Ui::BasicRawScrollArea*>(&ui);

    DcrawOptionsCollector *collector = new DcrawOptionsCollector(uiPointer);
    collector = new InterpolationOptionsCollector(collector, uiPointer);
    return collector->optionsString();
}

void BasicRawViewScrollArea::setOptionsText(const QString &text)
{
    Ui::BasicRawScrollArea *uiPointer = const_cast<Ui::BasicRawScrollArea*>(&ui);

    DcrawOptionsCollector *collector = new DcrawOptionsCollector(uiPointer);
    collector = new InterpolationOptionsCollector(collector, uiPointer);
    collector->setOptions(text);
}

void BasicRawViewScrollArea::setOptionsTextEnabledInput(bool inputEnabled)
{
    ui.interpolationGroupBox->setEnabled(inputEnabled);
}

void BasicRawViewScrollArea::setHelpTextPlain(const QString &text)
{
    Q_UNUSED(text);
}

void BasicRawViewScrollArea::setHelpWidgetEnabled(bool enabled)
{
    Q_UNUSED(enabled);
}

void BasicRawViewScrollArea::setButtonEnabledInput(bool inputEnabled)
{
    ui.dcrawPushButton->setEnabled(inputEnabled);
}

bool BasicRawViewScrollArea::closeWindow()
{
    return window()->close();
}

void BasicRawViewScrollArea::onRawEnabledChange(int state)
{
    controller->setRawStatus(state);
}
