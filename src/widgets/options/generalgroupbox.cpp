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

#include <QThread>
#include <QFileDialog>
#include <QImageWriter>
#include <QCompleter>
#include <QDirModel>
#include <QCoreApplication>
#include "generalgroupbox.h"
#include "settings.h"
#include "languageutils.h"

quint8 GeneralGroupBox::maxCoresCount = 50;

/** Default constructor.\n
  * Sets UI and creates connections.
  * \sa setupUi()
  */
GeneralGroupBox::GeneralGroupBox(QWidget *parent) : AbstractOptionsGroupBox(parent) {
    setupUi(this);

    languages = LanguageUtils::instance();
    fileToNiceName = new QMap<QString, QString>();

    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList list;

    foreach(QByteArray format, imageFormats) {
        list.append(*new QString(format));
    }

    targetFormatComboBox->insertItems(0,list);

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

    connect(browsePushButton, SIGNAL(clicked()),
            this, SLOT(browseDestination()));
    connect(coresCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(respondCoresSpinBox(bool)));

    delete dir;
    delete completer;
    delete completer2;
}

/** Destructor. */
GeneralGroupBox::~GeneralGroupBox() {
    delete fileToNiceName;
}

/** Returns CPU cores count. */
quint8 GeneralGroupBox::detectCoresCount() {
    int cores = QThread::idealThreadCount();
    if (cores == -1) {
        qWarning("GeneralGroupBox: cores count detect failed");
        return 1;
    }
    if (cores > maxCoresCount)
        return maxCoresCount;
    return (quint8)cores;
}

/** Load settings and sets member widgets values.
  * \sa Settings saveSettings()
  */
void GeneralGroupBox::loadSettings() {
    Settings *s = Settings::instance();
    // settings
    targetFolderLineEdit->setText(              s->settings.targetFolder);
    targetFormatComboBox->setCurrentIndex(
                targetFormatComboBox->findText( s->settings.targetFormat));
    targetPrefixLineEdit->setText(              s->settings.targetPrefix);
    targetSuffixLineEdit->setText(              s->settings.targetSuffix);
    qualitySpinBox->setValue(                   s->settings.quality);
    languagesComboBox->setCurrentIndex(
                languagesComboBox->findText(    s->settings.languageNiceName,
                                            Qt::MatchExactly) );
    coresCount =                                s->settings.cores;
    if (coresCount == 0) {
        coresCheckBox->setChecked(true);
        respondCoresSpinBox(true);
    }
    else {
        coresCheckBox->setChecked(false);
        respondCoresSpinBox(false);
    }
    dateDisplayFormatLineEdit->setText(         s->settings.dateDisplayFormat);
    timeDisplayFormatLineEdit->setText(         s->settings.timeDisplayFormat);
    historySpinBox->setValue(CommonOptions::instance()->maxHistoryCount());
    // size
    widthPxSpinBox->setValue(               s->size.widthPx);
    widthPercentDoubleSpinBox->setValue(    s->size.widthPercent);
    heightPxSpinBox->setValue(              s->size.heightPx);
    heightPercentDoubleSpinBox->setValue(   s->size.heightPercent);
    fileSizeSpinBox->setValue(              s->size.fileSizeValue);
    fileSizeComboBox->setCurrentIndex(      s->size.fileSizeUnit);
    sizeUnitComboBox->setCurrentIndex(      s->size.sizeUnit);
    aspectRatioCheckBox->setChecked(        s->size.keepAspectRatio);
}

/** Saves settings basing member widgets values.
  * \sa Settings loadSettings()
  */
void GeneralGroupBox::saveSettings() {
    Settings *s = Settings::instance();
    // settings
    s->settings.targetFolder         = targetFolderLineEdit->text();
    s->settings.targetFormat         = targetFormatComboBox->currentText();
    s->settings.targetPrefix         = targetPrefixLineEdit->text();
    s->settings.targetSuffix         = targetSuffixLineEdit->text();
    s->settings.quality              = qualitySpinBox->value();
    s->settings.languageNiceName     = languagesComboBox->currentText();
    s->settings.languageFileName     = fileToNiceName->value(
                                            languagesComboBox->currentText() );
    s->settings.dateDisplayFormat    = dateDisplayFormatLineEdit->text();
    s->settings.timeDisplayFormat    = timeDisplayFormatLineEdit->text();
    if (coresCheckBox->isChecked())
        s->settings.cores            = 0;
    else
        s->settings.cores            = coresSpinBox->value();
    s->settings.maxHistoryCount      = historySpinBox->value();
    // size
    s->size.widthPx          = widthPxSpinBox->value();
    s->size.widthPercent     = widthPercentDoubleSpinBox->value();
    s->size.heightPx         = heightPxSpinBox->value();
    s->size.heightPercent    = heightPercentDoubleSpinBox->value();
    s->size.fileSizeValue    = fileSizeSpinBox->value();
    s->size.fileSizeUnit     = fileSizeComboBox->currentIndex();
    s->size.sizeUnit         = sizeUnitComboBox->currentIndex();
    s->size.keepAspectRatio  = aspectRatioCheckBox->isChecked();
}

/** Allows the user to set target directory path. */
void GeneralGroupBox::browseDestination() {
    QString initialName = targetFolderLineEdit->text();
    if (initialName.isEmpty())
        initialName = QDir::homePath();
    QString fileName = QFileDialog::getExistingDirectory(this,
                                                         tr("Choose a directory"),
                                                         initialName,
                                                         QFileDialog::ShowDirsOnly );
    fileName = QDir::toNativeSeparators(fileName);
    if (!fileName.isEmpty())
        targetFolderLineEdit->setText(fileName);
}

/** Detects cores count and makes coresSpinBox read-only if \a checked is true.
  * Otherwise restore old value of coresSpinBox and makes them read-write.
  */
void GeneralGroupBox::respondCoresSpinBox(bool checked) {
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

/** Sets up language combo box. */
void GeneralGroupBox::createLanguageMenu() {
    foreach (QString qmFile, languages->fileNames()) {
        LanguageInfo info = languages->languageInfo(qmFile);
        QString languageName = info.niceName;
        QIcon countryFlag(QCoreApplication::applicationDirPath() +
                          "/../share/sir/images/" + info.flagFile);
        languagesComboBox->addItem(countryFlag, languageName);
        fileToNiceName->insert(languageName, qmFile);
    }
}
