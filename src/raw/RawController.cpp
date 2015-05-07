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
 * Program URL: http://marek629.github.io/sir/
 */

#include "raw/RawController.hpp"

#include "raw/RawModel.hpp"
#include "raw/RawView.hpp"
#include "widgets/MessageBox.hpp"
#include "widgets/options/CommonOptions.hpp"

#include <QFileDialog>


RawController::RawController(RawModel *model, RawView *view, QObject *parent)
    : QObject(parent)
{
    this->model = model;
    this->view = view;
    this->view->setController(this);
}

void RawController::loadSettings()
{
    view->rawCheckBox->setChecked(model->isEnabled());

    setRawStatus(model->isEnabled());

    view->dcrawLineEdit->setText(model->getDcrawPath());
    view->dcrawOptions->setText(model->getDcrawOptions());
}

void RawController::saveSettings()
{
    bool dcrawOk = false;
    bool firstState = view->rawCheckBox->isChecked();

    // check dcraw executable
    if (view->rawCheckBox->isChecked()) {
        if ((dcrawOk = checkDcrawPath(view->dcrawLineEdit->text()))) {
            model->setEnabled(true);
            model->setDcrawPath(view->dcrawLineEdit->text());
            model->setDcrawOptions(view->dcrawOptions->text());
        } else {
            view->rawCheckBox->setChecked(false);

            model->setEnabled(false);
            model->setDcrawPath(view->dcrawLineEdit->text());
            model->setDcrawOptions(view->dcrawOptions->text());

            setRawStatus(false);
        }
    } else {
        model->setEnabled(false);
        model->setDcrawPath(view->dcrawLineEdit->text());
        model->setDcrawOptions(view->dcrawOptions->text());
    }

    if (dcrawOk || !firstState) {
        emit ok();
        view->window()->close();
    }
}

void RawController::browseDcraw()
{
    QString fileName = QFileDialog::getOpenFileName(
                view, tr("Select dcraw executable"),
                CommonOptions::instance()->targetDirPath());

    if (fileName.isEmpty())
        return;

    fileName = QDir::toNativeSeparators(fileName);
    view->dcrawLineEdit->setText(fileName);
}

void RawController::setRawStatus(int state)
{
    view->dcrawLineEdit->setEnabled(state);
    view->dcrawPushButton->setEnabled(state);
    view->dcrawOptions->setEnabled(state);
}

bool RawController::checkDcrawPath(const QString &fileName)
{
    if (fileName.isEmpty()) {
        MessageBox::warning(
                    view, "SIR",
                    tr("No dcraw executable chosen. "
                       "RAW support will not be enabled!"));
        return false;
    } else {
        QFile dcraw(fileName);
        if (dcraw.exists()) {
            if (dcraw.permissions().testFlag(QFile::ExeOwner)) {
                return true;
            } else {
                MessageBox::warning(
                            view, "SIR",
                            tr("The chosen file is not executable. "
                               "RAW support will not be enabled!"));
                return false;
            }
        } else {
            MessageBox::warning(
                        view, "SIR",
                        tr("dcraw executable not found. "
                           "RAW support will not be enabled!"));
            return false;
        }
    }
}
