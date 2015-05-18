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

#include "raw/RawViewWidget.hpp"

#include "raw/RawController.hpp"


RawViewWidget::RawViewWidget(QWidget *parent) : QWidget(parent), RawView()
{
    setupUi(this);

#if QT_VERSION >= 0x050200
    helpTextBrowser->setPlaceholderText(tr("dcraw Help"));
#endif

    helpTextBrowser->setFontFamily("monospace");

    connect(dcrawPushButton, SIGNAL(clicked()),
            this, SLOT(browseButtonClicked()));
    connect(rawCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(rawEnabledStatusChanged(int)));
}

RawViewWidget::~RawViewWidget() {}

QWidget *RawViewWidget::qWidget()
{
    return this;
}

bool RawViewWidget::isEnabledChecked() const
{
    return rawCheckBox->isChecked();
}

void RawViewWidget::setEnabledChecked(bool enabled)
{
    rawCheckBox->setChecked(enabled);
}

QString RawViewWidget::pathText() const
{
    return dcrawLineEdit->text();
}

void RawViewWidget::setPathText(const QString &text)
{
    dcrawLineEdit->setText(text);
}

void RawViewWidget::setPathTextEnabledInput(bool inputEnabled)
{
    dcrawLineEdit->setEnabled(inputEnabled);
}

QString RawViewWidget::optionsText() const
{
    return dcrawOptions->text();
}

void RawViewWidget::setOptionsText(const QString &text)
{
    dcrawOptions->setText(text);
}

void RawViewWidget::setOptionsTextEnabledInput(bool inputEnabled)
{
    dcrawOptions->setEnabled(inputEnabled);
}

void RawViewWidget::setHelpTextPlain(const QString &text)
{
    helpTextBrowser->setPlainText(text);
}

void RawViewWidget::setButtonEnabledInput(bool inputEnabled)
{
    dcrawPushButton->setEnabled(inputEnabled);
}

bool RawViewWidget::closeWindow()
{
    return window()->close();
}

void RawViewWidget::browseButtonClicked()
{
    controller->browseDcraw();
}

void RawViewWidget::rawEnabledStatusChanged(int state)
{
    controller->setRawStatus(state);
}
