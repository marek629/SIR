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

#include "widgets/ConvertDialogConnector.hpp"

#include "widgets/ConvertDialog.hpp"
#include "widgets/DetailsBrowserController.hpp"


ConvertDialogConnector::ConvertDialogConnector(ConvertDialog *dialog)
    : QObject(dialog)
{
    this->dialog = dialog;
}

void ConvertDialogConnector::createConnections()
{
    // TODO: extract separated methods for connect listed elements
    // tree view's list menagement buttons
    connect(dialog->addFilepushButton, SIGNAL(clicked()),
            dialog->filesTreeWidget, SLOT(addFile()));
    connect(dialog->addDirpushButton, SIGNAL(clicked()),
            dialog->filesTreeWidget, SLOT(addDir()));
    connect(dialog->removePushButton, SIGNAL(clicked()),
            dialog->filesTreeWidget, SLOT(removeSelectedFromList()));
    connect(dialog->removeAllPushButton, SIGNAL(clicked()),
            dialog->filesTreeWidget, SLOT(removeAll()));
    // and actions
    connect(dialog->actionAdd_File, SIGNAL(triggered()),
            dialog->filesTreeWidget, SLOT(addFile()));
    connect(dialog->actionAdd_Dir, SIGNAL(triggered()),
            dialog->filesTreeWidget, SLOT(addDir()));
    connect(dialog->actionSelect, SIGNAL(triggered()),
            dialog, SLOT(showSelectionDialog()));
    connect(dialog->actionImport_files, SIGNAL(triggered()),
            dialog, SLOT(showSelectionDialog()));
    connect(dialog->actionRemoveAll, SIGNAL(triggered()),
            dialog->filesTreeWidget, SLOT(removeAll()));

    // convert options
    connect(dialog->targetFormatComboBox, SIGNAL(currentIndexChanged(QString)),
            dialog->optionsScrollArea, SLOT(onTargetFormatChanged(QString)));

    // status bar
    connect(dialog->filesTreeWidget, SIGNAL(loadingFilesStart(int)),
            dialog->statusWidget, SLOT(onFilesLoadingStart(int)));
    connect(dialog->filesTreeWidget, SIGNAL(loadingFilesTick(int)),
            dialog->statusWidget, SLOT(onFilesLoadingTick(int)));
    connect(dialog->filesTreeWidget, SIGNAL(loadingFilesStop()),
            dialog->statusWidget, SLOT(onFilesLoadingStop()));
    connect(dialog->detailsBrowserController(), SIGNAL(loadingDetailsStart()),
            dialog->statusWidget, SLOT(onDetailsLoadingStart()));
    connect(dialog->detailsBrowserController(), SIGNAL(loadingDetailsStop()),
            dialog->statusWidget, SLOT(onDetailsLoadingStop()));
    connect(dialog, SIGNAL(convertStart(int)),
            dialog->statusWidget, SLOT(onConvetionStart(int)));
    connect(dialog, SIGNAL(convertTick(int)),
            dialog->statusWidget, SLOT(onConvetionTick(int)));
    connect(dialog, SIGNAL(convertStop()),
            dialog->statusWidget, SLOT(onConvetionStop()));

    // menu actions
    connect(dialog->actionExit, SIGNAL(triggered()),
            dialog, SLOT(close()));
    connect(dialog->actionAbout_Qt, SIGNAL(triggered()),
            qApp, SLOT(aboutQt()));
    connect(dialog->actionAbout_Sir, SIGNAL(triggered()),
            dialog, SLOT(about()));
    connect(dialog->actionOptions, SIGNAL(triggered()),
            dialog, SLOT(setOptions()));
    connect(dialog->actionCheckforUpdates, SIGNAL(triggered()),
            dialog, SLOT(checkUpdates()));
    connect(dialog->actionRestoreSession, SIGNAL(triggered()),
            dialog, SLOT(restoreSession()));
    connect(dialog->actionSaveSession, SIGNAL(triggered()),
            dialog, SLOT(saveSession()));
    connect(dialog->actionRestoreEffects, SIGNAL(triggered()),
            dialog, SLOT(restoreEffects()));
    connect(dialog->actionSaveEffects, SIGNAL(triggered()),
            dialog, SLOT(saveEffects()));
    connect(dialog->actionSendInstall, SIGNAL(triggered()),
            dialog, SLOT(sendInstall()));

    // browse button
    connect(dialog->browseDestButton, SIGNAL(clicked()),
            dialog, SLOT(browseDestination()));

    // convert... & stop/exit buttons
    connect(dialog->convertButton, SIGNAL(clicked()),
            dialog, SLOT(convertAll()));
    connect(dialog->convertSelectedButton, SIGNAL(clicked()),
            dialog, SLOT(convertSelected()));
    connect(dialog->quitButton, SIGNAL(clicked()),
            dialog, SLOT(closeOrCancel()));
}
