/*
* This file is part of SIR, an open-source cross-platform Image tool
* 2007  Rafael Sachetto
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
* Program URL: http://sir.projet-libre.org/
*
*/

#include <QSettings>
#include <QDir>
#include <QImageWriter>
#include <QCompleter>
#include <QDirModel>
#include <QLocale>
#include <QMap>
#include <QIcon>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>

#include "optionsdialog.h"
#include "languageutils.h"
#include "convertdialog.h"

quint8 OptionsDialog::maxCoresCount = 50;

OptionsDialog::OptionsDialog( QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {

    setupUi(this);

    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList list;

    foreach(QByteArray format, imageFormats) {
        list.append(*new QString(format));
    }

    languages = new LanguageUtils();

    fileToNiceName = new QMap<QString, QString>();

    targetFormatComboBox->insertItems(0,list);

    // copying items of combo boxes from parent ConvertDialog window
    ConvertDialog* mainWindow = (ConvertDialog*)parent;
    int size = mainWindow->sizeUnitComboBox->count();
    for (int i=0; i<size; i++)
        sizeUnitComboBox->addItem(mainWindow->sizeUnitComboBox->itemText(i));
    size = mainWindow->fileSizeComboBox->count();
    for (int i=0; i<size; i++)
        fileSizeComboBox->addItem(mainWindow->fileSizeComboBox->itemText(i));

    QCompleter *completer = new QCompleter(this);
    QDirModel *dir = new QDirModel(completer);
    dir->setFilter(QDir::AllDirs);
    completer->setModel(dir);
    targetFolderLineEdit->setCompleter(completer);

    QStringList wordList;
    wordList << "web" << "thumb" << "thumbnail" << "resized" << "new";
    QCompleter *completer2 = new QCompleter(wordList, this);
    completer2->setCaseSensitivity(Qt::CaseInsensitive);
    targetPrefixLineEdit->setCompleter(completer2);

    coresSpinBox->setMaximum(maxCoresCount);

    createLanguageMenu();
    createConnections();
    setupWindow();
    readSettings();

    delete dir;
    delete completer;
    delete completer2;
}

OptionsDialog::~OptionsDialog() {
    delete languages;
    delete validator;
    delete fileToNiceName;
    delete groupBoxes;
}

void OptionsDialog::createConnections() {
    // general
    connect(listWidget, SIGNAL(currentRowChanged(int)),
            this, SLOT(categoryChanged(int)));
    connect(browsePushButton, SIGNAL(clicked()),
            this, SLOT(browseDestination()));
    connect(coresCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(respondCoresSpinBox(bool)));

    // metadata
    connect(metadataCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(enableMetadata(bool)));
    connect(saveMetadataCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(saveMetadata(bool)));
    connect(thumbUpdateCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(updateThumbnail(bool)));
    connect(exifArtistCheckBox, SIGNAL(toggled(bool)),
            exifArtistComboBox, SLOT(setEnabled(bool)));
    connect(exifCopyrightCheckBox, SIGNAL(toggled(bool)),
            exifCopyrightComboBox, SLOT(setEnabled(bool)));
    connect(exifUserCommentCheckBox, SIGNAL(toggled(bool)),
            exifUserCommentComboBox, SLOT(setEnabled(bool)));

    // raw
    connect(dcrawPushButton, SIGNAL(clicked()), this, SLOT(browseDcraw()));
    connect(rawCheckBox, SIGNAL(stateChanged(int)), SLOT(setRawStatus(int)));

    // ok button
    connect(okButton,SIGNAL(clicked()), this, SLOT(writeSettings()));
}

void OptionsDialog::setupWindow() {
    // validate only ASCII characters
    validator = new QRegExpValidator(QRegExp("^[\\00-\\0177]+"),this);
    exifArtistComboBox->setValidator(validator);
    exifCopyrightComboBox->setValidator(validator);

    QList <int> sizes;
    sizes.append(120);
    sizes.append(500);
    splitter->setSizes(sizes);

    currentListItem = 0;
    listWidget->setCurrentRow(currentListItem);

    int categoriesCount = listWidget->count();
    groupBoxes = new QGroupBox* [categoriesCount];
    groupBoxes[0] = generalBox;
    groupBoxes[1] = metadataBox;
    groupBoxes[2] = rawBox;
    for (int i=0; i<categoriesCount; i++)
        groupBoxes[i]->hide();
    groupBoxes[currentListItem]->show();
}

void OptionsDialog::categoryChanged(int current) {
    if (current == currentListItem)
        return;
    groupBoxes[currentListItem]->hide();
    groupBoxes[current]->show();
    currentListItem = current;
}

void OptionsDialog::browseDestination() {

    QString initialName = targetFolderLineEdit->text();

    if (initialName.isEmpty())
        initialName = QDir::homePath();


    QString fileName = QFileDialog::getExistingDirectory(
            this,
            tr("Choose a directory"),
            initialName,
            QFileDialog::ShowDirsOnly );

    fileName = QDir::convertSeparators(fileName);

    if (!fileName.isEmpty()) {
        targetFolderLineEdit->setText(fileName);
    }
}

bool OptionsDialog::checkDcrawPath(QString fileName) {
    if (!fileName.isEmpty()) {
        QFile dcraw(fileName);
        //Check if the file exists
        if(!dcraw.exists()) {
            QMessageBox::warning(
                    this, "SIR",
                    tr("dcraw executable not found. "
                       "RAW support will not be enabled!")
            );
            return false;
        }
        else {
            //is executable??
            if(dcraw.permissions().testFlag(QFile::ExeOwner)) {
                //the file is executable
                return true;
            }
            else {
                //Not executable
                QMessageBox::warning(
                        this, "SIR",
                        tr("The chosen file is not executable. "
                           "RAW support will not be enabled!" )
                );
                return false;

            }
        }
    }
    else {
        QMessageBox::warning(
                        this, "SIR",
                        tr("No dcraw executable chosen. "
                           "RAW support will not be enabled!" )
                        );
        return false;
    }
}

void OptionsDialog::browseDcraw() {

    QString initialName = targetFolderLineEdit->text();

    if (initialName.isEmpty())
        initialName = QDir::homePath();

       QString fileName = QFileDialog::getOpenFileName(
                            this,
                            tr("Select dcraw executable"),
                            initialName
                        );

    fileName = QDir::convertSeparators(fileName);
    dcrawLineEdit->setText(fileName);

}

void OptionsDialog::writeSettings() {
    QSettings settings("SIR");

    // general
    settings.beginGroup("Settings");
    settings.setValue("targetFolder", targetFolderLineEdit->text());
    settings.setValue("targetFormat", targetFormatComboBox->currentIndex());
    settings.setValue("targetPrefix",targetPrefixLineEdit->text());
    settings.setValue("targetSuffix", targetSuffixLineEdit->text());
    settings.setValue("quality", qualitySpinBox->value());
    settings.setValue("languageNiceName", languagesComboBox->currentText());
    settings.setValue("languageFileName",
                      fileToNiceName->value(languagesComboBox->currentText()));
    if (coresCheckBox->isChecked())
        settings.setValue("cores", 0);
    else
        settings.setValue("cores", coresSpinBox->value());
    settings.setValue("maxHistoryCount", historySpinBox->value());
    settings.endGroup(); // Settings

    // size
    settings.beginGroup("Size");
    settings.setValue("sizeUnit", sizeUnitComboBox->currentIndex());
    settings.setValue("widthPx",widthLineEdit->text());
    settings.setValue("widthPercent", widthPercentLineEdit->text());
    settings.setValue("heightPx",heightLineEdit->text());
    settings.setValue("heightPercent", heightPercentLineEdit->text());
    settings.setValue("fileSizeValue", fileSizeSpinBox->value());
    settings.setValue("fileSizeUnit", fileSizeComboBox->currentIndex());
    settings.endGroup(); // Size

    // metadata (general part)
    settings.beginGroup("Metadata");
    settings.setValue("metadata", metadataCheckBox->isChecked());
    settings.setValue("saveMetadata", saveMetadataCheckBox->isChecked());
    settings.setValue("realRotate", rotateRadioButton->isChecked());
    settings.setValue("updateThumbnail", thumbUpdateCheckBox->isChecked());
    settings.setValue("rotateThumbnail", thumbRotateCheckBox->isChecked());
    settings.endGroup(); // Metadata

    // metadata (Exif part)
    settings.beginGroup("Exif");
    QMap<QString,QVariant> exifMap;
    QList<QVariant> exifList;

    settings.setValue("artistOverwrite", exifArtistCheckBox->isChecked());
    exifArtistComboBox->exportHistory(&exifMap,&exifList,maxHistoryCount);
    settings.setValue("artistMap", exifMap);
    settings.setValue("artistList", exifList);

    settings.setValue("copyrightOverwrite", exifCopyrightCheckBox->isChecked());
    exifCopyrightComboBox->exportHistory(&exifMap,&exifList,maxHistoryCount);
    settings.setValue("copyrightMap", exifMap);
    settings.setValue("copyrightList", exifList);

    settings.setValue("userCommentOverwrite",
                      exifUserCommentCheckBox->isChecked());
    exifUserCommentComboBox->exportHistory(&exifMap,&exifList,maxHistoryCount);
    settings.setValue("userCommentMap", exifMap);
    settings.setValue("userCommentList", exifList);

    settings.endGroup(); // Exif

    // raw
    settings.beginGroup("Raw");
    bool dcrawOk = false;
    bool firstState = rawCheckBox->isChecked();
    //check dcraw executable
    if(rawCheckBox->isChecked()) {
        if((dcrawOk = checkDcrawPath(dcrawLineEdit->text()))) {
            settings.setValue("raw", true);
            settings.setValue("dcrawPath", dcrawLineEdit->text());
            settings.setValue("dcrawOptions", dcrawOptions->text());
        }
        else {
            settings.setValue("raw", false);
            rawCheckBox->setChecked(false);
            settings.setValue("dcrawPath", dcrawLineEdit->text());
            settings.setValue("dcrawOptions", dcrawOptions->text());
            setRawStatus(false);
        }
    }
    else {
        settings.setValue("raw", false);
        settings.setValue("dcrawPath", dcrawLineEdit->text());
        settings.setValue("dcrawOptions", dcrawOptions->text());
    }
    if(dcrawOk || !firstState) {
        emit ok();
        this->close();
    }
    settings.endGroup(); // Raw
}

void OptionsDialog::readSettings() {

    QSettings settings("SIR");

    QString locale = QLocale::system().name();

    QString defaultLanguage = languages->getLanguageInfo("sir_"+locale+".qm").niceName;

    // general
    settings.beginGroup("Settings");
    targetFolderLineEdit->setText(settings.value("targetFolder",
                                                 QDir::homePath()).toString());
    targetFormatComboBox->setCurrentIndex(settings.value("targetFormat", 0).toInt());
    targetPrefixLineEdit->setText(settings.value("targetPrefix", "web").toString());
    targetSuffixLineEdit->setText(settings.value("targetSuffix", "thumb").toString());
    qualitySpinBox->setValue(settings.value("quality", 100).toInt());
    languagesComboBox->setCurrentIndex(languagesComboBox->findText(
        settings.value("languageNiceName",defaultLanguage).toString(),
        Qt::MatchExactly)
    );
    coresCount = settings.value("cores",0).toInt();
    if (coresCount == 0) {
        coresCheckBox->setChecked(true);
        respondCoresSpinBox(true);
    }
    else {
        coresCheckBox->setChecked(false);
        respondCoresSpinBox(false);
    }
    maxHistoryCount = settings.value("maxHistoryCount",5).toInt();
    historySpinBox->setValue(maxHistoryCount);
    settings.endGroup(); // Settings

    // size
    settings.beginGroup("Size");
    sizeUnitComboBox->setCurrentIndex(settings.value("sizeUnit", 0).toInt());
    widthLineEdit->setText(settings.value("widthPx", "800").toString());
    widthPercentLineEdit->setText(settings.value("widthPercent", "100").toString());
    heightLineEdit->setText(settings.value("heightPx", "600").toString());
    heightPercentLineEdit->setText(settings.value("heightPercent", "100").toString());
    fileSizeSpinBox->setValue(settings.value("fileSizeValue", 300.).toDouble());
    fileSizeComboBox->setCurrentIndex(settings.value("fileSizeUnit", 0).toInt());
    settings.endGroup(); // Size

    // metadata (general part)
    settings.beginGroup("Metadata");
    metadataCheckBox->setChecked(settings.value("metadata",true).toBool());
    if (!metadataCheckBox->isChecked()) {
        saveMetadataCheckBox->setEnabled(false);
        thumbUpdateCheckBox->setEnabled(false);
        thumbRotateCheckBox->setEnabled(false);
    }
    else
        saveMetadataCheckBox->setChecked(settings.value("saveMetadata",true).toBool());

    if (saveMetadataCheckBox->isChecked()) {
        thumbUpdateCheckBox->setChecked(settings.value("updateThumbnail",true).toBool());
        if (thumbUpdateCheckBox->isChecked())
            thumbRotateCheckBox->setChecked(settings.value("rotateThumbnail",false).toBool());
        rotateRadioButton->setChecked(settings.value("realRotate",false).toBool());
    }
    else {
        thumbUpdateCheckBox->setEnabled(false);
        thumbRotateCheckBox->setEnabled(false);
        rotateRadioButton->setChecked(true);
    }
    settings.endGroup(); // Metadata

    // metadata (Exif part)
    settings.beginGroup("Exif");
    bool exifOverwrite;
    QMap<QString,QVariant> exifDefaultMap;

    exifOverwrite = settings.value("artistOverwrite",false).toBool();
    exifArtistCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.insert(tr("Camera owner: ; Photographer: "),false);
    exifArtistComboBox->
            loadHistory(settings.value("artistMap",exifDefaultMap).toMap(),
                        settings.value("artistList").toList(),
                        maxHistoryCount);
    exifArtistComboBox->setEnabled(exifOverwrite);

    exifOverwrite = settings.value("copyrightOverwrite",false).toBool();
    exifCopyrightCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.clear();
    exifDefaultMap.insert(tr("Copyright owner"),false);
    exifCopyrightComboBox->
            loadHistory(settings.value("copyrightMap",exifDefaultMap).toMap(),
                        settings.value("copyrightList").toList(),
                        maxHistoryCount);
    exifCopyrightComboBox->setEnabled(exifOverwrite);

    exifOverwrite = settings.value("userCommentOverwrite",false).toBool();
    exifUserCommentCheckBox->setChecked(exifOverwrite);
    exifDefaultMap.clear();
    exifDefaultMap.insert(
                tr("This picture was edited with Simple Image Resizer"),false);
    exifUserCommentComboBox->
            loadHistory(settings.value("userCommentMap",exifDefaultMap).toMap(),
                        settings.value("userCommentList").toList(),
                        maxHistoryCount);
    exifUserCommentComboBox->setEnabled(exifOverwrite);

    settings.endGroup(); // Exif

    // raw
    settings.beginGroup("Raw");
    int state = settings.value("raw", false).toBool();
    rawCheckBox->setChecked(state);
    setRawStatus(state);
    dcrawLineEdit->setText(settings.value("dcrawPath", "/usr/bin/dcraw").toString());
    dcrawOptions->setText(settings.value("dcrawOptions", "").toString());
    settings.endGroup(); // Raw
}

void OptionsDialog::createLanguageMenu() {

    QDir dir(":translations/");
    QStringList filter;
    filter << "sir_*.qm";
    QStringList fileNames = dir.entryList(filter);

    QString languageName;
    QIcon * countryFlag;
    LanguageInfo info;
    info = languages->getLanguageInfo("sir_en_US.qm");

    languageName = info.niceName;
    QString trlangName = tr(languageName.toStdString().c_str());

    countryFlag = new QIcon(":images/" + info.flagFile);

    languagesComboBox->insertItem(0, *(countryFlag), trlangName);

    fileToNiceName->insert(trlangName, "sir_en_US.qm");

    for (int i = 0; i < (int)fileNames.size(); ++i) {
        info = languages->getLanguageInfo(fileNames[i]);
        languageName = info.niceName;

        trlangName = tr(languageName.toStdString().c_str());
        countryFlag = new QIcon(":images/" + info.flagFile);

        languagesComboBox->insertItem(i+1, *(countryFlag), trlangName);

        fileToNiceName->insert(trlangName, fileNames[i]);
    }

}

void OptionsDialog::setRawStatus(int state) {
    dcrawLineEdit->setEnabled(state);
    dcrawPushButton->setEnabled(state);
    dcrawOptions->setEnabled(state);
}

void OptionsDialog::respondCoresSpinBox(bool checked) {
    if (checked) {
        coresCount = detectCoresCount();
        coresSpinBox->setValue(coresCount);
        coresSpinBox->setReadOnly(true);
    }
    else {
        coresSpinBox->setValue(coresCount);
        coresSpinBox->setReadOnly(false);
    }
}

void OptionsDialog::enableMetadata(bool checked) {
    if (checked) {
        saveMetadataCheckBox->setEnabled(true);
        exifArtistCheckBox->setEnabled(true);
        exifCopyrightCheckBox->setEnabled(true);
        exifUserCommentCheckBox->setEnabled(true);
    }
    else {
        saveMetadataCheckBox->setChecked(false);
        saveMetadataCheckBox->setEnabled(false);
        exifArtistCheckBox->setChecked(false);
        exifArtistCheckBox->setEnabled(false);
        exifCopyrightCheckBox->setChecked(false);
        exifCopyrightCheckBox->setEnabled(false);
        exifUserCommentCheckBox->setChecked(false);
        exifUserCommentCheckBox->setEnabled(false);
    }
}

void OptionsDialog::saveMetadata(bool save) {
    thumbUpdateCheckBox->setChecked(save);
    thumbUpdateCheckBox->setEnabled(save);
    thumbRotateCheckBox->setEnabled(save);
    if (save)
        exifOrientationRadioButton->setChecked(true);
    else {
        rotateRadioButton->setChecked(true);
    }
}

void OptionsDialog::updateThumbnail(bool update) {
    if (!update)
        thumbRotateCheckBox->setChecked(false);
}

quint8 OptionsDialog::detectCoresCount() {
    int cores = QThread::idealThreadCount();
    if (cores == -1) {
        qWarning("OptionsDialog: cores count detect failed");
        return 1;
    }
    else if (cores > maxCoresCount)
        return maxCoresCount;
    else
        return (quint8)cores;
}
