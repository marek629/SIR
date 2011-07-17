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
    connect(okButton,SIGNAL(clicked()), this, SLOT(writeSettings()));

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

    createLanguageMenu();
    createConnections();
    readSettings();
}

OptionsDialog::~OptionsDialog() {
    delete languages;
}

void OptionsDialog::createConnections() {
    connect(browsePushButton, SIGNAL(clicked()), this,
            SLOT(browseDestination()));

    connect(dcrawPushButton, SIGNAL(clicked()), this, SLOT(browseDcraw()));
    connect(rawCheckBox, SIGNAL(stateChanged (int)), SLOT(setRawStatus(int)));
    connect(coresCheckBox, SIGNAL(toggled(bool)), this, SLOT(respondCoresSpinBox(bool)));
    connect(metadataCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableMetadata(bool)));
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

    settings.beginGroup("Settings");
    settings.setValue("targetFolder", targetFolderLineEdit->text());
    settings.setValue("targetFormat", targetFormatComboBox->currentIndex());
    settings.setValue("width",widthLineEdit->text());
    settings.setValue("height",heightLineEdit->text());
    settings.setValue("targetPrefix",targetPrefixLineEdit->text());
    settings.setValue("quality", qualitySpinBox->value());
    settings.setValue("languageNiceName", languagesComboBox->currentText());
    settings.setValue("languageFileName",
                      fileToNiceName->value(languagesComboBox->currentText()));

    if (coresCheckBox->isChecked())
        settings.setValue("cores", 0);
    else
        settings.setValue("cores", coresSpinBox->value());

    settings.setValue("metadata", metadataCheckBox->isChecked());
    settings.setValue("saveMetadata", saveMetadataCheckBox->isChecked());

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

    settings.endGroup();

    if(dcrawOk || !firstState) {
        emit ok();
        this->close();
    }
}

void OptionsDialog::readSettings() {

    QSettings settings("SIR");

    QString locale = QLocale::system().name();

    QString defaultLanguage = languages->getLanguageInfo("sir_"+locale+".qm").niceName;

    settings.beginGroup("Settings");
    targetFolderLineEdit->setText(settings.value("targetFolder",
                                                 QDir::homePath()).toString());
    targetFormatComboBox->setCurrentIndex(settings.value("targetFormat", 0).toInt());
    widthLineEdit->setText(settings.value("width", "800").toString());
    heightLineEdit->setText(settings.value("height", "600").toString());
    targetPrefixLineEdit->setText(settings.value("targetPrefix", "web").toString());
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

    metadataCheckBox->setChecked(settings.value("metadata",true).toBool());
    if (!metadataCheckBox->isChecked())
        saveMetadataCheckBox->setEnabled(false);
    else
        saveMetadataCheckBox->setChecked(settings.value("saveMetadata",true).toBool());

    int state = settings.value("raw", false).toBool();
    rawCheckBox->setChecked(state);
    setRawStatus(state);

    dcrawLineEdit->setText(settings.value("dcrawPath", "/usr/bin/dcraw").toString());
    dcrawOptions->setText(settings.value("dcrawOptions", "").toString());

    settings.endGroup();
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
    if (checked)
        saveMetadataCheckBox->setEnabled(true);
    else {
        saveMetadataCheckBox->setChecked(false);
        saveMetadataCheckBox->setEnabled(false);
    }
}

quint8 OptionsDialog::detectCoresCount() {
    int cores = QThread::idealThreadCount();
    if (cores == -1) {
        qDebug("OptionsDialog: cores count detect failed");
        return 1;
    }
    else if (cores > 50) //coresSpinBox's maximum value
        return 50;
    else
        return (quint8)cores;
}
