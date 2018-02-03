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

#include "raw/BasicRawViewScrollArea.hpp"

#include "raw/RawController.hpp"
#include "raw/basic/BasicRawScrollAreaAdapter.hpp"
#include "raw/basic/DcrawOptionsCollector.hpp"
#include "raw/basic/OptionsCollectorFactory.hpp"


BasicRawViewScrollArea::BasicRawViewScrollArea(QWidget *parent)
    : QScrollArea(parent), RawView()
{
    ui.setupUi(this);

    connect(ui.dcrawPushButton, SIGNAL(clicked()),
            this, SLOT(onBrowseButtonClick()));
    connect(ui.eraseNoiseCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(onEraseNoiseToggle(bool)));
    connect(ui.interpolationPostProcessingCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(onInterpolationPostProcessingToggle(bool)));
    connect(ui.saturationCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(onSaturationToggle(bool)));
    connect(ui.rawCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(onRawEnabledChange(int)));
}

BasicRawViewScrollArea::~BasicRawViewScrollArea() {}

QWidget *BasicRawViewScrollArea::qWidget()
{
    return this;
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

bool BasicRawViewScrollArea::isPathTextEnabledInput() const
{
    return ui.dcrawLineEdit->isEnabled();
}

void BasicRawViewScrollArea::setPathTextEnabledInput(bool inputEnabled)
{
    ui.dcrawLineEdit->setEnabled(inputEnabled);
}

QString BasicRawViewScrollArea::optionsText() const
{
    Ui::BasicRawScrollArea *uiPointer = const_cast<Ui::BasicRawScrollArea*>(&ui);
    BasicRawScrollAreaAdapter uiAdapter = BasicRawScrollAreaAdapter(uiPointer);

    OptionsCollectorFactory factory;
    std::unique_ptr<DcrawOptionsCollector> collector =
            factory.dcrawFullDecoratedCollector(&uiAdapter);

    return collector->optionsString().simplified();
}

void BasicRawViewScrollArea::setOptionsText(const QString &text)
{
    Ui::BasicRawScrollArea *uiPointer = const_cast<Ui::BasicRawScrollArea*>(&ui);
    BasicRawScrollAreaAdapter uiAdapter = BasicRawScrollAreaAdapter(uiPointer);

    OptionsCollectorFactory factory;
    std::unique_ptr<DcrawOptionsCollector> collector =
            factory.dcrawFullDecoratedCollector(&uiAdapter);

    collector->setOptions(text);
}

bool BasicRawViewScrollArea::isOptionsTextEnabledInput() const
{
    return ui.interpolationGroupBox->isEnabled();
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

bool BasicRawViewScrollArea::isButtonEnabledInput() const
{
    return ui.dcrawPushButton->isEnabled();
}

void BasicRawViewScrollArea::setButtonEnabledInput(bool inputEnabled)
{
    ui.dcrawPushButton->setEnabled(inputEnabled);
}

bool BasicRawViewScrollArea::closeWindow()
{
    return window()->close();
}

void BasicRawViewScrollArea::retranslateStrings()
{
    ui.retranslateUi(this);
}

void BasicRawViewScrollArea::onBrowseButtonClick()
{
    controller->browseDcraw();
}

void BasicRawViewScrollArea::onEraseNoiseToggle(bool toggled)
{
    static int spinBoxEnabledValue = 100;

    spinBoxEnabledValue = toggleSpinBox(toggled,
                                        ui.eraseNoiseSpinBox,
                                        spinBoxEnabledValue);
}

void BasicRawViewScrollArea::onInterpolationPostProcessingToggle(bool toggled)
{
    static int spinBoxEnabledValue = 1;

    spinBoxEnabledValue = toggleSpinBox(toggled,
                                        ui.interpolationPostProcessingSpinBox,
                                        spinBoxEnabledValue);
}

void BasicRawViewScrollArea::onSaturationToggle(bool toggled)
{
    static int spinBoxEnabledValue = 14000;

    spinBoxEnabledValue = toggleSpinBox(toggled,
                                        ui.saturationSpinBox,
                                        spinBoxEnabledValue);
}

void BasicRawViewScrollArea::onRawEnabledChange(int state)
{
    controller->setRawStatus(state);
}

int BasicRawViewScrollArea::toggleSpinBox(bool enabled, QSpinBox *spinBox,
                                          int spinBoxEnabledValue)
{
    if (enabled) {
        spinBox->setMinimum(1);
        spinBox->setValue(spinBoxEnabledValue);
    } else {
        spinBoxEnabledValue = spinBox->value();
        spinBox->setMinimum(0);
        spinBox->setValue(0);
    }

    spinBox->setEnabled(enabled);

    return spinBoxEnabledValue;
}
