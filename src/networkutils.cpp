/*
 * This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto
 * 2011-2012  Marek Jędryka
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
 *                 Marek Jędryka   <jedryka89@gmail.com>
 * Program URL: http://sir.projet-libre.org/
 *
 */

#include "networkutils.h"
#include "defines.h"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

/** Default constructor. */
NetworkUtils::NetworkUtils() : QObject() {}

/** Destructor. */
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

    QUrl url("http://sir.projet-libre.org/check_updates.php?version="+QString(VERSION));
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
    request.setUrl(QUrl("http://sir.projet-libre.org/add_unique_install.php"));
    request.setRawHeader("User-Agent", "SIR");

    reply = nam->get(request);

}

/** Emits checkDone signal.
  * \note In fact, result information could be shown in checkDone slot.
  */
void NetworkUtils::showResults(QNetworkReply* reply) {

    QString *response = new QString("");
    bool error;

    // no error received?
    if (reply->error() == QNetworkReply::NoError) {
        // read data from QNetworkReply here

        QByteArray bytes = reply->readAll();
        response = new QString(bytes);
        error = false;

    }
    // Some http error received
    else
    {
        error = true;
    }

    emit checkDone(response, error);

}
