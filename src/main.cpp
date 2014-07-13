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

#include <QtCore/QLocale>
#include <QtCore/QTextCodec>
#include <QDebug>

#include "main.h"
#include "commandlineassistant.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#if QT_VERSION < 0x050000
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif // QT_VERSION
#if QT_VERSION >= 0x050000 || defined(Q_OS_OS2)
    findIconTheme();
#endif // QT_VERSION

    QTextCodec* codec = QTextCodec::codecForLocale ();

    // Parse the arguments from the command line
    QStringList args;
    for(int i = 1; i < argc; i++)
        args += codec->toUnicode(argv[i]);
    args.removeDuplicates();

    CommandLineAssistant cmd;
    int cmdParseResult = cmd.parse(args);
    if (cmdParseResult < 1)
        return cmdParseResult;

    ConvertDialog w(0, args, cmd);
    w.show();
    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
    return app.exec();
}

#if QT_VERSION >= 0x050000 || defined(Q_OS_OS2)
bool findIconTheme() {
#if defined(Q_OS_WIN32) || defined(Q_OS_OS2)
    /* For Windows build get Tango icon theme from
     * https://googledrive.com/host/0B9Il9AGwsKEvLVpDVmIwbHVoYlE/Tango.zip
     * and extract this into /share/pixmaps app directory.
     */
    QDir pixmapsDir(QCoreApplication::applicationDirPath() + "/../share/pixmaps");
    QStringList searchPaths;
    searchPaths << pixmapsDir.absolutePath();
    QIcon::setThemeSearchPaths(searchPaths);
#endif // Q_OS_WIN32

    QString themeTestIconName = "list-add";
    QStringList themes;
    themes << "Tango" << "oxygen" << "gnome";

    bool themeFound = false;
    foreach (QString themeName, themes) {
        QIcon::setThemeName(themeName);
        themeFound = QIcon::hasThemeIcon(themeTestIconName);
        if (themeFound)
            break;
    }

    if (themeFound)
        qDebug() << "Icon theme found:" << QIcon::themeName();
    else {
        qWarning() << "No icon theme found. Tried:" << themes;
        qDebug() << "Please save any icon theme to one of"
                 << QIcon::themeSearchPaths()
                 << "directory if you wish see icons in SIR.";
        qDebug() << "SIR needs *.theme file in specifed directory and 16x16 and"
                 << "32x32 subdirectories containing icons.";
    }

    return themeFound;
}
#endif // QT_VERSION
