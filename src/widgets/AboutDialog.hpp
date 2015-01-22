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

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "ui_AboutDialog.h"

#include <QTemporaryFile>

//! Class provides a window containing informations about SIR.
class AboutDialog : public QDialog, public Ui::About {
    Q_OBJECT

public:
    AboutDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    void setVersion(QString version);

private:
    QTemporaryFile tempFile;

    inline void setAboutText();
    QString htmlTableHead(const QString &text1, const QString &text2) const;
    QString htmlTableHead(const QString &text1, const QString &text2,
                          const QString &text3) const;
    QString htmlTableRow(const QString &text1, const QString &text2) const;
    QString htmlTableRow(const QString &text1, const QString &text2,
                         const QString &text3) const;
    QString htmlTableEnd() const { return "</table>"; }
    QString htmlLink(const QString &url) const;
    QString htmlLink(const QString &url, const QString &label) const;
    QString htmlImage(const QString &url, const QString &alternativeLabel) const;

private slots:
    void onGotImage(QImage* img, bool error);
};

#endif
