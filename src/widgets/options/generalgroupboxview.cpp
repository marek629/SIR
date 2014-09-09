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

#include <QFileDialog>
#include <QImageWriter>
#include <QCompleter>
#include <QDirModel>
#include <QCoreApplication>

#include <typeinfo>

#include "generalgroupboxview.h"
#include "generalgroupboxcontroller.h"
#include "languageutils.h"

GeneralGroupBoxView::GeneralGroupBoxView(QWidget *parent) : AbstractOptionsGroupBox(parent) {
    setupUi(this);

    languages = LanguageUtils::instance();
    fileToNiceName = new QMap<QString, QString>();

    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList list;

    foreach(QByteArray format, imageFormats) {
        list.append(*new QString(format));
    }

    targetFormatComboBox->insertItems(0, list);

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

    coresSpinBox->setMaximum(GeneralGroupBoxController::maxCoresCount());

    createLanguageMenu();

    connect(browsePushButton, SIGNAL(clicked()),
            this, SLOT(browseDestination()));
    connect(coresCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(coresSpinBoxChecked(bool)));

    delete dir;
    delete completer;
    delete completer2;
}

GeneralGroupBoxView::~GeneralGroupBoxView() {
    delete fileToNiceName;
}

void GeneralGroupBoxView::loadSettings() {
    controller->loadSettings();
}

void GeneralGroupBoxView::saveSettings() {
    controller->saveSettings();
}

void GeneralGroupBoxView::setController(AbstractOptionsController *controller) {
    Q_ASSERT(typeid(*controller) == typeid(GeneralGroupBoxController));

    this->controller = (GeneralGroupBoxController *)controller;
}

QString GeneralGroupBoxView::languageName(const QString &fileName) const {
    return fileToNiceName->value(fileName);
}

/** Allows the user to set target directory path. */
void GeneralGroupBoxView::browseDestination() {
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
void GeneralGroupBoxView::coresSpinBoxChecked(bool checked) {
    if (checked) {
        controller->setCoresCount(GeneralGroupBoxController::detectCoresCount());
        coresSpinBox->setValue(controller->coresCount());
        coresSpinBox->setReadOnly(true);
    }
    else {
        coresSpinBox->setValue(controller->coresCount());
        coresSpinBox->setReadOnly(false);
    }
}

/** Sets up language combo box. */
void GeneralGroupBoxView::createLanguageMenu() {
    foreach (QString qmFile, languages->fileNames()) {
        LanguageInfo info = languages->languageInfo(qmFile);
        QString languageName = info.niceName;
        QIcon countryFlag(QCoreApplication::applicationDirPath() +
                          "/../share/sir/images/" + info.flagFile);
        languagesComboBox->addItem(countryFlag, languageName);
        fileToNiceName->insert(languageName, qmFile);
    }
}
