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

#include <QFileDialog>

#include "rawgroupboxcontroller.h"
#include "rawgroupboxview.h"
#include "commonoptions.h"
#include "widgets/messagebox.h"

/** Default constructor.\n
  * Sets UI and creates connections.
  * \sa setupUi()
  */
RawGroupBoxController::RawGroupBoxController(Settings::RawGroup *model, RawGroupBoxView *view, QObject *parent)
    : AbstractOptionsController(parent) {
    this->model = model;
    this->view = view;
    this->view->setController(this);
}

/** Shows file name dialog and sets path of selected file as dcraw path line
  * edit text.
  */
void RawGroupBoxController::browseDcraw() {
    QString fileName = QFileDialog::getOpenFileName(view,
                                                    tr("Select dcraw executable"),
                                                    CommonOptions::instance()->targetDirPath() );
    if (fileName.isEmpty())
        return;

    fileName = QDir::toNativeSeparators(fileName);
    view->dcrawLineEdit->setText(fileName);
}

/** Enables or disables member widgets depending on \a state value. */
void RawGroupBoxController::setRawStatus(int state) {
    view->dcrawLineEdit->setEnabled(state);
    view->dcrawPushButton->setEnabled(state);
    view->dcrawOptions->setEnabled(state);
}

/** Load settings and sets member widgets values.
  * \sa Settings saveSettings()
  */
void RawGroupBoxController::loadSettings() {
    int state = model->enabled;
    view->rawCheckBox->setChecked(state);
    setRawStatus(state);
    view->dcrawLineEdit->setText(model->dcrawPath);
    view->dcrawOptions->setText(model->dcrawOptions);
}

/** Saves settings basing member widgets values.
  * \sa Settings loadSettings()
  */
void RawGroupBoxController::saveSettings() {
    bool dcrawOk = false;
    bool firstState = view->rawCheckBox->isChecked();
    //check dcraw executable
    if (view->rawCheckBox->isChecked()) {
        if((dcrawOk = checkDcrawPath(view->dcrawLineEdit->text()))) {
            model->enabled = true;
            model->dcrawPath = view->dcrawLineEdit->text();
            model->dcrawOptions = view->dcrawOptions->text();
        }
        else {
            view->rawCheckBox->setChecked(false);
            model->enabled = false;
            model->dcrawPath = view->dcrawLineEdit->text();
            model->dcrawOptions = view->dcrawOptions->text();
            setRawStatus(false);
        }
    }
    else {
        model->enabled = false;
        model->dcrawPath = view->dcrawLineEdit->text();
        model->dcrawOptions = view->dcrawOptions->text();
    }
    if (dcrawOk || !firstState) {
        emit ok();
        view->window()->close();
    }
}

/** Checks \a fileName dcraw path and returns true if this path is valid
  * (file exists and is executalbe). Otherwise returns false.\n
  * This function shows user warning dialog before returns false.
  */
bool RawGroupBoxController::checkDcrawPath(const QString &fileName) {
    if (fileName.isEmpty()) {
        MessageBox::warning(view, "SIR",
            tr("No dcraw executable chosen. RAW support will not be enabled!"));
        return false;
    }
    else {
        QFile dcraw(fileName);
        if (dcraw.exists()) {
            if (dcraw.permissions().testFlag(QFile::ExeOwner)) {
                return true;
            }
            else {
                MessageBox::warning(view, "SIR",
                    tr("The chosen file is not executable. "
                       "RAW support will not be enabled!"));
                return false;
            }
        }
        else {
            MessageBox::warning(view, "SIR",
                tr("dcraw executable not found. RAW support will not be enabled!"));
            return false;
        }
    }
}
