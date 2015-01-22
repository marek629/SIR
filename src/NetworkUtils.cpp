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

#include "NetworkUtils.hpp"
#include "Version.hpp"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextDocument>
#include <QImage>

#include <QDebug>

NetworkUtils::NetworkUtils() : QObject() {}

NetworkUtils::~NetworkUtils() {
    delete reply;
}

/** Checks if update is avalaible on SIR website server.
  * \sa showResults
  */
void NetworkUtils::checkUpdates() {

    QNetworkAccessManager* nam;

    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(showResults(QNetworkReply*)));

    QUrl url("http://www.web.sir.ugu.pl/check_updates.php?version=" +
             Version().version() );
    reply = nam->get(QNetworkRequest(url));

}

/** Sends anonymous information about SIR instalation to SIR website server.
  * \sa showResults
  */
void NetworkUtils::sendInstalltoSite() {

    QNetworkAccessManager* nam;

    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(showResults(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(QUrl("http://www.web.sir.ugu.pl/add_unique_install.php"));
    request.setRawHeader("User-Agent", "SIR");

    reply = nam->get(request);

}

void NetworkUtils::getImage(const QUrl &url) {
    QNetworkAccessManager *networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onGetImageFinished(QNetworkReply*)));

    reply = networkAccessManager->get(QNetworkRequest(url));
}

/** Emits checkDone signal.
  * \note In fact, result information could be shown in checkDone slot.
  */
void NetworkUtils::showResults(QNetworkReply* reply) {

    QString *response = NULL;
    bool error;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        QString html(bytes);
        QTextDocument document(html, this);

        response = new QString(document.toPlainText());
        error = false;
    } else {
        response = new QString("");
        error = true;
    }

    emit checkDone(response, error);
}

void NetworkUtils::onGetImageFinished(QNetworkReply *reply) {
    QImage *response = 0;
    bool error = false;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        qDebug() << "response size:" << bytes.size() << bytes.constData();

        response = new QImage();
        qDebug() << "load..." << response->loadFromData(bytes);
        error = false;
    } else {
        response = new QImage();
        error = true;
    }

    qDebug() << response->size() << response->isNull() << error;

    emit gotImage(response, error);
}
