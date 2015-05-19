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

#include "widgets/options/RawGroupBoxController.hpp"

#include "raw/RawController.hpp"
#include "raw/RawModel.hpp"
#include "raw/RawViewAdapter.hpp"
#include "widgets/MessageBox.hpp"
#include "widgets/options/CommonOptions.hpp"
#include "widgets/options/RawGroupBoxView.hpp"

#include <QFileDialog>


/** Default constructor.\n
  * Sets UI and creates connections.
  * \sa setupUi()
  */
RawGroupBoxController::RawGroupBoxController(RawModel *model,
                                             RawGroupBoxView *view,
                                             QObject *parent)
    : AbstractOptionsController(parent)
{
    this->model = model;
    this->view = view;
    this->view->setController(this);

    this->adaptedController = new RawController(model, new RawViewAdapter(view),
                                                parent);
    connect(adaptedController, SIGNAL(ok()), this, SLOT(propagateOk()));
}

RawGroupBoxController::~RawGroupBoxController()
{
    // TODO: delete adaptedController.view (memory leak?)
    delete adaptedController;
}

/** Shows file name dialog and sets path of selected file as dcraw path line
  * edit text.
  */
void RawGroupBoxController::browseDcraw()
{
    adaptedController->browseDcraw();
}

/** Enables or disables member widgets depending on \a state value. */
void RawGroupBoxController::setRawStatus(int state)
{
    adaptedController->setRawStatus(state);
}

void RawGroupBoxController::propagateOk()
{
    emit ok();
}

/** Load settings and sets member widgets values.
  * \sa Settings saveSettings()
  */
void RawGroupBoxController::loadSettings()
{
    adaptedController->loadSettings();
}

/** Saves settings basing member widgets values.
  * \sa Settings loadSettings()
  */
void RawGroupBoxController::saveSettings()
{
    adaptedController->saveSettings();
}
