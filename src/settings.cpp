/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
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

#include <QFile>
#include <QDir>
#include <QLocale>
#include <QImageWriter>
#include <QCoreApplication>
#include "settings.h"
#include "languageutils.h"

/** Returns pointer to the instance of Settings class.
  * \sa Settings(const QString &, const QString &, QObject *)
  */
Settings * Settings::instance() {
#ifdef Q_OS_WIN32
    // avoid registry
    Settings::setDefaultFormat(Settings::IniFormat);
#else
    // UNIX *.conf files in INI format
    Settings::setDefaultFormat(Settings::NativeFormat);
#endif // Q_OS_WIN32
    static Settings *object = 0;
    if (!object && QCoreApplication::instance())
        object = new Settings("SIR", QString(), QCoreApplication::instance());
    return object;
}

/** Default constructor. Use this constructor only if you know what you do.
  * In instance() method is used
  * Settings(const QString &, const QString &, QObject *) constructor.
  */
Settings::Settings(QObject *parent) : QSettings(parent) {}

/** Current used in instance() method constructor. Reads settings from settings
  * file corresponding \a organization and \a application.
  * \sa Settings() readSettings()
  */
Settings::Settings(const QString &organization, const QString &application,
                   QObject *parent)
    : QSettings(organization, application, parent) {
    readSettings();
}

/** Destructor of Settings object. Writes settings to file.
  * \sa writeSettings()
  */
Settings::~Settings() {
    writeSettings();
}

/** Reads settings from settings file. Migrates old settings (i.e. from old SIR
  * releases) if it's necessary and load settings data to public structs with
  * \e Group suffix.
  * \sa migrateFrom_2_1() migrateFrom_2_2() migrateFrom_2_3() migrateFrom_2_4()
  *     writeSettings()
  */
