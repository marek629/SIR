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

#include "detailsgroupboxview.h"
#include "detailsgroupboxcontroller.h"

DetailsGroupBoxView::DetailsGroupBoxView(QWidget *parent) : AbstractOptionsGroupBox(parent) {
    setupUi(this);

    // create connections
    // push buttons
    connect(showPushButton, SIGNAL(clicked()), this, SLOT(showPushButtonClicked()));
    connect(hidePushButton, SIGNAL(clicked()), this, SLOT(hidePushButtonClicked()));
    // tabs
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}

void DetailsGroupBoxView::loadSettings() {
    controller->loadSettings();
}

void DetailsGroupBoxView::saveSettings() {
    controller->saveSettings();
}

void DetailsGroupBoxView::setController(DetailsGroupBoxController *controller) {
    this->controller = controller;
}

/** Enables or disables \em "Show all" and \em "Hide all" fields push buttons
  * depending on parameters values.
  * \param enableShow Enables or disables the \em "Show all" showPushButton push button
  * \param enableHide Enables or disables the \em "Hide all" hidePushButton push button
  */
void DetailsGroupBoxView::enableFieldButtons(bool enableShow, bool enableHide) {
    showPushButton->setEnabled(enableShow);
    hidePushButton->setEnabled(enableHide);
}

void DetailsGroupBoxView::exifFieldToggled(bool checked) {
    controller->checkExifFields(checked);
}

void DetailsGroupBoxView::iptcFieldToggled(bool checked) {
    controller->checkIptcFields(checked);
}

void DetailsGroupBoxView::showPushButtonClicked() {
    controller->showAllFields();
}

void DetailsGroupBoxView::hidePushButtonClicked() {
    controller->hideAllFields();
}

void DetailsGroupBoxView::tabChanged(int currentTab) {
    controller->setCurrentTab(currentTab);
}
