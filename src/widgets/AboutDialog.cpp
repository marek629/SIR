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

#include "widgets/AboutDialog.hpp"
#include "NetworkUtils.hpp"

#include <QDir>
#include <QPainter>


const QSize donateButtonSize(168, 47);


AboutDialog::AboutDialog(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f) {
    setupUi(this);
    tempFile.setTextModeEnabled(false);
    tempFile.setFileName(QDir::tempPath() + QDir::separator() + "SIR_donate_logo.jpg");
    textBrowser->setOpenExternalLinks(true);
    setAboutText();
    retranslateUi(this);
}

void AboutDialog::setVersion(QString version) {
    this->version->setText(version);
}

void AboutDialog::setAboutText() {
    QString htmlStart = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" "
            "\"http://www.w3.org/TR/REC-html40/strict.dtd\">"
            "<html><head><meta name=\"qrichtext\" content=\"1\" />"
            "<style type=\"text/css\">p, li { white-space: pre-wrap; }"
            "table { margin-bottom: 15px; } td { padding: 1px 3px; } </style>"
            "</head><body style=\" font-family:'Sans Serif'; font-size:9pt;"
            "font-weight:400; font-style:normal;\"><center>";
    QString htmlStop = "</center></body></html>";
    QString htmlContent = htmlTableHead(tr("Status"),tr("Developer"),tr("E-mail"))
            + htmlTableRow(tr("Main developer"), "Marek Jędryka", "jedryka89@gmail.com")
            + htmlTableRow(tr("Developer"), "Rafael Sachetto", "rsachetto@gmail.com")
            + htmlTableRow(tr("Code contributor"), "Víctor Fernández Martínez", "")
            + htmlTableEnd()
            + htmlTableHead(tr("Language"), tr("Translators"))
            + htmlTableRow(tr("German"), "Michael Kruger")
            + htmlTableRow(tr("Portuguese"), "Rafael Sachetto")
            + htmlTableRow(tr("Slovak"), "Jozef Riha")
            + htmlTableRow(tr("Spanish"), "Víctor Fernández Martínez, Garbage Collector")
            + htmlTableRow(tr("Russian"), "Renat Gar, касьянъ, Timur Antipin")
            + htmlTableRow(tr("Hungarian"), "Kopiás Csaba")
            + htmlTableRow(tr("Polish"), "zoteek, Marek Jędryka")
            + htmlTableRow(tr("Dutch"), "Youp")
            + htmlTableRow(tr("Romanian"), "Ovidiu Pop")
            + htmlTableRow(tr("French"), "Jérôme Launay")
            + htmlTableRow(tr("Greek"), "Steve Jemens")
            + htmlTableRow(tr("Czech"), "Pavel Fric")
            + htmlTableRow(tr("Serbian"), "")
            + htmlTableEnd()
            + "<p>"
            + tr("To enable dcraw visit: ")
            + htmlLink("http://www.cybercom.net/~dcoffin/dcraw/") + "<br />"
            + tr("or do a sudo apt-get install dcraw on Debian based systems.")
            + "</p>"
            + "<p>"
            + htmlLink("http://marek629.github.io/SIR/donate.html",
                       htmlImage("https://www.paypalobjects.com/en_US/PL/i/btn/btn_donateCC_LG.gif",
                                 tr("Donate"), donateButtonSize))
            + "</p>"
            + htmlLink("http://marek629.github.io/SIR/");
    textBrowser->setHtml(htmlStart + htmlContent + htmlStop);
}

QString AboutDialog::htmlTableHead(const QString &text1, const QString &text2) const {
    QString result = "<table><tr><th>" + text1 + "</th>" + "<th>" + text2 + "</th></tr>";
    return result;
}

QString AboutDialog::htmlTableHead(const QString &text1, const QString &text2,
                                const QString &text3) const {
    QString result = "<table><tr><th>" + text1 + "</th>"
            + "<th>" + text2 + "</th><th>" + text3 + "</th></tr>";
    return result;
}

QString AboutDialog::htmlTableRow(const QString &text1, const QString &text2) const {
    QString result = "<tr><td>" + text1 + "</td>" + "<td>" + text2 + "</td></tr>";
    return result;
}

QString AboutDialog::htmlTableRow(const QString &text1, const QString &text2,
                               const QString &text3) const {
    QString result = "<tr><td>" + text1 + "</td>" + "<td>" + text2 + "</td>";
    if (text3.contains('@')) {
        QString mail = text3.toLower();
        QString mailUrl = "mailto:" + mail;
        result += "<td>" + htmlLink(mailUrl, mail) + "</td></tr>";
    }
    else
        result += "<td>" + text3 + "</td></tr>";
    return result;
}

QString AboutDialog::htmlLink(const QString &url) const {
    return htmlLink(url, url);
}

QString AboutDialog::htmlLink(const QString &url, const QString &label) const {
    QString result = "<a href=\"" + url + "\">" + label + "</a>";
    return result;
}

QString AboutDialog::htmlImage(const QString &url, const QString &alternativeLabel,
                               const QSize &imageSize) const {
    QUrl urlObject = url;
    if (!urlObject.isValid())
        return alternativeLabel;

    QString result;

    if (urlObject.isLocalFile()) {
        result = "<img src=\"" + url + "\" alt=\"" + alternativeLabel +"\" />";
    } else {
        NetworkUtils *networkUtils = new NetworkUtils();
        connect(networkUtils, SIGNAL(gotImage(QImage*,bool)),
                this, SLOT(onGotImage(QImage*,bool)));
        networkUtils->getImage(urlObject);
        result = "<img width=\"" + QString::number(imageSize.width())
                + "\" height=\"" + QString::number(imageSize.height())
                + "\" src=\"" + tempFile.fileName()
                + "\" alt=\"" + alternativeLabel +"\" />";
    }

    return result;
}

void AboutDialog::onGotImage(QImage *img, bool error) {
    if (error) {
        delete img;
        img = new QImage(donateButtonSize, QImage::Format_ARGB32);
        img->fill(Qt::yellow);

        QString donateString = tr("Donate");
        QPainter painter(img);
        painter.drawText(img->rect(), Qt::AlignCenter, donateString);
    }

    QFile file(tempFile.fileName());
    if (!file.open(QIODevice::WriteOnly))
        return;

    img->save(&file);
    delete img;

    textBrowser->loadResource(QTextDocument::ImageResource,
                              QUrl(tempFile.fileName()));
}
