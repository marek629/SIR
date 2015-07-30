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

#include "widgets/convert/RawWidget.hpp"

#include "Settings.hpp"
#include "raw/RawController.hpp"
#include "raw/RawModel.hpp"
#include "raw/RawViewWidget.hpp"


RawWidget::RawWidget(QWidget *parent) : QWidget(parent)
{
    model = new RawModel(Settings::instance()->raw);
    view = new RawViewWidget(this);
    controller = new RawController(model, view);
    controller->loadSettings();

    QLayout *layout = new QHBoxLayout(this);
    layout->addWidget(view->qWidget());
    setLayout(layout);
}

void RawWidget::loadSettings(const RawModel &rawModel)
{
    model = new RawModel(rawModel);
    controller->setModel(model);
    controller->loadSettings();
}

RawModel RawWidget::rawModel() const
{
    return RawModel(*model);
}

// TODO: destructor