void Settings::readSettings() {
    // old settings migration
    if (contains("MainWindow/cores"))
        migrateFrom_2_1();
    else if (contains("Settings/width"))
        migrateFrom_2_2();
    else if (contains("Exif/originalDateMap"))
        migrateFrom_2_3();
    else if (contains("Raw/raw"))
        migrateFrom_2_4();
    // now settings file is in current version
    // setting object fields
    beginGroup("MainWindow");
    mainWindow.maximized            = value("maximized",false).toBool();
    mainWindow.possition            = value("possition").toPoint();
    mainWindow.size                 = value("size").toSize();
    mainWindow.horizontalSplitter   = value("horizontalSplitter").toByteArray();
    mainWindow.verticalSplitter     = value("verticalSplitter").toByteArray();
    endGroup(); // MainWindow
    beginGroup("Settings");
    settings.alreadySent        = value("alreadySent",false).toBool();
    settings.targetFolder       = value("targetFolder",QDir::homePath()).toString();
    settings.targetFormat       = value("targetFormat","bmp").toString();
    settings.targetPrefix       = value("targetPrefix","web").toString();
    settings.targetSuffix       = value("targetSuffix","thumb").toString();
    LanguageUtils *languages = LanguageUtils::instance();
    QString localeFile = "sir_" + QLocale::system().name() + ".qm";
    QString defaultLanguage = languages->languageInfo(localeFile).niceName;
    settings.languageNiceName   = value("languageNiceName",defaultLanguage).toString();
    settings.languageFileName   = value("languageFileName",localeFile).toString();
    settings.dateDisplayFormat  = value("dateDisplayFormat","dd.MM.yyyy").toString();
    settings.timeDisplayFormat  = value("timeDisplayFormat","HH:mm:ss").toString();
    settings.lastDir            = value("lastDir",QDir::homePath()).toString();
    settings.quality            = value("quality",100).toInt();
    settings.cores              = value("cores",0).toInt();
    settings.maxHistoryCount    = value("maxHistoryCount",5).toInt();
    endGroup(); // Settings
    beginGroup("Size");
    size.widthPx            = value("widthPx",800).toInt();
    size.widthPercent       = value("widthPercent",100.f).toFloat();
    size.heightPx           = value("heightPx",600).toInt();
    size.heightPercent      = value("heightPercent",100.f).toFloat();
    size.fileSizeValue      = value("fileSizeValue",300.f).toFloat();
    size.fileSizeUnit       = value("fileSizeUnit",0).toInt();
    size.sizeUnit           = value("sizeUnit",0).toInt();
    size.keepAspectRatio    = value("keepAspectRatio",true).toBool();
    endGroup(); // Size
    beginGroup("TreeWidget");
    // all columns are visible by default
    treeWidget.columns = value("columns",0x3F).toInt();
    endGroup(); // TreeWidget
#ifdef SIR_METADATA_SUPPORT
    beginGroup("Metadata");
    metadata.enabled            = value("enabled",true).toBool();
    metadata.saveMetadata       = value("saveMetadata",true).toBool();
    metadata.realRotate         = value("realRotate",false).toBool();
    metadata.updateThumbnail    = value("updateThumbnail",true).toBool();
    metadata.rotateThumbnail    = value("rotateThumbnail",false).toBool();
    endGroup(); // Metadata
    beginGroup("Exif");
    exif.artistOverwrite        = value("artistOverwrite",false).toBool();
    exif.artistMap              = value("artistMap").toMap();
    exif.artistList             = value("artistList").toList();
    exif.copyrightOverwrite     = value("copyrightOverwrite",false).toBool();
    exif.copyrightMap           = value("copyrightMap").toMap();
    exif.copyrightList          = value("copyrightList").toList();
    exif.userCommentOverwrite   = value("userCommentOverwrite",false).toBool();
    exif.userCommentMap         = value("userCommentMap").toMap();
    exif.userCommentList        = value("userCommentList").toList();
    endGroup(); // Exif
    beginGroup("IPTC");
    iptc.editStatusMap  = value("editStatusMap").toMap();
    iptc.editStatusList = value("editStatusList").toList();
    endGroup(); // IPTC
    beginGroup("Details");
    details.exifImage  = value("exifImage",0x14).toInt();
    details.exifPhoto  = value("exifPhoto",0x1f).toInt();
    details.exifCamera = value("exifCamera",0x2).toInt();
    details.exifAuthor = value("exifAuthor",0x1).toInt();
    details.iptc = value("iptc",0xd00).toInt();
    endGroup(); // Details
#endif // SIR_METADATA_SUPPORT
    beginGroup("Selection");
    selection.subdirs           = value("subdirs",false).toBool();
    selection.clearSelection    = value("clearSelection",false).toBool();
    selection.selectImported    = value("selectImported",false).toBool();
    selection.fileSizeSymbol    = value("fileSizeSymbol","s").toString();
    selection.imageWidthSymbol  = value("imageWidthSymbol","x").toString();
    selection.imageHeightSymbol = value("imageHeightSymbol","y").toString();
    endGroup(); // Selection
    beginGroup("SelectionDialog");
    SelectionDialogGroup &sd = selectionDialog;
    sd.fileNameMap                  = value("fileNameMap").toMap();
    sd.fileNameList                 = value("fileNameList").toList();
    sd.fileSizeMap                  = value("fileSizeMap").toMap();
    sd.fileSizeList                 = value("fileSizeList").toList();
    sd.imageSizeMap                 = value("imageSizeMap").toMap();
    sd.imageSizeList                = value("imageSizeList").toList();
    sd.anyAuthorMap                 = value("anyAuthorMap").toMap();
    sd.anyAuthorList                = value("anyAuthorList").toList();
    sd.anyCopyrightMap              = value("anyCopyrightMap").toMap();
    sd.anyCopyrightList             = value("anyCopyrightList").toList();
    sd.exifSoftMap                  = value("exifSoftMap").toMap();
    sd.exifSoftList                 = value("exifSoftList").toList();
    sd.exifCameraManufacturerMap    = value("exifCameraManufacturerMap").toMap();
    sd.exifCameraManufacturerList   = value("exifCameraManufacturerList").toList();
    sd.exifCameraModelMap           = value("exifCameraModelMap").toMap();
    sd.exifCameraModelList          = value("exifCameraModelList").toList();
    sd.iptcObjectNameMap            = value("iptcObjectNameMap").toMap();
    sd.iptcObjectNameList           = value("iptcObjectNameList").toList();
    sd.iptcKeywordsMap              = value("iptcKeywordsMap").toMap();
    sd.iptcKeywordsList             = value("iptcKeywordsList").toList();
    sd.iptcDescriptionMap           = value("iptcDescriptionMap").toMap();
    sd.iptcDescriptionList          = value("iptcDescriptionList").toList();
    sd.iptcCountryNameMap           = value("iptcCountryNameMap").toMap();
    sd.iptcCountryNameList          = value("iptcCountryNameList").toList();
    sd.iptcCityMap                  = value("iptcCityMap").toMap();
    sd.iptcCityList                 = value("iptcCityList").toList();
    sd.iptcEditStatusMap            = value("iptcEditStatusMap").toMap();
    sd.iptcEditStatusList           = value("iptcEditStatusList").toList();
    endGroup(); // SelectionDialog
    beginGroup("Raw");
    raw.enabled         = value("enabled",false).toBool();
    raw.dcrawPath       = value("dcrawPath","/usr/bin/dcraw").toString();
    raw.dcrawOptions    = value("dcrawOptions","").toString();
    endGroup(); // Raw
}

