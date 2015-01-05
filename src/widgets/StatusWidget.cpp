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

#include "StatusWidget.hpp"

StatusWidget::StatusWidget(QWidget *parent) : QWidget(parent) {
    setupUi(this);

    defaultMessage = tr("Ready");
    ofMessage = tr("of");

    messageLabel->setText(defaultMessage);
    partLabel->setText("");
    ofLabel->setText("");
    totalLabel->setText("");
}

void StatusWidget::setStatus(const QString &message, int partQuantity,
                             int totalQuantity) {
    messageLabel->setText(message);

    if (totalQuantity > 0) {
        partLabel->setText(QString::number(partQuantity));
        ofLabel->setText(ofMessage);
        totalLabel->setText(QString::number(totalQuantity));
    }
    else {
        partLabel->setText("");
        ofLabel->setText("");
        totalLabel->setText("");
    }
}