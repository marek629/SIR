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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Program URL: http://sir.projet-libre.org/
 */

#ifndef SELECTION_H
#define SELECTION_H

#include "ui_selectiondialog.h"
#include "metadatautils.h"
#include <QFileInfoList>

class QProgressDialog;
class ConvertDialog;
class LogicalExpressionTree;

//! Conditionaly selection parameters struct.
struct SelectionParams {
    // selection
    bool selectImportedFiles;
    // directory
    QString path;
    bool browseSubdirs;
    // file
    MetadataUtils::String fileName;
    MetadataUtils::String fileSizeExp; // file size expression
    MetadataUtils::String imageSizeExp;
    // any metadata
    bool checkMetadata;
    QDateTime createdDateTime[2];
    QDateTime digitizedDateTime[2];
    MetadataUtils::String author;
    MetadataUtils::String copyright;
    // Exif
    bool checkExif;
    MetadataUtils::String processingSoftware;
    MetadataUtils::String cameraManufacturer;
    MetadataUtils::String cameraModel;
    // IPTC
    bool checkIPTC;
    MetadataUtils::String objectName;
    MetadataUtils::String keywords;
    MetadataUtils::String description;
    MetadataUtils::String countryName;
    MetadataUtils::String city;
    MetadataUtils::String editStatus;
};

//! Files conditionaly selection class.
class Selection : public QObject {
    Q_OBJECT
    friend class LogicalExpressionTree;
    typedef bool(Selection::*compareFileSizeFnPtr)(qint64);
    typedef bool(Selection::*compareImageSizeFnPtr)(int, const QList<int>&);
    typedef bool(Selection::*logicalFnPtr)(bool,bool);

public:
    Selection(ConvertDialog *parent = 0);
    ~Selection();
    int selectItems();
    int importFiles();

private:
    // fields
    SelectionParams params;
    ConvertDialog *convertDialog;
    QProgressDialog *progressDialog;
    QList<QRegExp*> fileNameListRx;
    QList<QRegExp*> anyAuthorListRx;
    QList<QRegExp*> anyCopyrightListRx;
    QList<QRegExp*> exifProcessingSoftwareListRx;
    QList<QRegExp*> exifCameraManufacturerListRx;
    QList<QRegExp*> exifCameraModelListRx;
    QList<QRegExp*> iptcObjectNameListRx;
    QList<QRegExp*> iptcKeywordsListRx;
    QList<QRegExp*> iptcDescriptionListRx;
    QList<QRegExp*> iptcCountryNameListRx;
    QList<QRegExp*> iptcCityListRx;
    QList<QRegExp*> iptcEditStatusListRx;
    static QStringList logicalOperators;
    static QStringList comparisonOperators;
    static QStringList allOperators;
    QStringList fileSizeSymbols;
    LogicalExpressionTree *fileSizeExpTree;
    QVector<qint64> *fileSizeVector;
    QStringList imageSizeSymbols;
    LogicalExpressionTree *imageSizeExpTree;
    QVector<qint64> *imageSizeVector;
    // methods
    void setupRegExps();
    void setupListRegExp(const MetadataUtils::String &strExp, QList<QRegExp*> *listRx);
    int loadFileInfo(const QString &dir, QFileInfoList *list, bool recursive);
    bool isCompatible(const QString &string, const QList<QRegExp*> &rxList);
    bool isCompatible(const QStringList &list, const QList<QRegExp*> &rxList);
    bool isCompatible(const QDate &date, const QTime &time, QDateTime *dtArray);
    bool isCompatible(const QDateTime &dateTime, QDateTime *dtArray);
    bool testFile(const QFileInfo &info);
    void clearPointerList(QList<QRegExp*> *list);
    void setupExpressionTrees();
    void loadSymbols();
};

//! Selection conditions dialog class.
class SelectionDialog : public QDialog, public Ui::SelectionDialog {
    Q_OBJECT

public:
    explicit SelectionDialog(SelectionParams *params, bool getDirPath,
                             QWidget *parent = 0, Qt::WindowFlags f = 0);

public slots:
    void accept();
    void browseDir();

private:
    // fields
    SelectionParams *params;
    ConvertDialog *convertDialog;
    // methods
    void loadSettings();
    void saveSettings();
};

#endif // SELECTION_H