/** Writes settings data from public structs with \e Group suffix to settings
  * file.
  * \sa readSettings()
  */
void Settings::writeSettings() {
    beginGroup("MainWindow");
    setValue("maximized",           mainWindow.maximized);
    setValue("possition",           mainWindow.possition);
    setValue("size",                mainWindow.size);
    setValue("horizontalSplitter",  mainWindow.horizontalSplitter);
    setValue("verticalSplitter",    mainWindow.verticalSplitter);
    endGroup(); // MainWindow
    beginGroup("Settings");
    setValue("alreadySent",         settings.alreadySent);
    setValue("targetFolder",        settings.targetFolder);
    setValue("targetFormat",        settings.targetFormat);
    setValue("targetPrefix",        settings.targetPrefix);
    setValue("targetSuffix",        settings.targetSuffix);
    setValue("languageNiceName",    settings.languageNiceName);
    setValue("languageFileName",    settings.languageFileName);
    setValue("dateDisplayFormat",   settings.dateDisplayFormat);
    setValue("timeDisplayFormat",   settings.timeDisplayFormat);
    setValue("lastDir",             settings.lastDir);
    setValue("quality",             settings.quality);
    setValue("cores",               settings.cores);
    setValue("maxHistoryCount",     settings.maxHistoryCount);
    endGroup(); // Settings
    beginGroup("Size");
    setValue("widthPx",         size.widthPx);
    setValue("widthPercent",    size.widthPercent);
    setValue("heightPx",        size.heightPx);
    setValue("heightPercent",   size.heightPercent);
    setValue("fileSizeValue",   size.fileSizeValue);
    setValue("fileSizeUnit",    size.fileSizeUnit);
    setValue("sizeUnit",        size.sizeUnit);
    setValue("keepAspectRatio", size.keepAspectRatio);
    endGroup(); // Size
    beginGroup("TreeWidget");
    setValue("columns", treeWidget.columns);
    endGroup(); // TreeWidget
#ifdef SIR_METADATA_SUPPORT
    beginGroup("Metadata");
    setValue("enabled",         metadata.enabled);
    setValue("saveMetadata",    metadata.saveMetadata);
    setValue("realRotate",      metadata.realRotate);
    setValue("updateThumbnail", metadata.updateThumbnail);
    setValue("rotateThumbnail", metadata.rotateThumbnail);
    endGroup(); // Metadata
    beginGroup("Exif");
    setValue("artistOverwrite",         exif.artistOverwrite);
    setValue("artistMap",               exif.artistMap);
    setValue("artistList",              exif.artistList);
    setValue("copyrightOverwrite",      exif.copyrightOverwrite);
    setValue("copyrightMap",            exif.copyrightMap);
    setValue("copyrightList",           exif.copyrightList);
    setValue("userCommentOverwrite",    exif.userCommentOverwrite);
    setValue("userCommentMap",          exif.userCommentMap);
    setValue("userCommentList",         exif.userCommentList);
    endGroup(); // Exif
    beginGroup("IPTC");
    setValue("editStatusMap",   iptc.editStatusMap);
    setValue("editStatusList",  iptc.editStatusList);
    endGroup(); // IPTC
    beginGroup("Details");
    setValue("exifImage",   details.exifImage);
    setValue("exifPhoto",   details.exifPhoto);
    setValue("exifCamera",  details.exifCamera);
    setValue("exifAuthor",  details.exifAuthor);
    setValue("iptc",        details.iptc);
    endGroup(); // Details
#endif // SIR_METADATA_SUPPORT
    beginGroup("Selection");
    setValue("subdirs",             selection.subdirs);
    setValue("clearSelection",      selection.clearSelection);
    setValue("selectImported",      selection.selectImported);
    setValue("fileSizeSymbol",      selection.fileSizeSymbol);
    setValue("imageWidthSymbol",    selection.imageWidthSymbol);
    setValue("imageHeightSymbol",   selection.imageHeightSymbol);
    endGroup(); // Selection
    beginGroup("SelectionDialog");
    SelectionDialogGroup &sd = selectionDialog;
    setValue("fileNameMap",                 sd.fileNameMap);
    setValue("fileNameList",                sd.fileNameList);
    setValue("fileSizeMap",                 sd.fileSizeMap);
    setValue("fileSizeList",                sd.fileSizeList);
    setValue("imageSizeMap",                sd.imageSizeMap);
    setValue("imageSizeList",               sd.imageSizeList);
    setValue("anyAuthorMap",                sd.anyAuthorMap);
    setValue("anyAuthorList",               sd.anyAuthorList);
    setValue("anyCopyrightMap",             sd.anyCopyrightMap);
    setValue("anyCopyrightList",            sd.anyCopyrightList);
    setValue("exifSoftMap",                 sd.exifSoftMap);
    setValue("exifSoftList",                sd.exifSoftList);
    setValue("exifCameraManufacturerMap",   sd.exifCameraManufacturerMap);
    setValue("exifCameraManufacturerList",  sd.exifCameraManufacturerList);
    setValue("exifCameraModelMap",          sd.exifCameraModelMap);
    setValue("exifCameraModelList",         sd.exifCameraModelList);
    setValue("iptcObjectNameMap",           sd.iptcObjectNameMap);
    setValue("iptcObjectNameList",          sd.iptcObjectNameList);
    setValue("iptcKeywordsMap",             sd.iptcKeywordsMap);
    setValue("iptcKeywordsList",            sd.iptcKeywordsList);
    setValue("iptcDescriptionMap",          sd.iptcDescriptionMap);
    setValue("iptcDescriptionList",         sd.iptcDescriptionList);
    setValue("iptcCountryNameMap",          sd.iptcCountryNameMap);
    setValue("iptcCountryNameList",         sd.iptcCountryNameList);
    setValue("iptcCityMap",                 sd.iptcCityMap);
    setValue("iptcCityList",                sd.iptcCityList);
    setValue("iptcEditStatusMap",           sd.iptcEditStatusMap);
    setValue("iptcEditStatusList",          sd.iptcEditStatusList);
    endGroup(); // SelectionDialog
    beginGroup("Raw");
    setValue("enabled",         raw.enabled);
    setValue("dcrawPath",       raw.dcrawPath);
    setValue("dcrawOptions",    raw.dcrawOptions);
    endGroup(); // Raw
}

