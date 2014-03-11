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
 * Program URL: http://sir.projet-libre.org/
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
#else
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

#if QT_VERSION >= 0x050000
bool findIconTheme() {
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
    else
        qWarning() << "No icon theme found. Tried:" << themes;

    return themeFound;
}
#endif // QT_VERSION
