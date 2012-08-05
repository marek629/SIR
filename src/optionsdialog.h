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

/** \brief File details options flags namespace.
  *
  * Flags are used to store settings whitch metadata print in file details view.
  * \sa ConvertDialog::showDetails
  */
namespace DetailsOptions {
/** Corresponding picture section in MetadataDialog's tool box. */
enum ExifPicture {
    ExifVersion = 0x1,
    ProcessingSoftware = 0x2,
    GeneratedDateAndTime = 0x4,
    DigitizedDateAndTime = 0x8,
    Orientation = 0x10
};
/** Corresponding photo section in MetadataDialog's tool box. */
enum ExifPhoto {
    FocalLenght = 0x1,
    Aperture = 0x2,
    ShutterSpeed = 0x4,
    IsoSpeed = 0x8,
    ExposureTime = 0x10,
    ExposureBias = 0x20,
    ExposureProgram = 0x40,
    LightMeteringMode = 0x80,
    FlashMode = 0x100
};
/** Corresponding camera section in MetadataDialog's tool box. */
enum ExifCamera {
    Manufacturer = 0x1,
    Model = 0x2
};
/** Corresponding author section in MetadataDialog's tool box. */
enum ExifAuthor {
    Artist = 0x1,
    Copyright = 0x2,
    UserComment = 0x4
};
/** Corresponding IPTC tab in MetadataDialog's tab widget. */
enum Iptc {
    ModelVersion = 0x1,
    DateCreated = 0x2,
    TimeCreated = 0x4,
    DigitizedDate = 0x8,
    DigitizedTime = 0x10,
    Byline = 0x20,
    CopyrightIptc = 0x40,
    ObjectName = 0x80,
    Keywords = 0x100,
    Caption = 0x200,
    CountryName = 0x400,
    City = 0x800,
    EditStatus = 0x1000
};
}

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