/** Migrates settings from SIR 2.1 format to format of current version.
  * \sa readSettings() migrateFrom_2_2() migrateFrom_2_3() migrateFrom_2_4()
  */
void Settings::migrateFrom_2_1() {
    // migration to SIR 2.2 format
    QFile configFile(fileName());
    if (configFile.open(QIODevice::ReadWrite)) {
        QString fileContent = configFile.read(configFile.size());
        fileContent.replace("[MainWindow]","[Settings]");
        configFile.resize(fileContent.size());
        if (configFile.seek(0))
            configFile.write(fileContent.toAscii());
        else {
            qWarning("Failed to settings migration from SIR 2.1 - seek file error");
            configFile.close();
            return;
        }
        configFile.close();
    }
    else {
        qWarning("Failed to settings migration from SIR 2.1 - opem file error");
        return;
    }
    // finally migration from SIR 2.2 format
    migrateFrom_2_2();
}

/** Migrates settings from SIR 2.2 format to format of current version.
  * \sa readSettings() migrateFrom_2_1() migrateFrom_2_3() migrateFrom_2_4()
  */
void Settings::migrateFrom_2_2() {
    // migrate settings from SIR 2.2
    bool    tempBool;
    QString tempString;
    int     tempInt;
    tempBool = value("MainWindow/alreadSent",false).toBool();
    remove("MainWindow/alreadSent");
    setValue("Settings/alreadySent",tempBool);
    tempString = value("Settings/width", "800").toString();
    remove("Settings/width");
    setValue("Size/widthPx",tempString.toInt());
    tempString = value("Settings/height", "600").toString();
    remove("Settings/height");
    setValue("Size/heightPx",tempString.toInt());
    tempBool = value("Settings/metadata",true).toBool();
    remove("Settings/metadata");
    setValue("Metadata/enabled",tempBool);
    tempBool = value("Settings/saveMetadata",true).toBool();
    remove("Settings/saveMetadata");
    setValue("Metadata/saveMetadata",tempBool);
    tempBool = value("Settings/raw", false).toBool();
    remove("Settings/raw");
    setValue("Raw/enabled",tempBool);
    beginGroup("Settings");
    tempInt = value("targetFormat",0).toInt();
    setValue("targetFormat",QString(QImageWriter::supportedImageFormats()[tempInt]));
    endGroup(); // Settings
}

