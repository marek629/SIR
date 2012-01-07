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

#ifndef SHAREDINFORMATION_H
#define SHAREDINFORMATION_H

#include <QMutex>
#include <QString>
#include <QDir>

class ConvertThread;

class SharedInformation {

public:
    
    SharedInformation();
    SharedInformation( const SharedInformation & );
    SharedInformation &operator=( const SharedInformation & );

    quint32 width;
    quint32 height;
    bool hasWidth;
    bool hasHeight;
    bool maintainAspect;
    quint32 sizeBytes;
    char sizeUnit;

    QDir destFolder;
    QString prefix;
    QString suffix;
    QString format;
    int quality;

    bool rotate;
    double angle;

    bool saveMetadata;
    bool realRotate;
    bool updateThumbnail;
    bool rotateThumbnail;

    QMutex mutex;
    bool abort;

    bool overwriteAll;
    bool noOverwriteAll;
    int overwriteResult;

    bool enlargeAll;
    bool noEnlargeAll;
    int enlargeResult;
};

#endif // SHAREDINFORMATION_H
