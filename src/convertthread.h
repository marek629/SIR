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
* File contributed by: Víctor Fernández <deejayworld@gmail.com>
*/

#ifndef CONVERTTHREAD_H
#define CONVERTTHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QImage>
#include <QDir>
#include "sharedinformation.h"
#include "metadatautils.h"

class ConvertThread : public QThread {

    Q_OBJECT

public:
    ConvertThread(QObject *parent, int tid);
    static void setDesiredSize(int width, int height, bool percent = false,
                        bool hasWidth = false, bool hasHeight = false,
                        bool maintainAspect = true);
    static void setDesiredSize(quint32 bytes);
    static void setDesiredFormat(const QString& format); // use after setSaveMetadata(bool)
    static void setDesiredRotation(bool rotate, double angle = 0.0);
    static void setQuality(int quality);
    static void setDestPrefix(const QString& destPrefix);
    static void setDestSuffix(const QString& destSuffix);
    static void setDestFolder(const QDir& destFolder);
    static void setOverwriteAll(bool overwriteAll = false);
    void convertImage(const QString& name, const QString& extension,
                      const QString& path);
    void confirmOverwrite(int result);
    void confirmEnlarge(int result);
    void confirmImage();
    void setAcceptWork(bool work);
    void getNextOrStop();
    void printError();
    static void setMetadataEnabled(bool value);
    static void setSaveMetadata(bool value);
    static void setRealRotate(bool rotate);
    static void setUpdateThumbnail(bool update);
    static void setRotateThumbnail(bool rotate);

    static SharedInformation *shared;

signals:
    void imageStatus(QStringList imageData, QString status, int statusNum);
    void question(const QString& targetFile, int tid, const QString& whatToDo);
    void getNextImage(int tid);

private:
    void run();

    bool work;
    QStringList imageData;
    QMutex imageMutex;
    QWaitCondition imageCondition;
    int tid;
    QMutex overwriteMutex;
    QWaitCondition overwriteCondition;
    QMutex enlargeMutex;
    QWaitCondition enlargeCondition;
    MetadataUtils::Metadata metadata;
};


#endif
