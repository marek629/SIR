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

#ifndef COMMANDLINEASSISTANT_H
#define COMMANDLINEASSISTANT_H

#include <QStringList>
#include <QCoreApplication>

/** \brief Command line arguments parser class.
  *
  * Supported arguments:
  *     * --help or -h                  print help message and quit
  *     * --lang or -l LANG             set application language to LANG witch is language symbol
  *     * --session or -s session.xml   restore session from XML session file
  *
  * The same language symbols are used to naming SIR translation files.
  *
  * SIR supports also common Qt command line options described on
  * http://qt-project.org/doc/qt-4.8/qapplication.html#QApplication
  *
  * Unexpected and invalid options are ignored.
  *
  * Example:
  * $ sir -hl pl
  * is equivalent to
  * $ sir --help --lang pl
  * both print help message in Polish language and quit.
  */
class CommandLineAssistant {
    Q_DECLARE_TR_FUNCTIONS(CommandLineAssistant)

public:
    CommandLineAssistant();
    int parse(const QStringList &args = QStringList());
    /** Returns XML session file to read by Session object. */
    QString sessionFile() const { return sessionFile_; }

private:
    QString sessionFile_;
};

#endif // COMMANDLINEASSISTANT_H
