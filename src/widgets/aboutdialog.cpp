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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
 *                 Marek Jędryka   <jedryka89@gmail.com>
 * Program URL: http://marek629.github.io/sir/
 *
 */

#include "aboutdialog.h"

/** Default constructor. */
AboutDialog::AboutDialog(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f) {
    setupUi(this);
    setAboutText();
    retranslateUi(this);
}

/** Sets version string. */
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
            + "<p>" + tr("To enable dcraw visit: ")
            + htmlLink("http://www.cybercom.net/~dcoffin/dcraw/") + "<br />"
            + tr("or do a sudo apt-get install dcraw on Debian based systems.") + "</p>"
            + htmlLink("http://marek629.github.io/sir/");
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
    if (text3.contains('@')) { // convert to email address
        QString mail = text3.toLower();
        result += "<td><a href=\"mailto:" + mail + "\">" + mail + "</a></td></tr>";
    }
    else
        result += "<td>" + text3 + "</td></tr>";
    return result;
}

QString AboutDialog::htmlLink(const QString &url) const {
    QString result = "<a href=\"" + url + "\">" + url + "</a>";
    return result;
}
