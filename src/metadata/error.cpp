/*
 * This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto
 * 2011-2012  Marek Jędryka
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
 *                 Marek Jędryka   <jedryka89@gmail.com>
 * Program URL: http://sir.projet-libre.org/
 *
 */

#include "error.h"

/** Default constructor.\n
  * Constructs error object with empty strings and random error code.
  */
MetadataUtils::Error::Error() {}

/** Constructs error object based on \b e with empty message string.
  * \sa copy
  */
MetadataUtils::Error::Error(Exiv2::Error &e) {
    copy(e);
}

/** Constructs error object based on \b e and \b message.
  * \sa setMessage copy
  */
MetadataUtils::Error::Error(const QString &message, Exiv2::Error &e) {
    setMessage(message);
    copy(e);
}

/** Sets user readable error message to \b str.
  * \sa copy
  */
void MetadataUtils::Error::setMessage(const QString &str) {
    message_ = str;
}


void MetadataUtils::Error::copy(Exiv2::Error &e) {
    code_ = e.code();
    string = QString::fromUtf8(e.what());
}
