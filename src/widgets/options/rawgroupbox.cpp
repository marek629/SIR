/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2012  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://sir.projet-libre.org/
 */

#include <QFileDialog>
#include <QMessageBox>
#include "rawgroupbox.h"
#include "settings.h"

/** Default constructor.\n
  * Sets UI, loads settings and create connections.
  * \sa setupUi() loadSettings()
  */
RawGroupBox::RawGroupBox(QWidget *parent) : QGroupBox(parent) {
    setupUi(this);
    loadSettings();
    connect(dcrawPushButton, SIGNAL(clicked()), this, SLOT(browseDcraw()));
    connect(rawCheckBox, SIGNAL(stateChanged(int)), SLOT(setRawStatus(int)));
}

/** Shows file name dialog and sets path of selected file as dcraw path line
  * edit text.
  */
void RawGroupBox::browseDcraw() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select dcraw executable"),
                                                    common.targetDirPath() );
    fileName = QDir::convertSeparators(fileName);
    dcrawLineEdit->setText(fileName);
}

/** Enables or disables member widgets depending on \a state value. */
void RawGroupBox::setRawStatus(int state) {
    dcrawLineEdit->setEnabled(state);
    dcrawPushButton->setEnabled(state);
    dcrawOptions->setEnabled(state);
}

/** Load settings and sets member widgets values.
  * \sa Settings saveSettings()
  */
void RawGroupBox::loadSettings() {
    Settings &s = Settings::instance();
    int state =             s.raw.enabled;
    rawCheckBox->setChecked(state);
    setRawStatus(state);
    dcrawLineEdit->setText( s.raw.dcrawPath);
    dcrawOptions->setText(  s.raw.dcrawOptions);
}

/** Saves settings basing member widgets values.
  * \sa Settings loadSettings()
  */
void RawGroupBox::saveSettings() {
    Settings &s = Settings::instance();
    bool dcrawOk = false;
    bool firstState = rawCheckBox->isChecked();
    //check dcraw executable
    if(rawCheckBox->isChecked()) {
        if((dcrawOk = checkDcrawPath(dcrawLineEdit->text()))) {
            s.raw.enabled       = true;
            s.raw.dcrawPath     = dcrawLineEdit->text();
            s.raw.dcrawOptions  = dcrawOptions->text();
        }
        else {
            s.raw.enabled       = false;
            rawCheckBox->setChecked(false);
            s.raw.dcrawPath     = dcrawLineEdit->text();
            s.raw.dcrawOptions  = dcrawOptions->text();
            setRawStatus(false);
        }
    }
    else {
        s.raw.enabled           = false;
        s.raw.dcrawPath         = dcrawLineEdit->text();
        s.raw.dcrawOptions      = dcrawOptions->text();
    }
    if(dcrawOk || !firstState) {
        emit ok();
        window()->close();
    }
}

/** Checks \a fileName dcraw path and returns true if this path is valid
  * (file exists and is executalbe). Otherwise returns false.\n
  * This function shows user warning dialog before returns false.
  */
bool RawGroupBox::checkDcrawPath(QString fileName) {
    if (!fileName.isEmpty()) {
        QFile dcraw(fileName);
        //Check if the file exists
        if(!dcraw.exists()) {
            QMessageBox::warning(
                        this, "SIR",
                        tr("dcraw executable not found. "
                           "RAW support will not be enabled!")
                        );
            return false;
        }
        else {
            //is executable??
            if(dcraw.permissions().testFlag(QFile::ExeOwner)) {
                //the file is executable
                return true;
            }
            else {
                //Not executable
                QMessageBox::warning(
                        this, "SIR",
                        tr("The chosen file is not executable. "
                           "RAW support will not be enabled!" )
                );
                return false;

            }
        }
    }
    else {
        QMessageBox::warning(
                        this, "SIR",
                        tr("No dcraw executable chosen. "
                           "RAW support will not be enabled!" )
                        );
        return false;
    }
}
