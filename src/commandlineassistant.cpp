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

#include <QTranslator>
#include <QLibraryInfo>
#include <iostream>
#include "commandlineassistant.h"
#include "languageutils.h"
#include "settings.h"
#include "metadata/string.h"

/** Creates CommandLineAssistant object. */
CommandLineAssistant::CommandLineAssistant() {}

/** Parses command line arguments and executes the arguments.
  * \param args Application argument list created of main functions \a argv array.
  * \return Possitive value if success finished, 0 if help message printed and
  *         negative value if error occured.
  */
int CommandLineAssistant::parse(const QStringList &args) {
    QStringList longArgs = args.filter(QRegExp("^(-){2}(help|lang)$"));
    QStringList shortArgs = args.filter(QRegExp("^(-){1}(h|l)+$"));
    QString lang;
    if (longArgs.contains("--lang"))
        lang = args[args.indexOf("--lang")+1];
    else if (!shortArgs.filter("l").isEmpty())
        lang = args[args.indexOf(shortArgs.filter("l")[0])+1];
    LanguageUtils *languages = LanguageUtils::instance();
    QString qmFile = languages->fileName(lang);
    languages->appTranslator->load(qmFile);
    languages->qtTranslator->load("qt_"+qmFile.split('_').at(1).split('.').first(),
                                 QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    // print help
    if (!shortArgs.filter("h").isEmpty() || !longArgs.filter("help").isEmpty()) {
        qApp->installTranslator(languages->qtTranslator);
        qApp->installTranslator(languages->appTranslator);
        MetadataUtils::String help =
            tr("Usage: sir [options] [files]\n"
               "Typed files will load to files list in main window.\n"
               "\n"
               "Supported arguments:\n"
               " --help or -h      print help message and quit\n"
               " --lang or -l LANG set application language to LANG witch is language symbol\n"
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
    // set language
    else if (!shortArgs.filter("l").isEmpty() || !longArgs.filter("lang").isEmpty()) {
        Settings *s = Settings::instance();
        s->settings.languageFileName = qmFile;
        s->settings.languageNiceName = languages->languageInfo(qmFile).niceName;
        return 2;
    }
    return 1;
}