/** Migrates settings from SIR 2.3 format to format of current version.
  * \sa readSettings() migrateFrom_2_1() migrateFrom_2_2() migrateFrom_2_4()
  */
void Settings::migrateFrom_2_3() {
    // migrate settings from SIR 2.3 to 2.4
    beginGroup("Exif");
    remove("originalDateMap");
    remove("originalDateList");
    remove("digitizedDateMap");
    remove("digitizedDateList");
    endGroup(); // Exifproperti
    // finally migration from SIR 2.4 format
    migrateFrom_2_4();
}

/** Migrates settings from SIR 2.4 format to format of current version.
  * \sa readSettings() migrateFrom_2_1() migrateFrom_2_2() migrateFrom_2_3()
  */
void Settings::migrateFrom_2_4() {
    // migrate from SIR 2.4 format
    bool    tempBool;
    QString tempString;
    int     tempInt;
    beginGroup("Settings");
    tempBool = value("alreadSent",false).toBool();
    remove("alreadSent");
    setValue("alreadySent",tempBool);
    tempInt = value("targetFormat",0).toInt();
    setValue("targetFormat",QString(QImageWriter::supportedImageFormats()[tempInt]));
    endGroup(); // Settings
    beginGroup("Metadata");
    tempBool = value("metadata",true).toBool();
    remove("metadata");
    setValue("enabled",tempBool);
    endGroup(); // Metadata
    beginGroup("Raw");
    tempBool = value("raw",false).toBool();
    remove("raw");
    setValue("enabled",tempBool);
    endGroup(); // Raw
    beginGroup("Size");
    tempString = value("widthPx","800").toString();
    setValue("widthPx",tempString.toInt());
    tempString = value("widthPercent","100").toString();
    setValue("widthPercent",tempString.toFloat());
    tempString = value("heightPx","800").toString();
    setValue("heightPx",tempString.toInt());
    tempString = value("heightPercent","100").toString();
    setValue("heightPercent",tempString.toFloat());
    endGroup(); // Size
}
