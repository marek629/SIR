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

#include "raw/RawViewAdapter.hpp"

#include "widgets/options/RawGroupBoxView.hpp"


RawViewAdapter::RawViewAdapter(RawGroupBoxView *groupBox) : RawView()
{
    this->adaptedGroupBox = groupBox;
}

RawViewAdapter::~RawViewAdapter() {}

QWidget *RawViewAdapter::qWidget()
{
    return adaptedGroupBox;
}

bool RawViewAdapter::isEnabledChecked() const
{
    return adaptedGroupBox->rawCheckBox->isChecked();
}

void RawViewAdapter::setEnabledChecked(bool enabled)
{
    adaptedGroupBox->rawCheckBox->setChecked(enabled);
}

QString RawViewAdapter::pathText() const
{
    return adaptedGroupBox->dcrawLineEdit->text();
}

void RawViewAdapter::setPathText(const QString &text)
{
    adaptedGroupBox->dcrawLineEdit->setText(text);
}

void RawViewAdapter::setPathTextEnabledInput(bool inputEnabled)
{
    adaptedGroupBox->dcrawLineEdit->setEnabled(inputEnabled);
}

QString RawViewAdapter::optionsText() const
{
    return adaptedGroupBox->dcrawOptions->text();
}

void RawViewAdapter::setOptionsText(const QString &text)
{
    adaptedGroupBox->dcrawOptions->setText(text);
}

void RawViewAdapter::setOptionsTextEnabledInput(bool inputEnabled)
{
    adaptedGroupBox->dcrawOptions->setEnabled(inputEnabled);
}

void RawViewAdapter::setHelpTextPlain(const QString &text)
{
    adaptedGroupBox->helpTextBrowser->setPlainText(text);
}

void RawViewAdapter::setButtonEnabledInput(bool inputEnabled)
{
    adaptedGroupBox->dcrawPushButton->setEnabled(inputEnabled);
}

bool RawViewAdapter::closeWindow()
{
    return adaptedGroupBox->window()->close();
}
