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

#ifndef ABSTRACTOPTIONS_H
#define ABSTRACTOPTIONS_H

#include <QDir>
#include <QGroupBox>

//! Common options data for classes inherited from AbstractOptions class.
class CommonOptions {
public:
    // singleton instance
    static CommonOptions *instance();
    // access methods
    QString targetDirPath() const { return targetDirPath_; }
    void setTargetDirPath(const QString &path)
    { targetDirPath_ = (path.isEmpty()) ? QDir::homePath() : path; }
    int maxHistoryCount() const { return maxHistoryCount_; }
    void setMaxHistoryCount(int v) { maxHistoryCount_ = v; }

private:
    // constructor
    CommonOptions();
    // fields
    QString targetDirPath_;
    int maxHistoryCount_;
};

//! Abstract base class for all derived classes from OptionsDialog.
class AbstractOptions {
public:
    AbstractOptions(QWidget *parent = 0) { Q_UNUSED(parent) }
    virtual ~AbstractOptions() {}

protected:
    // pure virtual functions
    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
};

//! Abstract base class for all derived classes from QGroupBox and AbstractOptions.
class AbstractOptionsGroupBox : public QGroupBox, public AbstractOptions {
public:
    AbstractOptionsGroupBox(QWidget *parent = 0) : QGroupBox(parent) {}
    // pure virtual functions
    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
};

#endif // ABSTRACTOPTIONS_H
