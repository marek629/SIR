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

#include "raw/AdvancedRawViewWidget.hpp"

#include "raw/RawController.hpp"


AdvancedRawViewWidget::AdvancedRawViewWidget(QWidget *parent) : QWidget(parent), RawView()
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
    connect(dcrawOptions, SIGNAL(editingFinished()),
            this, SLOT(dcrawOptionsStringChanged()));
}

AdvancedRawViewWidget::~AdvancedRawViewWidget() {}

QWidget *AdvancedRawViewWidget::qWidget()
{
    return this;
}

bool AdvancedRawViewWidget::isEnabledChecked() const
{
    return rawCheckBox->isChecked();
}

void AdvancedRawViewWidget::setEnabledChecked(bool enabled)
{
    rawCheckBox->setChecked(enabled);
}

QString AdvancedRawViewWidget::pathText() const
{
    return dcrawLineEdit->text();
}

void AdvancedRawViewWidget::setPathText(const QString &text)
{
    dcrawLineEdit->setText(text);
}

void AdvancedRawViewWidget::setPathTextEnabledInput(bool inputEnabled)
{
    dcrawLineEdit->setEnabled(inputEnabled);
}

QString AdvancedRawViewWidget::optionsText() const
{
    return dcrawOptions->text();
}

void AdvancedRawViewWidget::setOptionsText(const QString &text)
{
    dcrawOptions->setText(text);
}

void AdvancedRawViewWidget::setOptionsTextEnabledInput(bool inputEnabled)
{
    dcrawOptions->setEnabled(inputEnabled);
}

void AdvancedRawViewWidget::setHelpTextPlain(const QString &text)
{
    helpTextBrowser->setPlainText(text);
}

void AdvancedRawViewWidget::setButtonEnabledInput(bool inputEnabled)
{
    dcrawPushButton->setEnabled(inputEnabled);
}

bool AdvancedRawViewWidget::closeWindow()
{
    return window()->close();
}

void AdvancedRawViewWidget::browseButtonClicked()
{
    controller->browseDcraw();
}

void AdvancedRawViewWidget::rawEnabledStatusChanged(int state)
{
    controller->setRawStatus(state);
}

void AdvancedRawViewWidget::dcrawOptionsStringChanged()
{
    controller->setDcrawOptionsString(dcrawOptions->text());
}
