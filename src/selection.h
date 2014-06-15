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

#ifndef SELECTION_H
#define SELECTION_H

#include <QFileInfoList>
#include <QStringList>
#include <QDateTime>
#include "metadata/metadatautils.h"

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
    sir::String fileName;
    sir::String fileSizeExp; // file size expression
    sir::String imageSizeExp;
    // any metadata
    bool checkMetadata;
    QDateTime createdDateTime[2];
    QDateTime digitizedDateTime[2];
    sir::String author;
    sir::String copyright;
    // Exif
    bool checkExif;
    sir::String processingSoftware;
    sir::String cameraManufacturer;
    sir::String cameraModel;
    // IPTC
    bool checkIPTC;
    sir::String objectName;
    sir::String keywords;
    sir::String description;
    sir::String countryName;
    sir::String city;
    sir::String editStatus;
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
#ifdef SIR_METADATA_SUPPORT
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
#endif // SIR_METADATA_SUPPORT
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
    void setupListRegExp(const sir::String &strExp, QList<QRegExp*> *listRx);
    int loadFileInfo(const QString &dir, QFileInfoList *list, bool recursive);
    bool isInTime(const QDate &date, const QTime &time, QDateTime *dtArray);
    bool isInTime(const QDateTime &dateTime, QDateTime *dtArray);
    bool testFile(const QFileInfo &info);
    void setupExpressionTrees();
    void loadSymbols();
};

#endif // SELECTION_H
