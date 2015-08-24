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

#include "OptionsGroupBoxManager.hpp"

#include "widgets/options/AbstractOptionsGroupBox.hpp"

#include <QLayout>


OptionsGroupBoxManager::OptionsGroupBoxManager(int groupBoxListSize)
    : groupBoxListSize(groupBoxListSize)
{
    currentGroupBoxIndex = emptyListIndex;
}

OptionsGroupBoxManager::~OptionsGroupBoxManager() {}

void OptionsGroupBoxManager::append(AbstractOptionsGroupBox *groupBox)
{
    Q_ASSERT(groupBoxList.count() < groupBoxListSize);

    groupBoxList.append(groupBox);
}

AbstractOptionsGroupBox *OptionsGroupBoxManager::currentGroupBox()
{
    if (currentGroupBoxIndex == emptyListIndex) {
        return NULL;
    }

    Q_ASSERT(currentGroupBoxIndex >= 0);
    Q_ASSERT(currentGroupBoxIndex < groupBoxList.size());
    Q_ASSERT(currentGroupBoxIndex < groupBoxListSize);

    return groupBoxList.value(currentGroupBoxIndex);
}

AbstractOptionsGroupBox *OptionsGroupBoxManager::groupBoxByIndex(int index)
{
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < groupBoxList.size());
    Q_ASSERT(index < groupBoxListSize);

    return groupBoxList.value(index);
}

void OptionsGroupBoxManager::setCurrentGroupBoxIndex(int index)
{
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < groupBoxList.size());
    Q_ASSERT(index < groupBoxListSize);

    currentGroupBoxIndex = index;
}

void OptionsGroupBoxManager::loadSettings()
{
    Q_ASSERT(groupBoxList.size() == groupBoxListSize);

    for (int index = 0; index < groupBoxListSize; index++) {
        groupBoxByIndex(index)->loadSettings();
    }
}

void OptionsGroupBoxManager::saveSettings()
{
    Q_ASSERT(groupBoxList.size() == groupBoxListSize);

    for (int index = 0; index < groupBoxListSize; index++) {
        groupBoxByIndex(index)->saveSettings();
    }
}

void OptionsGroupBoxManager::hideAll()
{
    Q_ASSERT(groupBoxList.size() == groupBoxListSize);

    for (int index = 0; index < groupBoxListSize; index++) {
        groupBoxByIndex(index)->hide();
    }
}

void OptionsGroupBoxManager::addToLayout(QLayout *layout)
{
    Q_ASSERT(groupBoxList.size() == groupBoxListSize);

    for (int index = 0; index < groupBoxListSize; index++) {
        layout->addWidget(groupBoxByIndex(index));
    }
}

void OptionsGroupBoxManager::onCategoryChange(int newIndex)
{
    if (newIndex == currentGroupBoxIndex) {
        return;
    }

    currentGroupBox()->hide();
    setCurrentGroupBoxIndex(newIndex);
    currentGroupBox()->show();
}