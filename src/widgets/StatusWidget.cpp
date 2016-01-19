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
    setTextMessageLabel(statusWidgetState);
    setTextOfLabel(statusWidgetState);
}

void StatusWidget::setStatus(StatusWidgetState status, int partQuantity,
                             int totalQuantity) {
    statusWidgetState = status;

    setTextMessageLabel(statusWidgetState);
    setTextOfLabel(statusWidgetState);

    if (totalQuantity > 0 && totalQuantity != partQuantity) {
        partLabel->setText(QString::number(partQuantity));
        totalLabel->setText(QString::number(totalQuantity));
    } else {
        partLabel->setText("");
        totalLabel->setText("");
    }
}

void StatusWidget::onFilesLoadingStart(int totalQuantity) {
    setStatus(StatusFilesLoading, 0, totalQuantity);
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
    setStatus(StatusReady);
    QCoreApplication::processEvents();
}

void StatusWidget::onDetailsLoadingStart()
{
    setStatus(StatusDetailsLoading);
    QCoreApplication::processEvents();
}

void StatusWidget::onDetailsLoadingStop()
{
    setStatus(StatusReady);
    QCoreApplication::processEvents();
}

void StatusWidget::onConvetionStart(int totalQuantity) {
    setStatus(StatusConvertionProgress, 0, totalQuantity);

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

    setStatus(StatusConvertionSummary);
}

void StatusWidget::setTextMessageLabel(StatusWidgetState statusWidgetState) {
    switch (statusWidgetState) {
    case StatusReady:
        messageLabel->setText(tr("Ready"));
        break;
    case StatusDetailsLoading:
        messageLabel->setText(tr("Loading image details..."));
        break;
    case StatusFilesLoading:
        messageLabel->setText(tr("Loading files..."));
        break;
    case StatusConvertionProgress:
        messageLabel->setText(tr("Converting images..."));
        break;
    case StatusConvertionSummary:
        QString summaryMessage = tr("%1 images converted in %2 seconds")
                .arg(convertionTotalQuantity)
                .arg(convertionElapsedSeconds);
        messageLabel->setText(summaryMessage);
        break;
    }
    messageLabel->update();
}

void StatusWidget::setTextOfLabel(StatusWidgetState statusWidgetState) {
    switch (statusWidgetState) {
    case StatusReady:
    case StatusDetailsLoading:
    case StatusConvertionSummary:
        ofLabel->setText("");
        break;
    case StatusFilesLoading:
    case StatusConvertionProgress:
        ofLabel->setText(tr("of"));
        break;
    }
}
