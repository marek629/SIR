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

#include "StatusWidget.hpp"

StatusWidget::StatusWidget(QWidget *parent) : QWidget(parent) {
    setupUi(this);

    partLabel->setText("");
    ofLabel->setText("");
    totalLabel->setText("");

    statusWidgetState = StatusReady;

    retranslateStrings();

    tickTimer.start();
}

void StatusWidget::retranslateStrings() {
    readyMessage = tr("Ready");
    ofMessage = tr("of");
    filesLoadingMessage = tr("Loading files...");
    convertionMessage = tr("Converting images...");
    convertionSummaryMessage = tr("%1 images converted in %2 seconds");

    setTextMessageLabel(statusWidgetState);
    setTextOfLabel(statusWidgetState);
}

// TODO: change 1st parameter type to StatusWidgetState
void StatusWidget::setStatus(const QString &message, int partQuantity,
                             int totalQuantity) {
    messageLabel->setText(message);

    if (totalQuantity > 0 && totalQuantity != partQuantity) {
        partLabel->setText(QString::number(partQuantity));
        totalLabel->setText(QString::number(totalQuantity));
    }
    else {
        partLabel->setText("");
        totalLabel->setText("");
    }
}

void StatusWidget::onFilesLoadingStart(int totalQuantity) {
    setStatus(filesLoadingMessage, 0, totalQuantity);
    QCoreApplication::processEvents();
}

void StatusWidget::onFilesLoadingTick(int partQuantity) {
    qint64 elapsedMiliseconds = tickTimer.elapsed();

    if (elapsedMiliseconds > 500) {
        partLabel->setText(QString::number(partQuantity));
        QCoreApplication::processEvents();
        tickTimer.restart();
    }
}

void StatusWidget::onFilesLoadingStop() {
    setStatus(readyMessage);
    QCoreApplication::processEvents();
}

void StatusWidget::onConvetionStart(int totalQuantity) {
    setStatus(convertionMessage, 0, totalQuantity);

    QCoreApplication::processEvents();

    convertionTotalQuantity = totalQuantity;

    convertionTimer.start();
}

void StatusWidget::onConvetionTick(int partQuantity) {
    qint64 elapsedMiliseconds = tickTimer.elapsed();

    if (elapsedMiliseconds > 500) {
        partLabel->setText(QString::number(partQuantity));
        tickTimer.restart();
    }
}

void StatusWidget::onConvetionStop() {
    qint64 elapsedMiliseconds = convertionTimer.elapsed();
    convertionElapsedSeconds = elapsedMiliseconds / 1000 + 1;

    QString message = convertionSummaryMessage
            .arg(convertionTotalQuantity)
            .arg(convertionElapsedSeconds);
    setStatus(message);
}

void StatusWidget::setTextMessageLabel(StatusWidgetState statusWidgetState) {
    switch (statusWidgetState) {
    case StatusReady:
        messageLabel->setText(readyMessage);
        break;
    case StatusFilesLoading:
        messageLabel->setText(filesLoadingMessage);
        break;
    case StatusConvertionProgress:
        messageLabel->setText(convertionMessage);
        break;
    case StatusConvertionSummary:
        QString summaryMessage = convertionSummaryMessage
                .arg(convertionTotalQuantity)
                .arg(convertionElapsedSeconds);
        messageLabel->setText(summaryMessage);
        break;
    }
}

void StatusWidget::setTextOfLabel(StatusWidgetState statusWidgetState) {
    switch (statusWidgetState) {
    case StatusReady:
    case StatusConvertionSummary:
        ofLabel->setText("");
        break;
    case StatusFilesLoading:
    case StatusConvertionProgress:
        ofLabel->setText(ofMessage);
        break;
    }
}
