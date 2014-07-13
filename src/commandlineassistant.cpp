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

#include <QTranslator>
#include <QLibraryInfo>
#include <iostream>
#include "commandlineassistant.h"
#include "languageutils.h"
#include "settings.h"
#include "sir_string.h"

#include <QMessageBox>
#include <QDebug>

using namespace sir;

/** Creates CommandLineAssistant object. */
CommandLineAssistant::CommandLineAssistant() {}

/** Parses command line arguments and executes the arguments.
  * \param args Application argument list created of main functions \a argv array.
  * \return Possitive value if success finished, 0 if help message printed and
  *         negative value if error occured.
  */
int CommandLineAssistant::parse(const QStringList &args) {
    QString argString;
    foreach (QString arg, args) {
        argString += arg + '\n';
    }
//    QMessageBox::information(0, "CMD Arguments", argString);
    qDebug() << argString;

    QString uniqueID("Open as SIR from file manager");
    memory.setKey(uniqueID);
    qDebug() << memory.size() << memory.key() << memory.nativeKey();
    qDebug() << memory.error() << memory.errorString();
    if (memory.attach(QSharedMemory::ReadWrite)) {
        // first instance of SIR is already running
        QByteArray bytes;
        foreach (QString str, args) {
            if (str.contains(';'))
                continue;
            bytes.append(str);
            bytes.append(';');
        }
        bytes.append('\0');
        memory.lock();
        char *to = (char*)memory.data();
        const char *from = bytes.data();
        qDebug() << "bytes size:" << bytes.size();
        memcpy(to, from, qMin(memory.size(), bytes.size()));
        memory.unlock();
        qDebug(to);
        QMessageBox::warning(0, "Shared Memory", QString(to));

        return -200;
    }
    qDebug() << memory.error() << memory.errorString();
    if (!memory.create(200000, QSharedMemory::ReadWrite)) {
//        QMessageBox::critical(0, "Shared Memory Error", "Can not create single instance of SIR!");
        qDebug("Can not create single instance of SIR!");
        return -201;
    }
    qDebug() << memory.error() << memory.errorString();
//    QMessageBox::information(0, "Shared Memory Size", QString::number(memory.size()));
    qDebug() << memory.size() << memory.key() << memory.nativeKey();

    QStringList longArgs = args.filter(QRegExp("^(-){2}(help|lang|session)+$"));
    QStringList shortArgs = args.filter(QRegExp("^(-){1}(h|l|s)+$"));
    QString lang;
    int result = 1;

    if (longArgs.contains("--lang"))
        lang = args[args.indexOf("--lang")+1];
    else if (!shortArgs.filter("l").isEmpty())
        lang = args[args.indexOf(shortArgs.filter("l")[0])+1];
    if (longArgs.contains("--session"))
        sessionFile_ = args[args.indexOf("--session")+1];
    else if (!shortArgs.filter("s").isEmpty())
        sessionFile_ = args[args.indexOf(shortArgs.filter("s")[0])+1];

    LanguageUtils *languages = LanguageUtils::instance();
    QString qmFile = languages->fileName(lang);
    languages->appTranslator->load(qmFile,
                                   QCoreApplication::applicationDirPath()
                                   +QString("../share/sir/translations") );
    languages->qtTranslator->load("qt_"+qmFile.split('_').at(1).split('.').first(),
                                  QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(languages->qtTranslator);
    qApp->installTranslator(languages->appTranslator);

    // print help
    if (!shortArgs.filter("h").isEmpty() || !longArgs.filter("help").isEmpty()) {
        String help =
            tr("Usage: sir [options] [files]\n"
               "Typed files will load to files list in main window.\n"
               "\n"
               "Supported arguments:\n"
               " --help or -h       print help message and quit\n"
               " --lang or -l LANG  set application language to LANG witch is language symbol\n"
               " --session or -s session.xml   restore session from XML session file\n"
               "\n"
               "Example:\n"
               "$ sir -hl pl\n"
               "is equivalent to\n"
               "$ sir --help --lang pl\n"
               "both print help message in Polish language and quit.\n"
               );
        std::cout << help.toNativeStdString() << std::endl;
        return 0;
    }
    else {
        // set language
        if (!lang.isEmpty()) {
            Settings *s = Settings::instance();
            s->settings.languageFileName = qmFile;
            s->settings.languageNiceName = languages->languageInfo(qmFile).niceName;
            result += 2;
        }
        // restore session
        if (!sessionFile_.isEmpty())
            result += 1;
    }

    return result;
}
