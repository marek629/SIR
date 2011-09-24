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
    void setDesiredSize(int width, int height, bool hasWidth = true, bool hasHeight = false, bool maintainAspect = false);
    void setDesiredFormat(const QString& format);
    void setDesiredRotation(bool rotate, double angle = 0.0);
    void setQuality(int quality);
    void setDestPrefix(const QString& destPrefix);
    void setDestFolder(const QDir& destFolder);
    void setOverwriteAll(bool overwriteAll = false);
    void convertImage(const QString& name, const QString& extension, const QString& path, bool onlySelected = false);
    void confirmOverwrite(int result);
    void confirmEnlarge(int result);
    void confirmImage();
    void setAcceptWork(bool work);
    void getNextOrStop();
    static SharedInformation *shared;
    static void setSaveMetadata(bool value);

signals:
    void imageStatus(QStringList imageData, QString status, int statusNum);
    void question(const QString& targetFile, int tid, const QString& whatToDo);
    void getNextImage(int tid, bool onlySelected);

private:
    void run();

    int m_width;
    int m_height;
    bool m_hasWidth;
    bool m_hasHeight;
    bool m_maintainAspect;
    bool work;
    QStringList imageData;
    QString m_format;
    bool m_rotate;
    double m_angle;
    int m_quality;
    QString m_destPrefix;
    QDir m_destFolder;
    QMutex imageMutex;
    QWaitCondition imageCondition;
    int tid;
    bool onlySelected;
    QMutex overwriteMutex;
    QWaitCondition overwriteCondition;
    QMutex enlargeMutex;
    QWaitCondition enlargeCondition;
    MetadataUtils::Metadata metadata;
};


#endif
