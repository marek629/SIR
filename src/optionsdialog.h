/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2012  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://sir.projet-libre.org/
 */

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include "ui_optionsdialog.h"

class LanguageUtils;

//! Settings wizard window.
class OptionsDialog : public QDialog, public Ui::OptionsDialog {
    Q_OBJECT

public:
    OptionsDialog( QWidget * parent = 0, Qt::WFlags f = 0);
    ~OptionsDialog();
    static quint8 detectCoresCount();

private:
    // fields
    LanguageUtils * languages;
    QMap<QString, QString> * fileToNiceName;
    QRegExpValidator* validator;
    QGroupBox** groupBoxes;
    quint8 currentListItem;
    quint8 coresCount;
    static quint8 maxCoresCount;
    quint8 maxHistoryCount;
    // methods
    void createLanguageMenu();
    void createConnections();
    bool checkDcrawPath(QString fileName);
    void setupWindow();

private slots:
    void saveSettings();
    void loadSettings();
    void browseDestination();
    void browseDcraw();
    void setRawStatus(int state);
    void respondCoresSpinBox(bool checked);
    void enableMetadata(bool checked);
    void saveMetadata(bool save);
    void updateThumbnail(bool update);
    void categoryChanged(int current);

signals:
    void ok(); /**< Indicates write settings success. */
};
#endif
